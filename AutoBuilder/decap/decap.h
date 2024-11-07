#pragma once

#include <map>
#include <string>
#include <set>
#include <unordered_set>
#include <unordered_map>

#include "u_TypedFlags.h"
#include "u_Array.h"
#include "UcFlags.h"

enum class Dicf {
    // Capitalization
    CAP_ALL                 = 1 << 0,
    CAP_TITLE               = 1 << 1,
    CAP_SMALL               = 1 << 2,
    // Triggered by
    IF_SCRIPT               = 1 << 3,   ///< Capitalize if somewhere script is found
    IF_NEXT_NOT_NOUN        = 1 << 4,   ///< Capitalize if next is not noun

    // Parts of speech
    PART_ADJECTIVE          = 1 << 5,   ///< “letter Long A” → “long” propagates capitalization to the right
    PART_NOUN               = 1 << 6,   ///< Noun: stop propagation
    PART_MIXED              = 1 << 7,   ///< Noun / adjective
    PART_MAYBE_LETTER       = 1 << 8,   ///< Probably letter: adjectives → capitalize
    PART_NUMERAL            = 1 << 9,   ///< Probably letter: adjectives → capitalize

    // Triggers
    TRIG_SCRIPT             = 1 << 10,  ///< This word is script
    TRIG_TWO_WORD_SCRIPT    = 1 << 11,  ///< Script consisting of two words: Ol Chiki
    TRIG_CAP_NEXT           = 1 << 12,  ///< Capitalize next word
    TRIG_DIGIT_PROPAGATES_RIGHT = 1 << 13, ///< Digit propagates right “Mende Kikakui syllable M135 In”
    TRIG_THREE_WORD_SCRIPT  = 1 << 14,  ///< Script consisting of three words: Pau Cin Hau
    TRIG_SCRIPT_IF_FIRST    = 1 << 15,  ///< Script if 1st word (Yi)
    TRIG_SCRIPT_ADJECTIVE   = 1 << 16,  ///< OLD NORTH Arabian → propagate script to the left
    TRIG_FORCE_FULL_DECAP   = 1 << 17,  ///< Force full decap if we are requesting just dictionary
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


enum class IdiomAction : unsigned char {
    // Work AFTER idiom
    REST_CAPSMALL,       ///< Rest words → Sentence case
    NEXT_CAP,            ///< Next word → Capital
    NEXT_SMALL,          ///< Next word → small
    REST_SMALL,          ///< Rest words → small case
    REST_CAP,            ///< Rest words → Title Case
    REST_ALLCAP,         ///< Rest words → ALL CAPS CASE
    REST_DETECT_LETTERS, ///< Rest words → detect letters I J K some garbage
    // Work ON idiom
    SECOND_THIRD_SMALL,  ///< Second, third, fourth… words of idiom are small
    FIRST_CAP,           ///< First word of idiom is capital
    // Special constants
    NOMATTER = 0,
    FIRST_ON_IDIOM = SECOND_THIRD_SMALL,  ///< First action that works ON idiom
};


enum class IsPrefix : unsigned char { NO, YES };


struct IdiomEntry {
private:
    using This = IdiomEntry;
public:
    size_t nTriggers;
    static constexpr int SZ = 10;
    std::string_view triggers[SZ];
    const IsPrefix ecIsPrefix;
    const IdiomAction action;
    char32_t lo = 0, hi = 0xFFFFFF;

    template <size_t N>
    constexpr IdiomEntry(const std::string_view (&aTriggers)[N],
                          IsPrefix aIsPrefix, IdiomAction aAction)
        :  nTriggers(N), ecIsPrefix(aIsPrefix), action(aAction)
    {
        static_assert(N <= SZ, "Prefix too long");
        for (size_t i = 0; i < N; ++i)
            triggers[i] = aTriggers[i];
    }
    template <size_t N>
    constexpr IdiomEntry(const std::string_view (&aTriggers)[N],
                          char32_t aLo, char32_t aHi,
                          IsPrefix aIsPrefix,
                          const IdiomAction& aAction)
        : This(aTriggers, aIsPrefix, aAction)
        { lo = aLo; hi = aHi; }
    IdiomEntry(std::string_view aTrigger, IsPrefix aIsPrefix, IdiomAction aAction)
        : nTriggers(1), triggers { aTrigger, {} }, ecIsPrefix(aIsPrefix), action(aAction) {}

    Buf1d<const std::string_view> toBuf() const { return { nTriggers, triggers }; }
    constexpr bool doesNeedWordAfter() const { return (action < IdiomAction::FIRST_ON_IDIOM); }
    constexpr bool isPrefix() const { return static_cast<bool>(ecIsPrefix); }
};

inline bool operator < (
        const IdiomEntry& x, const IdiomEntry& y)
    { return x.triggers[0] < y.triggers[0]; }

///
///  Deprecated info
///
struct DepInfo
{
    std::u8string_view whatsInstead {}, whatsInstead2 {};
};

extern const std::unordered_map<std::string_view, DicEntry> dictionary;
extern const std::multiset<IdiomEntry> idioms;
extern const std::set<std::string_view> langNames;
extern const std::unordered_map<char32_t, std::string_view> abbrevs;
extern const std::unordered_set<std::string_view> cuneiformKeywords;
extern const std::unordered_set<std::string_view> cuneiformSymbols;
extern const std::unordered_map<char32_t, uc::Cfgs> drawMethods;
extern const std::unordered_map<char32_t, DepInfo> deprecatedInfo;

enum class DecapDebug : unsigned char { NO, YES };

enum class Dcfg : unsigned char {
    SHORTCUT = 1,   ///< Force shortcut decapitalization (by dictionary unless special keywords)
    LOCASE = 2,     ///< Letter is lowercase
    UPCASE = 0,     ///< Letter is uppercase, Right now does nothing
};

std::string decapitalize(
        std::string_view x,
        char32_t cp = 0,
        Flags<Dcfg> flags = {},
        DecapDebug debug = DecapDebug::NO);
inline std::string decapitalize(
        std::string_view x,
        char32_t cp,
        DecapDebug debug)
    { return decapitalize(x, cp, Flags<Dcfg>{}, debug); }

std::string decapitalizeEmoji(
        std::string_view x,
        DecapDebug debug = DecapDebug::NO);
std::string decapitalizeByTable(std::string_view x);
bool isAlternate(char32_t x);
bool isNoAa(char32_t x);
Flags<uc::Cfg> styleFlags(char32_t x);

size_t nExceptions();
