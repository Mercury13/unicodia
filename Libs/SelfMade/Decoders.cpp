// My header
#include "Decoders.h"

// C++
#include <regex>

using namespace std::string_view_literals;

constexpr char32_t PARA_SEP_32 = 0x2029;      // U+2029 paragraph separator
constexpr wchar_t PARA_SEP_16 = PARA_SEP_32;

std::u32string_view decode::normalizeEolSv(
        std::u32string_view x,
        std::u32string &cache)
{
    if (x.find(U'\r') == std::u32string_view::npos
            && x.find(PARA_SEP_32) == std::u32string_view::npos)
        return x;
    cache = x;
    str::replace(cache, U"\r\n", U"\n");
    str::replace(cache, U'\r', U'\n');
    str::replace(cache, PARA_SEP_32, U'\n');
    return cache;
}


std::wstring_view decode::normalizeEolSv(
        std::wstring_view x,
        std::wstring &cache)
{
    if (x.find(u'\r') == std::u32string_view::npos
            && x.find(PARA_SEP_16) == std::u32string_view::npos)
        return x;
    cache = x;
    str::replace(cache, L"\r\n", L"\n");
    str::replace(cache, L'\r', L'\n');
    str::replace(cache, PARA_SEP_16, L'\n');
    return cache;
}


bool decode::dcpp::isAlnum(char32_t x)
{
    return (x >= U'0' && x <= U'9')
        || (x >= U'a' && x <= U'z')
        || (x >= U'A' && x <= U'Z')
        || ( x == U'_');
}


bool decode::dcpp::isAlpha(char32_t x)
{
    return (x >= U'a' && x <= U'z')
        || (x >= U'A' && x <= U'Z')
        || ( x == U'_');
}


unsigned int decode::hexDigitValue(char32_t x)
{
    if (x >= U'0' && x <= U'9')
        return x - U'0';
    if (x >= U'A' && x <= U'F')
        return x - ('A' - 10);
    if (x >= U'a' && x <= U'f')
        return x - (U'a' - 10);
    return 999;
}


namespace {
    inline char32_t convertCp(char32_t c) {
        if (c < 0xD800) {
            if (c == U'\r')
                return U'\n';
            return c;
        } else {
            return (c >= 0xE000 && c <= 0x10FFFF) ? c : 0;
        }
    }

    void appendCode(std::u32string& r, char32_t c)
    {
        if (auto q = convertCp(c))
            r += q;
    }

    enum class CppState {
        SPACE,          // ␣␣      prefixStart YES
        SPACE_TRAIL,    // "x"␣␣   prefixStart YES
        PREFIX,         // ␣␣ab    prefixStart YES
        INSIDE,         // "x
        SLASH,          // "x/         BACK slash here
        OCT,            // "x/2        BACK slash here
        HEX,            // "x/uA       BACK slash here
        SUFFIX,         // "x"ab
            // Four states for unquoted string
            // When we see backslash outside quotes, we fall here!
            // Same order as INSIDE/SLASH/OCT/HEX
        OUTSIDE,        // ␣␣1+
        //OUT_SLASH,
        //OUT_OCT,
        //OUT_HEX,
        RAW_COLLECT,    // R"end
        RAW_INSIDE,     // R"end(x  prefixStart YES
    };

    enum class CppBase {
        INSIDE = static_cast<int>(CppState::INSIDE),
        OUTSIDE = static_cast<int>(CppState::INSIDE),
    };

    enum class CppDelta { INSIDE, SLASH, OCT, HEX };

    [[maybe_unused]] constexpr CppState operator + (CppBase base, CppDelta delta)
        { return CppState(static_cast<int>(base) + static_cast<int>(delta)); }
}   // anon namespace


