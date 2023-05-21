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
constinit const uc::InputMethods uc::InputMethods::NONE {};

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

// Usage: u8"15.0" VER_BETA
#define VER_BETA "\u00A0" "β"

const uc::Version uc::versionInfo[] {
    { {},      { 0000, Month::NUL } },
    //{ "1.0",  1991 },
    //{ "1.0.1",1992 },
    { u8"1.1", { 1993, Month::JUN } },
    { u8"2.0", { 1996, Month::JUL } },
    { u8"2.1", { 1998, Month::MAY } },
    { u8"3.0", { 1999, Month::SEP } },
    { u8"3.1", { 2001, Month::MAR } },
    { u8"3.2", { 2002, Month::MAR } },
    { u8"4.0", { 2003, Month::APR } },
    { u8"4.1", { 2005, Month::MAR } },
    { u8"5.0", { 2006, Month::JUL } },
    { u8"5.1", { 2008, Month::APR } },
    { u8"5.2", { 2009, Month::OCT } },
    { u8"6.0", { 2010, Month::OCT } },
    { u8"6.1", { 2012, Month::JAN } },
    { u8"6.2", { 2012, Month::SEP } },
    { u8"6.3", { 2013, Month::SEP } },
    { u8"7.0", { 2014, Month::JUN } },
    { u8"8.0", { 2015, Month::JUN } },
    { u8"Emoji 1.0", { 2015, Month::AUG } },
    { u8"Emoji 2.0", { 2015, Month::NOV } },
    { u8"9.0", { 2016, Month::JUN } },
    { u8"Emoji 4.0", { 2016, Month::NOV } },
    { u8"10.0", { 2017, Month::JUN } },
    { u8"11.0", { 2018, Month::JUN } },
    { u8"12.0", { 2019, Month::MAR } },
    { u8"12.1", { 2019, Month::MAY } },
    { u8"13.0", { 2020, Month::MAR } },
    { u8"Emoji 13.1", { 2020, Month::SEP } },
    { u8"14.0", { 2021, Month::SEP } },
    { u8"15.0", { 2022, Month::SEP } },
};
static_assert (std::size(uc::versionInfo) == static_cast<int>(uc::EcVersion::NN));

constinit const uc::Category uc::categoryInfo[static_cast<int>(uc::EcCategory::NN)] {
    { UpCategory::CONTROL,      "Cc",   Graphical::NO  },
    { UpCategory::FORMAT,       "Cf",   Graphical::NO  },
    // PRIVATE_USE,     -- unused as Unicodia has nothing to tell
    // SURROGATE,       -- unused as Unicodia has nothing to tell
    { UpCategory::LETTER,       "Ll",   Graphical::YES },
    { UpCategory::LETTER,       "Lm",   Graphical::YES },
    { UpCategory::LETTER,       "Lo",   Graphical::YES },
    { UpCategory::LETTER,       "Lt",   Graphical::YES },
    { UpCategory::LETTER,       "Lu",   Graphical::YES },
    { UpCategory::MARK,         "Mc",   Graphical::YES },
    { UpCategory::MARK,         "Me",   Graphical::YES },
    { UpCategory::MARK,         "Mn",   Graphical::YES },
    { UpCategory::NUMBER,       "Nd",   Graphical::YES },
    { UpCategory::NUMBER,       "Nl",   Graphical::YES },
    { UpCategory::NUMBER,       "No",   Graphical::YES },
    { UpCategory::PUNCTUATION,  "Pc",   Graphical::YES },
    { UpCategory::PUNCTUATION,  "Pd",   Graphical::YES },
    { UpCategory::PUNCTUATION,  "Pe",   Graphical::YES },
    { UpCategory::PUNCTUATION,  "Pf",   Graphical::YES },
    { UpCategory::PUNCTUATION,  "Pi",   Graphical::YES },
    { UpCategory::PUNCTUATION,  "Po",   Graphical::YES },
    { UpCategory::PUNCTUATION,  "Ps",   Graphical::YES },
    { UpCategory::SYMBOL,       "Sc",   Graphical::YES },
    { UpCategory::SYMBOL,       "Sk",   Graphical::YES },
    { UpCategory::SYMBOL,       "Sm",   Graphical::YES },
    { UpCategory::SYMBOL,       "So",   Graphical::YES },
    { UpCategory::SEPARATOR,    "Zl",   Graphical::NO  },
    { UpCategory::SEPARATOR,    "Zp",   Graphical::NO  },
    { UpCategory::SEPARATOR,    "Zs",   Graphical::YES },
    //{ u8"Error"sv },     //check for equal number
};


