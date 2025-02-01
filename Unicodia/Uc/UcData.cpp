#include "UcData.h"

// Qt
#include <QFontDatabase>
#include <QFontMetrics>
#include <QRawFont>

// Libs
#include "Fonts/TempFont.h"
#include "u_Strings.h"
#include "u_Qstrings.h"
#include "mojibake.h"

// Unicode
#include "UcCp.h"
#include "UcSkin.h"

// L10n
#include "LocDic.h"
#include "u_Decoders.h"


#define NBSP "\u00A0"

using namespace std::string_view_literals;
const uc::Cp* uc::cpsByCode[CAPACITY];
short uc::blocksByCode16[CAPACITY >> 4];
const QString uc::Font::qempty;
const uc::GlyphStyleSets uc::GlyphStyleSets::EMPTY;

constinit const match::MainFont match::MainFont::INST;
constinit const match::Normal match::Normal::INST;
constinit const match::NullForTofu match::NullForTofu::INST;
constinit const match::FallToLast match::FallToLast::INST;

// [+] any missing char is tofu (BUGGY)  [-] try smth from system
constexpr bool FORCE_TOFU = false;

constinit const sw::Char
    sw::EMPTY_CHAR    { .rot=0, .fill=0, .minSpecialFill=9 },
    sw::SIMPLE_CHAR   { .rot=1, .fill=1, .minSpecialFill=9 };

namespace fst {
    constexpr auto DEFAULT = FORCE_TOFU ? TOFU : COMPAT;
}

constexpr uc::LangLife langLifeInfoProto[] {
    { {} },
    { "Prop.Life.Alive" },
    { "Prop.Life.Danger" },
    { "Prop.Life.Competing" },
    { "Prop.Life.Dead" },
    { "Prop.Life.Conserved" },
    { "Prop.Life.Undeciphered" },
    { "Prop.Life.Partly" },
    { "Prop.Life.Deciphered" },
    { "Prop.Life.Sacred" },
    { "Prop.Life.New"},
    { "Prop.Life.Reborn" },
    { "Prop.Life.ForRev" },
    { "Prop.Life.Exp" },
    //{ "Error" },
};
constinit const ec::Array<uc::LangLife, uc::EcLangLife> uc::langLifeInfo {
    ec::ARRAY_INIT, langLifeInfoProto };

constexpr uc::WritingDir writingDirProto[] {
    { {} },
    { "Prop.Dir.L" },
    { "Prop.Dir.R" },
    { "Prop.Dir.L_cjk" },
    { "Prop.Dir.L_cunei" },
    { "Prop.Dir.L_ogam" },
    { "Prop.Dir.L_col" },
    { "Prop.Dir.TD" },
    { "Prop.Dir.Both" },
    { "Prop.Dir.R_col" },
    { "Prop.Dir.R_most" },
    { "Prop.Dir.Sogd" },
};

constinit const ec::Array<uc::WritingDir, uc::EcWritingDir> uc::writingDirInfo (
    ec::ARRAY_INIT, writingDirProto );


constinit const uc::ScriptType uc::scriptTypeInfo[] {
    { {} },
    { "Prop.Type.Alph" },
    { "Prop.Type.Argu" },
    { "Prop.Type.Cons" },
    { "Prop.Type.Syll" },
    { "Prop.Type.Asyl" },
    { "Prop.Type.Abug" },
    { "Prop.Type.Brah" },
    { "Prop.Type.Hier" },
    { "Prop.Type.Syhi" },
    { "Prop.Type.Cohi" },
    { "Prop.Type.Code" },
    { "Prop.Type.Nota" },
    //{ "Error" },
};
static_assert (std::size(uc::scriptTypeInfo) == static_cast<int>(uc::EcScriptType::NN));

constinit const uc::Version uc::versionInfo[] {
    { {},       { 0000, Month::NUL }, NO_FLAGS },
    //{ "1.0",  1991 },
    //{ "1.0.1",1992 },
    { u8"1.1",  { 1993, Month::JUN }, Vfg::TEXT },
    { u8"2.0",  { 1996, Month::JUL }, Vfg::TEXT },
    { u8"2.1",  { 1998, Month::MAY }, Vfg::TEXT },
    { u8"3.0",  { 1999, Month::SEP }, NO_FLAGS },
    { u8"3.1",  { 2001, Month::MAR }, Vfg::TEXT },
    { u8"3.2",  { 2002, Month::MAR }, NO_FLAGS },
    { u8"4.0",  { 2003, Month::APR }, NO_FLAGS },
    { u8"4.1",  { 2005, Month::MAR }, NO_FLAGS },
    { u8"5.0",  { 2006, Month::JUL }, NO_FLAGS },
    { u8"5.1",  { 2008, Month::APR }, NO_FLAGS },
    { u8"5.2",  { 2009, Month::OCT }, NO_FLAGS },
    { u8"6.0",  { 2010, Month::OCT }, NO_FLAGS, u8"0.6" },
    { u8"6.1",  { 2012, Month::JAN }, NO_FLAGS },
    { u8"6.2",  { 2012, Month::SEP }, Vfg::TEXT },
    { u8"6.3",  { 2013, Month::SEP }, Vfg::TEXT },
    { u8"7.0",  { 2014, Month::JUN }, NO_FLAGS, u8"0.7" },
    { u8"8.0",  { 2015, Month::JUN }, NO_FLAGS, u8"1.0", { 2015, Month::AUG } },
    { {},       { 2015, Month::NOV }, NO_FLAGS, u8"2.0" },
    { u8"9.0",  { 2016, Month::JUN }, NO_FLAGS, u8"3.0" },
    { {},       { 2016, Month::NOV }, NO_FLAGS, u8"4.0" },
    { u8"10.0", { 2017, Month::JUN }, NO_FLAGS, u8"5.0" },
    { u8"11.0", { 2018, Month::JUN }, NO_FLAGS },
    { u8"12.0", { 2019, Month::MAR }, NO_FLAGS },
    { u8"12.1", { 2019, Month::MAY }, Vfg::TEXT },
    { u8"13.0", { 2020, Month::MAR }, NO_FLAGS },
    { {},       { 2020, Month::SEP }, NO_FLAGS, u8"13.1" },
    { u8"14.0", { 2021, Month::SEP }, NO_FLAGS },
    { u8"15.0", { 2022, Month::SEP }, NO_FLAGS },
    { u8"15.1", { 2023, Month::SEP }, Vfg::TEXT },
    { u8"16.0", { 2024, Month::SEP }, Vfg::TEXT },
    { u8"17.0", { 2025, Month::SEP }, NO_FLAGS },
};
static_assert (std::size(uc::versionInfo) == static_cast<int>(uc::EcVersion::NN));
Buf1d<const uc::Version> uc::allVersions() { return versionInfo; }

constinit const uc::Category uc::categoryInfo[] {
    { EcUpCategory::CONTROL,      "Cc",   Graphical::NO,  Independent::YES },
    { EcUpCategory::FORMAT,       "Cf",   Graphical::NO,  Independent::YES },
    // PRIVATE_USE,     -- unused as Unicodia has nothing to tell
    // SURROGATE,       -- unused as Unicodia has nothing to tell
    { EcUpCategory::LETTER,       "Ll",   Graphical::YES, Independent::YES },
    { EcUpCategory::LETTER,       "Lm",   Graphical::YES, Independent::YES },
    { EcUpCategory::LETTER,       "Lo",   Graphical::YES, Independent::YES },
    { EcUpCategory::LETTER,       "Lt",   Graphical::YES, Independent::YES },
    { EcUpCategory::LETTER,       "Lu",   Graphical::YES, Independent::YES },
    { EcUpCategory::MARK,         "Mc",   Graphical::YES, Independent::NO  },
    { EcUpCategory::MARK,         "Me",   Graphical::YES, Independent::NO  },
    { EcUpCategory::MARK,         "Mn",   Graphical::YES, Independent::NO  },
    { EcUpCategory::NUMBER,       "Nd",   Graphical::YES, Independent::YES },
    { EcUpCategory::NUMBER,       "Nl",   Graphical::YES, Independent::YES },
    { EcUpCategory::NUMBER,       "No",   Graphical::YES, Independent::YES },
    { EcUpCategory::PUNCTUATION,  "Pc",   Graphical::YES, Independent::YES },
    { EcUpCategory::PUNCTUATION,  "Pd",   Graphical::YES, Independent::YES },
    { EcUpCategory::PUNCTUATION,  "Pe",   Graphical::YES, Independent::YES },
    { EcUpCategory::PUNCTUATION,  "Pf",   Graphical::YES, Independent::YES },
    { EcUpCategory::PUNCTUATION,  "Pi",   Graphical::YES, Independent::YES },
    { EcUpCategory::PUNCTUATION,  "Po",   Graphical::YES, Independent::YES },
    { EcUpCategory::PUNCTUATION,  "Ps",   Graphical::YES, Independent::YES },
    { EcUpCategory::SYMBOL,       "Sc",   Graphical::YES, Independent::NO  },
    { EcUpCategory::SYMBOL,       "Sk",   Graphical::YES, Independent::YES },
    { EcUpCategory::SYMBOL,       "Sm",   Graphical::YES, Independent::YES },
    { EcUpCategory::SYMBOL,       "So",   Graphical::YES, Independent::YES },
    { EcUpCategory::SEPARATOR,    "Zl",   Graphical::NO,  Independent::YES },
    { EcUpCategory::SEPARATOR,    "Zp",   Graphical::NO,  Independent::YES },
    { EcUpCategory::SEPARATOR,    "Zs",   Graphical::YES, Independent::YES },
};
static_assert(std::size(uc::categoryInfo) == static_cast<int>(uc::EcCategory::NN));

const uc::Category* uc::findCategory(std::string_view x)
    { return findInArray(x, categoryInfo); }

#define NUM_RAW(x) { .locKey = "Prop.Num." x, .searchLocKey = "Search.Num." x }

