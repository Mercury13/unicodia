#include "engine.h"

// STL
#include <span>

#include "u_Strings.h"
#include "u_TypedFlags.h"

constinit const srh::Prio srh::Prio::EMPTY;
constinit const srh::DefaultComparator srh::DefaultComparator::INST;
constinit const srh::RoleInfo srh::RoleInfo::EMOJI {
    .type = srh::RoleType::BRIEF,
    .indexLocation = IndexLocation::NEVER };


namespace {

    inline srh::FindStatus detectIndex(
            std::u8string_view haystack, std::u8string_view needle,
            bool isNeedleIndex, size_t where)
    {
        if (isNeedleIndex) [[unlikely]] {
            where += needle.length();
            if (where < haystack.size()) {
                if (std::isalpha(haystack[where])) {
                    return srh::FindStatus::INDEX;
                }
            }
        }
        return srh::FindStatus::INITIAL;
    }

}   // anon namespace

srh::FindStatus srh::DefaultComparator::find(
        std::u8string_view haystack, std::u8string_view needle,
        bool isNeedleIndex) const
{
    auto pos = haystack.find(needle);
    if ( pos == std::u8string_view::npos)
        return srh::FindStatus::NONE;
    if (pos == 0) {
        if (haystack.length() == needle.length()) {
            return srh::FindStatus::COMPLETE;
        } else {
            return detectIndex(haystack, needle, isNeedleIndex, pos);
        }
    } else {
        if (srh::classify(haystack.at(pos - 1)) == srh::Class::OTHER) {
            return detectIndex(haystack, needle, isNeedleIndex, pos);
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

    constexpr auto ALL_SEPARATORS = u8" ,():/";

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
      ccLast(classify(v.back())),
      isShortIndex(lat::isShortIndex(v))
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
    if (lat::isIndex(v)) {
        auto iFirst = v.find_first_of(u8"0123456789");
        auto iLast  = v.find_last_of (u8"0123456789");
        if (iLast != std::u8string_view::npos && iFirst <= iLast && v[iFirst] != 0) {
            auto numLen = iLast + 1 - iFirst;
            if (numLen < 3) {
                threeDigitIndex = v;
                threeDigitIndex.insert(iFirst, 3 - numLen, '0');
                if (numLen == 1) {
                    twoDigitIndex = v;
                    twoDigitIndex.insert(iFirst, 1, '0');
                }
            }
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


void srh::HaystackCache::load(
        std::u8string_view x,
        const RoleInfo& aRoleInfo,
        Flags<HaystackClass> aClasses,
        const Comparator& aComparator)
{
    comparator = &aComparator;
    classes = aClasses;
    roleInfo = aRoleInfo;
    // Uppercase haystack
    aComparator.prepareHaystack(x, text);
    // Words — simple
    words1.clear();
    str::splitByAnySvTo(text, ALL_SEPARATORS, words1);
    // Words — bigger
    words2.clear();
    words2.reserve(words1.size());
    for (auto v : words1) {
        if (!v.empty())
            words2.emplace_back(v);
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
            bool isIndex,
            size_t iStartWord)
    {
        for (auto itHay = haystack.begin() + iStartWord;
             itHay != haystack.end();
             ++itHay) {
            auto hayword = itHay->sv();
            auto status = comparator.find(hayword, needle, isIndex);
            if (status != srh::FindStatus::NONE) {
                return { .word = std::to_address(itHay),
                         .iWord = size_t(itHay - haystack.begin()),
                         .status = status };
            }
        }
        return {}; // not found
    }

}   // anon namespace


srh::Place srh::HaystackCache::findWord(const NeedleWord& needle) const
{
    bool isNeedleLowPrio = needle.lowPrioClass.haveAny(classes);
    Place r = Place::NONE;
    size_t pos = 0;
    while (true) {
        auto where = myFind(*comparator, words2, needle.sv(), needle.isShortIndex, pos);
        if (!where)
            break;
        Place r1 = Place::PARTIAL;
        switch (where.status) {
        case FindStatus::COMPLETE:
            r1 = isNeedleLowPrio ? Place::EXACT_LOPRIO : Place::EXACT;
            break;
        case FindStatus::INDEX:
            switch (roleInfo.indexLocation) {
            case IndexLocation::END:
                if (where.iWord + 1 != words2.size())
                    break;  // and fall through to initial;
                [[fallthrough]];  // otherwise fall through to ANYWHERE
            case IndexLocation::ANYWHERE:
                r1 = Place::INDEX; goto brk1;
            case IndexLocation::NEVER:;  // and fall through to initial
            }
            [[fallthrough]];
        case FindStatus::INITIAL:
            r1 = where.word->lowPrioClass.haveAny(classes)
                 ?  Place::INITIAL_LOPRIO : Place::INITIAL;
            break;
        case FindStatus::SUBSTR:
        case FindStatus::NONE: ;
        }
    brk1:;
        // Check for dictionary word
        switch (r1) {
        case Place::EXACT:
        case Place::EXACT_LOPRIO:
            return r1;
        case Place::INDEX:
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

srh::Prio srh::HaystackCache::findNeedle(const Needle& needle) const
{
    srh::Prio r { .roleType = roleInfo.type };
    bool isIndex1 = false;
    for (auto& v : needle.words) {
        auto type = findWord(v);
        switch (type) {
        case Place::EXACT: ++r.exact; isIndex1 = true; break;
        case Place::EXACT_LOPRIO: ++r.exactLoPrio; isIndex1 = true; break;
        case Place::INDEX: ++r.initial; ++r.initialIndex; isIndex1 = true; break;
        case Place::INITIAL: ++r.initial; break;
        case Place::INITIAL_LOPRIO: ++r.initialLoPrio; break;
        case Place::PARTIAL: ++r.partial; break;
        case Place::NONE: ;
        }
    }
    if (isIndex1
            && roleInfo.indexLocation != IndexLocation::NEVER
            && (needle.words.size() == 1)
            && needle.words[0].isShortIndex) {
        r.high = HIPRIO_INDEX_MATCH;
    }
    return r;
}
