#include "u_SearchEngine.h"

#include "u_Strings.h"

const srh::Prio srh::Prio::EMPTY;

namespace {

    /// If we find those words → treat as low-priority
    /// @warning Alphabetical order, upper case
    constinit std::u8string_view DIC_WORDS[] {
        u8"LETTER",
        u8"SIGN"
    };

}


srh::Class srh::classify(char8_t x)
{
    if ((x >= 'A' && x <= 'Z') || (x >= 'a' && (x <= 'z')))
        return Class::LETTER;
    if (x >= '0' && x <= '9')
        return Class::DIGIT;
    return Class::OTHER;
}


namespace {

    inline bool charsCiEq(char8_t c1, char8_t c2)
    {
        return (c1 == c2
            || std::toupper(c1) == std::toupper(c2));
    }

}   // anon namespace

bool srh::stringsCiEq(std::u8string_view s1, std::u8string_view s2)
{
    return ( s1.size() == s2.size()
        && std::equal(s1.begin(), s1.end(), s2.begin(), &charsCiEq) );
}



///// Word /////////////////////////////////////////////////////////////////////


srh::Word::Word(std::u8string x)
    : v(std::move(x)),
      ccFirst(classify(v.front())),
      ccLast(classify(v.back()))
{
    auto beg = std::begin(DIC_WORDS);
    auto end = std::end(DIC_WORDS);
    auto it = std::upper_bound(beg, end, v);
    if (it != beg) {
        --it;
        if (it->starts_with(v)) {
            dicWord = *it;
            doesCoincide = (dicWord == v);
        }
    }
}


///// Needle ///////////////////////////////////////////////////////////////////


srh::Needle::Needle(std::u8string_view x)
{
    auto w1 = str::splitSv(x, ' ');
    words.reserve(w1.size());
    for (auto v : w1) {
        if (!v.empty())
            words.emplace_back(str::toUpper(v));
    }
}


namespace {

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

    bool isOther(std::u8string_view s, size_t pos, srh::Class c)
    {
        if (pos >= s.size())
            return true;
        auto clazz = srh::classify(s[pos]);
        return (clazz == srh::Class::OTHER || clazz != c);
    }

}   // anon namespace

srh::Place srh::findWord(std::u8string_view haystack, const srh::Word& needle,
                         bool isNonScript)
{
    Place r = Place::NONE;
    size_t pos = 0;
    while (true) {
        auto where = ciFind(haystack, needle.sv(), pos);
        if (where == std::u8string_view::npos)
            break;
        Place r1 =
            isOther(haystack, where - 1, needle.ccFirst)
                ? (isOther(haystack, where + needle.length(), needle.ccLast)
                        ? (needle.doesCoincide && !isNonScript
                           // No need to find again if we found exactly a dictionary word
                           ? Place::EXACT_SCRIPT
                           : Place::EXACT)
                        : Place::INITIAL)
                : Place::PARTIAL;
        // Check for dictionary word
        switch (r1) {
        case Place::EXACT:
        case Place::EXACT_SCRIPT:
            return r1;
        case Place::INITIAL:
            if (!needle.dicWord.empty() && !isNonScript) {
                auto q = haystack.substr(where, needle.dicWord.length());
                if (stringsCiEq(q, needle.v))
                    r1 = Place::INITIAL_SRIPT;
            }
            [[fallthrough]];
        case Place::INITIAL_SRIPT:
        case Place::PARTIAL:
            r = std::max(r, r1);
            [[fallthrough]];
        case Place::NONE: ;
        }
        pos = where + 1;
    }
    return r;
}

srh::Prio srh::findNeedle(std::u8string_view haystack, const Needle& needle,
                          bool isNonScript)
{
    srh::Prio r;
    for (auto& v : needle.words) {
        auto type = findWord(haystack, v, isNonScript);
        switch (type) {
        case Place::EXACT: ++r.exact; break;
        case Place::EXACT_SCRIPT: ++r.exactScript; break;
        case Place::INITIAL: ++r.initial; break;
        case Place::INITIAL_SRIPT: ++r.initialScript; break;
        case Place::PARTIAL: ++r.partial; break;
        case Place::NONE: ;
        }
    }
    return r;
}