constexpr uc::NumType numTypeRawInfo[] {
    { .locKey = {}, .searchLocKey = {} },
    NUM_RAW("Digit"),
    NUM_RAW("SpecDigit"),
    NUM_RAW("Number"),
    NUM_RAW("CjkRare"),
    NUM_RAW("CjkAccount"),
    NUM_RAW("CjkZhuang"),
    NUM_RAW("CjkVietnamese"),
};
#undef NUM_RAW

constinit const ec::Array<uc::NumType, uc::EcNumType> uc::numTypeInfo {
    ec::ARRAY_INIT, numTypeRawInfo
};


constexpr uc::NumOrderInfo numOrderRawInfo[] {
    { .nDigs = 0, .locKey = "Prop.Lang.Num.Unit" }, // NONE
    { .nDigs = 0, .locKey = "Prop.Lang.Num.Unit" }, // UNIT
    { .nDigs = 0, .locKey = "Prop.Lang.Num.Tho"  }, // THOUSAND
    { .nDigs = 1, .locKey = "Prop.Lang.Num.Mil"  }, // HUN_THOUSAND
    { .nDigs = 0, .locKey = "Prop.Lang.Num.Mil"  }, // MILLION
    { .nDigs = 2, .locKey = "Prop.Lang.Num.Bil"  }, // DEC_MILLION
    { .nDigs = 1, .locKey = "Prop.Lang.Num.Bil"  }, // HUN_MILLION
};

constinit const ec::Array<uc::NumOrderInfo, uc::NumOrder> uc::numOrderInfo (
    ec::ARRAY_INIT, numOrderRawInfo );


constinit const uc::BidiClass uc::bidiClassInfo[static_cast<int>(EcBidiClass::NN)] {
    { "DIR", u8"FSI, LRE, LRI…",    EcBidiStrength::EXPLICIT, },    // Explicit chars in GenPunct
    { "AL",  u8"AL",                EcBidiStrength::STRONG },       // Any Arabic letter
    { "AN",  u8"AN",                EcBidiStrength::WEAK },         // Arabic digits (0660…), Rumi (10E60)

    { "B",   u8"B",                 EcBidiStrength::NEUTRAL },      // CR, LF and a few more
    { "BN",  u8"BN",                EcBidiStrength::NEUTRAL },      // Most controls
    { "CS",  u8"CS",                EcBidiStrength::WEAK },         // ,.

    { "EN",  u8"EN",                EcBidiStrength::WEAK },         // 012…
    { "ES",  u8"ES",                EcBidiStrength::WEAK },         // +-
    { "ET",  u8"ET",                EcBidiStrength::WEAK },         // #$°

    { "L",   u8"L",                 EcBidiStrength::STRONG },       // ABC…
    { "NSM", u8"NSM",               EcBidiStrength::WEAK },         // Combining umlaut
    { "ON",  u8"ON\u00A0+\u00A0¬\u00A0Bidi_M", EcBidiStrength::NEUTRAL },  // !"

    { "NM",  u8"ON\u00A0+\u00A0Bidi_M", EcBidiStrength::NEUTRAL },  // ()

    { "R",   u8"R",                 EcBidiStrength::STRONG },       // Hebrew
    { "S",   u8"S",                 EcBidiStrength::NEUTRAL },      // 09, 0B, 1F
    { "WS",  u8"WS",                EcBidiStrength::NEUTRAL },      // FF, space
    // { u8"Error" },  // check for equal number
};

void uc::BidiClass::printfLocKeyN(char* buf, size_t n, const char* suffix) const
    { snprintf(buf, n, "Prop.Bidi.%s.%s", id.data(), suffix); }


constinit const uc::TermCat uc::termCats[] {
    { "TermCat.Encoding",      SortTerms::YES, },
    { "TermCat.Serialization", SortTerms::NO   },
    { "TermCat.Scripts",       SortTerms::YES  },
    { "TermCat.Chars",         SortTerms::YES  },
    { "TermCat.Algos",         SortTerms::YES  },
    { "TermCat.General",       SortTerms::YES  },
    { "TermCat.Styles",        SortTerms::YES  },
    { "TermCat.Input",         SortTerms::NO   },
    { "TermCat.Misc",          SortTerms::YES  },
};
static_assert (std::size(uc::termCats) == static_cast<size_t>(uc::EcTermCat::NN));

constinit const uc::Term uc::terms[] {
    // Input — not sorted, OK
    { "winemoji", EcTermCat::INPUT, {} },
    { "altcode", EcTermCat::INPUT, u8"Alt codes" },
    { "appentry", EcTermCat::INPUT, {} },
    { "birman", EcTermCat::INPUT, {} },

    // Script classes sorted
    { "hiero", EcTermCat::SCRIPT_CLASS, u8"ideographic script" },
    { "abjad", EcTermCat::SCRIPT_CLASS, u8"consonant script, abjad" },
    { "syllabic", EcTermCat::SCRIPT_CLASS, u8"syllabic script" },
    { .key = "abugida", .ecCat = EcTermCat::SCRIPT_CLASS, .engName = u8"abugida",
                .ecFont = EcFont::ETHIOPIC_SMALLER },
    { "brahmic", EcTermCat::SCRIPT_CLASS, u8"Brahmic abugida, Indic script", {}, EcFont::DEVANAGARI_SYSTEM },
    { "alphabet", EcTermCat::SCRIPT_CLASS, u8"alphabet script" },
    { .key = "arguable", .ecCat = EcTermCat::SCRIPT_CLASS,
                .engName = u8"script of arguable type", .fgs = Tfg::SORT_KEY },
    { "cjk", EcTermCat::SCRIPT_CLASS, u8"CJK = Chinese, Japanese, Korean" },
    { "code", EcTermCat::SCRIPT_CLASS, u8"codes and shorthand" },
    { "nontext", EcTermCat::SCRIPT_CLASS, u8"non-text notation" },

    { "acrophonic", EcTermCat::CHAR_CLASS, u8"acrophonic character" },
    { "virama", EcTermCat::CHAR_CLASS, u8"virama", {}, EcFont::DEVANAGARI_SYSTEM },
    { "graphic", EcTermCat::CHAR_CLASS, u8"graphic character, printable character" },
    { .key = "deprecated", .ecCat = EcTermCat::CHAR_CLASS, .engName = u8"deprecated character",
            .search { .fgs = uc::Cfg::U_DEPRECATED } },
    { .key = "ignorable", .ecCat = EcTermCat::CHAR_CLASS, .engName = u8"default-ignorable character",
            .search { .fgs = uc::Cfg::U_DEF_IGNORABLE } },
    { .key = "combining", .ecCat = EcTermCat::CHAR_CLASS, .engName = u8"combining mark",
            .search { .ecUpCat = uc::EcUpCategory::MARK } },
    { "ligature", EcTermCat::CHAR_CLASS, u8"ligature" },
    { "precomposed", EcTermCat::CHAR_CLASS, u8"precomposed character" },
    { .key = "space", .ecCat = EcTermCat::CHAR_CLASS, .engName = u8"whitespace",
            .borrowedDesc = { "CharCat.Zs.Text" },
            .search { .ecCategory = uc::EcCategory::SEPARATOR_SPACE } },
    { "spacing", EcTermCat::CHAR_CLASS, u8"spacing character" },
    { "pseudographics", EcTermCat::CHAR_CLASS, u8"pseudographics, semigraphics, box drawing" },
    { .key = "control", .ecCat = EcTermCat::CHAR_CLASS, .engName = u8"control character",
            .borrowedDesc = { "CharCat.Cc.Text" },
            .search { .ecCategory = uc::EcCategory::CONTROL} },
    { "obsolete", EcTermCat::CHAR_CLASS, u8"obsolete character" },
    { .key = "format", .ecCat = EcTermCat::CHAR_CLASS, .engName = u8"format character",
            .borrowedDesc = { "CharCat.Cf.Text" },
            .search { .ecCategory = uc::EcCategory::FORMAT } },
    { .key = "number", .ecCat = EcTermCat::CHAR_CLASS, .engName = u8"number character",
            .search { .isNumber = true } },
    { "emoji", EcTermCat::CHAR_CLASS, u8"emoji" },

    { "stability", EcTermCat::PRINCIPLES, u8"stability guarantees" },
    { "dynamic", EcTermCat::PRINCIPLES, u8"dynamic composition" },
    { "logical", EcTermCat::PRINCIPLES, u8"logical order" },
    { "convertibility", EcTermCat::PRINCIPLES, u8"convertibility" },
    { "plaintext", EcTermCat::PRINCIPLES, u8"plain text" },
    { "semantics", EcTermCat::PRINCIPLES, u8"semantics" },
    { "character", EcTermCat::PRINCIPLES, u8"characters, not glyphs" },
    { "universality", EcTermCat::PRINCIPLES, u8"universality" },
    { "unification", EcTermCat::PRINCIPLES, u8"unification" },
    { "efficiency", EcTermCat::PRINCIPLES, u8"efficiency" },

    // Encoding sorted
    { .key = "ascii", .ecCat = EcTermCat::ENCODING, .engName = u8"ASCII",
            .borrowedDesc { "Block.0000.Text" } },
    { "bmp", EcTermCat::ENCODING, u8"BMP = Basic Multilingual Plane" },
    { "noncharacter", EcTermCat::ENCODING, u8"non-character" },
    { "codepoint", EcTermCat::ENCODING, u8"code point" },
    { "latin1", EcTermCat::ENCODING, u8"Latin-1", { "Block.0080.Text" } },
    { "plane", EcTermCat::ENCODING, u8"plane" },
    { "private", EcTermCat::ENCODING, u8"private-use character" },
    { .key = "reject", .ecCat = EcTermCat::ENCODING, .engName = u8"rejected proposals",
            .fgs = Tfg::SORT_KEY },
    { .key = "zwj", .ecCat = EcTermCat::ENCODING, .engName = u8"ZWJ = zero width joiner",
            .ecFont = EcFont::DEVANAGARI_SYSTEM },
    { .key = "zwnj", .ecCat = EcTermCat::ENCODING, .engName = u8"ZWNJ = zero width non-joiner",
            .ecFont = EcFont::DEVANAGARI_SYSTEM },
    { .key = "country", .ecCat = EcTermCat::ENCODING, .engName = u8"country codes",
            .fgs = Tfg::SORT_KEY },
    { .key = "egyp", .ecCat = EcTermCat::ENCODING, .engName = u8"Egyptian hieroglyphs",
            .fgs = Tfg::SORT_KEY },

    { "bidi", EcTermCat::ALGORITHM, u8"bidirectional text" },
    { "normalization", EcTermCat::ALGORITHM, u8"normalization" },
    { "casefolding", EcTermCat::ALGORITHM, u8"case folding" },

    // Serialization unsorted
    { "serialization", EcTermCat::SERIALIZATION, u8"serialization" },
    { "punycode", EcTermCat::SERIALIZATION, u8"Punycode", },
    { "utf7", EcTermCat::SERIALIZATION, u8"UTF-7" },
    { "utf8", EcTermCat::SERIALIZATION, u8"UTF-8" },
    { "utf16", EcTermCat::SERIALIZATION, u8"UTF-16, surrogate pair" },
    { "utf32", EcTermCat::SERIALIZATION, u8"UTF-32" },
    { "bom", EcTermCat::SERIALIZATION, u8"BOM = byte order mark" },

    { "cursive", EcTermCat::WRITING_STYLE, u8"cursive" },
    { "minuscule", EcTermCat::WRITING_STYLE, u8"minuscule writing" },

    { "abc", EcTermCat::OTHER, u8"collation" },
    { "mono", EcTermCat::OTHER, u8"character cell" },
    { "inclusion", EcTermCat::OTHER, u8"inclusion and diversity" },
    { "tofu", EcTermCat::OTHER, u8"tofu" },

    { "status", EcTermCat::HIDDEN, u8"" },
};

