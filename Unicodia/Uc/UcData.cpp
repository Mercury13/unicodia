#include "UcData.h"

// Qt
#include <QFontDatabase>
#include <QFontMetrics>
#include <QRawFont>

// Libs
#include "i_TempFont.h"
#include "u_Strings.h"
#include "u_Qstrings.h"
#include "u_Cmap.h"
#include "mojibake.h"

// Unicode
#include "UcCp.h"
#include "UcSkin.h"

// L10n
#include "LocDic.h"
#include "u_Decoders.h"


#define NBSP "\u00A0"

using ReverseMap = Cmap<char16_t, unsigned char, 128>;
template class Cmap<char16_t, unsigned char, 128>;

using namespace std::string_view_literals;
const uc::Cp* uc::cpsByCode[CAPACITY];
short uc::blocksByCode16[CAPACITY >> 4];
const QString uc::Font::qempty;
const uc::GlyphStyleSets uc::GlyphStyleSets::EMPTY;
constinit const uc::InputMethods uc::InputMethods::NONE {};

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

constinit const uc::LangLife uc::langLifeInfo[] {
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
    { "Prop.Life.Exp" },
    //{ "Error" },
};
static_assert (std::size(uc::langLifeInfo) == static_cast<size_t>(uc::EcLangLife::NN), "langLifeInfo size?");


constinit const uc::WritingDir uc::writingDirInfo[static_cast<int>(EcWritingDir::NN)] {
    { {} },
    { "Prop.Dir.L" },
    { "Prop.Dir.R" },
    { "Prop.Dir.L_cjk" },
    { "Prop.Dir.L_cunei" },
    { "Prop.Dir.L_ogam" },
    { "Prop.Dir.L_col" },
    { "Prop.Dir.TD" },
    { "Prop.Dir.Both" },
    { "Prop.Dir.Tang" },
    { "Prop.Dir.R_col" },
    { "Prop.Dir.R_most" },
    { "Prop.Dir.Sogd" },
    //{ "Error" },
};


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
    { u8"3.1",  { 2001, Month::MAR }, NO_FLAGS },
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
    { u8"16.0", { 2024, Month::SEP }, Vfg::TEXT | Vfg::BETA },
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

constexpr uc::NumType numTypeRawInfo[] {
    { {} },
    { "Prop.Num.Digit" },
    { "Prop.Num.SpecDigit" },
    { "Prop.Num.Number" },
    { "Prop.Num.CjkRare" },
    { "Prop.Num.CjkAccount" },
    { "Prop.Num.CjkZhuang" },
    { "Prop.Num.CjkVietnamese" },
};

constinit const ec::Array<uc::NumType, uc::EcNumType> uc::numTypeInfo {
    ec::ARRAY_INIT, numTypeRawInfo
};


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
    { "winemoji", EcTermCat::INPUT, {} },
    { "altcode", EcTermCat::INPUT, u8"Alt codes" },
    { "appentry", EcTermCat::INPUT, {} },
    { "birman", EcTermCat::INPUT, {} },

    { "hiero", EcTermCat::SCRIPT_CLASS, u8"ideographic script" },
    { "abjad", EcTermCat::SCRIPT_CLASS, u8"consonant script, abjad" },
    { "syllabic", EcTermCat::SCRIPT_CLASS, u8"syllabic script" },
    { "abugida", EcTermCat::SCRIPT_CLASS, u8"abugida", {}, EcFont::ETHIOPIC },
    { "brahmic", EcTermCat::SCRIPT_CLASS, u8"Brahmic abugida, Indic script", {}, EcFont::DEVANAGARI_SYSTEM },
    { "alphabet", EcTermCat::SCRIPT_CLASS, u8"alphabet script" },
    { "arguable", EcTermCat::SCRIPT_CLASS, {} },
    { "cjk", EcTermCat::SCRIPT_CLASS, u8"CJK = Chinese, Japanese, Korean" },
    { "code", EcTermCat::SCRIPT_CLASS, u8"codes and shorthand" },
    { "nontext", EcTermCat::SCRIPT_CLASS, u8"non-text notation" },

    { "acrophonic", EcTermCat::CHAR_CLASS, u8"acrophonic character" },
    { "virama", EcTermCat::CHAR_CLASS, u8"virama", {}, EcFont::DEVANAGARI_SYSTEM },
    { "graphic", EcTermCat::CHAR_CLASS, u8"graphic character, printable character" },
    { "deprecated", EcTermCat::CHAR_CLASS, u8"deprecated character", {}, {},
            { .fgs = uc::Cfg::U_DEPRECATED } },
    { "ignorable", EcTermCat::CHAR_CLASS, u8"default-ignorable character", {}, {},
            { .fgs = uc::Cfg::U_DEF_IGNORABLE } },
    { "combining", EcTermCat::CHAR_CLASS, u8"combining mark", {}, {},
            { .ecUpCat = uc::EcUpCategory::MARK } },
    { "ligature", EcTermCat::CHAR_CLASS, u8"ligature" },
    { "precomposed", EcTermCat::CHAR_CLASS, u8"precomposed character" },
    { "space", EcTermCat::CHAR_CLASS, u8"whitespace", { "CharCat.Zs.Text" }, {},
            { .ecCategory = uc::EcCategory::SEPARATOR_SPACE } },
    { "spacing", EcTermCat::CHAR_CLASS, u8"spacing character" },
    { "pseudographics", EcTermCat::CHAR_CLASS, u8"pseudographics, semigraphics, box drawing" },
    { "control", EcTermCat::CHAR_CLASS, u8"control character", { "CharCat.Cc.Text" }, {},
            { .ecCategory = uc::EcCategory::CONTROL} },
    { "obsolete", EcTermCat::CHAR_CLASS, u8"obsolete character" },
    { "format", EcTermCat::CHAR_CLASS, u8"format character", { "CharCat.Cf.Text" }, {},
            { .ecCategory = uc::EcCategory::FORMAT } },
    { "number", EcTermCat::CHAR_CLASS, u8"number character", {}, {},
            { .ecUpCat = uc::EcUpCategory::NUMBER } },
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

    { "ascii", EcTermCat::ENCODING, u8"ASCII", { "Block.0000.Text" } },
    { "bmp", EcTermCat::ENCODING, u8"BMP = Basic Multilingual Plane" },
    { "noncharacter", EcTermCat::ENCODING, u8"non-character" },
    { "codepoint", EcTermCat::ENCODING, u8"code point" },
    { "latin1", EcTermCat::ENCODING, u8"Latin-1", { "Block.0080.Text" } },
    { "plane", EcTermCat::ENCODING, u8"plane" },
    { "private", EcTermCat::ENCODING, u8"private-use character" },
    { "reject", EcTermCat::ENCODING, {} },
    { "zwj", EcTermCat::ENCODING, u8"ZWJ = zero width joiner", {}, EcFont::DEVANAGARI_SYSTEM },
    { "zwnj", EcTermCat::ENCODING, u8"ZWNJ = zero width non-joiner", {}, EcFont::DEVANAGARI_SYSTEM },

    { "bidi", EcTermCat::ALGORITHM, u8"bidirectional text" },
    { "normalization", EcTermCat::ALGORITHM, u8"normalization" },
    { "casefolding", EcTermCat::ALGORITHM, u8"case folding" },

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
    { { { 0xFF, 0xFF, 0xFF }, { 0x00, 0x00, 0x00 } }, { NO_COLOR, NO_COLOR }, true },
    // Tech
    { { { 0x22, 0x55, 0x00 }, { 0x7F, 0xFF, 0x2A },
            // Special Technical’s frame colour
            { 0x66, 0xff, 0x00 } },
            { NO_COLOR, NO_COLOR }, false },
    // Europe
    { { BG_EUROPE,            FG_EUROPE,           }, { NO_COLOR, NO_COLOR }, false  },
    // Asia
    { { { 0xff, 0xff, 0x83 }, { 0xf5, 0x79, 0x00 } }, { NO_COLOR, NO_COLOR }, false  },
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