std::u32string decode::cpp(std::u32string_view x)
{
    std::u32string cache;
    x = normalizeEolSv(x, cache);

    const char32_t* const start = x.data();
    const char32_t* const end = start + x.length();
    const char32_t* p = start;

    std::u32string r;

    CppState state = CppState::SPACE;
    const char32_t* prefixStart = p;
    char32_t charCode = 0;
    int nCodeCharsRemaining = 0;

    auto dropCode = [&](CppState revertState) {     // Both OCT and HEX → prefixStart NO
        appendCode(r, charCode);
        state = revertState;      // prefixStart keep NO
        nCodeCharsRemaining = 0;
        charCode = 0;
    };

    auto processCode = [&](char32_t c, unsigned base, CppState revertState) {
        if (auto val = hexDigitValue(c); val < base) {
            charCode = charCode * base + val;
            if ((--nCodeCharsRemaining) == 0) {
                dropCode(revertState);
            }
        } else {
            dropCode(revertState);
            r += c;
        }
    };

    auto processPostSlash = [&](char32_t c, CppBase base) {
        state = base + CppDelta::INSIDE;              // prefixStart keep NO
                // Maybe we’ll change state to another one!!
        switch (c) {
        case U'a': r += '\a'; break;
        case U'b': r += '\b'; break;
        case U't': r += '\t'; break;
        case U'n':
        case U'r': r += '\n'; break;     // both /n and /r yield LF!!
        case U'f': r += '\f'; break;
        case U'v': r += '\v'; break;
        case U'U':
            nCodeCharsRemaining = 8;
            charCode = 0;
            state = base + CppDelta::HEX;         // prefixStart keep NO
            break;
        case U'u':
            nCodeCharsRemaining = 4;
            charCode = 0;
            state = base + CppDelta::HEX;         // prefixStart keep NO
            break;
        case U'x':
            nCodeCharsRemaining = 2;
            charCode = 0;
            state = base + CppDelta::HEX;         // prefixStart keep NO
            break;
        case U'\n': break;               // do nothing, though is is strange string
        default:
            if (auto val = octDigitValue(c); val < 8) {
                charCode = val;
                nCodeCharsRemaining = 2;    // up to 3, incl. this one
                state = base + CppDelta::OCT;     // prefixStart keep NO
            } else {
                r += c;
            }
        }
    };

#define CASE_COMMA_CHARS  \
        case U',':   \
        case U';':   \
        case U')':   \
        case U'}'

    std::u32string rawEnd;
    size_t rawIndex = 0;

    for (; p != end; ++p) {
        auto c = *p;
        switch (state) {
        case CppState::SPACE:
        case CppState::SPACE_TRAIL: // prefixStart YES
            switch (c) {
            case U' ':
            case U'\n':
                break;  // do nothing
            case U'"':
                state = CppState::INSIDE;      // prefixStart YES→NO, do not dump
                prefixStart = nullptr;
                break;
            CASE_COMMA_CHARS:
                if (state == CppState::SPACE_TRAIL)
                    break;                  // do nothing in SPACE_TRAIL mode
                [[fallthrough]];
            default:
                if (dcpp::isAlpha(c)) {
                    state = CppState::PREFIX;  // prefixStart YES→YES, keep on stockpiling
                } else {
                    if (state == CppState::SPACE)
                        r.append(prefixStart, p);
                    r += c;
                    prefixStart = nullptr;
                    state = CppState::OUTSIDE; // prefixStart YES→NO, dump prefix+char (prefix not in SPACE_TRAIL)
                }
            }
            break;
        case CppState::PREFIX: // prefixStart YES
            switch (c) {
            case U' ':                       // ␣␣ab␣
            case U'\n':
                state = CppState::SPACE;       // prefixStart YES→YES, dump
                r.append(prefixStart, p);
                prefixStart = p;
                break;
            case U'"':                       // ␣␣ab"
                prefixStart = nullptr;  // do not dump in both branches, raw and escaped
                if (p != start && p[-1] == U'R') {
                        // 'R' is a prefix → thus from state PREFIX only
                        // always capital and last → thus [-1]
                        //   LR"()" OK,   RL"()" bad,   Lr"()" bad
                    state = CppState::RAW_COLLECT;  // prefixStart YES→NO, do not dump
                    rawEnd = U')';
                } else {
                    state = CppState::INSIDE;      // prefixStart YES→NO, do not dump
                }
                break;
            default:
                if (dcpp::isAlnum(c)) {     // ␣␣ab8
                    // do nothing, keep on stockpiling
                } else {                    // ␣␣ab+
                    state = CppState::OUTSIDE; // prefixStart YES→NO, dump prefix+char
                    r.append(prefixStart, p);
                    r += c;
                    prefixStart = nullptr;
                }
            }
            break;
        case CppState::OUTSIDE: // prefixStart NO
            switch (c) {
            case U' ':                       // ␣␣1+␣
            case U'\n':
                state = CppState::SPACE;       // prefixStart NO→YES
                prefixStart = p;
                break;
            case U'"':                       // ␣␣1+"
                state = CppState::INSIDE;      // prefixStart keep NO
                break;
            default:
                if (dcpp::isAlpha(c)) {     // ␣␣1+a
                    state = CppState::PREFIX;  // prefixStart NO→YES
                    prefixStart = p;
                } else {                    // ␣␣1+2
                    r += c;
                }
            }
            break;
        case CppState::INSIDE: // prefixStart NO
            switch (c) {
            case U'"':
                state = CppState::SUFFIX;  // prefixStart keep NO
                break;
            case U'\\':
                state = CppState::SLASH;   // prefixStart keep NO
                break;
            default:
                r += c;
            }
            break;
        case CppState::SLASH: // prefixStart NO
            processPostSlash(c, CppBase::INSIDE);
            break;
        case CppState::HEX:    // prefixStart NO
            processCode(c, 16, CppState::INSIDE);
            break;
        case CppState::OCT:    // prefixStart NO
            processCode(c, 8, CppState::INSIDE);
            break;
        case CppState::SUFFIX:
            switch (c) {
            case U' ':                          // "x"ab␣
            case U'\n':
            CASE_COMMA_CHARS:                   // "x"ab) etc,  do the same
                state = CppState::SPACE_TRAIL;     // prefixStart NO→YES
                prefixStart = p;
                break;
            default:
                if (dcpp::isAlnum(c)) {         // "x"ab8
                    //  do nothing
                } else {                        // "x"ab+
                    state = CppState::OUTSIDE;     // prefixStart keep NO
                    r += c;
                }
            }
            break;
        case CppState::RAW_COLLECT:     // prefixStart NO
            switch (c) {
            case U'(':
            case U')':  // just to ensure that no ) in rawEnd
                    // Non-ID chars etc cannot be in delimiter, but we
                    //   cannot throw errors, just recover somehow —
                    //   and this is the simplest way
                rawEnd += U'"';
                state = CppState::RAW_INSIDE;
                prefixStart = p + 1;
                rawIndex = 0;
                break;
            default:
                rawEnd += c;
            }
            break;
        case CppState::RAW_INSIDE:      // prefixStart YES
            if (c == rawEnd[rawIndex]) {
                if (++rawIndex == rawEnd.length()) {
                    // Special dump
                    ptrdiff_t tailLen = rawEnd.length() - 1;
                    auto dist = std::distance(prefixStart, p);
                    if (dist > tailLen) {   // over-assurance, should be OK
                        r.append(prefixStart, p - tailLen);
                    }
                    prefixStart = nullptr;      // prefixStart YES→NO
                    state = CppState::SUFFIX;
                }
            }
        }   // Big switch (state)
    }
    if (prefixStart && state != CppState::SPACE_TRAIL) {
        // RAW_INSIDE here too
        r.append(prefixStart, end);
    }
    if (nCodeCharsRemaining > 0)
        appendCode(r, charCode);
    return r;
}


