// My header
#include "u_Decoders.h"

// C++
#include <regex>

using namespace std::string_view_literals;

constexpr char32_t PARA_SEP_32 = 0x2029;      // U+2029 paragraph separator
constexpr wchar_t PARA_SEP_16 = PARA_SEP_32;


///// escape::Text /////////////////////////////////////////////////////////////


std::u8string escape::Text::bannedSubstring() const
{
    switch (lineBreak) {
    case LineBreakMode::BANNED:
        return u8"\n";
    case LineBreakMode::SPECIFIED_TEXT:
        return lineBreakText;
    case LineBreakMode::C_CR:
    case LineBreakMode::C_LF:
        return {};
    }
    throw std::logic_error("[TextEscape.bannedSubstring] Strange mode");
}


void escape::Text::writeQuoted(std::ostream& os, std::u8string_view x)
{
#define S_QUOTE "\""
    constexpr auto C_QUOTE = '"';
    os << C_QUOTE;
    if (x.find(C_QUOTE) != std::u8string_view::npos) { // Char-by-char
        for (auto v : x) {
            if (v == C_QUOTE) {
                os << S_QUOTE S_QUOTE;
            } else {
                os << static_cast<char>(v);
            }
        }
    } else { // Quick
        os << str::toSv(x);
    }
    os << C_QUOTE;
#undef S_QUOTE
}


void escape::Text::writeSimpleString(std::ostream& os, std::u8string_view x) const
{
    if (space == SpaceMode::QUOTED) {
        writeQuoted(os, x);
    } else {
        os << str::toSv(x);
    }
}


void escape::Text::write(
        std::ostream& os, std::u8string_view x, std::u8string& cache) const
{
    switch (lineBreak) {
    case LineBreakMode::BANNED:
        writeSimpleString(os, x);
        break;
    case LineBreakMode::SPECIFIED_TEXT: {
            auto s = str::replaceSv(x, u8"\n", lineBreakText, cache);
            writeSimpleString(os, s);
        } break;
    case LineBreakMode::C_CR:
    case LineBreakMode::C_LF: {
            auto ch = (lineBreak == LineBreakMode::C_CR) ? 'r' : 'n';
            auto s = escape::cppSv(x, cache, ch,
                ecIf<escape::Spaces>(space == SpaceMode::SLASH_SPACE),
                ecIf<Enquote>(space == SpaceMode::QUOTED));
            os << str::toSv(s);
        } break;
    }
    os << str::toSv(activeSpaceDelimiter());
}


std::u8string escape::Text::unescapeMaybeQuoted(std::u8string_view text) const
{
    if (space == SpaceMode::QUOTED) {
        return decode::quoted(text);
    } else {
        return std::u8string{text};
    }
}


std::u8string escape::Text::unescape(std::u8string_view text) const
{
    // Delimited mode: ends → remove
    if (space == SpaceMode::DELIMITED && text.ends_with(spaceDelimiter))
        text = text.substr(text.length() - spaceDelimiter.length());
    switch (lineBreak) {
    case LineBreakMode::SPECIFIED_TEXT:
        if (!lineBreakText.empty()) {
            std::u8string tmp {text};
            str::replace(tmp, lineBreakText, u8"\n");
            return unescapeMaybeQuoted(tmp);
        }
        [[fallthrough]];
    case LineBreakMode::BANNED:
        return unescapeMaybeQuoted(text);
    case LineBreakMode::C_CR:
    case LineBreakMode::C_LF: { // We recognize both \r and \n
            // Only two modes remain here: bare/quoted
            // \s will be recognized anyway
            auto mq = ecIf<decode::MaybeQuoted>(space == SpaceMode::QUOTED);
            return decode::cppLite(text, mq);
        }
    }
    return {};
}


void escape::Text::setLineBreakText(std::u8string_view x)
{
    if (x.empty()) {
        lineBreak = LineBreakMode::BANNED;
    } else {
        lineBreak = LineBreakMode::SPECIFIED_TEXT;
        lineBreakText = x;
    }
}


std::u8string_view escape::Text::visibleLineBreakText() const noexcept
{
    return (lineBreak == escape::LineBreakMode::SPECIFIED_TEXT)
            ? lineBreakText
            : DEFAULT_LINE_BREAK_TEXT;
}


void escape::Text::setSpaceDelimiter(std::u8string_view x)
{
    if (x.empty()) {
        space = SpaceMode::BARE;
    } else {
        space = SpaceMode::DELIMITED;
        spaceDelimiter = x;
    }
}


std::u8string_view escape::Text::visibleSpaceDelimiter() const noexcept
{
    return (space == escape::SpaceMode::DELIMITED)
            ? spaceDelimiter
            : DEFAULT_SPACE_DELIMITER;
}


std::u8string_view escape::Text::activeSpaceDelimiter() const noexcept
{
    return (space == escape::SpaceMode::DELIMITED)
            ? spaceDelimiter
            : std::u8string_view{};
}


///// Misc functions ///////////////////////////////////////////////////////////

namespace {

    constexpr auto I_EOF = std::istream::traits_type::eof();

    void myPutBack(std::istream& is, std::istream::traits_type::int_type x)
    {
        if (x != I_EOF)
            is.putback(x);
    }