constinit const uc::OldCompInfo uc::oldCompInfo[] {
    { u8"Amstrad" NBSP "CPC" },
    { u8"Apple" NBSP "II+" },
    { u8"Mattel" NBSP "Aquaruis" },
    { u8"Atari" NBSP "ST" },
    { u8"Atari" NBSP "400/800" },
    { u8"Minitel" },
    { u8"MSX" },
    { u8"Oric" },
    { u8"Commodore" NBSP "PET/64" },
    { u8"RISC OS" },
    { u8"Sinclair" NBSP "ZX80/81" },
    { {}, "Prop.OldComp.Teletext" },
    { u8"Tandy" NBSP "TRS80" NBSP "Model" NBSP "I/III/4" },
    { u8"Tandy" NBSP "TRS80" NBSP "Color" },
    { {}, "Prop.OldComp.Korvet" },
    { u8"Smalltalk" },
    { u8"Sharp" NBSP "MZ" },
    { u8"Ohio" NBSP "Scientific" },
    { u8"Robotron" NBSP "Z9001" },
    { {}, "Prop.OldComp.Hp" },
    { u8"Kaypro" },
};

constexpr auto I_LAST_OLD_COMP = std::size(uc::oldCompInfo) - 1;
static_assert((1 << I_LAST_OLD_COMP) == static_cast<int>(uc::OldComp::LAST));

std::u8string uc::OldCompInfo::locName() const
{
    if (locKey.empty()) {
        return std::u8string{fixedName};
    } else {
        std::u8string r = loc::get(locKey);
        str::replace(r, u8" ", u8"" NBSP);
        return r;
    }
}

#define REP_2(x) (x),(x)
#define REP_3(x) (x),(x),(x)
#define REP_4(x) (x),(x),(x),(x)
#define REP_5(x) (x),(x),(x),(x),(x)
#define REP_6(x) (x),(x),(x),(x),(x),(x)
#define REP_7(x) (x),(x),(x),(x),(x),(x),(x)
#define REP_8(x) REP_4(x),REP_4(x)
#define REP_10(x) REP_4(x),REP_4(x),x,x
#define REP_11(x) REP_4(x),REP_4(x),REP_3(x)
#define REP_12(x) REP_4(x),REP_4(x),REP_4(x)
#define REP_13(x) REP_12(x),x
#define REP_16(x) REP_4(x),REP_4(x),REP_4(x),REP_4(x)
#define REP_32(x) REP_16(x),REP_16(x)
#define REP_45(x) REP_16(x),REP_16(x),REP_13(x)
#define REP_48(x) REP_16(x),REP_16(x),REP_16(x)

namespace {

    using OC = uc::OldComp;

    constexpr auto OC_6BLOCK_USERS = OC::AQUARIUS | OC::TANDY_COCO | OC::MINITEL | OC::ORIC | OC::TELETEXT | OC::TANDY_TRS80 | OC::KORVET;

