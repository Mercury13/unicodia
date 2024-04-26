#include "u_SearchEngine.h"

#include "u_Strings.h"
#include "u_TypedFlags.h"

const srh::Prio srh::Prio::EMPTY;

namespace {

    enum class AlwaysLowPrio : unsigned char { NO, YES };

    struct DicWord {
        std::u8string_view word;
        Flags<srh::HaystackClass> lowPrioClass;
    };

    inline bool operator < (std::u8string_view x, const DicWord& y)
        { return (x < y.word); }

    /// If we find those words → treat as low-priority
    /// @warning Alphabetical order, upper case
    constinit DicWord DIC_WORDS[] {
        { u8"LETTER", srh::HaystackClass::SCRIPT },
        { u8"SIGN", srh::hc::EVERYWHERE },
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



///// NeedleWord ///////////////////////////////////////////////////////////////


srh::NeedleWord::NeedleWord(std::u8string x)
    : v(std::move(x)),
      ccFirst(classify(v.front())),
      ccLast(classify(v.back()))
{
    auto beg = std::begin(DIC_WORDS);
    auto end = std::end(DIC_WORDS);
    auto it = std::upper_bound(beg, end, v);
    if (it != beg) {
        --it;
        if (it->word.starts_with(v)) {
            dicWord = it->word;
            isDicWord = (dicWord == v);
            lowPrioClass = it->lowPrioClass;
        }
    }
}


///// HayWord //////////////////////////////////////////////////////////////////


srh::HayWord::HayWord(std::u8string_view x)
    : v(x)
{
    auto beg = std::begin(DIC_WORDS);
    auto end = std::end(DIC_WORDS);
    auto it = std::upper_bound(beg, end, v);
    if (it != beg) {
        --it;
        if (it->word == v) {
            lowPrioClass = it->lowPrioClass;
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

    struct FindPlace {
        const srh::HayWord* word = nullptr;
        size_t iWord = std::numeric_limits<size_t>::max();
        size_t iInner = std::numeric_limits<size_t>::max();

        explicit operator bool() const { return word; }
        size_t length() const { return word ? word->length() : 0; }
    };

    template<typename T>
    FindPlace ciFind(std::span<const srh::HayWord> haystack, const T& needle, size_t iStartWord)
    {
        for (auto itHay = haystack.begin() + iStartWord;
             itHay != haystack.end();
             ++itHay) {
            auto hayword = itHay->sv();
            typename T::const_iterator it = std::search(
                hayword.begin(), hayword.end(),
                needle.begin(), needle.end(), myEqual<typename T::value_type> );
            if ( it != hayword.end() ) {
                return { .word = std::to_address(itHay),
                         .iWord = size_t(itHay - haystack.begin()),
                         .iInner = size_t(it - hayword.begin()) };
            }
        }
        return {}; // not found
    }

}   // anon namespace

srh::Place srh::findWord(std::span<HayWord> haystack, const NeedleWord& needle,
                         HaystackClass hclass)
{
    bool isNeedleLowPrio = needle.lowPrioClass.have(hclass);
    Place r = Place::NONE;
    size_t pos = 0;
    while (true) {
        auto where = ciFind(haystack, needle.sv(), pos);
        if (!where)
            break;
        Place r1 = Place::PARTIAL;
        if (where.iInner == 0) {
            if (where.length() == needle.length()) {
                // Full word match
                r1 = isNeedleLowPrio ? Place::EXACT_SCRIPT : Place::EXACT;
            } else {
                // Initial match
                r1 = where.word->lowPrioClass.have(hclass)
                     ?  Place::INITIAL_SRIPT : Place::INITIAL;
            }
        }
        // Check for dictionary word
        switch (r1) {
        case Place::EXACT:
        case Place::EXACT_SCRIPT:
            return r1;
        case Place::INITIAL:
        case Place::INITIAL_SRIPT:
        case Place::PARTIAL:
            r = std::max(r, r1);
            [[fallthrough]];
        case Place::NONE: ;
        }
        pos = where.iWord + 1;
    }
    return r;
}

srh::Prio srh::findNeedle(std::span<HayWord> haystack, const Needle& needle,
                          HaystackClass hclass)
{
    srh::Prio r;
    for (auto& v : needle.words) {
        auto type = findWord(haystack, v, hclass);
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


srh::Prio srh::findNeedle(std::u8string_view haystack, const Needle& needle,
                          HaystackClass hclass)
{
    auto w1 = str::splitSv(haystack, ' ');
    SafeVector<HayWord> words;
    words.reserve(w1.size());
    for (auto v : w1) {
        if (!v.empty())
            words.emplace_back(v);
    }
    return findNeedle(words, needle, hclass);
}