constinit const uc::Term* uc::sortedTerms[std::size(terms)] { nullptr };

size_t uc::nTerms() { return std::size(terms); }


constinit const uc::Continent uc::continentInfo[] {
    // None
    { { BG_INTER,             FG_INTER             }, { NO_COLOR, NO_COLOR }, true },
    // Tech
    { { { 0x22, 0x55, 0x00 }, { 0x7F, 0xFF, 0x2A },
            // Special Technical’s frame colour
            { 0x66, 0xff, 0x00 } },
            { NO_COLOR, NO_COLOR }, false },
    // Europe
    { { BG_EUROPE,            FG_EUROPE,           }, { NO_COLOR, NO_COLOR }, false  },
    // Asia RtL
    { { BG_ASIA_R,            FG_ASIA_R            }, { NO_COLOR, NO_COLOR }, false  },
    // Asia
    { { BG_ASIA_L,            { 0xF5, 0x79, 0x00 } }, { NO_COLOR, NO_COLOR }, false  },
    // CJK
    { { BG_CJK,               FG_CJK               }, { BG_CJK,  TX_CJK    }, false  },
    // Ocean
    { { BG_OCEAN,             { 0x00, 0x64, 0x00 } }, { NO_COLOR, NO_COLOR }, false  },
    // Africa
    { { BG_AFRICA,            FG_AFRICA            }, { CO_AFRICA, TX_AFRICA }, false},
    // America
    { { { 0xff, 0xc8, 0xc8 }, { 0xcc, 0x00, 0x00 } }, { NO_COLOR, NO_COLOR }, false  },
};

// Missing
constinit const uc::Continent MISSING_CONTINENT
    { { { 0xa4, 0x00, 0x00 }, { 0xFF, 0xFF, 0xFF } }, { NO_COLOR, NO_COLOR }, false };


static_assert(std::size(uc::continentInfo) == static_cast<int>(uc::EcContinent::NN));

const uc::GlyphStyleChannel uc::glyphStyleChannelInfo[] = {
    { 0, {}, uc::EcGlyphStyleChannel::NONE },      // -warn in static analyzer: init [0] as normal, and everything’s OK
    { .count = 2, .name = "Glag", .value = uc::EcGlyphStyleChannel::GLAGOLITIC },
    { .count = 2, .name = "Vert", .value = uc::EcGlyphStyleChannel::VERTICAL },
    { .count = 4, .name = "Arab", .value = uc::EcGlyphStyleChannel::ARABIC },
};
static_assert(std::size(uc::glyphStyleChannelInfo) == static_cast<size_t>(uc::EcGlyphStyleChannel::NN));


///// Sutton SignWriting ///////////////////////////////////////////////////////

sw::Info::Info(const uc::Cp& aCp) : fCp(aCp), fData(&EMPTY_CHAR)
{
    auto index = fCp.subj.ch32() - 0x1D800;
    if (index < sw::CLEN) {
        // OK, we are here!
        fData = &sw::data[index];
    }
}


bool sw::Info::hasFill0(int i) const
{
    if (i >= N_FILL)
        return false;    
    return fData->hasFill(i);
}


bool sw::Info::hasRot0(int i) const
{
    if (i >= N_ROT)
        return false;
    return fData->hasRotation(i);
}

constexpr char32_t DOTTED_CIRCLE = 0x25CC;
[[maybe_unused]] constexpr char32_t SGNW_HEAD = 0x1D9FF;

char32_t sw::Info::baseChar() const noexcept
{
    if (cp().category().upCat == uc::EcUpCategory::MARK) {
        return DOTTED_CIRCLE;
    }
    return 0;
}


///// Continent ////////////////////////////////////////////////////////////////


const uc::Continent& uc::SynthIcon::maybeMissingContinent() const
{
    if (flags.have(Ifg::MISSING))
        return MISSING_CONTINENT;
    return normalContinent();
}


///// UC misc //////////////////////////////////////////////////////////////////

namespace {
    // Debug
    [[maybe_unused]] std::u8string cache;
    [[maybe_unused]] inline std::string_view esc(std::u8string_view x)
    {
        return str::toSv(escape::cppSv(
                        x, cache, 'n', escape::Spaces::YES, Enquote::NO));
    }

    enum class EmojiClass : unsigned char {
        NONE,
        SINGLE_CHAR,    ///< incl. VS16
        /// @todo [urgent] Change ZWJ concept
        ZWJ_RIGHT,
        ZWJ_COLOR,
        ZWJ_GENDER,
        ZWJ_ACTIVITY,
        ZWJ_APPEARANCE,
        ZWJ_MULTIRACIAL,    ///< By convention, isSkintone=0
        ZWJ_OTHER,
        OTHER_NATIONAL,     ///< National flags, like [U][A]; all OTHER have no skintone
        OTHER_SUBDIVISION,
        OTHER_KEYCAP };

    struct ClassifyEmoji {
        EmojiClass clazz;
        bool isSkintone;
    };

    /// @param [in] x   any accepted emoji
    ClassifyEmoji classifyEmoji(std::u32string_view x)
    {
        switch (x.length()) {
        case 0:
            return { .clazz = EmojiClass::NONE, .isSkintone = false };
        case 1:
            return { .clazz = EmojiClass::SINGLE_CHAR, .isSkintone = false };
        case 2:
            switch (x[1]) {
            case cp::VS16:
                return { .clazz = EmojiClass::SINGLE_CHAR, .isSkintone = false };
            case cp::SKIN1:
            case cp::SKIN2:
            case cp::SKIN3:
            case cp::SKIN4:
            case cp::SKIN5:
                return { .clazz = EmojiClass::SINGLE_CHAR, .isSkintone = true };
            default:
                if (x[0] >= cp::FLAG_A && x[0] <= cp::FLAG_Z)
                    return { .clazz = EmojiClass::OTHER_NATIONAL, .isSkintone = false };
            }
            break;
        case 3:
            if (x[1] >= cp::VS16 && x[2] == cp::MARK_KEYCAP)
                return { .clazz = EmojiClass::OTHER_KEYCAP, .isSkintone = false };
            break;
        default: ;
        }

        unsigned nSkin = 0, nZwj = 0, nColor = 0, nGender = 0, nPeople = 0,
                 nAppearance = 0;
        for (auto c : x) {
            switch (c) {
            case cp::ZWJ:
                ++nZwj; break;
            case cp::SKIN1:
            case cp::SKIN2:
            case cp::SKIN3:
            case cp::SKIN4:
            case cp::SKIN5:
                ++nSkin; break;
            case cp::SQUARE_BLACK:
            case cp::SQUARE_WHITE:
            case cp::SQUARE_RED:
            case cp::SQUARE_ORANGE:
            case cp::SQUARE_YELLOW:
            case cp::SQUARE_GREEN:
            case cp::SQUARE_BLUE:
            case cp::SQUARE_PURPLE:
            case cp::SQUARE_BROWN:
                ++nColor; break;
            case cp::MALE:
            case cp::FEMALE:
                ++nGender; break;
            case cp::ADULT:
            case cp::CHILD:
            case cp::MAN:
            case cp::WOMAN:
            case cp::BOY:
            case cp::GIRL:
                ++nPeople; break;
            case cp::PERSON_BLOND:
            case cp::HAIR_RED:
            case cp::HAIR_CURLY:
            case cp::HAIR_BALD:
            case cp::HAIR_WHITE:
                ++nAppearance; break;
            default: ;
            }
        }
        // Multiracial?
        if (nSkin >= 2) {
            if (nZwj == 0)
                throw std::logic_error("Multiracial w/o ZWJ?");
            return { .clazz = EmojiClass::ZWJ_MULTIRACIAL,
                     .isSkintone = false };  // by convention
        }
        // Detech non-ZWJ sequences
        if (nZwj == 0) {
            if (nSkin != 0)
                throw std::logic_error("Strange non-ZWJ sequence with skintone?");
            // char+skintone: already present
            // keycap: already present
            if (x.starts_with(cp::BLACK_FLAG) && x.ends_with(cp::TAG_CANCEL))
                return { .clazz = EmojiClass::OTHER_SUBDIVISION, .isSkintone = false };
            throw std::logic_error("Unknown non-ZWJ sequence");
        }
        // Detect ZWJ sequences
        bool isSkin = nSkin;
        if (x.length() >= 4     // 4 = 1 + 3tail
                && x.ends_with(U32_ZWJ_RIGHT_ARROW_VS16))
            return { .clazz = EmojiClass::ZWJ_RIGHT, .isSkintone = isSkin };
        if (nAppearance > 0) {
            return { .clazz = EmojiClass::ZWJ_APPEARANCE, .isSkintone = isSkin };
        }
        if (nGender > 0) {
            if (nPeople == 0)
                return { .clazz = EmojiClass::ZWJ_GENDER, .isSkintone = isSkin };
            return     { .clazz = EmojiClass::ZWJ_OTHER,  .isSkintone = isSkin };
        }
        if (nPeople == 1)
            return { .clazz = EmojiClass::ZWJ_ACTIVITY, .isSkintone = isSkin };
        if (nColor > 0)
            return { .clazz = EmojiClass::ZWJ_COLOR, .isSkintone = isSkin };
        return { .clazz = EmojiClass::ZWJ_OTHER, .isSkintone = isSkin };
    }