    constinit const Flags<uc::OldComp> primaryOldCompData[] {
        REP_45 (OC_6BLOCK_USERS),                                               // 00..2C 6block
                OC_6BLOCK_USERS | OC::HP_TERMINAL,                              // 2D lower ⅓
        REP_11 (OC_6BLOCK_USERS),                                               // 2E..38
                OC_6BLOCK_USERS | OC::HP_TERMINAL,                              // 39 lower ⅔
        REP_2  (OC_6BLOCK_USERS),
        REP_48 (OC::TELETEXT ),                                                 // 3C..6B diagonals and braided flags
        REP_4  (OC::MSX | OC::TELETEXT ),                                       // 6C..6F triangles
                OC::COMMODORE | OC::TELETEXT,                                   // 70 col2
        REP_4  (OC::COMMODORE),                                                 // 71..74 col3..6
                OC::COMMODORE | OC::TELETEXT,                                   // 75 col7
        REP_6  (OC::COMMODORE),                                                 // 76..7B line2..6
                OC::APPLE | OC::COMMODORE,                                      // 7C LB corner
        REP_3  (OC::COMMODORE),                                                 // 7D..7F rest 3 corners
                OC::APPLE,                                                      // 80 lines18
                OC::APPLE,                                                      // 81 lines1358
                OC::MSX | OC::COMMODORE,                                        // 82 lines12
        REP_2  (OC::COMMODORE),                                                 // 83,84 lines123,1-5
                OC::MSX | OC::COMMODORE,                                        // 85 lines1-6
                OC::COMMODORE,                                                  // 86 lines1-7
                OC::MSX | OC::COMMODORE,                                        // 87 cols78
        REP_2  (OC::COMMODORE),                                                 // 88,89 cols678,4-8
                OC::MSX | OC::COMMODORE | OC::TANDY_TRS80,                      // 8A cols3-8
                OC::COMMODORE,                                                  // 8B cols2-8
                                        // 16 chars skipped by Consortium
                OC::AMSTRAD | OC::AQUARIUS | OC::COMMODORE,                     // 8C left50%
                OC::AMSTRAD | OC::AQUARIUS,                                     // 8D right50%
                OC::AMSTRAD | OC::AQUARIUS | OC::SINCLAIR,                      // 8E top50%
                OC::AMSTRAD | OC::AQUARIUS | OC::COMMODORE | OC::SINCLAIR,      // 8F bot50%
                OC::APPLE | OC::COMMODORE | OC::SINCLAIR,                       // 90 all50%
                OC::COMMODORE | OC::SINCLAIR,                                   // 91 top100%+bot50%
                OC::SINCLAIR,                                                   // 92 top50%+bot100%
                {},                                                             // 93 hole
                OC::COMMODORE,                                                  // 94 left50%+right100%
                OC::MSX | OC::COMMODORE,                                        // 95 checker1
                OC::AMSTRAD | OC::COMMODORE,                                    // 96 checker2
                OC::TANDY_TRS80,                                                // 97 heavy=
        REP_2  (OC::MSX | OC::COMMODORE),                                       // 98 99 fill\ and /
        REP_2  (OC::MSX),                                                       // 9A,9B butterflies
        REP_4  (OC::AMSTRAD),                                                   // 9C..9F diag50%
        REP_8  (OC::AMSTRAD | OC::MINITEL | OC::TELETEXT),                      // A0..A3 quarter / or \…
                                                                                // A4..A7 two such slashes nearby
        REP_2  (OC::AMSTRAD | OC::MINITEL),                                     // A8 A9 two such slashes opposite
        REP_4  (OC::MINITEL),                                                   // AA..AD three such slashes
                OC::AMSTRAD | OC::MINITEL,                                      // AE four such slashes
                OC::MSX,                                                        // AF full -, incomplete |
                OC::APPLE,                                                      // B0 mouse cursor
                OC::APPLE | OC::COMMODORE,                                      // B1 inverse check
        REP_11 (OC::APPLE),                                                     // B2..BC misc Apple chars
        REP_3  (OC::ATARI_ST),                                                  // BD..BF inverse saltires
                OC::RISC_OS,                                                    // C0 white saltire (U16 disunified)
        REP_4  (OC::TANDY_TRS80),                                               // C1..C3 pointing finger
                                                                                // C4 inverse ?
                OC::AMSTRAD | OC::TANDY_TRS80,                                  // C5 man
        REP_3  (OC::AMSTRAD),                                                   // C6..C8 three men
        REP_5  (OC::TANDY_TRS80),                                               // C9 woman, CA chevron, CB cross,
                                                                                // CC small [, CD small chevron
        REP_2  (OC::NONE),                                                      // CE CF ⅔ blocks
        REP_8  (OC::OHIO_SCIENTIFIC | OC::ROBOTRON | OC::SHARP_MZ),             // D0..D7 diags 2:1
        REP_4  (OC::OHIO_SCIENTIFIC),                                           // D8..DB corners 45°
        REP_4  (OC::ROBOTRON),                                                  // DC..DF corners 2:1
                OC::ROBOTRON,                                                   // E0 top ½ circle
                OC::OHIO_SCIENTIFIC | OC::ROBOTRON,                             // E1 left ½ circle
                OC::ROBOTRON,                                                   // E2 bottom ½ circle
                OC::OHIO_SCIENTIFIC | OC::ROBOTRON,                             // E3 right ½ circle
        REP_2  (OC::OHIO_SCIENTIFIC),                                           // E4,E5 upper/lower ¼
        REP_2  (OC::KAYPRO | OC::OHIO_SCIENTIFIC),                              // E6,E7 left/right ¼
        REP_4  (OC::AQUARIUS | OC::SHARP_MZ),                                   // E8..EB black ½ circles
        REP_4  (OC::AQUARIUS),                                                  // EC..EF ¼ circles
        REP_10 (OC::ATARI_ST)                                                   // F0..F9 7seg digits
    };
    static_assert(std::size(primaryOldCompData) == 0xFA);                       // FA first free

    constinit const Flags<uc::OldComp> arrowOldCompData[] {
        OC::ATARI_8BIT,
        OC::TANDY_TRS80,
        OC::SMALLTALK,
        REP_3(OC::SHARP_MZ),
        OC::SHARP_MZ | OC::SMALLTALK,
        OC::SHARP_MZ,
        REP_4(OC::OHIO_SCIENTIFIC),
    };
    static_assert(std::size(arrowOldCompData) == 12);

    constinit const Flags<uc::OldComp> controlPicOldCompData[] {
        OC::APPLE,                          // 2427
        OC::TANDY_TRS80 | OC::HP_TERMINAL,  // 2428
        OC::AMSTRAD,                        // 2429
    };
    static_assert(std::size(controlPicOldCompData) == 3);

    struct OldCompSpan {
        std::span<const Flags<uc::OldComp>> span;
        char32_t firstCp;

    };
    inline bool operator < (char32_t x, const OldCompSpan& y) { return (x < y.firstCp); }

    constinit const OldCompSpan oldCompSpans[] { // Should go in increasing order
        { .span = controlPicOldCompData, .firstCp = 0x2427  },
        { .span = arrowOldCompData,      .firstCp = 0x1F8B0 },
        { .span = primaryOldCompData,    .firstCp = 0x1FB00 },
    };
    static_assert(std::size(oldCompSpans) == 3);

    #define REV_4(x, y) {(x),(y)}, {(x)+1,(y)+1}, {(x)+2,(y)+2}, {(x)+3,(y)+3}
    #define REV_8(x, y)  REV_4(x, y), REV_4((x)+4, (y)+4)
    #define REV_16(x, y) REV_8(x, y), REV_8((x)+8, (y)+8)
    #define REV_32(x, y) REV_16(x, y), REV_16((x)+16, (y)+16)

