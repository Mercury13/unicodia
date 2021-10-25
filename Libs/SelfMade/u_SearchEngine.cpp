#include "u_SearchEngine.h"

#include "u_Strings.h"

const srh::Prio srh::Prio::EMPTY;


srh::Class srh::classify(char8_t x)
{
    if ((x >= 'A' && x <= 'Z') || (x >= 'a' && (x <= 'z')))
        return Class::LETTER;
    if (x >= '0' && x <= '9')
        return Class::DIGIT;
    return Class::OTHER;
}


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

srh::Place srh::findWord(std::u8string_view haystack, const srh::Word& needle)
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
                        ? Place::EXACT : Place::INITIAL)
                : Place::PARTIAL;
        if (r1 == Place::EXACT)
            return r1;
        r = std::max(r, r1);
        pos = where + 1;
    }
    return r;
}

srh::Prio srh::findNeedle(std::u8string_view haystack, const Needle& needle)
{
    srh::Prio r;
    for (auto& v : needle.words) {
        auto type = findWord(haystack, v);
        switch (type) {
        case Place::EXACT: ++r.exact; break;
        case Place::INITIAL: ++r.initial; break;
        case Place::PARTIAL: ++r.partial; break;
        case Place::NONE: ;
        }
    }
    return r;
}