constinit const uc::Script uc::scriptInfo[] {
    { "Zyyy", QFontDatabase::Any,
        EcScriptType::NONE, EcLangLife::NOMATTER, EcWritingDir::NOMATTER, EcContinent::NONE,
            Dating::none(),
            EcFont::NORMAL, Sfg::NONSCRIPT | Sfg::NO_LANGS },
    // Arrows pseudo-script
    { "ZARR", QFontDatabase::Any,
        EcScriptType::NONE, EcLangLife::NOMATTER, EcWritingDir::NOMATTER, EcContinent::NONE,
            Dating::none(),
            EcFont::NORMAL, Sfg::NONSCRIPT | Sfg::NO_LANGS | Sfg::SORT_KEY },
    // Mathematical pseudo-script
    { "ZMAT", QFontDatabase::Any,
        EcScriptType::NONE, EcLangLife::NOMATTER, EcWritingDir::NOMATTER, EcContinent::NONE,
            Dating::none(),
            EcFont::NORMAL, Sfg::NONSCRIPT | Sfg::NO_LANGS | Sfg::SORT_KEY },
    // Symbols and pictographs pseudo-script
    { "ZSYM", QFontDatabase::Any,
        EcScriptType::NONE, EcLangLife::NOMATTER, EcWritingDir::NOMATTER, EcContinent::NONE,
            Dating::none(),
            EcFont::NORMAL, Sfg::NONSCRIPT | Sfg::NO_LANGS | Sfg::SORT_KEY },
    // Dingbats pseudo-script
    { "ZDIN", QFontDatabase::Any,
        EcScriptType::NONE, EcLangLife::NOMATTER, EcWritingDir::NOMATTER, EcContinent::NONE,
            Dating::none(),
            EcFont::NORMAL, Sfg::NONSCRIPT | Sfg::NO_LANGS | Sfg::SORT_KEY },
    // Diacritical pseudo-script
    { "ZDIA", QFontDatabase::Any,
        EcScriptType::NONE, EcLangLife::NOMATTER, EcWritingDir::NOMATTER, EcContinent::NONE,
            Dating::none(),
            EcFont::NORMAL, Sfg::NONSCRIPT | Sfg::NO_LANGS | Sfg::SORT_KEY },
    // Adlam OK, W10 has, but placement of umlauts + RTL = ??? → better Noto
    { "Adlm", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::NEW, EcWritingDir::RTL, EcContinent::AFRICA,
        Dating::special(), EcFont::ADLAM },
    // Caucasian Albanian OK, W10 none, installed Google Noto
    { "Aghb", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::DECIPHERED, EcWritingDir::LTR, EcContinent::EUROPE,
        Dating::yapprox(400), EcFont::CAUCASIAN_ALBANIAN },
    // Ahom OK, installed Google Noto font
    { "Ahom", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::REVIVED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(13), EcFont::AHOM },
    // Arabic OK, three fonts: SIL Scheherazade main, Google Noto Math + Google Noto Arabic for special ranges
    { "Arab", QFontDatabase::Arabic,
        EcScriptType::CONSONANT, EcLangLife::ALIVE, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::century(7, StdNote::MODERN_FORM), EcFont::ARABIC },
    // Imperial Aramaic OK, because of sheer importance install Google Noto
    { "Armi", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::DECIPHERED, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::crange(-10, -9), EcFont::ARAMAIC },
    // Armenian OK, Cambria+Noto is more than nice!
    { "Armn", QFontDatabase::Armenian,
        EcScriptType::ALPHABET, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(405), EcFont::ARMENIAN },
    // Avestan OK, installed Google Noto
    /// @todo [future, link] wait for book Pahlavi (sorry, cannot put todo at lockit)
    { "Avst", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::SACRED, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::yapprox(400), EcFont::AVESTAN },
    // Balinese OK, installed Google Noto font
    { "Bali", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::SACRED, EcWritingDir::LTR, EcContinent::OCEAN,
        Dating::yapprox(1000), EcFont::BALINESE },
    // Bamum OK, none even in W10, installed Google Noto and fixed stub
    { "Bamu", QFontDatabase::Any,
        EcScriptType::SYLLABLE, EcLangLife::DEAD, EcWritingDir::LTR, EcContinent::AFRICA,
        Dating::yrange(1895, 1910), EcFont::BAMUM },
    // Bassa Vah OK, none in W10, installed Google Noto
    { "Bass", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::COMPETING, EcWritingDir::LTR, EcContinent::AFRICA,
        Dating::yapprox(1900), EcFont::BASSA_VAH },
    // Batak OK, installed Google Noto font
    { "Batk", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::DEAD, EcWritingDir::LTR, EcContinent::OCEAN,
        Dating::yapprox(1300), EcFont::BATAK },
    // Bengali OK, W7 has tofu of 2009+ → installed Google Noto
    { "Beng", QFontDatabase::Bengali,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::special(), EcFont::BENGALI },
    // Bhaiksuki OK, installed Google Noto
    { "Bhks", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::CONSERVED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(11), EcFont::BHAIKSUKI },
    // Bopomofo OK
    { "Bopo", QFontDatabase::Any,
        EcScriptType::ALPHASYLLABLE, EcLangLife::COMPETING, EcWritingDir::LTR, EcContinent::CJK,
        Dating::year(1913), EcFont::CJK },
    // Brahmi OK, because of sheer importance installed Google Noto
    { "Brah", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::DECIPHERED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::special(), EcFont::BRAHMI },
    // Braille OK, “Segoe UI Symbol”
    { "Brai", QFontDatabase::Any,
        EcScriptType::CODE, EcLangLife::NOMATTER, EcWritingDir::NOMATTER, EcContinent::NONE,
        Dating::year(1824), EcFont::SYMBOL },
    // Buginese OK, W10 only → made LelawadeeUI → Google Noto chain
    { "Bugi", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::COMPETING, EcWritingDir::LTR, EcContinent::OCEAN,
        Dating::crange(15, 17), EcFont::BUGINESE },
    // Buhid OK, installed Google Noto font
    { "Buhd", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::OCEAN,
        Dating::yapprox(1300), EcFont::BUHID },
    // Chakma OK, added Noto font, mostly because of missing glyphs
    { "Cakm", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::COMPETING, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::unknown(), EcFont::CHAKMA },
    // Carian OK, W10 P1
    /// @todo [font???] Carian
    { "Cari", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::PARTLY_DECIPHERED, EcWritingDir::BOTH, EcContinent::ASIA,
        Dating::century(-7), EcFont::HISTORIC },
    // Canadian syllabics OK, W10 “Gadugi”, W7 lacks 2009 extensions, installed Google Noto font
    { "Cans", QFontDatabase::Any,
        EcScriptType::ABUGIDA, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::AMERICA,
        Dating::year(1833), EcFont::CANADIAN_ABORIGINAL },
    { "Cham", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::COMPETING, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(4), EcFont::CHAM },
    // Cherokee OK, installed Google Noto font. Need it, W7 has no 2014 extensions.
    { "Cher", QFontDatabase::Any,
        EcScriptType::SYLLABLE, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::AMERICA,
        Dating::year(1821), EcFont::CHEROKEE },
    // Chorasmian OK, installed Google Noto
    { "Chrs", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::DECIPHERED, EcWritingDir::SOGDIAN, EcContinent::ASIA,
        Dating::century(2, StdNote::FIRST_KNOWN), EcFont::CHORASMIAN },
    // Coptic OK, W7 problems → installed Google Noto font
    { "Copt", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::SACRED, EcWritingDir::LTR, EcContinent::AFRICA,
        Dating::century(2), EcFont::COPTIC },
    // Cypro-Minoan OK, made nice script at FunkySample
    { "Cpmn", QFontDatabase::Any,
        EcScriptType::SYLLABLE, EcLangLife::PARTLY_DECIPHERED, EcWritingDir::LTR, EcContinent::EUROPE,
        Dating::yapprox(-1550, StdNote::FIRST_KNOWN), EcFont::FUNKY },
    // Cypriot OK, W10 Segoe Historic
    { "Cprt", QFontDatabase::Any,
        EcScriptType::SYLLABLE, EcLangLife::DECIPHERED, EcWritingDir::RTL_MOSTLY, EcContinent::EUROPE,
        Dating::century(-11), EcFont::HISTORIC },
    // Cyr OK except enclosing; managed to modify Noto
    { "Cyrl", QFontDatabase::Cyrillic,
        EcScriptType::ALPHABET, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::EUROPE,
        Dating::ybefore(900), EcFont::NORMAL },
    // Devanagari OK, added 8 characters to Noto Serif
    { "Deva", QFontDatabase::Devanagari,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::crange(1, 7), EcFont::DEVANAGARI },
    // Diak OK, built some samples
    { "Diak", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::DECIPHERED, EcWritingDir::LTR, EcContinent::OCEAN,
        Dating::crange(9, 10, StdNote::FIRST_KNOWN), EcFont::DIVES_AKURU },
    // Dogri OK, W10 off → installed Google Noto
    { "Dogr", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::REVIVED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(19), EcFont::DOGRA },
    // Deseret OK, W10 Segoe UI Symbol, plane 1
    { "Dsrt", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::EXPERIMENTAL, EcWritingDir::LTR, EcContinent::AMERICA,
        Dating::year(1854), EcFont::SYMBOL },
    // Duployan OK, W10 none → installed Google Noto
    { "Dupl", QFontDatabase::Any,
        EcScriptType::CODE, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::EUROPE,
        Dating::year(1860), EcFont::DUPLOYAN },
    // Egyptian hiero OK
    { "Egyp", QFontDatabase::Any,
        EcScriptType::CONSONANTHIEROGLYPH, EcLangLife::DECIPHERED, EcWritingDir::BOTH, EcContinent::AFRICA,
        Dating::yapprox(-3200), EcFont::EGYPTIAN },
    // Elbasan OK, none in W10 → installed Google Noto
    { "Elba", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::EXPERIMENTAL, EcWritingDir::LTR, EcContinent::EUROPE,
        Dating::century(18), EcFont::ELBASAN },
    // Elymaic OK, W10 none → installed Google Noto
    { "Elym", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::DECIPHERED, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::century(-2), EcFont::ELYMAIC },
    // Ethiopic OK, lots of tofu, espec. in W7 → installed Google Noto
    { "Ethi", QFontDatabase::Any,
        EcScriptType::ABUGIDA, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::AFRICA,
        Dating::century(4, StdNote::CUSTOM), EcFont::ETHIOPIC },
    // Georgian OK, installed Google Noto font
    { "Geor", QFontDatabase::Georgian,
        EcScriptType::ALPHABET, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::EUROPE,
        Dating::year(430, StdNote::FIRST_KNOWN), EcFont::GEORGIAN },
    // Glagolitic OK, installed Google Noto font
    { "Glag", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::CONSERVED, EcWritingDir::LTR, EcContinent::EUROPE,
        Dating::century(9), EcFont::GLAGOLITIC },
    // Gondi/Gunjala OK, W10 none, installed Google Noto
    { "Gong", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::REVIVED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(1750, StdNote::FIRST_KNOWN), EcFont::GUNJALA_GONDI },
    // Gondi/Masaram OK, W10 none, installed Google Noto
    { "Gonm", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::COMPETING, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(1918), EcFont::MASARAM_GONDI },
    // Gothic OK, took from Junicode and enlarged ×135%
    { "Goth", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::DECIPHERED, EcWritingDir::LTR, EcContinent::EUROPE,
        Dating::century(4), EcFont::FUNKY },
    // Grantha OK, W10 none → installed Google Noto
    { "Gran", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::SACRED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::special(), EcFont::GRANTHA },
    // Greek OK, W7 Cambria + Noto do the job
    { "Grek", QFontDatabase::Greek,
        EcScriptType::ALPHABET, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::EUROPE,
        Dating::crange_ins(-9, -8), EcFont::NOTO },
    // Gujarati OK, installed Google Noto: cannot find a good pair for W7/10, and Noto is REALLY nice.
    { "Gujr", QFontDatabase::Gujarati,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(1592, StdNote::FIRST_KNOWN), EcFont::GUJARATI },
    // Gurmukhi OK, installed Google Noto *UI* because of W7 troubles; UI better handles umlauts
    { "Guru", QFontDatabase::Gurmukhi,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(16), EcFont::GURMUKHI },
    // Hangul OK, installed Noto CJK font
    { "Hang", QFontDatabase::Korean,
        EcScriptType::ARGUABLE, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::CJK,
        Dating::year(1443), EcFont::KOREAN },
    // Hani OK
    { "Hani", WS_HANI,       // Special rules for hieroglyphs, SimChi triggers them
        EcScriptType::HIEROGLYPH, EcLangLife::ALIVE, EcWritingDir::LTR_CJK, EcContinent::CJK,
        Dating::yapprox(-2000), EcFont::CJK },
    // Hanunoo OK, installed Google Noto font
    { "Hano", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::OCEAN,
        Dating::yapprox(1300), EcFont::HANUNOO },
    // Hatran OK, W10 none, installed Google Noto
    { "Hatr", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::DECIPHERED, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::yapprox(-100), EcFont::HATRAN },
    // Hebrew OK, installed very nice Noto, implemented yod triangle
    { "Hebr", QFontDatabase::Hebrew,
        EcScriptType::CONSONANT, EcLangLife::ALIVE, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::century(-2, StdNote::MODERN_FORM), EcFont::HEBREW },
    // Hentaigana currently OK
    { "Hent", QFontDatabase::Japanese,
        EcScriptType::SYLLABLE, EcLangLife::CONSERVED, EcWritingDir::LTR_CJK, EcContinent::CJK,
        Dating::crange(8, 9), EcFont::HENTAIGANA },
    // Hiragana OK, installed small version of Noto CJK Korean
    { "Hira", QFontDatabase::Japanese,
        EcScriptType::SYLLABLE, EcLangLife::ALIVE, EcWritingDir::LTR_CJK, EcContinent::CJK,
        Dating::crange(8, 9), EcFont::KOREAN },
    // Anatolian hiero OK, installed Google Noto
    { "Hluw", QFontDatabase::Any,
        EcScriptType::HIEROGLYPH, EcLangLife::DECIPHERED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::crange(-14, -13, StdNote::FIRST_KNOWN), EcFont::ANATOLIAN },
    // Pahawh Hmong OK, W10 none → installed Google Noto
    { "Hmng", QFontDatabase::Any,
        EcScriptType::ABUGIDA, EcLangLife::COMPETING, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(1959, StdNote::CUSTOM), EcFont::PAHAWH_HMONG },
    // NP Hmong OK, W10 none → installed Google Noto
    { "Hmnp", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::NEW, EcWritingDir::LTR, EcContinent::AMERICA,
        Dating::decade(1980), EcFont::NP_HMONG },
    // Hungarian runes OK, W10 none → installed some open-source font
    { "Hung", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::CONSERVED, EcWritingDir::RTL, EcContinent::EUROPE,
        Dating::century(10, StdNote::MAYBE_EARLIER), EcFont::OLD_HUNGARIAN },
    // Old Italic OK, installed Google Noto (Segoe has no newer chars od 2014+)
    { "Ital", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::DECIPHERED, EcWritingDir::BOTH, EcContinent::EUROPE,
        Dating::yapprox(-700), EcFont::OLD_ITALIC },
    // Javanese OK, W10 has “Javanese Text”, W7 does not, installed Google Noto font
    { "Java", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::COMPETING, EcWritingDir::LTR, EcContinent::OCEAN,
        Dating::century(15), EcFont::JAVANESE },
    // Kayah Li OK, W10 none, installed Google Noto font
    { "Kali", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::NEW, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(1962), EcFont::KAYAH_LI },
    // Katakana OK, installed small version of Noto CJK Korean
    { "Kana", QFontDatabase::Japanese,
        EcScriptType::SYLLABLE, EcLangLife::ALIVE, EcWritingDir::LTR_CJK, EcContinent::CJK,
        Dating::crange(8, 9), EcFont::KOREAN },
    // Kawi OK
    { "Kawi", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::CONSERVED, EcWritingDir::LTR, EcContinent::OCEAN,
        Dating::century(8, StdNote::FIRST_KNOWN), EcFont::KAWI },
    // Kharoshthi OK, W10 tofu → installed Noto
    { "Khar", QFontDatabase::Any,
        EcScriptType::ABUGIDA, EcLangLife::DECIPHERED, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::century(-4), EcFont::KHAROSHTHI },
    // Khmer OK, fancy and inconsistent in W7/10, installed Google Noto *UI* because of umlauts
    { "Khmr", QFontDatabase::Khmer,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(611, StdNote::FIRST_KNOWN), EcFont::KHMER },
    { "Khoj", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::SACRED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::crange(14, 16), EcFont::KHOJKI },
    // Khitan small OK, installed a font
    { "Kits", QFontDatabase::Any,
        EcScriptType::SYLLABOHIEROGLYPH, EcLangLife::PARTLY_DECIPHERED, EcWritingDir::RTL_COL, EcContinent::CJK,
        Dating::year(925), EcFont::KHITAN_SMALL },
    // Kannada OK, W7 has no recent extensions → installed Google Noto
    { "Knda", QFontDatabase::Kannada,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(14, StdNote::CUSTOM), EcFont::KANNADA },
    // Kaithi OK, W10 none → installed Google Noto
    { "Kthi", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::DEAD, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(16), EcFont::KAITHI},
    // Lanna OK, W10 none, installed Google Noto font
    { "Lana", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::COMPETING, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(1376, StdNote::FIRST_KNOWN), EcFont::LANNA },
    // Lao OK, W10 partial, installed SengBuhan font
    { "Laoo", QFontDatabase::Lao,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::yapprox(1350), EcFont::LAO },
    // Latin OK, managed to modify
    { "Latn", QFontDatabase::Latin,
        EcScriptType::ALPHABET, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::EUROPE,
        Dating::yapprox(-700), EcFont::NORMAL },
    // Lepcha OK, W10 none, installed Google Noto font
    { "Lepc", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::yapprox(1700), EcFont::LEPCHA },
    // Limbu OK, W10 none, installed Google Noto font
    { "Limb", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(18), EcFont::LIMBU },
    // Linear A OK, W10 none, installed Google Noto font
    { "Lina", QFontDatabase::Any,
        EcScriptType::SYLLABOHIEROGLYPH, EcLangLife::UNDECIPHERED, EcWritingDir::LTR, EcContinent::EUROPE,
        Dating::yapprox(-1800), EcFont::LINEAR_A },
    // Linear B OK, W10 none, installed Google Noto font
    { "Linb", QFontDatabase::Any,
        EcScriptType::SYLLABOHIEROGLYPH, EcLangLife::DECIPHERED, EcWritingDir::LTR, EcContinent::EUROPE,
        Dating::yapprox(-1450), EcFont::LINEAR_B },
    // W10 has, W7 none (though lots of software install) → installed Google Noto
    { "Lisu", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(1915), EcFont::LISU },
    // Lycian OK, W10 P1 Segoe Historic
    /// @todo [font???] Lycian
    { "Lyci", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::DECIPHERED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(-6), EcFont::HISTORIC },
    // Lydian OK, W10 P1 Segoe Historic
    /// @todo [font???] Lydian
    { "Lydi", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::DECIPHERED, EcWritingDir::RTL_MOSTLY, EcContinent::ASIA,
        Dating::crange(-8, -7), EcFont::HISTORIC },
    // Mahajani OK, W10 none, installed Google Noto
    { "Mahj", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::DEAD, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::unknown(), EcFont::MAHAJANI },
    // Makasar OK, installed strange font by Anshuman Pandey
    { "Maka", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::CONSERVED, EcWritingDir::LTR, EcContinent::OCEAN,
        Dating::crange(16, 17), EcFont::MAKASAR },
    // Mandaic OK, W10 has not, installed Google Noto
    { "Mand", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::ENDANGERED, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::crange(2, 7), EcFont::MANDAIC },
    // Manichaean OK, W10 has not, installed Google Noto
    { "Mani", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::DECIPHERED, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::century(3), EcFont::MANICHAEAN },
    // Marchen OK, W10 none, installed Google Noto
    { "Marc", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::SACRED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(7), EcFont::MARCHEN },
    // Medefaidrin OK, W10 none, installed Google Noto
    { "Medf", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::DEAD, EcWritingDir::LTR, EcContinent::AFRICA,
        Dating::yapprox(1931), EcFont::MEDEFAIDRIN },
    // Mende Kikakui OK, W10 none → installed Google Noto
    { "Mend", QFontDatabase::Any,
        EcScriptType::SYLLABLE, EcLangLife::DEAD, EcWritingDir::RTL, EcContinent::AFRICA,
        Dating::yapprox(1920), EcFont::MENDE_KIKAKUI },
    // Meroitic cursive OK, W10 has small part
    { "Merc", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::DECIPHERED, EcWritingDir::RTL, EcContinent::AFRICA,
        Dating::century(-3), EcFont::MEROITIC },
    // Meroitic OK, W10 none
    { "Mero", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::DECIPHERED, EcWritingDir::RTL_COL, EcContinent::AFRICA,
        Dating::century(-3), EcFont::MEROITIC, Sfg::DESC_FROM_PREV },
    // Malayalam OK, W7 does not have 2014/17 extensions → installed Google Noto
    { "Mlym", QFontDatabase::Malayalam,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(830, StdNote::FIRST_KNOWN), EcFont::MALAYALAM },
    // Modi OK, W10 none, installed Google Noto
    { "Modi", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::DEAD, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::yrange(1200, 1600), EcFont::MODI },
    // Mongol OK, dislike standard font of W10 → installed Google Noto
    { "Mong", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::REVIVED, EcWritingDir::LTR_COL, EcContinent::ASIA,
        Dating::special(), EcFont::MONGOLIAN },
    // Mro OK, installed Google Noto font
    { "Mroo", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::decade(1980), EcFont::MRO },
    // Meetei Mayek OK, W10 does not have extensions → installed Google Noto
    { "Mtei", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::REVIVED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::yapprox(1500, StdNote::FIRST_KNOWN), EcFont::MEETEI },
    // Multani OK, W10 none → installed Google Noto
    { "Mult", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::DEAD, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(18), EcFont::MULTANI },
    // Myanmar OK, W10 “Myanmar Text”, W7 none → installed Google Noto font
    { "Mymr", QFontDatabase::Myanmar,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(1035, StdNote::FIRST_KNOWN), EcFont::MYANMAR },
    // Nag OK, done in FunkySample
    { "Nagm", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::NEW, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::yrange(1949, 1982), EcFont::FUNKY },
    // Nandinagari OK, in Nov 2021 Google finally made Noto font
    { "Nand", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::CONSERVED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(8, StdNote::FIRST_KNOWN), EcFont::NANDINAGARI },
    // North Arabian OK, W10 none → installed Google Noto
    { "Narb", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::DECIPHERED, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::century(-3, StdNote::FIRST_KNOWN), EcFont::NORTH_ARABIAN },
    // Nabataean OK, W10 none → installed Google Noto
    { "Nbat", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::DECIPHERED, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::century(-2), EcFont::NABATAEAN },
    // Newa OK, W10 none → installed Google Noto
    { "Newa", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(1654, StdNote::CUSTOM), EcFont::NEWA },
    // NKo ok, W10 “Ebrima”, W7 lacks a few chars → installed Google Noto
    { "Nkoo", QFontDatabase::Nko,
        EcScriptType::ALPHABET, EcLangLife::NEW, EcWritingDir::RTL, EcContinent::AFRICA,
        Dating::year(1949), EcFont::NKO },
    // Nushu OK, W10 off, installed Google Noto
    { "Nshu", QFontDatabase::Any,
        EcScriptType::SYLLABLE, EcLangLife::DEAD, EcWritingDir::LTR_CJK, EcContinent::CJK,
        Dating::special(), EcFont::NUSHU },
    // Ogham OK, Windows font is differently named in W7 and W10 → installed Google Noto
    { "Ogam", QFontDatabase::Ogham,
        EcScriptType::ALPHABET, EcLangLife::CONSERVED, EcWritingDir::LTR_BU, EcContinent::EUROPE,
        Dating::century(4, StdNote::FIRST_KNOWN), EcFont::OGHAM },
    // Ol Chiki OK, W10 “Nirmala UI”, W7 none → installed Google Noto
    { "Olck", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::NEW, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(1925), EcFont::OLCHIKI },
    { "Orkh", QFontDatabase::Any,
        EcScriptType::ALPHASYLLABLE, EcLangLife::DECIPHERED, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::century(6), EcFont::ORKHON },
    // Oriya OK, took Noto + everything bad manually fixed
    { "Orya", QFontDatabase::Oriya,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(14, StdNote::MODERN_FORM), EcFont::ORIYA },
    // Osage OK, W10 Gadugi
    { "Osge", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::NEW, EcWritingDir::LTR, EcContinent::AMERICA,
        Dating::year(2006), EcFont::GADUGI },
    // Osmanya OK, W10 Ebrima
    { "Osma", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::NEW, EcWritingDir::LTR, EcContinent::AFRICA,
        Dating::yapprox(1920), EcFont::VAI },
    // Old Uyghur OK, installed Noto font
    { "Ougr", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::DECIPHERED, EcWritingDir::LTR_COL, EcContinent::ASIA,
        Dating::century(8), EcFont::OLD_UYGHUR },
    // Plamyrene OK, W10 none, installed Google Noto
    { "Palm", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::DECIPHERED, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::year(-44, StdNote::FIRST_KNOWN), EcFont::PALMYRENE },
    // Pau Cin Hau OK, W10 none, installed Google Noto
    { "Pauc", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::COMPETING, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::special(), EcFont::PAU_CIN_HAU},
    // Old Permic OK, W10 none, installed Google Noto
    { "Perm", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::DECIPHERED, EcWritingDir::LTR, EcContinent::EUROPE,
        Dating::year(1372), EcFont::OLD_PERMIC },
    // Phags-Pa OK, have “Microsoft PhagsPa” font
    { "Phag", QFontDatabase::Any,
        EcScriptType::ARGUABLE, EcLangLife::SACRED, EcWritingDir::LTR_COL, EcContinent::ASIA,
        Dating::year(1269), EcFont::PHAGS_PA },
    // Inscriptional Pahlavi OK, W10 Historic
    { "Phli", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::DECIPHERED, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::century(-2), EcFont::HISTORIC },
    // Psalter Pahlavi OK, W10 none → installed Google Noto
    { "Phlp", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::DECIPHERED, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::century(6, StdNote::FIRST_KNOWN), EcFont::PSALTER_PAHLAVI },
    // Phoenician OK, because of importance installed Google Noto
    { "Phnx", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::DECIPHERED, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::century(-11), EcFont::PHOENICIAN },
    // Miao/Pollard OK, W10 none, installed Google Noto
    { "Plrd", QFontDatabase::Any,
        EcScriptType::ABUGIDA, EcLangLife::COMPETING, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(1936), EcFont::POLLARD },
    // Inscriptional Parthian OK, W10 Historic
    { "Prti", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::DECIPHERED, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::yapprox(-250), EcFont::PARTHIAN },
    // Rejang OK, installed Google Noto font
    { "Rjng", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::DEAD, EcWritingDir::LTR, EcContinent::OCEAN,
        Dating::yapprox(1750, StdNote::FIRST_KNOWN), EcFont::REJANG },
    // Rohingya, installed Google Noto font
    { "Rohg", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::NEW, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::decade(1980), EcFont::ROHINGYA },
    // Runes OK, several tofu → installed Google Noto font
    { "Runr", QFontDatabase::Runic,
        EcScriptType::ALPHABET, EcLangLife::CONSERVED, EcWritingDir::LTR, EcContinent::EUROPE,
        Dating::century(2, StdNote::FIRST_KNOWN), EcFont::RUNIC },
    // Samaritan OK, installed Google Noto font, added dotted circle
    { "Samr", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::ENDANGERED, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::yrange(-600, -200), EcFont::SAMARITAN },
    // South Arabian OK, W10 Segoe UI Historic
    /// @todo [font???] South Arabian
    { "Sarb", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::DECIPHERED, EcWritingDir::RTL_MOSTLY, EcContinent::ASIA,
        Dating::century(-9, StdNote::FIRST_KNOWN), EcFont::HISTORIC },
    // Saurashtra OK, W10 none, installed Google Noto
    { "Saur", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::COMPETING, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(19, StdNote::MAYBE_EARLIER), EcFont::SAURASHTRA },
    // Sutton SignWriting OK, installed Google Noto font
    { "Sgnw", QFontDatabase::Any,
        EcScriptType::NONTEXT, EcLangLife::NEW, EcWritingDir::TD, EcContinent::NONE,
        Dating::year(1974), EcFont::SIGNWRITING },
    // Shavian OK, W10 Segoe UI Historic
    { "Shaw", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::EXPERIMENTAL, EcWritingDir::LTR, EcContinent::EUROPE,
        Dating::year(1960), EcFont::HISTORIC },
    // Sharada OK, W10 none → installed Google Noto
    { "Shrd", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::SACRED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::yapprox(700), EcFont::SHARADA },
    // Siddham OK, W10 none, installed Google Noto font
    { "Sidd", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::SACRED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(6), EcFont::SIDDHAM },
    // Sindhi (Khudawadi) OK, W10 none, installed Google Noto font
    { "Sind", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::COMPETING, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(16), EcFont::KHUDAWADI },
    // Sinhala OK, W10 obviously has no 2020 extension → installed Google Noto
    { "Sinh", QFontDatabase::Sinhala,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::OCEAN,
        Dating::century(13, StdNote::MODERN_FORM), EcFont::SINHALA },
    // Sogdian OK, W10 None → installed Google Noto
    { "Sogd", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::DECIPHERED, EcWritingDir::SOGDIAN, EcContinent::ASIA,
        Dating::yapprox(500), EcFont::SOGDIAN },
    // Old Sogdian OK, W10 None → installed Google Noto
    { "Sogo", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::DECIPHERED, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::yapprox(100), EcFont::OLD_SOGDIAN, Sfg::DESC_FROM_PREV },
    // Sora Sompeng OK, W10 Nirmala UI
    { "Sora", QFontDatabase::Any,
        EcScriptType::ARGUABLE, EcLangLife::NEW, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(1936), EcFont::TAMIL },
    // Soyombo OK, W10 none → installed Google Noto
    { "Soyo", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::SACRED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(1686), EcFont::SOYOMBO },
    // Sundanese OK, even W10 off → installed Google Noto
    { "Sund", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::REVIVED, EcWritingDir::LTR, EcContinent::OCEAN,
        Dating::century(14), EcFont::SUNDANESE },
    // Syloti Nagri OK, W10 none → installed Google Noto
    { "Sylo", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::REVIVED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(15), EcFont::SYLOTI_NAGRI },
    // Syriac OK, W10 has no Malayalam extensions → Beth Mardutho made a nice font.
    { "Syrc", QFontDatabase::Syriac,
        EcScriptType::CONSONANT, EcLangLife::SACRED, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::century(1, StdNote::FIRST_KNOWN), EcFont::SYRIAC },
    // Tagbanwa OK, W10 off → installed Google Noto
    { "Tagb", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::OCEAN,
        Dating::yapprox(1300), EcFont::TAGBANWA },
    // Takri OK, W10 none → installed Google Noto
    { "Takr", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::DEAD, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::crange(14, 18), EcFont::TAKRI },
    // Old Tai Le OK, M$ has a good font
    { "Tale", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::yapprox(1200), EcFont::TAILE },
    // New Tai Lue OK, M$ has a good font
    { "Talu", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::EXPERIMENTAL, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::decade(1950), EcFont::TAILUE },
    // Tamil BMP OK, different fonts in W10 and W7
    { "Taml", QFontDatabase::Tamil,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::crange(6, 9), EcFont::TAMIL },
    // Tangut OK, installed Google Noto
    { "Tang", QFontDatabase::Any,
        EcScriptType::HIEROGLYPH, EcLangLife::DECIPHERED, EcWritingDir::TANGUT, EcContinent::CJK,
        Dating::year(1036), EcFont::TANGUT },
    // Tai Viet OK, installed Google Noto font
    { "Tavt", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(16), EcFont::TAI_VIET },
    // Telugu OK, W7 lacks recent extensions → installed Google Noto
    { "Telu", QFontDatabase::Telugu,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(14, StdNote::CUSTOM), EcFont::TELUGU },
    // Tifinagh OK, W7 “Ebrima” of course has no 2012 chars → installed Google Noto
    { "Tfng", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::REVIVED, EcWritingDir::LTR, EcContinent::AFRICA,
        Dating::special(), EcFont::TIFINAGH },
    // Tagalog OK, W10 off → installed Google Noto
    { "Tglg", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::REVIVED, EcWritingDir::LTR, EcContinent::OCEAN,
        Dating::yrange(1300, 1500), EcFont::TAGALOG },
    // Thaana OK, Monotype/M$ “MV Boli” font
    { "Thaa", QFontDatabase::Thaana,
        EcScriptType::ARGUABLE, EcLangLife::ALIVE, EcWritingDir::RTL, EcContinent::OCEAN,
        Dating::century(18), EcFont::THAANA },
    // Thai OK, W7 and 10 fonts are different, but managed to
    { "Thai", QFontDatabase::Thai,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(1283), EcFont::THAI },
    // Tibetan OK, W10 did not have swastika, installed Google Noto
    { "Tibt", QFontDatabase::Tibetan,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::yapprox(650), EcFont::TIBETAN },
    // Tirhuta OK, W10 none, installed Google Noto
    { "Tirh", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::COMPETING, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(13, StdNote::CUSTOM), EcFont::TIRHUTA },
    // Tangsa OK, U14, called author and he gave font
    { "Tnsa", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::NEW, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(1990), EcFont::TANGSA },
    // Toto OK, U14, moved that font to FunkySample
    { "Toto", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(2015), EcFont::FUNKY },
    // Ugaritic OK
    { "Ugar", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::DECIPHERED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::yapprox(-1400), EcFont::HISTORIC },
    // Vai OK, have good M$ font Ebrima
    { "Vaii", QFontDatabase::Any,
        EcScriptType::SYLLABLE, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::AFRICA,
        Dating::decade(1830), EcFont::VAI },
    // Vithkuqi OK, installed Google Noto
    { "Vith", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::EXPERIMENTAL, EcWritingDir::LTR, EcContinent::EUROPE,
        Dating::year(1844, StdNote::FIRST_KNOWN), EcFont::VITHKUQI },
    // Wancho OK, W10 none, installed Google Noto
    { "Wcho", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::NEW, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::yrange(2001, 2012), EcFont::WANCHO },
    // Warang Citi OK, W10 none → installed Google Noto
    { "Wara", QFontDatabase::Any,
        EcScriptType::ARGUABLE, EcLangLife::NEW, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::decade(1950), EcFont::WARANG_CITI },
    // Old Persian OK. Small font, let’s install!!
    { "Xpeo", QFontDatabase::Any,
        EcScriptType::ALPHASYLLABLE, EcLangLife::DECIPHERED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::yapprox(-520), EcFont::OLD_PERSIAN },
    /// @todo [semi-tofu, P1] Sumerian cuneiform shows badly, Noto = M$ (but Noto is larger!)
    { "Xsux", QFontDatabase::Any,
        EcScriptType::SYLLABOHIEROGLYPH, EcLangLife::DECIPHERED, EcWritingDir::LTR_CUNEIFORM, EcContinent::ASIA,
        Dating::yapprox(-3200), EcFont::CUNEIFORM },
    // Yezidi OK, W10 none → installed Google Noto
    { "Yezi", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::SACRED, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::crange(13, 17), EcFont::YEZIDI },
    // Yi OK, have M$ font. Though quite thin, Noto is better, I managed to counteract by increasing font.
    { "Yiii", QFontDatabase::Any,
        EcScriptType::SYLLABLE, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(1974, StdNote::CUSTOM), EcFont::YI },
    // Zanabazar square OK, installed Google Noto
    { "Zanb", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::CONSERVED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::ybefore(1700), EcFont::ZANABAZAR },
    { "Zinh", QFontDatabase::Any,
        EcScriptType::NONE, EcLangLife::NOMATTER, EcWritingDir::NOMATTER, EcContinent::NONE,
        Dating::none(), EcFont::NORMAL, Sfg::NO_LANGS },
};