    constinit const ReverseMap rmDosCommon {{
        { u'☺',  1 }, { u'☻',  2 }, { u'♥',  3 },  { u'♦',  4 },
        { u'♣',  5 }, { u'♠',  6 }, { u'•',  7 },  { u'◘',  8 },
        { u'○',  9 }, { u'◙', 10 }, { u'♂', 11 },  { u'♀', 12 },
        { u'♪', 13 }, { u'♫', 14 }, { u'☼', 15 },  { u'►', 16 },
        { u'◄', 17 }, { u'↕', 18 }, { u'‼', 19 },  { u'¶', 20 },
        { u'§', 21 }, { u'▬', 22 }, { u'↨', 23 },  { u'↑', 24 },
        { u'↓', 25 }, { u'→', 26 }, { u'←', 27 },  { u'∟', 28 },
        { u'↔', 29 }, { u'▲', 30 }, { u'▼', 31 },  { u'⌂', 127 },
        { u'░', 0xB0 }, { u'▒', 0xB1 }, { u'▓', 0xB2 }, { u'│', 0xB3 },
        { u'┤', 0xB4 }, { u'╡', 0xB5 }, { u'╢', 0xB6 }, { u'╖', 0xB7 },
        { u'╕', 0xB8 }, { u'╣', 0xB9 }, { u'║', 0xBA }, { u'╗', 0xBB },
        { u'╝', 0xBC }, { u'╜', 0xBD }, { u'╛', 0xBE }, { u'┐', 0xBF },
        { u'└', 0xC0 }, { u'┴', 0xC1 }, { u'┬', 0xC2 }, { u'├', 0xC3 },
        { u'─', 0xC4 }, { u'┼', 0xC5 }, { u'╞', 0xC6 }, { u'╟', 0xC7 },
        { u'╚', 0xC8 }, { u'╔', 0xC9 }, { u'╩', 0xCA }, { u'╦', 0xCB },
        { u'╠', 0xCC }, { u'═', 0xCD }, { u'╬', 0xCE }, { u'╧', 0xCF },
        { u'╨', 0xD0 }, { u'╤', 0xD1 }, { u'╥', 0xD2 }, { u'╙', 0xD3 },
        { u'╘', 0xD4 }, { u'╒', 0xD5 }, { u'╓', 0xD6 }, { u'╫', 0xD7 },
        { u'╪', 0xD8 }, { u'┘', 0xD9 }, { u'┌', 0xDA }, { u'█', 0xDB },
        { u'▄', 0xDC }, { u'▌', 0xDD }, { u'▐', 0xDE }, { u'▀', 0xDF },
        { u'■', 0xFE }, { 0xA0, 0xFF }
    }};

    constinit const ReverseMap rmDosRu {{
        REV_32(u'А', 0x80),      // Cyrillic A
        REV_16(u'а', 0xA0),      // Cyrillic a
        REV_16(u'р', 0xE0),      // Cyrillic r
        { u'Ё', 0xF0 }, { u'ё', 0xF1 }, { u'Є', 0xF2 }, { u'є', 0xF3 },
        { u'Ї', 0xF4 }, { u'ї', 0xF5 }, { u'Ў', 0xF6 }, { u'ў', 0xF7 },
        { u'°', 0xF8 }, { u'∙', 0xF9 }, { u'·', 0xFA }, { u'√', 0xFB },
        { u'№', 0xFC }, { u'¤', 0xFD }
    }};

    constinit const ReverseMap rmDosEn {{
        { u'Ç', 0x80 }, { u'ü', 0x81 }, { u'é', 0x82 }, { u'â', 0x83 },
        { u'ä', 0x84 }, { u'à', 0x85 }, { u'å', 0x86 }, { u'ç', 0x87 },
        { u'ê', 0x88 }, { u'ë', 0x89 }, { u'è', 0x8A }, { u'ï', 0x8B },
        { u'î', 0x8C }, { u'ì', 0x8D }, { u'Ä', 0x8E }, { u'Å', 0x8F },
        { u'É', 0x90 }, { u'æ', 0x91 }, { u'Æ', 0x92 }, { u'ô', 0x93 },
        { u'ö', 0x94 }, { u'ò', 0x95 }, { u'û', 0x96 }, { u'ù', 0x97 },
        { u'ÿ', 0x98 }, { u'Ö', 0x99 }, { u'Ü', 0x9A }, { u'¢', 0x9B },
        { u'£', 0x9C }, { u'¥', 0x9D }, { u'₧', 0x9E }, { u'ƒ', 0x9F },
        { u'á', 0xA0 }, { u'í', 0xA1 }, { u'ó', 0xA2 }, { u'ú', 0xA3 },
        { u'ñ', 0xA4 }, { u'Ñ', 0xA5 }, { u'ª', 0xA6 }, { u'º', 0xA6 },
        { u'¿', 0xA8 }, { u'⌐', 0xA9 }, { u'¬', 0xAA }, { u'½', 0xAB },
        { u'¼', 0xAC }, { u'¡', 0xAD }, { u'«', 0xAE }, { u'»', 0xAF },
        { u'α', 0xE0 }, { u'ß', 0xE1 }, { u'Γ', 0xE2 }, { u'π', 0xE3 },
        { u'Σ', 0xE4 }, { u'σ', 0xE5 }, { u'µ', 0xE6 }, { u'τ', 0xE7 },
        { u'Φ', 0xE8 }, { u'Θ', 0xE9 }, { u'Ω', 0xEA }, { u'δ', 0xEB },
        { u'∞', 0xEC }, { u'φ', 0xED }, { u'ε', 0xEE }, { u'∩', 0xEF },
        { u'≡', 0xF0 }, { u'±', 0xF1 }, { u'≥', 0xF2 }, { u'≤', 0xF3 },
        { u'⌠', 0xF4 }, { u'⌡', 0xF5 }, { u'÷', 0xF6 }, { u'≈', 0xF7 },
        { u'°', 0xF8 }, { u'∙', 0xF9 }, { u'·', 0xFA }, { u'√', 0xFB },
        { u'ⁿ', 0xFC }, { u'²', 0xFD }
    }};