    enum class EmojiVersion { THIS, OLD };

    EmojiVersion classifyEmojiVersion(char32_t c, const uc::Version& v)
    {
        if (auto cp = uc::cpsByCode[c]) {
            if (cp->ecVersion == v.stats.thisEcVersion)
                return EmojiVersion::THIS;
        }
        return EmojiVersion::OLD;
    }

    void completeEmojiData(size_t index)
    {
        auto& node = uc::libNodes[index];
        if (node.flags.have(uc::Lfg::NO_COUNTING))
            return;

        auto& version = node.emojiVersion();
        // Emoji type
        auto ce = classifyEmoji(node.value);
        switch (ce.clazz) {
        case EmojiClass::NONE: break;
        case EmojiClass::SINGLE_CHAR:
            if (ce.isSkintone) {
                ++version.stats.emoji.nw.other.nSingleSkintone;
            } else {
                switch (classifyEmojiVersion(node.value[0], version)) {
                case EmojiVersion::THIS:
                    ++version.stats.emoji.nw.singleChar.nThisUnicode; break;
                case EmojiVersion::OLD:
                    ++version.stats.emoji.nw.singleChar.nOldUnicode; break;
                }
            }
            break;
        case EmojiClass::ZWJ_MULTIRACIAL:
            ++version.stats.emoji.nw.zwj.nMultiracial; break;
        case EmojiClass::ZWJ_RIGHT:
            version.stats.emoji.nw.zwj.right.add(ce.isSkintone); break;
        case EmojiClass::ZWJ_ACTIVITY:
            version.stats.emoji.nw.zwj.activity.add(ce.isSkintone); break;
        case EmojiClass::ZWJ_APPEARANCE:
            version.stats.emoji.nw.zwj.appearance.add(ce.isSkintone); break;
        case EmojiClass::ZWJ_COLOR:
            version.stats.emoji.nw.zwj.color.add(ce.isSkintone); break;
        case EmojiClass::ZWJ_GENDER:
            version.stats.emoji.nw.zwj.gender.add(ce.isSkintone); break;
        case EmojiClass::ZWJ_OTHER:
            version.stats.emoji.nw.zwj.other.add(ce.isSkintone); break;
        case EmojiClass::OTHER_KEYCAP:
            ++version.stats.emoji.nw.other.nKeycaps; break;
        case EmojiClass::OTHER_NATIONAL:
            ++version.stats.emoji.nw.other.nNationalFlags; break;
        case EmojiClass::OTHER_SUBDIVISION:
            ++version.stats.emoji.nw.other.nSubdivisionFlags; break;
        }
        // Traverse children
        const auto a = node.iFirstChild;
        const auto b = a + node.nChildren;
        for (int i = a; i < b; ++i)
            completeEmojiData(i);
    }

}   // anon namespace


void uc::completeData()
{
    // Fill CP → block mapping
    std::fill(std::begin(blocksByCode16), std::end(blocksByCode16), -1);
    for (int iBlock = 0; iBlock < N_BLOCKS; ++iBlock) {
        auto& block = blocks[iBlock];
        auto i16 = block.startingCp >> 4;
        auto end16 = block.endingCp >> 4;
        for (; i16 <= end16; ++i16)
            blocksByCode16[i16] = iBlock;
    }

    // Fill CP info
    std::fill(std::begin(cpsByCode), std::end(cpsByCode), nullptr);
    for (auto& cp : cpInfo) {
        // Bidi class
        ++cp.bidiClass().nChars;
        // Category
        ++cp.category().nChars;
        // Script
        auto& script = cp.script();
        ++script.nChars;
        script.ecVersion = std::min(script.ecVersion, cp.ecVersion);
        if (script.plane == PLANE_UNKNOWN)
            script.plane = cp.plane();
        // Block
        char32_t code = cp.subj;
        auto block = blockOf(code);
        if (!block->firstAllocated)
            block->firstAllocated = &cp;
        block->lastAllocated = &cp;
        ++block->nChars;
        block->ecVersion = std::min(block->ecVersion, cp.ecVersion);
        block->ecLastVersion = std::max(block->ecLastVersion, cp.ecVersion);
        // Lookup table
        cpsByCode[cp.subj.val()] = &cp;
    }

    // Coptic is 4.1 (script of those characters was Greek)
    uc::scriptInfo[static_cast<int>(EcScript::Copt)].ecVersion = uc::EcVersion::V_4_1;

    for (auto& cp : cpInfo) {
        // Version
        auto& ver = cp.version();
        switch (cp.ecScript) {
        case EcScript::Hani:
            ++ver.stats.chars.nw.nHani; break;
        case EcScript::NONE:
        case EcScript::Zinh:
            if (cp.ecCategory == EcCategory::FORMAT) {
                ++ver.stats.chars.nw.nFormat; break;
            } else {
                ++ver.stats.chars.nw.nSymbols; break;
            }
        default:
            if (cp.script().ecVersion == cp.ecVersion) {
                ++ver.stats.chars.nw.nNewScripts;
            } else {
                ++ver.stats.chars.nw.nExistingScripts;
            }
        }
    }

    // Check blocks — they should have at least one char
    for (auto& v : allBlocks()) {
        if (!v.firstAllocated || !v.lastAllocated)
            throw std::logic_error("Block w/o chars leaked into data!");

        // Check synthesized icon        
        if (!v.synthIcon.flags.have(Ifg::MISSING)) {
            auto sv = v.synthIcon.subj.sv();
            for (auto c : sv) {
                if (c < v.startingCp || c > v.endingCp)
                throw std::logic_error(
                    "Synthesized icon is not within block " + std::string{v.name});
                if (!cpsByCode[c])
                    throw std::logic_error("Synthesized icon points to bad char!");
            }
        }
        if (!v.synthIcon.flags.have(Ifg::CONTINENT_OK)) {
            auto& script = v.script();
            if (script.ecContinent != v.synthIcon.ecContinent) {
                std::cout << "Continent mismatch: " << v.name << '\n';
            }
        }

        // Check alphabetic key
        if (v.alphaKey.subKey == 0) {
            if (v.ecScript != EcScript::NONE
                    && v.alphaKey.ecScript == v.ecScript) { // Actual script
                v.script().mainBlock = &v;
            } else if (v.alphaKey.script().flags.have(Sfg::SORT_KEY)) { // Pseudo-script for sorting
                v.alphaKey.script().mainBlock = &v;
            }
        }

        // Check version
        ++v.version().stats.blocks.nNew;

        // Most versions do not extend/shrink blocks. So check and cache
        v.resizeHistoryT([](EcVersion ecv, char32_t before, char32_t after) {
            auto& v = versionInfo[static_cast<int>(ecv)];
            if (before < after) {
                v.stats.blocks.wereExtended = true;
            } else {
                v.stats.blocks.wereShrunk = true;
            }
        });
    }

    // Find associated version for every version
    for (EcVersion i = EcVersion::FIRST_MEANING; i < EcVersion::NN; ++i) {
        auto& v = versionInfo[static_cast<int>(i)];
        v.stats.thisEcVersion = i;
    }

    completeEmojiData(uc::ILIB_EMOJI_ROOT);

    // Scripts
    for (unsigned i = 0; i < uc::N_SCRIPTS; ++i) {
        auto& sc = uc::scriptInfo[i];
        if (!sc.flags.have(Sfg::NO_STATS)) {
            auto& version = sc.version();
            ++version.stats.scripts.nNew;
        }
    }

    // Check versions
    unsigned nChars = 0, nEmoji = 0, nScripts = 0;
    // v1.1
    auto& v11 = uc::versionInfo[static_cast<int>(uc::EcVersion::V_1_1)];
    v11.stats.chars.nTransient = 4306 + 2350;  // Hangul syllables
    // Go!
    for (auto& v : versionInfo) {
        // Chars
        nChars += v.stats.chars.nw.nTotal();
        v.stats.chars.nTotal = nChars + v.stats.chars.nTransient;
        // Emoji
        nEmoji += v.stats.emoji.nw.nTotal();
        v.stats.emoji.nTotal = nEmoji;
        // Scripts
        nScripts += v.stats.scripts.nNew;
        v.stats.scripts.nTotal = nScripts;
    }

    // Save term INI
//    std::ofstream os("term.ini");
//    for (auto& v : terms) {
//        os << "[Term." << v.key << "]\n";
//        os << "Name=" << esc(v.locName) << '\n';
//        os << "Text=" << esc(v.locDesc) << '\n';
//    }
}


struct uc::LoadedFont : public dumb::SpTarget
{
    QList<QString> families;
    QString familiesComma;
    intptr_t tempId = FONT_NOT_INSTALLED;
    std::unique_ptr<QFont> probe {}, normal {};
    std::unique_ptr<QFontMetrics> probeMetrics;
    CompressedBits cps;
    bool isRejected = false;