std::wstring decode::htmlBr(std::wstring_view x)
{
    std::wstring cache;
    x = normalizeEolSv(x, cache);

    if (x.empty())
        return std::wstring{x};

    std::wregex rex(LR"(<br[ ]*[/]?>(?!\n))");
    std::wstring r;
    std::regex_replace(
            std::back_inserter(r),
            x.begin(), x.end(),
            rex,
            L"<br>\n");
    return r;
}


std::u8string_view escape::cppSv(
        std::u8string_view x,
        std::u8string& cache,
        char8_t lf,
        escape::Spaces escapeSpaces,
        Enquote enquote)
{
    switch (x.length()) {
    case 0:
        return static_cast<bool>(enquote)
            ? std::u8string_view { u8"\"\"" }
            : std::u8string_view {};
    case 1:
        // Special bhv on " " → now leading space is not the same as trailing
        if (x[0] == ' ') {
            if (static_cast<bool>(escapeSpaces)) {
                return static_cast<bool>(enquote)
                    ? std::u8string_view { u8R"("\s")" }
                    : std::u8string_view { u8R"(\s)" };
            } else {
                return static_cast<bool>(enquote)
                    ? std::u8string_view { u8R"(" ")" }
                    : std::u8string_view { u8" " };
            }
        }
        break;
    default: ;
    }

    char data[5] { '\n', '\\' };
    char* dend = data + 2;
    if (static_cast<bool>(enquote))
        *(dend++) = '"';
    *dend = 0;

    bool hasLeadingSpace = false;
    bool hasTrailingSpace = false;
    if (static_cast<bool>(escapeSpaces)) {
        hasLeadingSpace = x.starts_with(' ');
        hasTrailingSpace = x.ends_with(' ');
    }
    bool hasSpace = hasLeadingSpace || hasTrailingSpace;

    auto n = std::count_if(x.begin(), x.end(),
                [&data](auto c){ return strchr(data, c); });
    // Simple check
    if (n == 0 && !static_cast<bool>(enquote) && !hasSpace) {
        return x;
    }

    int myOffset = hasLeadingSpace;
    int myLength = x.length() - myOffset - static_cast<int>(hasTrailingSpace);
    auto xx = x.substr(myOffset, myLength);

    size_t newSize = x.size() + n + 16;     // enough for
    if (cache.size() < static_cast<size_t>(newSize))
        cache.resize(newSize);
    auto p = cache.data();
    if (static_cast<bool>(enquote))
        *(p++) = '"';
    if (hasLeadingSpace) {
        *(p++) = '\\';
        *(p++) = 's';
    }
    for (auto c : xx) {
        switch (c) {
        case '\n':
            *(p++) = '\\';
            *(p++) = lf;
            break;
        case '\\':
            *(p++) = '\\';
            *(p++) = '\\';
            break;
        case '"':
            if (static_cast<bool>(enquote)) {
                *(p++) = '\\';
                *(p++) = '"';
                break;
            }
            [[fallthrough]];
        default:
            *(p++) = c;
        }
    }
    if (hasTrailingSpace) {
        *(p++) = '\\';
        *(p++) = 's';
    }
    if (static_cast<bool>(enquote))
        *(p++) = '"';
    return { cache.data(), p };
}