    constinit const ReverseMap rmDosEl {{
        REV_16(u'Α', 0x80),        // Greek Α
        { u'Ρ', 0x90 }, { u'Σ', 0x91 }, { u'Τ', 0x92 }, { u'Υ', 0x93 },
        { u'Φ', 0x94 }, { u'Χ', 0x95 }, { u'Ψ', 0x96 }, { u'Ω', 0x97 },
        REV_8 (u'α', 0x98),
        REV_16(u'ι', 0xA0),
        { u'ω', 0xe0 }, { u'ά', 0xE1 }, { u'έ', 0xE2 }, { u'ή', 0xE3 },
        { u'ϊ', 0xE4 }, { u'ί', 0xE5 }, { u'ό', 0xE6 }, { u'ύ', 0xE7 },
        { u'ϋ', 0xe8 }, { u'ώ', 0xE9 }, { u'Ά', 0xEA }, { u'Έ', 0xEB },
        { u'Ή', 0xEC }, { u'Ί', 0xED }, { u'Ό', 0xEE }, { u'Ύ', 0xEF },
        { u'Ώ', 0xF0 }, { u'±', 0xF1 }, { u'≥', 0xF2 }, { u'≤', 0xF3 },
        { u'Ϊ', 0xF4 }, { u'Ϋ', 0xF5 }, { u'÷', 0xF6 }, { u'≈', 0xF7 },
        { u'°', 0xF8 }, { u'∙', 0xF9 }, { u'·', 0xFA }, { u'√', 0xFB },
        { u'ⁿ', 0xFC }, { u'²', 0xFD }
    }};

    constinit const ReverseMap rmWin {{
        { u'€', 128 }, { u'‚', 130 }, { u'ƒ', 131 }, { u'„', 132 },
        { u'…', 133 }, { u'†', 134 }, { u'‡', 135 }, { u'ˆ', 136 },
        { u'‰', 137 }, { u'Š', 138 }, { u'‹', 139 }, { u'Œ', 140 },
        { u'Ž', 142 }, { u'‘', 145 }, { u'’', 146 }, { u'“', 147 },
        { u'”', 148 }, { u'•', 149 }, { u'–', 150 }, { u'—', 151 },
        { u'˜', 152 }, { u'™', 153 }, { u'š', 154 }, { u'›', 155 },
        { u'œ', 156 }, { u'ž', 158 }, { u'Ÿ', 159 },
    }};

    using AltgrPair = std::pair<char16_t, uc::AltgrKey>;

    consteval AltgrPair altgr(char16_t subj, char key)
        { return { subj, { key } }; }

    consteval AltgrPair altgruml(char16_t subj, char key)
        { return { subj, { key, 0, true } }; }