    const QString& onlyFamily() const;
    const QFont& get(
            std::unique_ptr<QFont>& font,
            QFont::StyleStrategy strategy,
            Flags<uc::Ffg> flags) const;
};

template class dumb::Sp<uc::LoadedFont>;


const QString& uc::LoadedFont::onlyFamily() const
{
    return (families.size() == 1) ? families[0] : uc::Font::qempty;
}


const QFont& uc::LoadedFont::get(
        std::unique_ptr<QFont>& font,
        QFont::StyleStrategy strategy,
        Flags<uc::Ffg> flags) const
{
    if (!font) {
        auto& family1 = onlyFamily();
        font.reset(new QFont(family1, 50, QFont::Normal));
        if (family1.isEmpty())
            font->setFamilies(families);
        // Weight
        if (flags.have(Ffg::BOLD)) {
            font->setBold(true);
        } else if (flags.have(Ffg::SEMIBOLD)) {
            font->setWeight(QFont::DemiBold);
        } else if (flags.have(Ffg::LIGHT)) {
            font->setWeight(QFont::Light);
        }
        font->setStyleStrategy(strategy);
    }
    return *font;
}


namespace {

    QList<QString> toQList(std::string_view s)
    {
        auto strs = str::splitSv(s, ',');
        QList<QString> r;
        r.reserve(strs.size());
        for (auto& v : strs) {
            r.emplaceBack(str::toQ(v));
        }
        return r;
    }

    std::unordered_map<std::string_view, dumb::Sp<uc::LoadedFont>> loadedFonts;

    bool isFontFname(std::string_view name)
    {
        return name.ends_with(".ttf"sv)
            || name.ends_with(".ttc"sv)
            || name.ends_with(".otf"sv);
    }

    void preloadFont(uc::EcFont font)
    {
        uc::fontInfo[static_cast<int>(font)].load(0);
    }

    bool wantPreloadFonts = true;

    ///
    /// @brief preloadFonts
    ///   Preloads fonts for unglitching
    ///   (we somehow depend on font loading order)
    /// @return [+] preload occured
    ///
    bool preloadFonts()
    {
        bool r = wantPreloadFonts;
        if (r) {
            // Prevent from triggering again…
            wantPreloadFonts = false;
            // …and preload, here are fonts
            preloadFont(uc::EcFont::BRAHMI);
            preloadFont(uc::EcFont::TAMIL_SUPPLEMENT);
        }
        return r;
    }

}   // anon namespace


void uc::Font::newLoadedStruc() const
{
    auto newLoaded = dumb::makeSp<LoadedFont>();
    loadedFonts[family.text] = newLoaded;
    q.loaded = newLoaded;
}


void uc::Font::load(char32_t trigger) const
{
onceAgain:
    // Loaded?
    if (q.loaded || q.isRejected)
        return;

    // Find in cache
    if (auto it = loadedFonts.find(family.text); it != loadedFonts.end()) {
        q.loaded = it->second;
        q.isRejected = q.loaded->isRejected;
        return;
    }

    // Create/load it
    if (isFontFname(family.text)) {
        // FILE
        if (preloadFonts())     // File → preload other files
            goto onceAgain;
        newLoadedStruc();

        auto tempFont = installTempFontRel(
                family.text, family.flags.have(uc::Fafg::DEHINT_DOTC), trigger);
        q.loaded->tempId = tempFont.id;
        q.loaded->familiesComma = tempFont.families.join(',');
        q.loaded->families = std::move(tempFont.families);
        if (q.loaded->families.empty()) {
            q.loaded->isRejected = true;
            q.isRejected = true;
            return;
        }

        q.loaded->cps = std::move(tempFont.cps);
    } else {
        // FAMILY
        newLoadedStruc();
        q.loaded->familiesComma = str::toQ(family.text);
        q.loaded->families = toQList(family.text);
    }

    // Does not support → make probe font, force EXACT match
    if (q.loaded->cps.isEmpty()) {
        q.loaded->get(q.loaded->probe,  fst::TOFU,   flags);
        q.loaded->probeMetrics = std::make_unique<QFontMetrics>(*q.loaded->probe);
        doesSupportChar(trigger);
        if (family.probeChar && !doesSupportChar(family.probeChar.value)) {
            q.loaded->isRejected = true;
            q.isRejected = true;
            return;
        }
    }
    q.loaded->get(q.loaded->normal, fst::DEFAULT, flags);
}


QFont uc::Font::get(FontPlace place, int size, Flags<uc::FontGetFg> flags,
                    const uc::Cp* subj) const
{
    load(subj ? subj->subj.ch32() : NO_TRIGGER);
    if (!q.loaded || !q.loaded->normal)
        return {};
    QFont font = *q.loaded->normal;
    font.setPointSize(computeSize(place, size));
    auto oldStrategy = font.styleStrategy();
    auto strategy = oldStrategy;
    if (place == FontPlace::CELL && flags.have(uc::FontGetFg::NO_AA)) {
        strategy = fst::NO_AA;
    }
    if (flags.have(uc::FontGetFg::KNOWN_TOFU) && subj && !doesSupportChar(subj->subj)) {
        strategy = QFont::StyleStrategy(strategy | QFont::StyleStrategy::NoFontMerging);
    }
    if (subj && subj->flags.have(uc::Cfg::M_NO_SHAPING)) {
        strategy = QFont::StyleStrategy(strategy | QFont::StyleStrategy::PreferNoShaping);
    }
    if (strategy != oldStrategy) {
        font.setStyleStrategy(strategy);
    }
    return font;
}


const QString& uc::Font::familiesComma() const
{
    load(NO_TRIGGER);
    if (q.isRejected) {
        // FONT REJECTED
        auto that = this;
        ++that;
        return that->familiesComma();
    } else {
        // Font normal
        return q.loaded->familiesComma;
    }
}


bool uc::Font::doesSupportChar(char32_t subj) const
{
    // Font rejected → we support nothing
    if (q.isRejected)
        return false;

    // Check recoding table: if recoded, we clearly support it!
    if (family.recode) {
        auto a = family.recode(subj);
        if (a != subj)
            return true;
    }
    // Then load and check using one of methods:
    // rawFont or probeMetrics
    load(subj);
    if (q.loaded->isRejected) {
        q.isRejected = true;
        return false;
    }
    if (q.loaded->cps.hasSmth()) {
        return q.loaded->cps.have(subj);
    } else if (q.loaded->probeMetrics) {
        return q.loaded->probeMetrics->inFontUcs4(subj);
    } else {
        return false;
    }
}

int uc::Font::computeSize(FontPlace place, int size) const
{
    auto r = sizeAdjust.apply(size);
    switch (place) {
    case FontPlace::CELL:
        if (flags.have(Ffg::CELL_SMALLER))
            r = r * 9 / 10;     // =90%
        if (flags.have(Ffg::CELL_BIGGER))
            r = r * 23 / 20;    // =115%
        break;
    case FontPlace::SAMPLE:
    case FontPlace::PROBE: break;
    }
    return r;
}


std::u8string_view uc::Cp::autoName() const
{
    switch (ecScript) {
    case uc::EcScript::Egyp:
        return u8"Egyptian hieroglyph-#"sv;         // ONE
    case uc::EcScript::Hani: {
            char32_t cp = subj;
            if ((cp >= 0xF900 && cp <= 0xFAFF)
                || (cp >= 0x2F800 && cp <= 0x2FA1F))
                return u8"CJK compatibility ideograph-#"sv;  // TWO
            return u8"CJK unified ideograph-#"sv;   // Does not count (real auto, special DB)
        }
    case uc::EcScript::Kits:
        return u8"Khitan small script character-#"sv;   // THREE
    case uc::EcScript::Nshu:
        return u8"Nushu character-#";   // FOUR
    case uc::EcScript::Tang:
        return u8"Tangut ideograph-#"sv;    // Does not count (real auto, AFAIK no data at all)
    default: throw std::logic_error("No auto name!");
    }
    static_assert(uc::N_AUTO_NAMES == 4, "Check auto-names.log");
}


std::u8string_view uc::Cp::traverseTexts(
        AutoName aut, const TextSink& sink) const
{
    auto checkAutoName =
            [this, &sink, needMain = static_cast<bool>(aut)]
            () mutable -> std::u8string_view {
        if (needMain) {
            needMain = false;
            auto name = autoName();
            if (sink.onText(TextRole::MAIN_NAME, name) == Action::STOP)
                return name;
        }
        return {};
    };
    // }
    //                      || info.name == "<CJK Ideograph Extension J, First>"sv) {
    //     firstCp = info.cp;
    //     needNumeric = true;
    //     repeatedText = "CJK unified ideograph-#"sv;  // Let it be for Hangul
    // } else if (info.name == "<Tangut Ideograph, First>"sv
    //         || info.name == "<Tangut Ideograph Supplement, First>"sv) {
    //     firstCp = info.cp;
    //     needNumeric = false;
    //     repeatedText =


    auto p = allStrings + name.iTech.val();
    while (true) {
        auto role = static_cast<TextRole>(*(p++));
        switch (role) {
        // Commands
        case TextRole::CMD_END:
            if (auto q = checkAutoName(); !q.empty())
                return q;
            return {};
        // Actual texts        
        case TextRole::ALT_NAME:
        case TextRole::HTML:
        case TextRole::DEP_INSTEAD:
        case TextRole::DEP_INSTEAD2:
        case TextRole::ABBREV:
        case TextRole::EMOJI_NAME:
        case TextRole::EGYP_EWP:
        case TextRole::EGYP_UC:
            if (auto q = checkAutoName(); !q.empty())
                return q;
            [[fallthrough]];
        case TextRole::MAIN_NAME: {
                unsigned length = *(p++);
                // 2-byte string?
                if (length >= uc::detail::MIN_2BYTE_STRING) {
                    length = ((length - uc::detail::MIN_2BYTE_STRING) << 8)
                             | static_cast<unsigned>(*(p++));
                }
                std::u8string_view text {p, length};
                auto action = sink.onText(role, text);
                if (action == Action::STOP)
                    return text;
                p += length;
            } continue;
        }
        // Something’s really bad!
        static char c[80];
        snprintf(c, std::size(c), "[Wrong command %d]", int(role));
        std::u8string_view sv = str::toU8sv(c);
        sink.onText(TextRole::MAIN_NAME, sv);
        return sv;
    }
}


