#include "engine.h"

#include "u_Strings.h"
#include "u_TypedFlags.h"

const srh::Prio srh::Prio::EMPTY;
const srh::DefaultComparator srh::DefaultComparator::INST;

srh::FindStatus srh::DefaultComparator::find(
        std::u8string_view haystack, std::u8string_view needle) const
{
    auto pos = haystack.find(needle);
    if ( pos == std::u8string_view::npos)
        return srh::FindStatus::NONE;
    if (pos == 0) {
        if (haystack.length() == needle.length()) {
            return srh::FindStatus::COMPLETE;
        } else {
            return srh::FindStatus::INITIAL;
        }
    } else {
        if (srh::classify(haystack.at(pos - 1)) == srh::Class::OTHER) {
            return srh::FindStatus::INITIAL;
        } else {
            return srh::FindStatus::SUBSTR;
        }
    }
}


void srh::DefaultComparator::prepareHaystack(
        std::u8string_view haystack, std::u8string& result) const
{
    result = haystack;
    str::toUpperInPlace(result);
}

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
        { u8"AT", srh::HaystackClass::EXCEPT_COOL_1 },
        { u8"BY", srh::HaystackClass::EXCEPT_COOL_1 },
        { u8"IDEOGRAM", srh::HaystackClass::EVERYWHERE },
        { u8"IDEOGRAPH", srh::HaystackClass::EVERYWHERE },
        { u8"IN", srh::HaystackClass::EXCEPT_COOL_1 },
        { u8"LETTER", srh::HaystackClass::SCRIPT },
        { u8"LETTERFORM", srh::HaystackClass::EVERYWHERE },
        { u8"MAN", srh::HaystackClass::EMOJI | srh::HaystackClass::EGYPTIAN },
        { u8"OF", srh::HaystackClass::EVERYWHERE },  // No letter Of at all
        { u8"ON", srh::HaystackClass::EXCEPT_COOL_2 },
        { u8"OPPOSING", srh::HaystackClass::IDEOGRAPH },
        { u8"OVER", srh::HaystackClass::IDEOGRAPH },
        { u8"PATTERN", srh::HaystackClass::SCRIPT },
        { u8"PERSON", srh::HaystackClass::EMOJI },
        { u8"PLUS", srh::HaystackClass::IDEOGRAPH },
        { u8"SIGN", srh::HaystackClass::EVERYWHERE },
        { u8"SQUARED", srh::HaystackClass::IDEOGRAPH },
        { u8"SYLLABIC", srh::HaystackClass::EVERYWHERE },
        { u8"SYLLABICS", srh::HaystackClass::EVERYWHERE },
        { u8"SYLLABLE", srh::HaystackClass::EVERYWHERE },
        { u8"SYMBOL", srh::HaystackClass::EVERYWHERE },
        { u8"TIMES", srh::HaystackClass::IDEOGRAPH },
        { u8"TO", srh::HaystackClass::EXCEPT_COOL_1 },
        { u8"WITH", srh::HaystackClass::EVERYWHERE }, // Let it be this way
        { u8"WOMAN", srh::HaystackClass::EMOJI | srh::HaystackClass::EGYPTIAN },
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
            || lat::toUpper(c1) == lat::toUpper(c2));
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

    struct FindPlace {
        const srh::HayWord* word = nullptr;
        size_t iWord = std::numeric_limits<size_t>::max();
        srh::FindStatus status = srh::FindStatus::NONE;

        explicit operator bool() const { return word; }
        size_t length() const { return word ? word->length() : 0; }
    };

    FindPlace myFind(
            const srh::Comparator& comparator,
            std::span<const srh::HayWord> haystack,
            std::u8string_view needle,
            size_t iStartWord)
    {
        for (auto itHay = haystack.begin() + iStartWord;
             itHay != haystack.end();
             ++itHay) {
            auto hayword = itHay->sv();
            auto status = comparator.find(hayword, needle);
            if (status != srh::FindStatus::NONE) {
                return { .word = std::to_address(itHay),
                         .iWord = size_t(itHay - haystack.begin()),
                         .status = status };
            }
        }
        return {}; // not found
    }

}   // anon namespace

srh::Place srh::findWord(
        std::span<HayWord> haystack, const NeedleWord& needle,
        Flags<HaystackClass> hclasses, const Comparator& comparator)
{
    bool isNeedleLowPrio = needle.lowPrioClass.haveAny(hclasses);
    Place r = Place::NONE;
    size_t pos = 0;
    while (true) {
        auto where = myFind(comparator, haystack, needle.sv(), pos);
        if (!where)
            break;
        Place r1 = Place::PARTIAL;
        switch (where.status) {
        case FindStatus::COMPLETE:
            r1 = isNeedleLowPrio ? Place::EXACT_LOPRIO : Place::EXACT;
            break;
        case FindStatus::INITIAL:
            r1 = where.word->lowPrioClass.haveAny(hclasses)
                 ?  Place::INITIAL_LOPRIO : Place::INITIAL;
            break;
        case FindStatus::SUBSTR:
        case FindStatus::NONE: ;
        }
        // Check for dictionary word
        switch (r1) {
        case Place::EXACT:
        case Place::EXACT_LOPRIO:
            return r1;
        case Place::INITIAL:
        case Place::INITIAL_LOPRIO:
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
                          Flags<HaystackClass> hclasses,
                          RoleType roleType, const Comparator& comparator)
{
    srh::Prio r { .roleType = roleType };
    for (auto& v : needle.words) {
        auto type = findWord(haystack, v, hclasses, comparator);
        switch (type) {
        case Place::EXACT: ++r.exact; break;
        case Place::EXACT_LOPRIO: ++r.exactLoPrio; break;
        case Place::INITIAL: ++r.initial; break;
        case Place::INITIAL_LOPRIO: ++r.initialLoPrio; break;
        case Place::PARTIAL: ++r.partial; break;
        case Place::NONE: ;
        }
    }
    return r;
}


srh::Prio srh::findNeedle(std::u8string_view haystack, const Needle& needle,
                          Flags<HaystackClass> hclasses,
                          RoleType roleType, Cache& cache,
                          const Comparator& comparator)
{
    // Uppercase haystack
    comparator.prepareHaystack(haystack, cache.haystack);
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
    return findNeedle(cache.words2, needle, hclasses, roleType, comparator);
}