static_assert(std::size(uc::scriptInfo) == static_cast<size_t>(uc::EcScript::NN));


const uc::Script* uc::findScript(std::string_view x) { return findInArray(x, scriptInfo); }


constinit const uc::NumType uc::numTypeInfo[static_cast<int>(uc::EcNumType::NN)] {
    { {} },
    { "Prop.Num.Digit" },
    { "Prop.Num.SpecDigit" },
    { "Prop.Num.Number" },
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

void uc::BidiClass::printfLocKey(char* buf, size_t n, const char* suffix) const
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
    { "brahmic", EcTermCat::SCRIPT_CLASS, u8"Brahmic abugida, Indic script", {}, EcFont::DEVANAGARI },
    { "alphabet", EcTermCat::SCRIPT_CLASS, u8"alphabet script" },
    { "arguable", EcTermCat::SCRIPT_CLASS, {} },
    { "cjk", EcTermCat::SCRIPT_CLASS, u8"CJK = Chinese, Japanese, Korean" },
    { "code", EcTermCat::SCRIPT_CLASS, u8"codes and shorthand" },
    { "nontext", EcTermCat::SCRIPT_CLASS, u8"non-text notation" },

    { "acrophonic", EcTermCat::CHAR_CLASS, u8"acrophonic character" },
    { "virama", EcTermCat::CHAR_CLASS, u8"virama" },
    { "graphic", EcTermCat::CHAR_CLASS, u8"graphic character, printable character" },
    { "deprecated", EcTermCat::CHAR_CLASS, u8"deprecated character" },
    { "ignorable", EcTermCat::CHAR_CLASS, u8"default-ignorable character" },
    { "combining", EcTermCat::CHAR_CLASS, u8"combining mark" },
    { "ligature", EcTermCat::CHAR_CLASS, u8"ligature" },
    { "precomposed", EcTermCat::CHAR_CLASS, u8"precomposed character" },
    { "space", EcTermCat::CHAR_CLASS, u8"whitespace", { "CharCat.Zs.Text" } },
    { "spacing", EcTermCat::CHAR_CLASS, u8"spacing character" },
    { "pseudographics", EcTermCat::CHAR_CLASS, u8"pseudographics, semigraphics, box drawing" },
    { "control", EcTermCat::CHAR_CLASS, u8"control character", { "CharCat.Cc.Text" } },
    { "obsolete", EcTermCat::CHAR_CLASS, u8"obsolete character" },
    { "format", EcTermCat::CHAR_CLASS, u8"format character", { "CharCat.Cf.Text" } },
    { "number", EcTermCat::CHAR_CLASS, u8"number character" },
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
    { { { 0xFF, 0xFF, 0xFF }, { 0x00, 0x00, 0x00 } } },
    // Tech
    { { { 0x22, 0x55, 0x00 }, { 0x7F, 0xFF, 0x2A },
            // Special Technical’s frame colour
            { 0x66, 0xff, 0x00 } } },
    // Europe
    { { { 0x7f, 0xff, 0xff }, { 0x2f, 0x5e, 0x9d } } },
    // Asia
    { { { 0xff, 0xff, 0x83 }, { 0xf5, 0x79, 0x00 } } },
    // CJK
    { { { 0xff, 0xf0, 0xf5 }, { 0x5c, 0x35, 0x66 } } },
    // Ocean
    { { { 0xc8, 0xff, 0xc8 }, { 0x00, 0x64, 0x00 } } },
    // Africa
    { { { 0xf5, 0xde, 0xb3 }, { 0x8f, 0x59, 0x02 } } },
    // America
    { { { 0xff, 0xc8, 0xc8 }, { 0xcc, 0x00, 0x00 } } },
};

