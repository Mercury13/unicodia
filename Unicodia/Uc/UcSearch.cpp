// My header
#include "UcSearch.h"

// STL
#include <unordered_set>

// Libs
#include "u_Strings.h"

// Unicode
#include "UcData.h"

// Other
#include "u_SearchEngine.h"


const uc::SearchPrio uc::SearchPrio::EMPTY;

std::u8string_view uc::errorStrings[uc::SingleError_N] {
    {},     // one
    {},     // multiple
    {},     // no search
    u8"Не найдено",
    u8"Код символа не распознан",
    u8"Код символа слишком велик",
    u8"Выброшенная позиция",
    u8"Символ для личного пользования",
    u8"Код из суррогатной пары",
    u8"Нераспределённый символ",
    u8"Свободное место",
};


uc::SingleSearchResult uc::findCode(char32_t code)
{
    // Too big?
    if (code >= uc::N_CHARS)
        return { SingleError::TOO_BIG };

    // Have that character?
    auto pCp = uc::cps[code];
    if (pCp)
        return { SingleError::ONE, pCp };

    // If not → find what the heck
    if (isNonChar(code))
        return { SingleError::NONCHARACTER };
    if ((code >= 0xE000 && code <= 0xF8FF) || code >= 0xF'0000)
        return { SingleError::PRIVATE_USE };
    if (code >= 0xD800 && code <= 0xDFFF)
        return { SingleError::SURROGATE };
    auto v = uc::blockOf(code, 0);
    if (code > v->endingCp)
        return { SingleError::UNALLOCATED };

    return { SingleError::RESERVED, v->firstAllocated  };
}


uc::SingleSearchResult uc::findStrCode(QStringView what, int base)
{
    uint code = 0;
    bool isOk = false;
    if (code = what.toUInt(&isOk, base); isOk) {
        return findCode(code);
    } else {
        return { SingleError::CONVERT_ERROR };
    }
}


bool uc::isNameChar(char32_t cp)
{
    return (cp >= 'A' && cp <= 'Z')
        || (cp >= 'a' && cp <= 'z')
        || (cp >= '0' && cp <= '9')
        || (cp == '-')
        || (cp == ' ');
}


bool uc::isNameChar(QStringView x)
{
    for (auto v : x) {
        if (!isNameChar(v.unicode()))
            return false;
    }
    return true;
}


bool uc::isMnemoChar(char32_t cp)
{
    return (cp >= 'A' && cp <= 'Z')
        || (cp >= 'a' && cp <= 'z')
        || (cp >= '0' && cp <= '9');
}


bool uc::isMnemoChar(QStringView x)
{
    for (auto v : x) {
        if (!isMnemoChar(v.unicode()))
            return false;
    }
    return true;
}


namespace {

    inline std::u8string_view toU8(std::string& x)
    {
        return { reinterpret_cast<const char8_t*>(x.data()), x.length() };
    }

    inline bool charsCIeq(char8_t c1, char8_t c2)
    {
        return (c1 == c2
            || std::toupper(c1) == std::toupper(c2));
    }

    bool stringsCIeq(std::u8string_view s1, std::u8string_view s2)
    {
        return ( s1.size() == s2.size()
            && std::equal(s1.begin(), s1.end(), s2.begin(), &charsCIeq) );
    }

}   // anon namespace


std::u8string uc::toMnemo(QString x)
{
    // Check for &
    if (!x.startsWith('&'))
        return {};
    // Virtually chop &
    auto sv = QStringView(x).sliced(1);
    // Virtually chop ;
    if (sv.endsWith(';'))
        sv.chop(1);
    // Result is bad/empty?
    if (sv.empty() || !isMnemoChar(sv))
        return {};
    // Buils string: & + sv + ;
    std::u8string s;
    s.reserve(sv.length() + 2);
    s += '&';
    for (auto v : sv)
        s += v.unicode();
    s += ';';
    return s;
}


namespace {

    struct CompiledNeedle
    {
        SafeVector<srh::Word> words;

        CompiledNeedle(std::u8string_view x);
    };

    CompiledNeedle::CompiledNeedle(std::u8string_view x)
    {
        auto w1 = str::splitSv(x, ' ');
        words.reserve(w1.size());
        for (auto v : w1) {
            if (!v.empty())
                words.emplace_back(str::toUpper(v));
        }
    }

    template<typename charT>
    bool myEqual(charT ch1, charT ch2) {
        return std::toupper(ch1) == std::toupper(ch2);
    }

    template<typename T>
    size_t ciFind( const T& haystack, const T& needle, size_t pos)
    {
        typename T::const_iterator it = std::search( haystack.begin() + pos, haystack.end(),
            needle.begin(), needle.end(), myEqual<typename T::value_type> );
        if ( it != haystack.end() ) return it - haystack.begin();
        else return std::string::npos; // not found
    }

    enum class ResultType { NONE, PARTIAL, INITIAL, EXACT };

    bool isOther(std::u8string_view s, size_t pos, srh::Class c)
    {
        if (pos >= s.size())
            return true;
        auto clazz = srh::classify(s[pos]);
        return (clazz == srh::Class::OTHER || clazz != c);
    }

    ResultType myFind1(std::u8string_view haystack, const srh::Word& needle)
    {
        ResultType r = ResultType::NONE;
        size_t pos = 0;
        while (true) {
            auto where = ciFind(haystack, needle.sv(), pos);
            if (where == std::u8string_view::npos)
                break;
            ResultType r1 =
                isOther(haystack, where - 1, needle.ccFirst)
                    ? (isOther(haystack, where + needle.length(), needle.ccLast)
                            ? ResultType::EXACT : ResultType::INITIAL)
                    : ResultType::PARTIAL;
            if (r1 == ResultType::EXACT)
                return r1;
            r = std::max(r, r1);
            pos = where + 1;
        }
        return r;
    }

    uc::SearchPrio myFind(
            std::u8string_view haystack, CompiledNeedle needle)
    {
        uc::SearchPrio r;
        for (auto& v : needle.words) {
            auto type = myFind1(haystack, v);
            switch (type) {
            case ResultType::EXACT: ++r.exact; break;
            case ResultType::INITIAL: ++r.initial; break;
            case ResultType::PARTIAL: ++r.partial; break;
            case ResultType::NONE: ;
            }
        }
        return r;
    }

}   // anon namespace


uc::SearchResult uc::doSearch(QString what)
{
    if (what.isEmpty())
        return {{ SingleError::NO_SEARCH }};

    // Find a single character, maybe space
    if (what.size() <= 2) {
        auto u32 = what.toUcs4();
        if (u32.size() == 1) {
            auto code = u32[0];
            if (code == ' ' || !isNameChar(code)) {
                return uc::findCode(code);
            }
        }
    }

    what = what.trimmed();

    // Searching for nothing?
    if (what.isEmpty())
        return {{ SingleError::NO_SEARCH }};

    if (what.startsWith("U+", Qt::CaseInsensitive)) {
        // U+:
        auto sHex = QStringView(what).mid(2);
        return uc::findStrCode(sHex, 16);
    }

    SafeVector<uc::SearchLine> r;

    if (auto mnemo = toMnemo(what); !mnemo.empty()) {
        // SEARCH BY HTML MNEMONIC
        // Search
        for (auto& cp : uc::cpInfo) {
            auto names = cp.allRawNames();
            for (auto& nm : names) {
                if (stringsCIeq(mnemo, nm)) {
                    r.emplace_back(&cp);
                    break;
                }
            }
        }
        if (r.size() == 1)
            return {{ SingleError::ONE, r[0].cp }};
    } else if (isNameChar(what)) {
        // Try find hex
        const uc::Cp* hex = nullptr;
        if (what.size() >= 2) {
            if (auto q = uc::findStrCode(what, 16); q.err == SingleError::ONE) {
                hex = q.one;
                auto& bk = r.emplace_back(q.one);
                bk.prio.high = uc::HIPRIO_HEX;
            }
        }

        // SEARCH BY KEYWORD/mnemonic
        auto u8Name = what.toStdString();
        auto sv = toU8(u8Name);
        CompiledNeedle needle(sv);
        for (auto& cp : uc::cpInfo) {
            if (&cp != hex) {   // Do not check hex once again
                auto names = cp.allRawNames();
                uc::SearchPrio prio;
                for (auto& nm : names) {
                    if (nm.starts_with('&')) {
                        // Search by HTML mnemonic
                        if (nm.size() == sv.size() + 2) {
                            auto mnemo = nm.substr(1, sv.size());
                            if (sv == mnemo) {
                                auto& bk = r.emplace_back(&cp);
                                bk.prio.high = HIPRIO_MNEMONIC_EXACT;
                                goto brk;
                            } else if (stringsCIeq(sv, mnemo)) {
                                auto& bk = r.emplace_back(&cp);
                                bk.prio.high = HIPRIO_MNEMONIC_CASE;
                                goto brk;
                            }
                        }
                    } if (nm.find('#') == std::u8string_view::npos) {
                        // Search by keyword
                        if (auto pr = myFind(nm, needle); pr > prio) {
                            prio = pr;
                        }
                    }
                }
                if (prio > SearchPrio::EMPTY) {
                    r.emplace_back(&cp, prio);
                }
            brk:;
            }
        }

        if (r.size() == 1 && r[0].prio.high >= HIPRIO_FIRST_ONE)
            return {{ SingleError::ONE, r[0].cp }};

        // Sort by relevance
        std::stable_sort(r.begin(), r.end());
    } else {
        // DEBRIEF STRING
        auto u32 = what.toUcs4();
        for (auto v : u32) {
            auto find = uc::findCode(v);
            if (find.one)
                r.emplace_back(find.one);
        }
    }

    if (r.empty()) {
        return {{ SingleError::NOT_FOUND }};
    } else {
        return r;
    }
}