    constinit Cmap kmBirman {{
        altgr ( 0xA0, ' ' ),
        altgruml ( u'̀', '~' ),
            { u'ẁ', { '~', 'w' } },
            { u'è', { '~', 'e' } },
            { u'ỳ', { '~', 'y' } },
            { u'ù', { '~', 'u' } },
            { u'ì', { '~', 'i' } },
            { u'ò', { '~', 'o' } },
            { u'à', { '~', 'a' } },
            { u'ǹ', { '~', 'n' } },
            { u'Ẁ', { '~', 'W' } },
            { u'È', { '~', 'E' } },
            { u'Ỳ', { '~', 'Y' } },
            { u'Ù', { '~', 'U' } },
            { u'Ì', { '~', 'I' } },
            { u'Ò', { '~', 'O' } },
            { u'À', { '~', 'A' } },
            { u'Ǹ', { '~', 'N' } },
        altgr ( u'¹', '1' ),
        altgr ( u'¡', '!' ),
        altgr ( u'²', '2' ),
        altgr ( u'½', '@' ),
        altgr ( u'³', '3' ),
        altgr ( u'⅓', '#' ),
        altgr ( u'$', '4' ),
        altgr ( u'¼', '$' ),
        altgr ( u'‰', '5' ),
        altgr ( u'↑', '6' ),
        altgruml ( u'̂', '^' ),
            { u'ŵ', { '^', 'w' } },
            { u'ê', { '^', 'e' } },
            { u'ŷ', { '^', 'y' } },
            { u'û', { '^', 'u' } },
            { u'î', { '^', 'i' } },
            { u'ô', { '^', 'o' } },
            { u'â', { '^', 'a' } },
            { u'ŝ', { '^', 's' } },
            { u'ĝ', { '^', 'g' } },
            { u'ĥ', { '^', 'h' } },
            { u'ĵ', { '^', 'j' } },
            { u'ẑ', { '^', 'z' } },
            { u'ĉ', { '^', 'c' } },
            { u'Ŵ', { '^', 'W' } },
            { u'Ê', { '^', 'E' } },
            { u'Ŷ', { '^', 'Y' } },
            { u'Û', { '^', 'U' } },
            { u'Î', { '^', 'I' } },
            { u'Ô', { '^', 'O' } },
            { u'Â', { '^', 'A' } },
            { u'Ŝ', { '^', 'S' } },
            { u'Ĝ', { '^', 'G' } },
            { u'Ĥ', { '^', 'H' } },
            { u'Ĵ', { '^', 'J' } },
            { u'Ẑ', { '^', 'Z' } },
            { u'Ĉ', { '^', 'C' } },
        altgr ( u'¿', '&' ),
        altgr ( u'∞', '8' ),
        altgr ( u'←', '9' ),
        altgr ( u'‹', '(' ),
        altgr ( u'→', '0' ),
        altgr ( u'›', ')' ),
        altgr ( u'—', '-' ),
        altgr ( u'–', '_' ),
        altgr ( u'≠', '=' ),
        altgr ( u'±', '+' ),
        altgruml ( u'̆', 'Q' ),
            { u'ĕ', { 'Q', 'e' } },
            { u'ŭ', { 'Q', 'u' } },
            { u'ĭ', { 'Q', 'i' } },
            { u'ŏ', { 'Q', 'o' } },
            { u'ă', { 'Q', 'a' } },
            { u'ğ', { 'Q', 'g' } },
            { u'Ĕ', { 'Q', 'E' } },
            { u'Ŭ', { 'Q', 'U' } },
            { u'Ĭ', { 'Q', 'I' } },
            { u'Ŏ', { 'Q', 'O' } },
            { u'Ă', { 'Q', 'A' } },
            { u'Ğ', { 'Q', 'G' } },
        altgr ( u'✓', 'w' ),
        altgr ( u'⌃', 'W' ),
        altgr ( u'€', 'e' ),
        altgr ( u'⌥', 'E' ),
        altgr ( u'®', 'r' ),
        altgruml ( u'̊', 'R' ),
            { u'ů', { 'R', 'u' } },
            { u'å', { 'R', 'a' } },
            { u'Ů', { 'R', 'U' } },
            { u'Å', { 'R', 'A' } },
        altgr ( u'™', 't' ),
        altgr ( u'ѣ', 'y' ),
        altgr ( u'Ѣ', 'Y' ),
        altgr ( u'ѵ', 'u' ),
        altgr ( u'Ѵ', 'U' ),
        altgr ( u'і', 'i' ),
        altgr ( u'І', 'I' ),
        altgr ( u'ѳ', 'o' ),
        altgr ( u'Ѳ', 'O' ),
        altgr ( u'′', 'p' ),
        altgr ( u'″', 'P' ),
        altgr ( u'[', '[' ),
        altgr ( u'{', '{' ),
        altgr ( u']', ']' ),
        altgr ( u'}', '}' ),
        altgr ( u'≈', 'a' ),
        altgr ( u'⌘', 'A' ),
        altgr ( u'§', 's' ),
        altgr ( u'⇧', 'S' ),
        altgr ( u'°', 'd' ),
        altgr ( u'⌀', 'D' ),
        altgr ( u'£', 'f' ),
        altgr ( u'₽', 'h' ),
        altgruml ( u'̋', 'H' ),
            { u'ő', { 'H', 'o' } },
            { u'ű', { 'H', 'u' } },
            { u'Ő', { 'H', 'O' } },
            { u'Ű', { 'H', 'U' } },
        altgr ( u'„', 'j' ),
        altgr ( u'“', 'k' ),
        altgr ( u'”', 'l' ),
        altgr ( u'‘', ';' ),
        altgruml ( u'̈', ':' ),
            { u'ẅ', { ':', 'w' } },
            { u'ë', { ':', 'e' } },
            { u'ÿ', { ':', 'y' } },
            { u'ü', { ':', 'u' } },
            { u'ï', { ':', 'i' } },
            { u'ö', { ':', 'o' } },
            { u'ä', { ':', 'a' } },
            { u'ẍ', { ':', 'x' } },
            { u'Ẅ', { ':', 'W' } },
            { u'Ë', { ':', 'E' } },
            { u'Ÿ', { ':', 'Y' } },
            { u'Ü', { ':', 'U' } },
            { u'Ï', { ':', 'I' } },
            { u'Ö', { ':', 'O' } },
            { u'Ä', { ':', 'A' } },
            { u'Ẍ', { ':', 'X' } },
        altgr ( u'’', '\'' ),
        altgr ( u'\'', '"' ),
        altgruml ( u'̧', 'Z' ),
            { u'ȩ', { 'Z', 'e' } },
            { u'ŗ', { 'Z', 'r' } },
            { u'ţ', { 'Z', 't' } },
            { u'ş', { 'Z', 's' } },
            { u'ḑ', { 'Z', 'd' } },
            { u'ģ', { 'Z', 'g' } },
            { u'ḩ', { 'Z', 'h' } },
            { u'ķ', { 'Z', 'k' } },
            { u'ļ', { 'Z', 'l' } },
            { u'ç', { 'Z', 'c' } },
            { u'ņ', { 'Z', 'n' } },
            { u'Ȩ', { 'Z', 'E' } },
            { u'Ŗ', { 'Z', 'R' } },
            { u'Ţ', { 'Z', 'T' } },
            { u'Ş', { 'Z', 'S' } },
            { u'Ḑ', { 'Z', 'D' } },
            { u'Ģ', { 'Z', 'G' } },
            { u'Ḩ', { 'Z', 'H' } },
            { u'Ķ', { 'Z', 'K' } },
            { u'Ļ', { 'Z', 'L' } },
            { u'Ç', { 'Z', 'C' } },
            { u'Ņ', { 'Z', 'N' } },
        altgr ( u'×', 'x' ),
        altgr ( u'·', 'X' ),
        altgr ( u'©', 'c' ),
        altgr ( u'¢', 'C' ),
        altgr ( u'↓', 'v' ),
        altgruml ( u'̌', 'V' ),
            { u'ě', { 'V', 'e' } },
            { u'ř', { 'V', 'r' } },
            { u'ť', { 'V', 't' } },
            { u'ǔ', { 'V', 'u' } },
            { u'ǐ', { 'V', 'i' } },
            { u'ǒ', { 'V', 'o' } },
            { u'ǎ', { 'V', 'a' } },
            { u'š', { 'V', 's' } },
            { u'ď', { 'V', 'd' } },
            { u'ǧ', { 'V', 'g' } },
            { u'ȟ', { 'V', 'h' } },
            { u'ǰ', { 'V', 'j' } },
            { u'ǩ', { 'V', 'k' } },
            { u'ž', { 'V', 'z' } },
            { u'č', { 'V', 'c' } },
            { u'ň', { 'V', 'n' } },
            { u'Ě', { 'V', 'E' } },
            { u'Ř', { 'V', 'R' } },
            { u'Ť', { 'V', 'T' } },
            { u'Ǔ', { 'V', 'U' } },
            { u'Ǐ', { 'V', 'I' } },
            { u'Ǒ', { 'V', 'O' } },
            { u'Ǎ', { 'V', 'A' } },
            { u'Š', { 'V', 'S' } },
            { u'Ď', { 'V', 'S' } },
            { u'Ǧ', { 'V', 'G' } },
            { u'Ȟ', { 'V', 'H' } },
            //{ u'ǰ', { 'V', 'J' } },  no cap letter J-caron here
            { u'Ǩ', { 'V', 'K' } },
            { u'Ž', { 'V', 'Z' } },
            { u'Č', { 'V', 'C' } },
            { u'Ň', { 'V', 'N' } },
        altgr ( u'ß', 'b' ),
        altgr ( u'ẞ', 'B' ),
        altgruml ( u'̃', 'N' ),
            { u'ỹ', { 'N', 'y' } },
            { u'ũ', { 'N', 'u' } },
            { u'ĩ', { 'N', 'i' } },
            { u'õ', { 'N', 'o' } },
            { u'ã', { 'N', 'a' } },
            { u'ṽ', { 'N', 'v' } },
            { u'ñ', { 'N', 'n' } },
            { u'Ỹ', { 'N', 'Y' } },
            { u'Ũ', { 'N', 'U' } },
            { u'Ĩ', { 'N', 'I' } },
            { u'Õ', { 'N', 'O' } },
            { u'Ã', { 'N', 'A' } },
            { u'Ṽ', { 'N', 'V' } },
            { u'Ñ', { 'N', 'N' } },
        altgr ( u'−', 'm' ),
        altgr ( u'•', 'm' ),
        altgr ( u'«', ',' ),
        //altgr ( u'„', '<' ),  need multimap!
        altgr ( u'»', '.' ),
        altgr ( u'…', '/' ),
        altgruml ( u'́', '?' ),
            { u'ẃ', { '?', 'w' } },
            { u'é', { '?', 'e' } },
            { u'ŕ', { '?', 'r' } },
            { u'ý', { '?', 'y' } },
            { u'ú', { '?', 'u' } },
            { u'í', { '?', 'i' } },
            { u'ó', { '?', 'o' } },
            { u'ṕ', { '?', 'p' } },
            { u'á', { '?', 'a' } },
            { u'ś', { '?', 's' } },
            { u'ǵ', { '?', 'g' } },
            { u'ĺ', { '?', 'l' } },
            { u'ź', { '?', 'z' } },
            { u'ć', { '?', 'c' } },
            { u'ń', { '?', 'n' } },
            { u'ḿ', { '?', 'm' } },
            { u'Ẃ', { '?', 'W' } },
            { u'É', { '?', 'E' } },
            { u'Ŕ', { '?', 'R' } },
            { u'Ý', { '?', 'Y' } },
            { u'Ú', { '?', 'U' } },
            { u'Í', { '?', 'I' } },
            { u'Ó', { '?', 'O' } },
            { u'Ṕ', { '?', 'P' } },
            { u'Á', { '?', 'A' } },
            { u'Ś', { '?', 'S' } },
            { u'Ǵ', { '?', 'G' } },
            { u'Ĺ', { '?', 'L' } },
            { u'Ź', { '?', 'Z' } },
            { u'Ć', { '?', 'C' } },
            { u'Ń', { '?', 'N' } },
            { u'Ḿ', { '?', 'M' } },
    }};

}   // anon namespace