// Missing
constinit const uc::Continent MISSING_CONTINENT
    { { { 0xa4, 0x00, 0x00 }, { 0xFF, 0xFF, 0xFF } } };


static_assert(std::size(uc::continentInfo) == static_cast<int>(uc::EcContinent::NN));

constinit const uc::OldCompInfo uc::oldCompInfo[] {
    { u8"Amstrad" NBSP "CPC" },
    { u8"Apple" NBSP "IIc+" },
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
    { u8"Tandy" NBSP "TRS80" NBSP "Mark" NBSP "1" },
    { u8"Tandy" NBSP "TRS80" NBSP "Color" },
    { {}, "Prop.OldComp.Korvet" },
};

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

#define REP_2(x) x,x
#define REP_3(x) x,x,x
#define REP_4(x) x,x,x,x
#define REP_5(x) x,x,x,x,x
#define REP_6(x) x,x,x,x,x,x
#define REP_7(x) x,x,x,x,x,x,x
#define REP_8(x) REP_4(x),REP_4(x)
#define REP_10(x) REP_4(x),REP_4(x),x,x
#define REP_11(x) REP_4(x),REP_4(x),REP_3(x)
#define REP_12(x) REP_4(x),REP_4(x),REP_4(x)
#define REP_13(x) REP_12(x),x
#define REP_16(x) REP_4(x),REP_4(x),REP_4(x),REP_4(x)
#define REP_32(x) REP_16(x),REP_16(x)
#define REP_48(x) REP_16(x),REP_16(x),REP_16(x)
#define REP_60(x) REP_48(x),REP_12(x)

