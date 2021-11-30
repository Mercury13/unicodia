#pragma once

#include <map>
#include <string>
#include <set>

#include "u_TypedFlags.h"
#include "u_Array.h"

enum class Dicf {
    // Capitalization
    CAP_ALL                 = 1 << 0,
    CAP_TITLE               = 1 << 1,
    CAP_SMALL               = 1 << 2,
    // Triggered by
    IF_SCRIPT               = 1 << 3,   // Capitalize if somewhere script is found
    IF_NEXT_NOT_NOUN        = 1 << 4,   // Capitalize if next is not noun

    // Parts of speech
    PART_ADJECTIVE          = 1 << 5,   // “letter Long A” → “long” propagates capitalization to the left
                                        // “Old Permic” → propagate script to the left
    PART_NOUN               = 1 << 6,   // Noun: stop propagation
    PART_MIXED              = 1 << 7,   // Noun / adjective
    PART_MAYBE_LETTER       = 1 << 8,   // Probably letter: adjectives → capitalize
    PART_NUMERAL            = 1 << 9,   // Probably letter: adjectives → capitalize

    // Triggers
    TRIG_SCRIPT             = 1 << 10,  // This word is script
    TRIG_TWO_WORD_SCRIPT    = 1 << 11,  // Script consisting of two words: Old Italic
    TRIG_CAP_NEXT           = 1 << 12,  // Capitalize next word
    TRIG_DIGIT_PROPAGATES_RIGHT = 1 << 13, // Digit propagates right “Mende Kikakui syllable M135 In”
    TRIG_THREE_WORD_SCRIPT  = 1 << 14,  // Script consisting of three words: Pau Cin Hau
    TRIG_SCRIPT_IF_FIRST    = 1 << 15,  // Script if 1st word (Yi)
};

DEFINE_ENUM_OPS(Dicf)

struct DicEntry
{
    Flags<Dicf> flags;
    std::string_view customCap {};
    std::string_view trigger1 {}, trigger2 {}, trigger3 {}, trigger4 {}, trigger5 {};

    constexpr DicEntry() = default;

    // Many flags
    constexpr DicEntry(Flags<Dicf> aFlags) : flags(aFlags) {}
    constexpr DicEntry(Flags<Dicf> aFlags, std::string_view aCustomCap)
        : flags(aFlags), customCap(aCustomCap) {}
    constexpr DicEntry(Flags<Dicf> aFlags, std::string_view aCustomCap,
                std::string_view aTrigger1, std::string_view aTrigger2 = {},
                std::string_view aTrigger3 = {}, std::string_view aTrigger4 = {},
                std::string_view aTrigger5 = {} )
        : flags(aFlags), customCap(aCustomCap), trigger1(aTrigger1), trigger2(aTrigger2),
          trigger3(aTrigger3), trigger4(aTrigger4), trigger5(aTrigger5) {}

    // One flag
    constexpr DicEntry(Dicf aFlag) : flags(aFlag) {}
    constexpr DicEntry(Dicf aFlag, std::string_view aCustomCap)
        : flags(aFlag), customCap(aCustomCap) {}
    constexpr DicEntry(Dicf aFlag, std::string_view aCustomCap,
                std::string_view aTrigger1, std::string_view aTrigger2 = {},
                std::string_view aTrigger3 = {}, std::string_view aTrigger4 = {},
                std::string_view aTrigger5 = {})
        : flags(aFlag), customCap(aCustomCap), trigger1(aTrigger1), trigger2(aTrigger2),
          trigger3(aTrigger3), trigger4(aTrigger4), trigger5(aTrigger5) {}
};


enum class PrefixAction {
    REST_CAPSMALL,
    NEXT_CAP,
    NEXT_SMALL,
    REST_SMALL,
    REST_CAP,
    REST_ALLCAP,
    NOMATTER = 0
};

struct PrefixEntry {
private:
    using This = PrefixEntry;
public:
    size_t nTriggers;
    enum { SZ = 10 };
    std::string_view triggers[SZ];
    const PrefixAction action;
    char32_t lo = 0, hi = 0xFFFFFF;

    template <size_t N>
    constexpr PrefixEntry(const std::string_view (&aTriggers)[N],
                          PrefixAction aAction)
        :  nTriggers(N), action(aAction)
    {
        static_assert(N <= SZ, "Prefix too long");
        for (size_t i = 0; i < N; ++i)
            triggers[i] = aTriggers[i];
    }
    template <size_t N>
    constexpr PrefixEntry(const std::string_view (&aTriggers)[N],
                          char32_t aLo, char32_t aHi,
                          PrefixAction aAction)
        : This(aTriggers, aAction)
        { lo = aLo; hi = aHi; }
    PrefixEntry(std::string_view aTrigger, PrefixAction aAction)
        : nTriggers(1), triggers { aTrigger, {} }, action(aAction) {}

    Buf1d<const std::string_view> toBuf() const { return { nTriggers, triggers }; }
};

inline bool operator < (const PrefixEntry& x, const PrefixEntry& y) { return x.triggers[0] < y.triggers[0]; }


extern const std::map<std::string_view, DicEntry> dictionary;
extern const std::map<std::string_view, std::string_view> exceptions;
extern const std::multiset<PrefixEntry> prefixes;
extern const std::set<std::string_view> langNames;
extern const std::map<char32_t, std::string_view> abbrevs;
extern const std::set<std::string_view> cuneiformKeywords;
extern const std::set<std::string_view> cuneiformSymbols;
extern const std::set<char32_t> virtualViramas;

enum class DecapDebug { NO, YES };

std::string decapitalize(
        std::string_view x,
        char32_t cp = 0,
        DecapDebug debug = DecapDebug::NO);
bool isAlternate(char32_t x);