    decode::BomType detectBomEnd(
            std::istream& is, unsigned char cRead, unsigned char cNext,
            decode::BomType btWanted, std::istream::iostate state)
    {
        auto c2 = is.get();
        if (c2 != cNext) {
            is.clear(state);
            myPutBack(is, c2);
            is.putback(cRead);
            return decode::BomType::NONE;
        }
        return btWanted;
    }

}

decode::BomType decode::detectBom(std::istream& is)
{
    auto state = is.rdstate();
    auto c1 = is.get();
    if (!is)
        return BomType::NONE;

    switch (c1) {
    case 0xEF: {  // UTF-8
            // Char 2
            auto c2 = is.get();
            if (c2 != 0xBB) {
                is.clear(state);
                myPutBack(is, c2);
                is.putback(c1);
                return BomType::NONE;
            }
            // Char 3
            int c3 = is.get();
            if (c3 != 0xBF) {
                is.clear(state);
                myPutBack(is, c3);
                is.putback(c2);
                is.putback(c1);
                return BomType::NONE;
            }
            return BomType::UTF8;
        }
    case 0xFF:
        return detectBomEnd(is, 0xFF, 0xFE, BomType::UTF16LE, state);
    case 0xFE:
        return detectBomEnd(is, 0xFE, 0xFF, BomType::UTF16BE, state);
    default:    // also I_EOF
        is.clear(state);
        myPutBack(is, c1);
        return BomType::NONE;
    }
}


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
            : std::u8string_view{};
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


void decode::ini(std::istream& is, IniCallback& cb)
{
    auto bomType = detectBom(is);
    switch (bomType) {
    case BomType::UTF16BE:
    case BomType::UTF16LE:
        throw std::logic_error("UTF-16 INI files are not supported!");
    case BomType::NONE:     // These two are supported
    case BomType::UTF8:;
    }
    std::string line;
    while (std::getline(is, line)) {
        std::u8string_view s = str::toU8sv(str::trimLeftSv(line));
        if (s.empty()) {
            cb.onEmptyLine();
            continue;
        }
        if (s.starts_with(';') || s.starts_with('#')) {
            s = str::trimSv(s.substr(1));
            cb.onComment(s);
            continue;
        }
        if (s.starts_with('[')) {   // “[ group ] ”
            // GROUP
            s = s.substr(1);        // “ group ] ”
            auto s1 = str::trimRightSv(s);  // “ group ]”
            if (s1.ends_with(']'))
                s = s1.substr(0, s1.length() - 1);  // “ group ”
            s = str::trimSv(s);     // Let it be: IDs are trimmed, values are not
            cb.onGroup(s);
        } else {
            // VARIABLE
            auto pEq = s.find('=');
            if (pEq != 0 && pEq != std::string_view::npos) {
                auto name = s.substr(0, pEq);  // starts with non-blank!
                auto value = s.substr(pEq + 1);
                // Check for “name = value”, rather common
                if (name.back() == ' '
                        && !value.empty()
                        && str::isBlank(value.front())) {
                    value = value.substr(1);
                }
                name = str::trimRightSv(name);
                cb.onVar(name, value);
            } else {
                cb.onEmptyLine();
            }
        }
    }
}


std::u8string decode::cppLite(std::u8string_view x, MaybeQuoted maybeQuoted)
{
    bool hasRightQuote = false;
    if (maybeQuoted != MaybeQuoted::NO) {
        auto x1 = str::trimLeftSv(x);
        if (x1.starts_with('"')) {
            x = x.substr(1);
            auto x2 = str::trimRightSv(x);
            if (x2.ends_with('"')) {
                x = x2.substr(0, x2.length() - 1);
                hasRightQuote = true;
            }
        }
    }
    // Totally unescaped?
    if (x.find('\\') == std::u8string_view::npos) {
        return std::u8string{x};
    }
    std::u8string r;
    auto p = std::to_address(x.begin());
    auto end = std::to_address(x.end());
    while (p != end) {
        auto c = *(p++);
        if (c == '\\') {
            if (p == end) {
                if (hasRightQuote)
                    r += '"';
            } else {
                c = *(p++);
                switch (c) {
                case 'n':
                case 'r':
                    r += '\n';
                    break;
                case 's':
                    r += ' ';
                    break;
                case '\\':
                    r += '\\';
                    break;
                default:
                    r += c;
                }
            }
        } else {
            r += c;
        }
    }
    return r;
}


std::u8string decode::quoted(std::u8string_view x)
{
    // Does not start with quote → return param
    auto x1 = str::trimLeftSv(x);
    if (!x1.starts_with('"')) {
        return std::u8string(x);
    }
    // Now have starting quote
    x = x1.substr(1);

    // Find ending quote; cut it if found
    auto x2 = str::trimRightSv(x);
    if (x2.ends_with('"'))
        x = x2.substr(0, x2.length() - 1);

    std::u8string r;
    r.reserve(x.length());
    auto p = std::to_address(x.begin());
    auto end = std::to_address(x.end());
    while (p != end) {
        auto c = *(p++);
        if (c == '"') {
            // Skip one more quote
            if (p != end && *p == '"')
                ++p;
            r += '"';
        } else {
            r += c;
        }
    }
    r.shrink_to_fit();
    return r;
}