namespace {

    using OC = uc::OldComp;

    constinit Flags<uc::OldComp> oldCompData[] {
        REP_60 (OC::AQUARIUS | OC::TANDY_COCO | OC::MINITEL | OC::ORIC | OC::TELETEXT | OC::TANDY_TRS80 | OC::KORVET ), // 00..3B 6block
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
        REP_2  (OC::MSX | OC::COMMODORE),                                       // 98,99 fill\ and /
        REP_2  (OC::MSX),                                                       // 9A,9B butterflies
        REP_4  (OC::AMSTRAD),                                                   // 9C..9F diag50%
        REP_8  (OC::AMSTRAD | OC::MINITEL | OC::TELETEXT),                      // A0..A3 quarter / or \…
                                                                                // A4..A7 two such slashes nearby
        REP_2  (OC::AMSTRAD | OC::MINITEL),                                     // A8,A9 two such slashes opposite
        REP_4  (OC::MINITEL),                                                   // AA..AD three such slashes
                OC::AMSTRAD | OC::MINITEL,                                      // AE four such slashes
                OC::MSX,                                                        // AF full -, incomplete |
                OC::APPLE,                                                      // B0 mouse cursor
                OC::APPLE | OC::COMMODORE,                                      // B1 inverse check
        REP_11 (OC::APPLE),                                                     // B2..BC misc Apple chars
        REP_3  (OC::ATARI_ST),                                                  // BD..BF inverse saltires
                OC::RISC_OS | OC::TANDY_TRS80,                                  // C0 white saltire
        REP_4  (OC::TANDY_TRS80),                                               // C1..C3 pointing finger
                                                                                // C4 inverse ?
                OC::AMSTRAD | OC::TANDY_TRS80,                                  // C5 man
        REP_3  (OC::AMSTRAD),                                                   // C6..C8 three men
        REP_2  (OC::TANDY_TRS80),                                               // C9 woman
                                                                                // CA chevron
        REP_5  ({}),                                                            // CB..CF unused
        REP_32 ({}),                                                            // D0..EF unused
        REP_10 (OC::ATARI_ST)                                                   // F0..F9 7seg digits
    };

