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
    /// Dislike such constexpr, but it’s within one TU, and need for static_assert
    inline constexpr const DicWord DIC_WORDS[] {
        { u8"IDEOGRAM", srh::hc::EVERYWHERE },
        { u8"IDEOGRAPH", srh::hc::EVERYWHERE },
        { u8"LETTER", srh::HaystackClass::SCRIPT },
        { u8"LETTERFORM", srh::hc::EVERYWHERE },
        { u8"MAN", srh::HaystackClass::EMOJI },
        { u8"OF", srh::hc::EVERYWHERE },  // No letter Of at all
        { u8"PATTERN", srh::HaystackClass::SCRIPT },
        { u8"PERSON", srh::HaystackClass::EMOJI },
        { u8"SIGN", srh::hc::EVERYWHERE },
        { u8"SYLLABIC", srh::hc::EVERYWHERE },
        { u8"SYLLABICS", srh::hc::EVERYWHERE },
        { u8"SYLLABLE", srh::hc::EVERYWHERE },
        { u8"SYMBOL", srh::hc::EVERYWHERE },
        { u8"WOMAN", srh::HaystackClass::EMOJI },
    };

    constexpr auto ALL_SEPARATORS = u8" ,():";

    template <size_t N>
    consteval bool isAsc(const DicWord (&x)[N])
    {
        for (size_t i = 1; i < std::size(x); ++i) {
            if (x[i - 1].word >= x[i].word)
                return false;
        }
        return true;
    }

    static_assert(isAsc(DIC_WORDS), "Words should be ascending");

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
    auto w1 = str::splitByAnySv(x, ALL_SEPARATORS);
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

    enum class FindStatus : unsigned char { COMPLETE, INITIAL, OTHER };

    struct FindPlace {
        const srh::HayWord* word = nullptr;
        size_t iWord = std::numeric_limits<size_t>::max();
        FindStatus status = FindStatus::OTHER;

        explicit operator bool() const { return word; }
        size_t length() const { return word ? word->length() : 0; }
    };

    template<typename T>
    FindPlace myFind(std::span<const srh::HayWord> haystack, const T& needle, size_t iStartWord)
    {
        for (auto itHay = haystack.begin() + iStartWord;
             itHay != haystack.end();
             ++itHay) {
            auto hayword = itHay->sv();
            auto pos = hayword.find(needle);
            if ( pos != std::u8string_view::npos ) {
                auto status = FindStatus::OTHER;
                if (pos == 0) {
                    if (hayword.length() == needle.length()) {
                        status = FindStatus::COMPLETE;
                    } else {
                        status = FindStatus::INITIAL;
                    }
                } else {
                    if (srh::classify(hayword.at(pos - 1)) == srh::Class::OTHER)
                        status = FindStatus::INITIAL;
                }
                return { .word = std::to_address(itHay),
                         .iWord = size_t(itHay - haystack.begin()),
                         .status = status };
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
        auto where = myFind(haystack, needle.sv(), pos);
        if (!where)
            break;
        Place r1 = Place::PARTIAL;
        switch (where.status) {
        case FindStatus::COMPLETE:
            r1 = isNeedleLowPrio ? Place::EXACT_SCRIPT : Place::EXACT;
            break;
        case FindStatus::INITIAL:
            r1 = where.word->lowPrioClass.have(hclass)
                 ?  Place::INITIAL_SRIPT : Place::INITIAL;
            break;
        case FindStatus::OTHER:;
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
                          HaystackClass hclass, Cache& cache)
{
    // Uppercase haystack
    cache.haystack = haystack;
    str::toUpperInPlace(cache.haystack);
    // Words — simple
    cache.words1.clear();
    str::splitByAnySvTo(cache.haystack, ALL_SEPARATORS, cache.words1);
    // Words — bigger
    cache.words2.clear();
    cache.words2.reserve(cache.words1.size());
    for (auto v : cache.words1) {
        if (!v.empty())
            cache.words2.emplace_back(v);
    }
    return findNeedle(cache.words2, needle, hclass);
}