std::u8string_view uc::Cp::getText(TextRole role) const
{
    auto autoName = (role == TextRole::MAIN_NAME) ? AutoName::YES : AutoName::NO;
    return traverseTextsT(autoName,
        [role](TextRole aRole, std::u8string_view) {
            return static_cast<Action>(role == aRole);
        });
}


std::u8string_view uc::Cp::techName() const
{
    return getText(TextRole::MAIN_NAME);
}


QString uc::Cp::viewableName() const
{
    QString r = str::toQ(techName());
    if (auto pos = r.indexOf('#'); pos >= 0) {
        char buf[30];
        snprintf(buf, std::size(buf), "%04X", static_cast<unsigned>(subj.uval()));
        r.replace(pos, 1, buf);
    }
    return r;
}


std::u8string_view uc::Cp::abbrev() const
{
    if (!isAbbreviated())
        return {};
    return getText(TextRole::ABBREV);
}


QString uc::Cp::markProxy() const
{
    auto sc = script();
    if (sc.flags.have(Sfg::STUB_RTL))
        return str::toQ(STUB_RTL_CIRCLE) + str::toQ(subj);
    if (sc.flags.have(Sfg::STUB_ALM))
        return str::toQ(STUB_ALM_CIRCLE) + str::toQ(subj);
    return STUB_CIRCLE + str::toQ(subj);
}


uc::SampleProxy uc::Cp::sampleProxy(
        ProxyType proxyType,
        EmojiDraw emojiDraw,
        const uc::GlyphStyleSets& glyphSets) const
{
    switch (drawMethod(emojiDraw, glyphSets)) {
    case DrawMethod::MARCHEN:
    case DrawMethod::SAMPLED_CONTROL:
        if (proxyType == uc::ProxyType::EXTENDED)
            break;  // go through on extended only
        return {};
    case DrawMethod::SAMPLE:
        break;  // go through
    default:
        return {};
    }

    auto fn = font(match::Normal::INST);
    auto style = fn->styleSheet;
    auto code = subj.ch32();

    if (fn->family.recode) {
        code = fn->family.recode(code);
    }

    if (fn->flags.have(Ffg::STUB_FINEGRAINED)) {
        // Special stubs char-by char, enabled by STUB_FINEGRAINED flag
        switch (code) {
        case 0x1BC9E:   // Duployan shorthand
            return { NBSP + str::toQ(code) + NBSP + NBSP + NBSP + NBSP, style };
        case 0x1CF42:   // Znamenny
        case 0x1CF43:
        case 0x1CF44:
        case 0x1CF45:
        case 0x1CF46:
            return { str::toQ(code), style };
        }
    }

    // Recode according to glyph style
    // Assume style 0 always exists in fonts, even if it’s actually other style!
    auto channel = ecStyleChannel();
    // Have channel?
    if (channel != EcGlyphStyleChannel::NONE) {
        auto setting = glyphSets[channel];
        // Setting OK?
        if (setting < styleChannel().count) {
            auto flag = uc::Cfg::G_STYLE_0 << setting;
            // Have this style?
            if (flags.have(flag)) {
                // Code works for setting == 1 only
                if (setting == 1)
                    code += fn->styleChange.delta;
            }
        }
    }

    switch (ecCategory) {
    case EcCategory::MARK_ENCLOSING:
    case EcCategory::MARK_NONSPACING:
    case EcCategory::MARK_SPACING: {
            // Stub off?
            if (fn->flags.have(Ffg::STUB_OFF))
                break;
            return { markProxy(), style };
        }
    default: ;
    }
    if (fn->flags.have(Ffg::STUB_INTERCHAR))
        return { str::toQ(code) + str::toQ(code), style };
    return { str::toQ(code), style };
}


bool uc::Cp::hasGlyph() const
{
    // All default-ignorable have no glyph, either format or marks…
    if (isDefaultIgnorable())
        return false;
    // Otherwise
    switch (ecCategory) {
    case EcCategory::CONTROL:
    case EcCategory::SEPARATOR_LINE:
    case EcCategory::SEPARATOR_PARAGRAPH:
        return false;   // Surely no glyph (they all have abbreviation)
    case EcCategory::FORMAT: // check for abbreviation
        return getText(TextRole::ABBREV).empty();
    default:
        return true;
    }
}

namespace {

    bool isFullEmojiByCase(char32_t ch)
    {
        switch (ch) {
        // Misc symbols
        case 0x260E:  // telephone
        case 0x2618:  // shamrock
        case 0x2611:  // ballot with check
        case 0x2622:  // radioactive
        case 0x2623:  // biohazard
        case 0x261D:  // finger up
        case 0x2626:  // Orthodox
        case 0x262A:  // Muslim
        case 0x262E:  // peace
        case 0x262F:  // Dao (yin/yang)
        case 0x2638:  // Hindu
        case 0x2639:  // sad smiley
        case 0x263A:  // smiley
        case 0x2640:  // ♀
        case 0x2642:  // ♂
        case 0x265F:  // black pawn
        case 0x2660:  // ♠
        case 0x2663:  // ♣
        case 0x2665:  // ♥
        case 0x2666:  // ♦
        case 0x2668:  // hot springs
        case 0x267B:  // recycle
        case 0x267E:  // non-acid (permanent) paper
        case 0x2692:  // hammer and pick
        case 0x2694:  // crossed swords
        case 0x2695:  // staff of Aesculapius
        case 0x2696:  // scales
        case 0x2697:  // alembic
        case 0x2699:  // gear
        case 0x269B:  // atom symbol
        case 0x269C:  // fleur-de-lis
        case 0x26A0:  // warning
        case 0x26A7:  // transgender
        case 0x26B0:  // coffin
        case 0x26B1:  // funeral urn
        case 0x26CF:  // pick
        case 0x26D1:  // helmet with white cross
        case 0x26D3:  // chains
        case 0x26E9:  // Shinto shrine
        case 0x26F0:  // mountain
        case 0x26F1:  // umbrella on ground
        case 0x26F4:  // ferry
        case 0x26F7:  // skier
        case 0x26F8:  // ice skate
        case 0x26F9:  // person with ball
            return false;
        default:
            return true;
        }
    }

}   // anon namespace


uc::DrawMethod uc::Cp::drawMethod(
        EmojiDraw emojiMode, const uc::GlyphStyleSets& glyphSets) const
{
    switch ((flags & m::ALL).numeric()) {
    case m::CUSTOM_CONTROL.numeric():
        return uc::DrawMethod::CUSTOM_CONTROL;
    case m::SVG_EMOJI.numeric(): {
            bool isSvg = true;
            switch (emojiMode) {
            case EmojiDraw::FORCE_TEXT:
                isSvg = false;
                break;
            case EmojiDraw::MOSTLY_TEXT:
                isSvg = !isVs16Emoji();
                break;
            case EmojiDraw::CONSERVATIVE: {
                    auto& blk = block();
                    isSvg = !isVs16Emoji()  // Simple emoji w/o VS16 → automatically SVG
                                // Both flags are off → graphic
                            || !blk.flags.haveAny(Bfg::NO_EMOJI | Bfg::EMOJI_BY_CASE)
                                // NO_EMOJI → sample, EMOJI_BY_CASE → check
                            || (blk.flags.have(Bfg::EMOJI_BY_CASE) && isFullEmojiByCase(subj.ch32()));
                } break;
            case EmojiDraw::GRAPHIC:
                break;
            }
            return isSvg ? uc::DrawMethod::SVG_EMOJI : uc::DrawMethod::SAMPLE;
        }
    case m::SPACE.numeric():
        return uc::DrawMethod::SPACE;
    case m::ABBREVIATION.numeric():
        return uc::DrawMethod::ABBREVIATION;
    case m::SAMPLED_CONTROL.numeric():
        return uc::DrawMethod::SAMPLED_CONTROL;
    case m::VIRTUAL_VIRAMA.numeric():
        return uc::DrawMethod::VIRTUAL_VIRAMA;
    default:;
    }

    if (glyphSets[EcGlyphStyleChannel::VERTICAL] != 0
            && block().ecStyleChannel == EcGlyphStyleChannel::VERTICAL) {
        return block().flags.have(uc::Bfg::CCW)
                ? uc::DrawMethod::VERTICAL_CCW : uc::DrawMethod::VERTICAL_CW;
    }
    // Vertical scripts do not have spaces, so OK
    if (isTrueSpace()) {
        return uc::DrawMethod::SPACE;
    }
    switch (ecCategory) {
    case uc::EcCategory::CONTROL:
    case uc::EcCategory::FORMAT:   // These are “sampled control”
        return uc::DrawMethod::SAMPLED_CONTROL;
    case uc::EcCategory::MARK_NONSPACING:
        // Marchen
        if (ecScript == uc::EcScript::Marc
                && subj.ch32() <= 0x11CB0) {
            return uc::DrawMethod::MARCHEN;
        }
        break;
    default: ;
    }
    return uc::DrawMethod::SAMPLE;
}


QString uc::Cp::osProxy() const
{
    if (!hasGlyph())
        return {};

    switch (category().upCat) {
    case EcUpCategory::MARK:
        return markProxy();
    default: ;
    }
    return str::toQ(subj.ch32());
}


uc::EcScript uc::Cp::ecScriptEx() const
{
    if (ecScript != EcScript::NONE && ecScript != EcScript::Zinh)
        return ecScript;
    return block().ecScript;
}