const uc::GlyphStyleChannel uc::glyphStyleChannelInfo[] = {
    { 0, {}, uc::EcGlyphStyleChannel::NONE },      // -warn in static analyzer: init [0] as normal, and everything’s OK
    { .count = 2, .name = "Glag", .value = uc::EcGlyphStyleChannel::GLAGOLITIC },
    { .count = 2, .name = "Vert", .value = uc::EcGlyphStyleChannel::VERTICAL },
    { .count = 4, .name = "Arab", .value = uc::EcGlyphStyleChannel::ARABIC },
};
static_assert(std::size(uc::glyphStyleChannelInfo) == static_cast<size_t>(uc::EcGlyphStyleChannel::NN));


uc::InputMethods uc::cpInputMethods(char32_t cp)
{
    uc::InputMethods r;
    if (cp < 127) {   // ASCII characters except 7F
        switch (cp) {
        case 13:
        case 10:
            r.sometimesKey = u8"{{k|Enter}}, {{k|Shift/Ctrl/Alt|Enter}}";
            break;
        case 9:
            r.sometimesKey = u8"{{k|Tab}}";
            break;
        default:;
        }
    } else if (cp >= 0xA0 && cp < 0x10000) {  // Rest of BMP
        rmDosCommon.query(cp, r.alt.dosCommon);
        rmDosEn.query(cp, r.alt.dosEn);
        rmDosRu.query(cp, r.alt.dosRu);
        rmDosEl.query(cp, r.alt.dosEl);
        if (cp <= 0xFF) {     // ISO1
            r.alt.win = cp;
        } else {
            rmWin.query(cp, r.alt.win);
        }
        r.alt.unicode = cp;
        kmBirman.query(cp, r.birman);
    }
    return r;
}