    static_assert(std::size(oldCompData) == 0xFA);                              // FA first free

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

    constinit const ReverseMap rmWin {{
        { u'‚', 130 }, { u'ƒ', 131 }, { u'„', 132 }, { u'…', 133 },
        { u'†', 134 }, { u'‡', 135 }, { u'ˆ', 136 }, { u'‰', 137 },
        { u'Š', 138 }, { u'‹', 139 }, { u'Œ', 140 }, { u'Ž', 142 },
        { u'‘', 145 }, { u'’', 146 }, { u'“', 147 }, { u'”', 148 },
        { u'•', 149 }, { u'–', 150 }, { u'—', 151 }, { u'˜', 152 },
        { u'™', 153 }, { u'š', 154 }, { u'›', 155 }, { u'œ', 156 },
        { u'ž', 158 }, { u'Ÿ', 159 }
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
    cp -= 0x1FB00;
    if (cp >= std::size(oldCompData))
        return {};
    return oldCompData[cp];
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
constexpr char32_t SGNW_HEAD = 0x1D9FF;

char32_t sw::Info::baseChar() const noexcept
{
    if (cp().category().upCat == uc::UpCategory::MARK) {
        return DOTTED_CIRCLE;
    }
    return 0;
}


///// Continent ////////////////////////////////////////////////////////////////


const uc::Continent& uc::SynthIcon::continent() const
{
    if (flags.have(Ifg::MISSING))
        return MISSING_CONTINENT;
    return continentInfo[static_cast<int>(ecContinent)];
}


///// UC misc //////////////////////////////////////////////////////////////////

namespace {
    [[maybe_unused]] std::u8string cache;
    [[maybe_unused]] inline std::string_view esc(std::u8string_view x)
    {
        return str::toSv(escape::cppSv(
                        x, cache, 'n', escape::Spaces::YES, Enquote::NO));
    }
}


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
        auto block = blockOf(cp.subj);
        if (!block->firstAllocated)
            block->firstAllocated = &cp;
        block->lastAllocated = &cp;
        ++block->nChars;
        block->ecVersion = std::min(block->ecVersion, cp.ecVersion);
        block->ecLastVersion = std::max(block->ecLastVersion, cp.ecVersion);
        // Lookup table
        cpsByCode[cp.subj.val()] = &cp;
    }

    // Check blocks — they should have at least one char
    for (auto& v : allBlocks()) {
        if (!v.firstAllocated || !v.lastAllocated)
            throw std::logic_error("Block w/o chars leaked into data!");

        // Check synthesized icon
        if (!v.synthIcon.flags.have(Ifg::MISSING)
                && (v.synthIcon.subj < v.startingCp || v.synthIcon.subj > v.endingCp))
            throw std::logic_error(
                    "Synthesized icon is not within block " + std::string{v.name});
        if (!cpsByCode[v.synthIcon.subj])
            throw std::logic_error("Synthesized icon points to bad char!");
        if (!v.synthIcon.flags.have(Ifg::CONTINENT_OK)) {
            auto& script = v.script();
            if (script.ecContinent != v.synthIcon.ecContinent) {
                std::cout << "Continent mismatch: " << v.name << std::endl;
            }
        }

        if (v.alphaKey.subKey == 0) {
            if (v.ecScript != EcScript::NONE
                    && v.alphaKey.ecScript == v.ecScript) { // Actual script
                v.script().mainBlock = &v;
            } else if (v.alphaKey.script().flags.have(Sfg::SORT_KEY)) { // Pseudo-script for sorting
                v.alphaKey.script().mainBlock = &v;
            }
        }
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
    if (q.loaded)
        return;

    // Find in cache
    if (auto it = loadedFonts.find(family.text); it != loadedFonts.end()) {
        q.loaded = it->second;
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

        if (family.flags.have(Fafg::RAW_FONT) && tempFont.mems) {
            q.loaded->rawFont = std::make_unique<QRawFont>(tempFont.mems->qdata(), 50);
        }
    } else {
        // FAMILY
        newLoadedStruc();
        q.loaded->familiesComma = str::toQ(family.text);
        q.loaded->families = toQList(family.text);
    }

    // Make probe font
    q.loaded->get(q.loaded->probe,  fst::TOFU,   flags);
    q.loaded->get(q.loaded->normal, fst::DEFAULT, flags);
        // force EXACT match
    q.loaded->probeMetrics = std::make_unique<QFontMetrics>(*q.loaded->probe);
    doesSupportChar(trigger);
    if (probeChar && !doesSupportChar(probeChar.value))
        q.isRejected = true;
}


QFont uc::Font::get(FontPlace place, int size, bool noAa, char32_t trigger) const
{
    load(trigger);
    QFont font = *q.loaded->normal;
    font.setPointSize(computeSize(place, size));
    if (place == FontPlace::CELL && noAa) {
        font.setStyleStrategy(fst::NO_AA);
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
    if (q.loaded->rawFont) {
        return q.loaded->rawFont->supportsCharacter(subj);
    } else {
        return q.loaded->probeMetrics->inFontUcs4(subj);
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
    std::u8string_view r;
    traverseAllT([&r, role](TextRole aRole, std::u8string_view aText) {
        if (role == aRole) {
            r = aText;
            return Action::STOP;
        }
        return Action::CONTINUE;
    });
    return r;
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


uc::SampleProxy uc::Cp::sampleProxy(EmojiDraw emojiDraw) const
{
    switch (drawMethod(emojiDraw)) {
    case DrawMethod::SAMPLE:
        break;  // go through
    default:
        return {};
    }

    auto fn = font(uc::MatchLast::NO);
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

    switch (ecCategory) {
    case EcCategory::MARK_ENCLOSING:
    case EcCategory::MARK_NONSPACING:
    case EcCategory::MARK_SPACING:
        // Stub off?
        if (fn->flags.have(Ffg::STUB_OFF))
            break;
        if (fn->flags.have(Ffg::STUB_RTL))
            return { str::toQ(STUB_RTL_CIRCLE) + str::toQ(code), style };
        if (fn->flags.have(Ffg::STUB_ALM))
            return { str::toQ(STUB_ALM_CIRCLE) + str::toQ(code), style };
        return { STUB_CIRCLE + str::toQ(code), style };
    case EcCategory::SEPARATOR_SPACE:
        if (isTrueSpace()) {
            return { QChar(L'▕') + str::toQ(code) + QChar(L'▏'), style };
        }
        break;
    default: ;
    }
    if (fn->flags.have(Ffg::STUB_INTERCHAR))
        return { str::toQ(code) + str::toQ(code), style };
    return { str::toQ(code), style };
}


bool uc::Cp::isGraphical() const
{
    return static_cast<bool>(category().isGraphical);
}


uc::DrawMethod uc::Cp::drawMethod(EmojiDraw emojiMode) const
{
    if (flags.haveAny(Cfg::M_ALL)) [[unlikely]] {
        if (flags.have(Cfg::M_CUSTOM_CONTROL))
            return uc::DrawMethod::CUSTOM_CONTROL;
        if (flags.have(Cfg::M_SVG_EMOJI)) {
                bool isSvg = true;
                switch (emojiMode) {
                case EmojiDraw::TEXT:
                    isSvg = !flags.have(Cfg::U_VS16_EMOJI);
                    break;
                case EmojiDraw::CONSERVATIVE:
                    isSvg = !block().flags.have(Bfg::NO_EMOJI);
                    break;
                case EmojiDraw::GRAPHIC:
                    break;
                }
                return isSvg ? uc::DrawMethod::SVG_EMOJI : uc::DrawMethod::SAMPLE;
            }
        if (flags.have(Cfg::M_SPACE))
            return uc::DrawMethod::SPACE;
        if (flags.have(Cfg::M_EGYPTIAN_HATCH))
            return uc::DrawMethod::EGYPTIAN_HATCH;
        if (isAbbreviated())
            return uc::DrawMethod::ABBREVIATION;
    }
    if (isTrueSpace())
        return uc::DrawMethod::SPACE;
    return uc::DrawMethod::SAMPLE;
}


QString uc::Cp::osProxy() const
{
    if (!isGraphical())
        return {};

    switch (category().upCat) {
    case UpCategory::MARK:
        return STUB_CIRCLE + str::toQ(subj.ch32());
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
    if (blk.flags.have(Bfg::UNGLITCH_MARKS) && category().upCat == UpCategory::MARK) {
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


const uc::Font* uc::Cp::font(MatchLast matchLast) const
{
    if (subj.ch32() == 0xFFFFFF) {
        std::cout << "Debug here!" << std::endl;
    }
    auto v = &firstFont();
    bool isBuggy = flags.have(Cfg::RENDER_BUG);
    auto sb = subj.ch32();
    while (v->flags.have(Ffg::FALL_TO_NEXT)) {
        auto wantSkip = isBuggy
                ? v->flags.have(Ffg::BUG_AVOID)     // BUGGY: avoid flag → bad, it’s for normal only
                : v->flags.have(Ffg::BUG_FIXUP);    // NORMAL: fixup flag → bad, it’s for buggy only
        if (!wantSkip) {
            if (v->doesSupportChar(sb))
                return v;
        }
        // Once again if accepted and that flag is present
        if (!v->q.isRejected && v->flags.have(Ffg::FALL2_IF_ACCEPTED)) {
            ++v;
            // Check for that font’s flag: none → STOP
            // (“system → backup” pair, currently unused, maybe Lontara?)
            if (!v->flags.have(Ffg::FALL_TO_NEXT)) {
                // what the frag to do?
                if (matchLast == MatchLast::NO)
                    return v;
                return nullptr;
            }
        }
        ++v;
    }
    if (matchLast != MatchLast::NO) {
        if (!v->doesSupportChar(sb))
            return nullptr;
    }
    return v;
}


uc::TofuInfo uc::Cp::tofuInfo() const
{
    uc::TofuInfo r;
    r.block = &block();
    if (r.block->flags.haveAny(Bfg::COLLAPSIBLE | Bfg::CJK)
            || script().ecContinent == EcContinent::CJK)
        r.place = TofuPlace::CJK;

    if (drawMethod(EmojiDraw::CONSERVATIVE) > uc::DrawMethod::LAST_FONT) {
        r.state = TofuState::NO_FONT;
    } else {
        r.state = font(MatchLast::YES) ? TofuState::PRESENT : TofuState::TOFU;
    }
    return r;
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


void uc::Block::printfLocKey(char* buf, size_t n, const char* suffix) const
{
    snprintf(buf, n, "Block.%04X.%s", (int)startingCp, suffix);
}


void uc::Script::printfLocKey(char* buf, size_t n, const char* suffix) const
{
    snprintf(buf, n, "Script.%s.%s", id.data(), suffix);
}


void uc::Category::printfLocKey(char* buf, size_t n, const char* suffix) const
{
    snprintf(buf, n, "CharCat.%s.%s", id.data(), suffix);
}

void uc::Term::printfLocKey(char* buf, size_t n, const char* suffix) const
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


QFont uc::funkyFont(FontPlace place, int size, char32_t trigger)
{
    auto& font = fontInfo[static_cast<int>(uc::EcFont::FUNKY)];
    return font.get(place, size, false, trigger);
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

    for (auto& sc : scriptInfo) {
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
        auto specialSort = SpecialSort::NO;
        if (blk.alphaKey.ecScript != EcScript::NONE) {
            keyName = script.loc.name;
            specialSort = SpecialSort::YES;
            if (script.mainBlock)
                keyName = script.mainBlock->loc.name;
        }
        buildSortKey(keyName, specialSort, sortOrder, blk.loc.sortKey);
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