const uc::Font& uc::Cp::firstFont() const
{
    // Priority: block → script — block’s script
    auto& blk = block();
    // Block
    if (blk.flags.have(Bfg::UNGLITCH_MARKS) && category().upCat == EcUpCategory::MARK) {
        // All enclosing marks are in Funky now,
        // and the rest are still in Noto
        if (ecCategory == EcCategory::MARK_ENCLOSING)
            return fontInfo[static_cast<int>(EcFont::FUNKY)];
        return fontInfo[static_cast<int>(EcFont::NOTO)];
    }
    auto hfont = blk.ecFont;
    if (hfont != EcFont::NORMAL || blk.flags.have(Bfg::FORCE_FONT)) {
        return fontInfo[static_cast<int>(hfont)];
    }
    // Script
    if (ecScript != EcScript::NONE && ecScript != EcScript::Zinh) {
        auto& si = scriptInfo[static_cast<int>(ecScript)];
        if (si.ecFont != EcFont::NORMAL)
            return si.font();
    }
    // Block’s script
    return blk.script().font();
}


const uc::Font* uc::Cp::font(const FontMatcher& matcher) const
{
    auto sb = subj.ch32();
    // if (subj.ch32() == 0xFFFFFF) {
    //     std::cout << "Debug here!" << std::endl;
    // }
    auto v = &firstFont();
    bool isBuggy = flags.have(Cfg::G_RENDER_BUG);
    while (v->flags.have(Ffg::FALL_TO_NEXT)) {
        auto wantSkip = isBuggy
                ? v->flags.have(Ffg::BUG_AVOID)     // BUGGY: avoid flag → bad, it’s for normal only
                : v->flags.have(Ffg::BUG_FIXUP);    // NORMAL: fixup flag → bad, it’s for buggy only
        if (category().upCat == uc::EcUpCategory::MARK) {
            wantSkip |= v->flags.have(Ffg::MARK_AVOID);
        }
        if (!wantSkip) {
            if (matcher.check(sb, *v))
                return v;
        }
        // Once again if accepted and that flag is present
        if (!v->q.isRejected && v->flags.have(Ffg::FALL2_IF_ACCEPTED)) {
            ++v;
            // Check for that font’s flag: none → STOP
            // (“system → backup” pair, currently unused, maybe Lontara?)
            // e.g. have system → never check backup
            if (!v->flags.have(Ffg::FALL_TO_NEXT)) {
                // what the frag to do?
                return matcher.lastHopeMatch(sb, *v);
            }
        }
        ++v;
    }
    return matcher.lastHopeMatch(sb, *v);
}


bool uc::Cp::isMark() const noexcept
{
    return (category().upCat == uc::EcUpCategory::MARK);
}


bool uc::Cp::isVs15Emoji() const noexcept
{
    return ((flags & m::ALL) == m::SVG_EMOJI)
        && !isVs16Emoji()
        && (ecCategory == uc::EcCategory::SYMBOL_OTHER);
}


uc::TofuInfo uc::Cp::tofuInfo(SvgChecker& svgChecker) const
{
    uc::TofuInfo r;
    r.block = &block();
    if (r.block->flags.haveAny(Bfg::CJK)
            || script().ecContinent == EcContinent::CJK)
        r.place = TofuPlace::CJK;

    auto method = drawMethod(EmojiDraw::CONSERVATIVE, uc::GlyphStyleSets::EMPTY);
    if (method > uc::DrawMethod::LAST_FONT) {
        if (method == uc::DrawMethod::SVG_EMOJI) {
            r.state = svgChecker.canDraw(subj)
                      ? TofuState::NO_FONT : TofuState::TOFU;
        } else {
            r.state = TofuState::NO_FONT;
        }
    } else {
        r.state = font(match::NullForTofu::INST) ? TofuState::PRESENT : TofuState::TOFU;
    }
    return r;
}


uc::EcGlyphStyleChannel uc::Cp::ecStyleChannel() const
{
    if (!hasStyle())
        return uc::EcGlyphStyleChannel::NONE;
    return block().ecStyleChannel;
}


const uc::Block* uc::blockOf(char32_t subj)
{
    unsigned index = subj >> 4;
    if (index >= std::size(blocksByCode16))
        return nullptr;
    auto q = blocksByCode16[index];
    return (q >= 0) ? &blocks[q] : nullptr;
}


size_t uc::Block::permanentIndex() const
{
    return this - blocks;
}


unsigned uc::Block::nNonChars() const
{
    unsigned r = 0;
    if (flags.have(Bfg::HAS_32_NONCHARS))
        r += 32;
    if ((endingCp & 0xFFFF) == 0xFFFF)
        r += 2;
    return r;
}


void uc::Block::resizeHistory(const BlockResizeSink& x) const
{
    if (history.ecVersion != EcVersion::NO_VALUE) {
        x.act(history.ecVersion, history.oldEndingCp, endingCp);
    }
}


void uc::Block::printfLocKeyN(char* buf, size_t n, const char* suffix) const
{
    snprintf(buf, n, "Block.%04X.%s", (int)startingCp, suffix);
}


void uc::Script::printfLocKeyN(char* buf, size_t n, const char* suffix) const
{
    snprintf(buf, n, "Script.%s.%s", id.data(), suffix);
}


void uc::Category::printfLocKeyN(char* buf, size_t n, const char* suffix) const
{
    snprintf(buf, n, "CharCat.%s.%s", id.data(), suffix);
}

void uc::Term::printfLocKeyN(char* buf, size_t n, const char* suffix) const
{
    snprintf(buf, n, "Term.%s.%s", key.data(), suffix);
}

const uc::Term* uc::findTerm(std::string_view id)
{
    for (auto& v : terms) {
        if (v.key == id)
            return &v;
    }
    return nullptr;
}


const uc::Version* uc::findVersion(std::string_view id)
{
    auto id1 = str::toU8sv(id);
    if (id1.length() < 2)
        return nullptr;
    if (id1.starts_with(u8'e')) {
        // Find emoji
        id1 = id1.substr(1);
        for (auto& v : versionInfo) {
            if (v.emojiName == id1) {
                return &v;
            }
        }
    } else {
        // Find Unicode
        for (auto& v : versionInfo) {
            if (v.unicodeName == id1) {
                return &v;
            }
        }
    }
    return nullptr;
}


QFont uc::funkyFont(FontPlace place, int size)
{
    auto& font = fontInfo[static_cast<int>(uc::EcFont::FUNKY)];
    return font.get(place, size, NO_FLAGS, nullptr);
}


namespace {

    bool isTermLess(const uc::Term* x, const uc::Term* y)
    {
        return std::lexicographical_compare(
                    std::begin(x->loc.sortKey), std::end(x->loc.sortKey),
                    std::begin(y->loc.sortKey), std::end(y->loc.sortKey));
    }

    void sortTerms(const uc::Term** beg, const uc::Term** end)
    {
        if ((*beg)->cat().sort != uc::SortTerms::NO) {
            std::stable_sort(beg, end, isTermLess);
        }
    }

    // opening parenthesis
    // en dash
    enum class SpecialSort { NO, YES };

    constexpr std::u32string_view STOP_CHARS = U"(—";

    void buildSortKey(std::u32string_view name32, SpecialSort specialSort,
                      const std::unordered_map<char32_t, int>& sortOrder,
                      uc::LocSortKey& r,
                      uc::Block::Loc::Telltales& telltales)
    {
        // Trim by special shars — only if subkey is set (manual sorting)
        if (specialSort != SpecialSort::NO) {
            auto index = name32.find_first_of(STOP_CHARS);
            if (index != std::u32string::npos)
                name32 = name32.substr(0, index);
        }

        std::fill(std::begin(r), std::end(r), -1);
        size_t len = 0;
        for (auto v : name32) {
            auto it = sortOrder.find(v);
            if (it != sortOrder.end()) {
                r[len] = it->second;
                if ((++len) >= std::size(r))
                    break;
            } else {
                if (len == 0) {
                    telltales.isFirstCharBad = true;
                }
            }
        }
    }

    void buildSortKey(std::u8string_view x, SpecialSort specialSort,
                      const std::unordered_map<char32_t, int>& sortOrder,
                      uc::LocSortKey& r, uc::Block::Loc::Telltales& telltales)
    {
        auto name32 = mojibake::toS<std::u32string>(x);
        return buildSortKey(name32, specialSort, sortOrder, r, telltales);
    }

    constinit const uc::Block::Loc::Telltales NO_TELLTALES;

}   // anon namespace