Flags<uc::OldComp> uc::cpOldComps(char32_t cp)
{
    auto it = std::upper_bound(std::begin(oldCompSpans), std::end(oldCompSpans), cp);
    if (it == std::begin(oldCompSpans))
        return {};
    --it;
    cp -= it->firstCp;
    if (cp < it->span.size())
        return it->span[cp];
    return {};
}


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

    enum class EmojiClass {
        NONE,
        SINGLE_CHAR,    ///< incl. VS16
        SEQ_RACIAL, SEQ_MULTIRACIAL, SEQ_RIGHT, SEQ_RIGHT_RACIAL,
        SEQ_OTHER_ZWJ,
        SEQ_FLAG,   ///< National flags, like [U][A]
        SEQ_OTHER_NONZWJ };

    EmojiClass classifyEmoji(std::u32string_view x)
    {
        switch (x.length()) {
        case 0:
            return EmojiClass::NONE;
        case 1:
            return EmojiClass::SINGLE_CHAR;
        case 2:
            if (x[1] == cp::VS16)
                return EmojiClass::SINGLE_CHAR;
            if (x[0] >= cp::FLAG_A && x[0] <= cp::FLAG_Z)
                return EmojiClass::SEQ_FLAG;
            break;
        default: ;
        }

        size_t nSkin = 0, nZwj = 0;
        for (auto c : x) {
            switch (c) {
            case cp::ZWJ:  ++nZwj; break;
            case cp::SKIN1:
            case cp::SKIN2:
            case cp::SKIN3:
            case cp::SKIN4:
            case cp::SKIN5: ++nSkin; break;
            default: ;
            }
        }
        switch (nSkin) {
        case 0:
            if (x.length() > 3 && x.ends_with(U32_ZWJ_RIGHT_ARROW_VS16))
                return EmojiClass::SEQ_RIGHT;
            return (nZwj > 0) ? EmojiClass::SEQ_OTHER_ZWJ : EmojiClass::SEQ_OTHER_NONZWJ;
        case 1:
            if (x.length() > 3 && x.ends_with(U32_ZWJ_RIGHT_ARROW_VS16))
                return EmojiClass::SEQ_RIGHT_RACIAL;
            return EmojiClass::SEQ_RACIAL;
        default:
            return EmojiClass::SEQ_MULTIRACIAL;
        }
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
        auto& version = node.emojiVersion();
        // Emoji type
        auto clazz = classifyEmoji(node.value);
        switch (clazz) {
        case EmojiClass::NONE: break;
        case EmojiClass::SINGLE_CHAR: {
                switch (classifyEmojiVersion(node.value[0], version)) {
                case EmojiVersion::THIS:
                    ++version.stats.emoji.nw.singleChar.nThisUnicode; break;
                case EmojiVersion::OLD:
                    ++version.stats.emoji.nw.singleChar.nOldUnicode; break;
                }
                break;
            }
        case EmojiClass::SEQ_RACIAL:
            ++version.stats.emoji.nw.seq.nRacial; break;
        case EmojiClass::SEQ_MULTIRACIAL:
            ++version.stats.emoji.nw.seq.nMultiracial; break;
        case EmojiClass::SEQ_RIGHT:
            ++version.stats.emoji.nw.seq.nRightFacing; break;
        case EmojiClass::SEQ_RIGHT_RACIAL:
            ++version.stats.emoji.nw.seq.nRightFacingRacial; break;
        case EmojiClass::SEQ_FLAG:
            ++version.stats.emoji.nw.seq.nFlags; break;
        case EmojiClass::SEQ_OTHER_ZWJ:
            ++version.stats.emoji.nw.seq.nOtherZwj; break;
        case EmojiClass::SEQ_OTHER_NONZWJ:
            ++version.stats.emoji.nw.seq.nOtherNonZwj; break;
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
                std::cout << "Continent mismatch: " << v.name << std::endl;
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

    completeEmojiData(1);

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
    std::unique_ptr<QRawFont> rawFont;
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

        auto tempFont = installTempFontRel(family.text, trigger);
        q.loaded->tempId = tempFont.id;
        q.loaded->familiesComma = tempFont.families.join(',');
        q.loaded->families = std::move(tempFont.families);
        if (q.loaded->families.empty()) {
            q.loaded->isRejected = true;
            q.isRejected = true;
            return;
        }

        if (family.flags.have(Fafg::RAW_FONT) && tempFont.mems) {
            q.loaded->rawFont = std::make_unique<QRawFont>(tempFont.mems->qdata(), 50);
        }
    } else {
        // FAMILY
        newLoadedStruc();
        q.loaded->familiesComma = str::toQ(family.text);
        q.loaded->families = toQList(family.text);
    }

    // Make probe font, force EXACT match
    if (!q.loaded->rawFont) {
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
    if (q.loaded->rawFont) {
        return q.loaded->rawFont->supportsCharacter(subj);
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


std::u8string_view uc::Cp::Name::traverseAll(const TextSink& sink) const
{
    auto p = allStrings + iTech.val();
    while (true) {
        auto role = static_cast<TextRole>(*(p++));
        switch (role) {
        // Commands
        case TextRole::CMD_END: goto brk;
        // Actual texts
        case TextRole::MAIN_NAME:
        case TextRole::ALT_NAME:
        case TextRole::HTML:
        case TextRole::DEP_INSTEAD:
        case TextRole::DEP_INSTEAD2:
        case TextRole::ABBREV: {
                auto length = static_cast<unsigned char>(*(p++));
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
brk:
    return {};
}


std::u8string_view uc::Cp::Name::getText(TextRole role) const
{
    return traverseAllT([role](TextRole aRole, std::u8string_view) {
        return static_cast<Action>(role == aRole);
    });
}


const std::u8string_view uc::Cp::Name::tech() const
{
    return traverseAllT([](TextRole role, std::u8string_view)
                          { return stopIf(role == TextRole::MAIN_NAME); } );
}


QString uc::Cp::viewableName() const
{
    QString r = str::toQ(name.tech());
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
    return name.traverseAllT([](TextRole role, std::u8string_view)
                               { return stopIf(role == TextRole::ABBREV); } );
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
        return name.getText(TextRole::ABBREV).empty();
    default:
        return true;
    }
}

namespace {

    bool isFullEmojiByCase(char32_t ch)
    {
        switch (ch) {
        // Misc technical
        case 0x23CF:  // VCR eject
        case 0x23ED:  // VCR next
        case 0x23EE:  // VCR prev
        case 0x23EF:  // VCR play/pause
        case 0x23F8:  // VCR pause
        case 0x23F9:  // VCR stop
        case 0x23FA:  // VCR record
        // Misc symbols
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
        case 0x26A7:  // transgender
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

    void buildSortKey(std::u8string_view x, SpecialSort specialSort,
                      const std::unordered_map<char32_t, int>& sortOrder,
                      uc::LocSortKey& r)
    {
        // Get name
        auto name32 = mojibake::toS<std::u32string>(x);

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
            }
        }
    }

}   // anon namespace


void uc::finishTranslation(
        const std::unordered_map<char32_t, int>& sortOrder,
        std::u32string_view ellipsisBlocks)
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
        auto& script = scriptInfo[static_cast<int>(blk.alphaKey.ecScript)];

        // Sorting key
        std::u8string_view keyName = blk.loc.name;
        if (blk.alphaKey.ecScript != EcScript::NONE) {
            keyName = script.loc.name;
            if (script.mainBlock)
                keyName = script.mainBlock->loc.name;
        }
        buildSortKey(keyName, SpecialSort::NO, sortOrder, blk.loc.sortKey);
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
        buildSortKey(term.loc.name, SpecialSort::YES, sortOrder, term.loc.sortKey);

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


#define TEXT_BETA u8"\u00A0" "β"

std::u8string uc::Version::locName() const
{
    std::u8string r;
    if (!unicodeName.empty()) {
        r = unicodeName;
        if (flags.have(Vfg::BETA))
            r += TEXT_BETA;
    } else {
        r = emojiName;
        if (flags.have(Vfg::BETA))
            r += TEXT_BETA;
        r = loc::get("Prob.Bullet.EmojiV").arg(r);
    }
    return r;
}


std::u8string uc::Version::termName() const
{
    std::u8string r;
    if (!unicodeName.empty()) {
        r = unicodeName;
        if (flags.have(Vfg::BETA))
            r += TEXT_BETA;
    } else {
        r = emojiName;
        if (flags.have(Vfg::BETA))
            r += TEXT_BETA;
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
    if (flags.have(Vfg::BETA))
        r += TEXT_BETA;
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