void uc::finishTranslation(
        const std::unordered_map<char32_t, int>& sortOrder,
        loc::EngTerms engTerms,
        std::u32string_view ellipsisBlocks,
        const std::unordered_map<char32_t, std::u32string>& alphaFixup)
{
    char c[40];

    for (unsigned i = 0; i < uc::N_SCRIPTS; ++i) {
        auto& sc = uc::scriptInfo[i];
        if (!sc.flags.have(Sfg::SORT_KEY)) {
            sc.printfLocKey(c, "Name");
            sc.loc.name = loc::get(c);
        }
        if (!sc.flags.have(Sfg::NO_LANGS)) {
            sc.printfLocKey(c, "Lang");
            sc.loc.langs = loc::get(c);
        }
        if (sc.time.needsCustomNote()) {
            sc.printfLocKey(c, "Note");
            sc.loc.timeComment = loc::get(c);
        }
        if (sc.flags.have(Sfg::DESC_FROM_PREV)) {
            auto p = &sc;
            --p;
            sc.loc.description = p->loc.description;
        } else {
            sc.printfLocKey(c, "Text");
            sc.loc.description = loc::get(c);
        }
    }

    // Blocks, pass 1 (retrieve keys)
    for (auto& blk : allBlocks()) {
        blk.printfLocKey(c, "Name");
        blk.loc.name = loc::get(c);
        blk.loc.hasEllipsis = std::binary_search(
                    ellipsisBlocks.begin(), ellipsisBlocks.end(), blk.startingCp);

        if (blk.hasDescription()) {
            blk.printfLocKey(c, "Text");
            blk.loc.description = loc::get(c);
        }
    }

    // Blocks, pass 2 (build sort order)
    for (auto& blk : allBlocks()) {
        blk.loc.telltales = NO_TELLTALES;
        auto& script = scriptInfo[static_cast<int>(blk.alphaKey.ecScript)];

        // Sorting key        
        if (auto q = alphaFixup.find(blk.startingCp); q != alphaFixup.end()) {
            buildSortKey(q->second, SpecialSort::NO, sortOrder,
                         blk.loc.sortKey, blk.loc.telltales);
        } else {
            std::u8string_view keyName = blk.loc.name;
            if (blk.alphaKey.ecScript != EcScript::NONE) {
                keyName = script.loc.name;
                if (script.mainBlock) {
                    if (auto q = alphaFixup.find(script.mainBlock->startingCp); q != alphaFixup.end()) {
                        buildSortKey(q->second, SpecialSort::NO, sortOrder,
                                     blk.loc.sortKey, blk.loc.telltales);
                        goto alreadyBuilt;
                    }
                    keyName = script.mainBlock->loc.name;
                }
            }
            buildSortKey(keyName, SpecialSort::NO, sortOrder,
                         blk.loc.sortKey, blk.loc.telltales);
        alreadyBuilt: ;
        }
    }

    for (auto& bidi : bidiClassInfo) {
        bidi.printfLocKey(c, "Name");
            bidi.loc.name = loc::get(c);
        bidi.printfLocKey(c, "Short");
            bidi.loc.shortName = loc::get(c);
        bidi.printfLocKey(c, "Text");
            bidi.loc.description = loc::get(c);
    }

    for (auto& cat : categoryInfo) {
        cat.printfLocKey(c, "Name");
            cat.loc.name = loc::get(c);
        cat.printfLocKey(c, "Text");
            cat.loc.description = loc::get(c);
    }

    auto pSorted = std::begin(sortedTerms);
    for (auto& term : terms) {
        // Get name
        term.printfLocKey(c, "Name");
        term.loc.name = loc::get(c);

        // Build sort key
        // Terms have no telltales, but for some languages we show English terms more aggressively
        Block::Loc::Telltales dummyTelltales;
        std::u8string_view sortName = (engTerms == loc::EngTerms::SORT_KEY)
                ? term.engName : term.loc.name;
        buildSortKey(sortName, SpecialSort::YES, sortOrder,
                     term.loc.sortKey, dummyTelltales);

        // Get desc
        if (term.borrowedDesc.empty()) {
            term.printfLocKey(c, "Text");
            term.loc.description = loc::get(c);
        } else {
            term.loc.description = loc::get(term.borrowedDesc);
        }

        // Add to sorted
        *(pSorted++) = &term;
    }

    auto pBeg = std::begin(sortedTerms);
    auto oldCat = (*pBeg)->ecCat;
    for (auto& pt : sortedTerms) {
        auto newCat = pt->ecCat;
        if (newCat != oldCat) {
            sortTerms(pBeg, &pt);
            pBeg = &pt;
            oldCat = newCat;
        }
    }
    sortTerms(pBeg, std::end(sortedTerms));
}


uc::FracType uc::Numeric::fracType() const
{
    if (!isPresent())
        return FracType::NONE;
    if (denom == 1)
        return FracType::INTEGER;
    // Only 1/2 is known among negative fractions
    // Only #/2 is known among top-heavy fractions
    if (num > denom && denom == 2)
        return FracType::DECIMAL;
    return FracType::VULGAR;
}


size_t uc::appendUPLUS(char* buf, size_t n, size_t n1, char32_t code)
{
    if (n1 == 0)
        return sprintUPLUS(buf, n, code);

    if (n1 + 1 >= n)
        return n1;

    size_t nPrinted = snprintf(
                buf + n1,   // buffer
                n - n1,     // size
                "+%04X",    // fmt
                static_cast<unsigned>(code));   // params
    return n1 + nPrinted;
}


size_t uc::appendPlus(char* buf, size_t n, size_t n1, char32_t code)
{
    if (n1 == 0)
        return sprint(buf, n, code);

    if (n1 + 1 >= n)
        return n1;

    size_t nPrinted = snprintf(
                buf + n1,   // buffer
                n - n1,     // size
                "+%04X",    // fmt
                static_cast<unsigned>(code));   // params
    return n1 + nPrinted;
}


size_t uc::sprintPlus(char* buf, size_t n, std::u32string_view text)
{
    *buf = 0;
    size_t pos = 0;
    for (auto c : text) {
        pos = uc::appendPlus(buf, n, pos, c);
    }
    return pos;
}


size_t uc::LibNode::sprintUPLUS(char* data, size_t n) const
{
    *data = 0;
    size_t pos = 0;
    for (auto c : value) {
        pos = uc::appendUPLUS(data, n, pos, c);
    }
    return pos;
}


QString uc::LibNode::viewableTitle(TitleMode mode) const
{
    if (flags.have(uc::Lfg::TRANSLATE)) {
        /// @todo [future] We do not translate characters
        auto key = str::cat("Lib.Group.", str::toSv(text));
        return loc::get(key).q();
    } else if (mode == TitleMode::SHORT && flags.have(uc::Lfg::CODE_AS_NAME)) {
        char data[50];
        sprintUPLUS(data);
        return data;
    } else if (!text.empty()) {
        return str::toQ(text);
    } else if (!value.empty()) {
        if (auto cp = uc::cpsByCode[value[0]])
            return cp->viewableName();
    }
    // Should not happen
    return {};
}


uc::EmojiDraw uc::LibNode::emojiDraw() const
{
    if (value.length() == 2 && value[1] == cp::VS15)
        return EmojiDraw::FORCE_TEXT;
    return flags.have(Lfg::GRAPHIC_EMOJI) ? EmojiDraw::GRAPHIC : EmojiDraw::MOSTLY_TEXT;
}


const uc::Version& uc::LibNode::emojiPrevVersion() const
{
    if (ecEmojiVersion <= uc::EcVersion::FIRST_MEANING) {
        return versionInfo[0];
    }
    return versionInfo[static_cast<int>(ecEmojiVersion) - 1];
}


#define TEXT_ALPHA  u8"\u00A0" "α"
#define TEXT_BETA   u8"\u00A0" "β"

inline void appendState(std::u8string& s, const uc::Version& version)
{
    if constexpr (uc::LATEST_STATE != uc::LatestState::RELEASE) {
        static constexpr std::u8string_view SUFFIX = (uc::LATEST_STATE == uc::LatestState::ALPHA)
                ? u8"\u00A0" "α" : u8"\u00A0" "β";
        if (&version == uc::versionInfo + static_cast<int>(uc::EcVersion::LAST))
            s += SUFFIX;
    }
}

std::u8string uc::Version::locName() const
{
    std::u8string r;
    if (!unicodeName.empty()) {
        r = unicodeName;
        appendState(r, *this);
    } else {
        r = emojiName;
        appendState(r, *this);
        r = loc::get("Prob.Bullet.EmojiV").arg(r);
    }
    return r;
}


std::u8string uc::Version::termName() const
{
    std::u8string r;
    if (!unicodeName.empty()) {
        r = unicodeName;
        appendState(r, *this);
    } else {
        r = emojiName;
        appendState(r, *this);
        r = loc::get("TermCat.Ev").arg(r);
    }
    return r;
}


std::u8string uc::Version::techName() const
{
    if (!unicodeName.empty()) {
        return str::cat(u8"v", unicodeName);
    } else {
        return str::cat(u8"e", emojiName);
    }
}

std::u8string uc::Version::link(std::u8string_view prefix) const
{
    if (!unicodeName.empty()) {
        return str::cat(prefix, unicodeName);
    } else {
        return str::cat(prefix, u8'e', emojiName);
    }
}


std::u8string uc::Version::link() const
{
    if (!unicodeName.empty()) {
        return std::u8string(unicodeName);
    } else {
        return str::cat(u8'e', emojiName);
    }
}


std::u8string uc::Version::locLongName() const
{
    std::u8string r;
    const char* key;
    if (!unicodeName.empty()) {
        r = unicodeName;
        key = "Prop.Head.Uc";
    } else {
        r = emojiName;
        key = "Prop.Head.Em";
    }
    appendState(r, *this);
    r = loc::get(key).arg(r);
    return r;
}


///// Font matchers ////////////////////////////////////////////////////////////


const uc::Font* uc::FontMatcher::lastHopeMatch(char32_t cp, const uc::Font& font) const
{
    if (check(cp, font))
        return &font;
    return nullptr;
}


bool match::MainFont::check(char32_t, const uc::Font& font) const
{
    if (font.flags.haveAny(uc::Ffg::BUG_FIXUP | uc::Ffg::DESC_AVOID))
        return false;
    font.load(NO_TRIGGER);  // font probably won’t support cp
    return true;
}


bool match::Normal::check(char32_t cp, const uc::Font& font) const
{
    if (!font.flags.have(uc::Ffg::FALL_TO_NEXT))
        return true;
    return font.doesSupportChar(cp);
}


bool match::NullForTofu::check(char32_t cp, const uc::Font& font) const
{
    return font.doesSupportChar(cp);
}


bool match::FallToLast::check(char32_t, const uc::Font& font) const
{
    return !font.flags.have(uc::Ffg::FALL_TO_NEXT);
}


///// StyleSheet ///////////////////////////////////////////////////////////////


QString uc::StyleSheet::toQ() const
{
    QString r;
    char buf[40];
    if (topPc != 0) {
        snprintf(buf, std::size(buf), "padding-top: %d%%;", int(topPc));
        r += buf;
    }
    if (botPc != 0) {
        snprintf(buf, std::size(buf), "padding-bottom: %d%%;", int(botPc));
        r += buf;
    }
    return r;
}


QRectF uc::StyleSheet::applyToGraphicSample(const QRect& rect, qreal pointSize) const
{
    QRectF r = rect;
    if (topPc != 0) {
        r.setTop(r.top() - pointSize * topPc / 100.0);
    }
    if (botPc != 0) {
        r.setBottom(r.bottom() + pointSize * botPc / 100.0);
    }
    return r;
}
