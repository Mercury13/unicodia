#include "UcData.h"

constexpr uc::LangLoc LOC_AFRICA_W      { .locSubKey = "AfricaW"     };
constexpr uc::LangLoc LOC_BANGLADESH    { .locSubKey = "Bangladesh"  };
constexpr uc::LangLoc LOC_BHUTAN        { .locSubKey = "Bhutan"      };
constexpr uc::LangLoc LOC_CAMBODIA      { .locSubKey = "Cambodia"    };
constexpr uc::LangLoc LOC_CHINA         { .locSubKey = "China"       };
constexpr uc::LangLoc LOC_INDIA_CEN     { .locSubKey = "IndiaCen"    };
constexpr uc::LangLoc LOC_INDIA_E       { .locSubKey = "IndiaE"      };
constexpr uc::LangLoc LOC_INDIA_N       { .locSubKey = "IndiaN"      };
constexpr uc::LangLoc LOC_INDIA_NE      { .locSubKey = "IndiaNe"     };
constexpr uc::LangLoc LOC_INDIA_S       { .locSubKey = "IndiaS"      };
constexpr uc::LangLoc LOC_INDIA_SE      { .locSubKey = "IndiaSe"     };
constexpr uc::LangLoc LOC_INDIA_SW      { .locSubKey = "IndiaSw"     };
constexpr uc::LangLoc LOC_INDIA_W       { .locSubKey = "IndiaW"      };
constexpr uc::LangLoc LOC_LIBERIA       { .locSubKey = "Liberia"     };
constexpr uc::LangLoc LOC_MYANMAR       { .locSubKey = "Myanmar"     };
constexpr uc::LangLoc LOC_NEPAL         { .locSubKey = "Nepal"       };
constexpr uc::LangLoc LOC_PAKISTAN      { .locSubKey = "Pakistan"    };
constexpr uc::LangLoc LOC_SIERRA_LEONE  { .locSubKey = "SierraLeone" };
constexpr uc::LangLoc LOC_SRI_LANKA     { .locSubKey = "SriLanka"     };
constexpr uc::LangLoc LOC_SUMATRA       { .locSubKey = "Sumatra"     };
constexpr uc::LangLoc LOC_THAILAND      { .locSubKey = "Thailand"    };
constexpr uc::LangLoc LOC_USA           { .locSubKey = "Usa"         };
constexpr uc::LangLoc LOC_VIETNAM       { .locSubKey = "Vietnam"     };

constinit const uc::Script uc::scriptInfo[] {
    { "Zyyy", QFontDatabase::Any,
        EcScriptType::NONE, EcLangLife::NOMATTER, EcWritingDir::NOMATTER, EcContinent::NONE,
            Dating::none(),
            EcFont::NORMAL, Sfg::NONSCRIPT | Sfg::NO_LANGS | Sfg::NO_STATS },
    // Arrows pseudo-script
    { "ZARR", QFontDatabase::Any,
        EcScriptType::NONE, EcLangLife::NOMATTER, EcWritingDir::NOMATTER, EcContinent::NONE,
            Dating::none(),
            EcFont::NORMAL, Sfg::NONSCRIPT | Sfg::NO_LANGS | Sfg::SORT_KEY | Sfg::NO_STATS },
    // Mathematical pseudo-script
    { "ZMAT", QFontDatabase::Any,
        EcScriptType::NONE, EcLangLife::NOMATTER, EcWritingDir::NOMATTER, EcContinent::NONE,
            Dating::none(),
            EcFont::NORMAL, Sfg::NONSCRIPT | Sfg::NO_LANGS | Sfg::SORT_KEY | Sfg::NO_STATS },
    // Symbols and pictographs pseudo-script
    { "ZSYM", QFontDatabase::Any,
        EcScriptType::NONE, EcLangLife::NOMATTER, EcWritingDir::NOMATTER, EcContinent::NONE,
            Dating::none(),
            EcFont::NORMAL, Sfg::NONSCRIPT | Sfg::NO_LANGS | Sfg::SORT_KEY | Sfg::NO_STATS },
    // Dingbats pseudo-script
    { "ZDIN", QFontDatabase::Any,
        EcScriptType::NONE, EcLangLife::NOMATTER, EcWritingDir::NOMATTER, EcContinent::NONE,
            Dating::none(),
            EcFont::NORMAL, Sfg::NONSCRIPT | Sfg::NO_LANGS | Sfg::SORT_KEY | Sfg::NO_STATS },
    // Diacritical pseudo-script
    { "ZDIA", QFontDatabase::Any,
        EcScriptType::NONE, EcLangLife::NOMATTER, EcWritingDir::NOMATTER, EcContinent::NONE,
            Dating::none(),
            EcFont::NORMAL, Sfg::NONSCRIPT | Sfg::NO_LANGS | Sfg::SORT_KEY | Sfg::NO_STATS },
    // Kangxi pseudo-script
    { "ZKXI", QFontDatabase::Any,
        EcScriptType::NONE, EcLangLife::NOMATTER, EcWritingDir::NOMATTER, EcContinent::NONE,
            Dating::none(),
            EcFont::NORMAL, Sfg::NONSCRIPT | Sfg::NO_LANGS | Sfg::SORT_KEY | Sfg::NO_STATS },
    // Adlam OK, W10 has, but placement of umlauts + RTL = ??? → better Noto
    { "Adlm", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::NEW, EcWritingDir::RTL, EcContinent::AFRICA,
        Dating::special(), EcFont::ADLAM, NO_FLAGS,
        { .mantissa = 10, .numOrder = NumOrder::MILLION, .flags = Langfg::GREATER_THAN | Langfg::CUSTOM_PRENOTE,
          .year = 0 } },
    // Caucasian Albanian OK, W10 none, installed Google Noto
    { "Aghb", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::DECIPHERED, EcWritingDir::LTR, EcContinent::EUROPE,
        Dating::yapprox(400), EcFont::FRANKEN_SANS },
    // Ahom OK, installed Google Noto font
    { "Ahom", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::REVIVED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(13), EcFont::AHOM },
    // Arabic OK, three fonts: SIL Scheherazade main, Google Noto Math + Google Noto Arabic for special ranges
    { "Arab", QFontDatabase::Arabic,
        EcScriptType::CONSONANT, EcLangLife::ALIVE, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::century(7, StdNote::MODERN_FORM), EcFont::ARABIC, Sfg::STUB_ALM,
        { .mantissa = 380, .numOrder = NumOrder::MILLION, .flags = Langfg::AS_NATIVE | Langfg::DECADE, .year = 2020 } },
    // Imperial Aramaic OK, because of sheer importance install Google Noto
    { "Armi", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::DECIPHERED, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::crange(-10, -9), EcFont::FRANKEN_SANS },
    // Armenian OK, Cambria+Noto is more than nice!
    { "Armn", QFontDatabase::Armenian,
        EcScriptType::ALPHABET, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(405), EcFont::ARMENIAN, NO_FLAGS,
        { .mantissa = 38, .numOrder = NumOrder::HUN_THOUSAND, .flags = Langfg::GREATER_THAN, .year = 2022 } },
    // Avestan OK, installed Google Noto
    /// @todo [future, link] wait for book Pahlavi (sorry, cannot put todo at lockit)
    { "Avst", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::SACRED, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::yapprox(400), EcFont::FRANKEN_SANS },
    // Balinese OK, none even in W10, installed Google Noto font
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
        Dating::yapprox(1900), EcFont::FRANKEN_SANS, NO_FLAGS,
        { .mantissa = 400, .numOrder = NumOrder::THOUSAND, .year = 2006, .locations { LOC_LIBERIA } } },
    // Batak OK, installed Google Noto font
    { "Batk", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::DEAD, EcWritingDir::LTR, EcContinent::OCEAN,
        Dating::yapprox(1300), EcFont::BATAK },
    // Bengali OK, W7 has tofu of 2009+ → installed Google Noto
    { "Beng", QFontDatabase::Bengali,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::special(), EcFont::BENGALI, NO_FLAGS,
        { .mantissa = 230, .numOrder = NumOrder::MILLION, .flags = Langfg::AS_NATIVE, .year = 2021,
          .locations { LOC_BANGLADESH, LOC_INDIA_E } } },
    // Bhaiksuki OK, installed Google Noto
    { "Bhks", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::CONSERVED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(11), EcFont::BHAIKSUKI },
    // Bopomofo OK
    { "Bopo", QFontDatabase::Any,
        EcScriptType::ALPHASYLLABLE, EcLangLife::COMPETING, EcWritingDir::LTR_CJK, EcContinent::CJK,
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
        Dating::crange(15, 17), EcFont::BUGINESE, NO_FLAGS,
        { .mantissa = 4, .numOrder = NumOrder::MILLION, .year = 2015 } },
    // Buhid OK, installed Google Noto font
    { "Buhd", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::OCEAN,
        Dating::yapprox(1300), EcFont::BUHID, NO_FLAGS,
        { .mantissa = 12, .numOrder = NumOrder::THOUSAND, .flags = Langfg::CUSTOM_PRENOTE, .year = 2010 }},
    // Chakma OK, added Noto font, mostly because of missing glyphs
    { "Cakm", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::COMPETING, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::unknown(), EcFont::CHAKMA, NO_FLAGS,
        { .mantissa = 700, .numOrder = NumOrder::THOUSAND, .flags = Langfg::DECADE | Langfg::GREATER_THAN,
          .year = 2010, .locations { LOC_BANGLADESH, LOC_INDIA_E, LOC_MYANMAR } } },
    // Canadian syllabics OK, Noto supports it in the best way possible (their glyphs are UC samples)
    { "Cans", QFontDatabase::Any,
        EcScriptType::ABUGIDA, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::AMERICA,
        Dating::year(1833), EcFont::CANADIAN_ABORIGINAL },
    // Carian OK, W10 P1
    { "Cari", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::PARTLY_DECIPHERED, EcWritingDir::BOTH_LTR, EcContinent::ASIA,
        Dating::century(-7), EcFont::HISTORIC },
    { "Cham", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::COMPETING, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(4), EcFont::CHAM, NO_FLAGS,
        { .mantissa = 490, .numOrder = NumOrder::THOUSAND, .year = 2019,
          .locations { LOC_CAMBODIA, LOC_VIETNAM } } },
    // Cherokee OK, installed Google Noto font. Need it, W7 has no 2014 extensions.
    { "Cher", QFontDatabase::Any,
        EcScriptType::SYLLABLE, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::AMERICA,
        Dating::year(1821), EcFont::FRANKEN_SANS, NO_FLAGS,
        { .mantissa = 2100, .numOrder = NumOrder::UNIT, .flags = Langfg::LESS_THAN, .year = 2019,
          .locations { LOC_USA } } },
    // Chorasmian OK, installed Google Noto
    { "Chrs", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::DECIPHERED, EcWritingDir::SOGDIAN, EcContinent::ASIA,
        Dating::century(2, StdNote::FIRST_KNOWN), EcFont::FRANKEN_SANS },
    // Coptic OK, W7 problems → installed Google Noto font
    { "Copt", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::SACRED, EcWritingDir::LTR, EcContinent::AFRICA,
        Dating::crange(2, 4), EcFont::FUNKY, Sfg::DISUNIFIED },
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
        Dating::ybefore(900), EcFont::NORMAL, NO_FLAGS,
        // Russian here as main Cyrillic language
        { .mantissa = 150, .numOrder = NumOrder::MILLION, .flags = Langfg::AS_NATIVE, .year = 2020 } },
    // Devanagari OK, added 8 characters to Noto Serif
    { "Deva", QFontDatabase::Devanagari,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::crange(1, 7), EcFont::DEVANAGARI, NO_FLAGS,
        // Hindi here as main Devanagari language
        { .mantissa = 350, .numOrder = NumOrder::MILLION, .flags = Langfg::AS_NATIVE, .year = 2011 } },
    // Diak OK, W10 off, Noto is really cool
    { "Diak", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::DECIPHERED, EcWritingDir::LTR, EcContinent::OCEAN,
        Dating::crange(9, 10, StdNote::FIRST_KNOWN), EcFont::DIVES_AKURU },
    // Dogri OK, W10 off → installed Google Noto
    { "Dogr", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::REVIVED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(19), EcFont::DOGRA, NO_FLAGS,
        { .mantissa = 26, .numOrder = NumOrder::HUN_THOUSAND, .year = 2011,
          .locations = { LOC_INDIA_N } } },
    // Deseret OK, W10 Segoe UI Symbol, plane 1
    { "Dsrt", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::EXPERIMENTAL, EcWritingDir::LTR, EcContinent::AMERICA,
        Dating::year(1854), EcFont::SYMBOL },
    // Duployan OK, W10 none → installed Google Noto
    { "Dupl", QFontDatabase::Any,
        EcScriptType::CODE, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::EUROPE,
        Dating::year(1860), EcFont::DUPLOYAN },
    // Egyptian hiero OK, W7 none, W10 troubles → better Noto + temporary fallbacks
    { "Egyp", QFontDatabase::Any,
        EcScriptType::CONSONANTHIEROGLYPH, EcLangLife::DECIPHERED, EcWritingDir::RTL_MOSTLY, EcContinent::AFRICA,
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
        Dating::century(4, StdNote::CUSTOM), EcFont::ETHIOPIC, NO_FLAGS,
        // Amharic here as main Ethiopic language
        { .mantissa = 33, .numOrder = NumOrder::MILLION, .flags = Langfg::AS_NATIVE, .year = 2020 } },
    // Garay OK somehow, though hangs
    { "Gara", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::EXPERIMENTAL, EcWritingDir::RTL, EcContinent::AFRICA,
        Dating::year(1961), EcFont::FUNKY_BIGGER, Sfg::STUB_RTL,
        // Wolof here as main Garay language
        { .mantissa = 22, .numOrder = NumOrder::MILLION, .year = 2021, .locations = { LOC_AFRICA_W} } },
    // Georgian OK, installed Google Noto font
    { "Geor", QFontDatabase::Georgian,
        EcScriptType::ALPHABET, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::EUROPE,
        Dating::year(430, StdNote::FIRST_KNOWN), EcFont::GEORGIAN, NO_FLAGS,
        { .mantissa = 37, .numOrder = NumOrder::HUN_THOUSAND, .year = 2020 } },
    // Glagolitic OK, installed Google Noto font
    { "Glag", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::CONSERVED, EcWritingDir::LTR, EcContinent::EUROPE,
        Dating::century(9), EcFont::GLAGOLITIC },
    // Gondi/Gunjala OK, W10 none, installed Google Noto
    { "Gong", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::REVIVED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(1750, StdNote::FIRST_KNOWN), EcFont::GUNJALA_GONDI, NO_FLAGS,
        { .mantissa = 100, .numOrder = NumOrder::THOUSAND, .flags = Langfg::DECADE, .year = 2010,
          .locations { LOC_INDIA_CEN } } },
    // Gondi/Masaram OK, W10 none, installed Google Noto
    { "Gonm", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::COMPETING, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(1918), EcFont::MASARAM_GONDI, NO_FLAGS,
        { .mantissa = 3, .numOrder = NumOrder::MILLION, .year = 2011, .locations { LOC_INDIA_CEN } } },
    // Gothic OK, took from Junicode and enlarged ×135%
    { "Goth", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::DECIPHERED, EcWritingDir::LTR, EcContinent::EUROPE,
        Dating::century(4), EcFont::FUNKY },
    // Grantha OK, W10 none → installed Google Noto
    { "Gran", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::SACRED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::special(), EcFont::GRANTHA },
    // Greek OK, Noto does the job
    { "Grek", QFontDatabase::Greek,
        EcScriptType::ALPHABET, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::EUROPE,
        Dating::crange_ins(-9, -8), EcFont::GREEK_COPTIC, NO_FLAGS,
        { .mantissa = 135, .numOrder = NumOrder::HUN_THOUSAND, .year = 2012 } },  // 13.5M as of 2012
    // Gujarati OK, installed Google Noto: cannot find a good pair for W7/10, and Noto is REALLY nice.
    { "Gujr", QFontDatabase::Gujarati,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(1592, StdNote::FIRST_KNOWN), EcFont::GUJARATI, NO_FLAGS,
        { .mantissa = 57, .numOrder = NumOrder::MILLION, .flags = Langfg::AS_NATIVE, .year = 2011,
          .locations { LOC_INDIA_W } } },
    // Gurung Khema now in Funky
    { "Gukh", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(1977), EcFont::FUNKY, NO_FLAGS,
        { .mantissa = 380, .numOrder = NumOrder::THOUSAND, .year = 2021, .locations { LOC_NEPAL } } },
    // Gurmukhi OK, installed Google Noto *UI* because of W7 troubles; UI better handles umlauts
    { "Guru", QFontDatabase::Gurmukhi,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(16), EcFont::GURMUKHI, NO_FLAGS,
        // Punjabi here as the main Gurmikhi language
        { .mantissa = 150, .numOrder = NumOrder::MILLION, .flags = Langfg::DECADE, .year = 2010,
          .locations { LOC_PAKISTAN, LOC_INDIA_N } } },
    // Hangul OK, installed Noto CJK font
    { "Hang", QFontDatabase::Korean,
        EcScriptType::ARGUABLE, EcLangLife::ALIVE, EcWritingDir::LTR_CJK, EcContinent::CJK,
        Dating::year(1443), EcFont::KOREAN, NO_FLAGS,
        { .mantissa = 80, .numOrder = NumOrder::MILLION, .year = 2020 } },
    // Hani OK
    { "Hani", WS_HANI,       // Special rules for hieroglyphs, SimChi triggers them
        EcScriptType::HIEROGLYPH, EcLangLife::ALIVE, EcWritingDir::LTR_CJK, EcContinent::CJK,
        Dating::yapprox(-2000), EcFont::CJK, NO_FLAGS,
        // Chinese here as the main Han language
        { .mantissa = 135, .numOrder = NumOrder::DEC_MILLION, .flags = Langfg::CUSTOM_PRENOTE, .year = 2022 }},
    // Hanunoo OK, installed Google Noto font
    { "Hano", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::OCEAN,
        Dating::yapprox(1300), EcFont::HANUNOO, NO_FLAGS,
        { .mantissa = 13, .numOrder = NumOrder::THOUSAND, .year = 2000 } },
    // Hatran OK, W10 none, installed Google Noto
    { "Hatr", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::DECIPHERED, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::yapprox(-100), EcFont::FRANKEN_SANS },
    // Hebrew OK, installed very nice Noto, implemented yod triangle
    { "Hebr", QFontDatabase::Hebrew,
        EcScriptType::CONSONANT, EcLangLife::ALIVE, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::century(-2, StdNote::MODERN_FORM), EcFont::HEBREW, NO_FLAGS,
        { .mantissa = 9, .numOrder = NumOrder::MILLION, .flags = Langfg::GREATER_THAN, .year = 2018 } },
    // Hentaigana currently OK
    { "Hent", QFontDatabase::Japanese,
        EcScriptType::SYLLABLE, EcLangLife::CONSERVED, EcWritingDir::LTR_CJK, EcContinent::CJK,
        Dating::crange(8, 9), EcFont::HENTAIGANA, Sfg::NO_STATS },
    // Hiragana OK, installed small version of Noto CJK Korean
    { "Hira", QFontDatabase::Japanese,
        EcScriptType::SYLLABLE, EcLangLife::ALIVE, EcWritingDir::LTR_CJK, EcContinent::CJK,
        Dating::crange(8, 9), EcFont::KOREAN, NO_FLAGS,
        // Japanese here
        { .mantissa = 120, .numOrder = NumOrder::MILLION, .year = 2020 } },
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
        EcScriptType::ALPHABET, EcLangLife::DECIPHERED, EcWritingDir::BOTH_LTR, EcContinent::EUROPE,
        Dating::yapprox(-700), EcFont::FRANKEN_SANS },
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
        Dating::century(-4), EcFont::KHAROSHTHI, Sfg::STUB_RTL },
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
        Dating::century(14, StdNote::CUSTOM), EcFont::KANNADA, NO_FLAGS,
        { .mantissa = 44, .numOrder = NumOrder::MILLION, .flags = Langfg::AS_NATIVE, .year = 2011,
          .locations { LOC_INDIA_SW } } },
    // Kirat OK, excellent font from SIL
    { "Krai", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(1920), EcFont::KIRAT, NO_FLAGS,
        { .mantissa = 170, .numOrder = NumOrder::THOUSAND, .year = 2011,
          .locations { LOC_INDIA_NE, LOC_NEPAL } } },
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
        Dating::yapprox(-700), EcFont::NORMAL, NO_FLAGS,
        // English here as the main Latin script
        { .mantissa = 380, .numOrder = NumOrder::MILLION, .flags = Langfg::AS_NATIVE, .year = 2021 } },
    // Lepcha OK, W10 none, installed Google Noto font
    { "Lepc", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::yapprox(1700), EcFont::LEPCHA, NO_FLAGS,
        { .mantissa = 65, .numOrder = NumOrder::THOUSAND, .year = 2011,
          .locations { LOC_INDIA_E, LOC_BHUTAN, LOC_NEPAL } } },
    // Limbu OK, W10 none, installed Google Noto font
    { "Limb", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(18), EcFont::LIMBU, NO_FLAGS,
        { .mantissa = 410, .numOrder = NumOrder::THOUSAND, .flags = Langfg::DECADE, .year = 2010,
          .locations { LOC_NEPAL } } },
    // Linear A OK, W10 none, installed Google Noto font
    { "Lina", QFontDatabase::Any,
        EcScriptType::SYLLABOHIEROGLYPH, EcLangLife::UNDECIPHERED, EcWritingDir::LTR, EcContinent::EUROPE,
        Dating::yapprox(-1800), EcFont::FRANKEN_SANS },
    // Linear B OK, W10 none, installed Google Noto font
    { "Linb", QFontDatabase::Any,
        EcScriptType::SYLLABOHIEROGLYPH, EcLangLife::DECIPHERED, EcWritingDir::LTR, EcContinent::EUROPE,
        Dating::yapprox(-1450), EcFont::FRANKEN_SANS },
    // W10 has, W7 none (though lots of software install) → installed Google Noto
    { "Lisu", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(1915), EcFont::FRANKEN_SANS, NO_FLAGS,
        { .mantissa = 940, .numOrder = NumOrder::THOUSAND, .year = 2007,
          .locations { LOC_CHINA, LOC_MYANMAR, LOC_THAILAND } } },
    // Lycian OK, W10 P1 Segoe Historic
    { "Lyci", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::DECIPHERED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(-6), EcFont::HISTORIC },
    // Lydian OK, W10 P1 Segoe Historic
    { "Lydi", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::DECIPHERED, EcWritingDir::RTL_MOSTLY, EcContinent::ASIA,
        Dating::crange(-8, -7), EcFont::HISTORIC },
    // Mahajani OK, W10 none, installed Google Noto
    { "Mahj", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::DEAD, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::unknown(), EcFont::MAHAJANI },
    // Makasar OK, recreated strange font by Anshuman Pandey in Funky/OpenType
    { "Maka", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::CONSERVED, EcWritingDir::LTR, EcContinent::OCEAN,
        Dating::crange(16, 17), EcFont::FUNKY_BIGGER },
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
        Dating::yapprox(1931), EcFont::FRANKEN_SANS },
    // Mende Kikakui OK, W10 none → installed Google Noto
    { "Mend", QFontDatabase::Any,
        EcScriptType::SYLLABLE, EcLangLife::DEAD, EcWritingDir::RTL, EcContinent::AFRICA,
        Dating::yapprox(1920), EcFont::FRANKEN_SANS, NO_FLAGS,
        { .mantissa = 25, .numOrder = NumOrder::HUN_THOUSAND, .year = 2021,
          .locations { LOC_SIERRA_LEONE } } },
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
        Dating::year(830, StdNote::FIRST_KNOWN), EcFont::MALAYALAM, NO_FLAGS,
        { .mantissa = 37, .numOrder = NumOrder::MILLION, .year = 2011,
          .locations { LOC_INDIA_S } } },
    // Modi OK, W10 none, installed Google Noto
    { "Modi", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::DEAD, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::yrange(1200, 1600), EcFont::MODI, NO_FLAGS,
        // Marathi here as the main Modi script
        { .mantissa = 83, .numOrder = NumOrder::MILLION, .flags = Langfg::AS_NATIVE, .year = 2011,
          .locations { LOC_INDIA_W } } },
    // Mongol OK, dislike standard font of W10 → installed Google Noto
    { "Mong", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::REVIVED, EcWritingDir::LTR_COL, EcContinent::ASIA,
        Dating::special(), EcFont::MONGOLIAN },
    // Mro OK, installed Google Noto font
    { "Mroo", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::decade(1980), EcFont::FRANKEN_SANS, NO_FLAGS,
        { .mantissa = 70, .numOrder = NumOrder::THOUSAND, .flags = Langfg::GREATER_THAN, .year = 2022,
          .locations { LOC_BANGLADESH, LOC_MYANMAR } } },
    // Meetei Mayek OK, W10 does not have extensions → installed Google Noto
    { "Mtei", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::REVIVED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::yapprox(1500, StdNote::FIRST_KNOWN), EcFont::MEETEI, NO_FLAGS,
        { .mantissa = 17, .numOrder = NumOrder::HUN_THOUSAND, .flags = Langfg::GREATER_THAN, .year = 2011,
          .locations { LOC_INDIA_E } }},
    // Multani OK, W10 none → installed Google Noto
    { "Mult", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::DEAD, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(18), EcFont::MULTANI },
    // Myanmar OK, W10 “Myanmar Text”, W7 none → installed Google Noto font
    { "Mymr", QFontDatabase::Myanmar,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(1035, StdNote::FIRST_KNOWN), EcFont::MYANMAR, NO_FLAGS,
        { .mantissa = 33, .numOrder = NumOrder::MILLION, .flags = Langfg::AS_NATIVE | Langfg::BURMESE,
          .year = 2007, .locations { LOC_MYANMAR } } },
    // Nag OK, done in FunkySample
    { "Nagm", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::NEW, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::yrange(1949, 1982), EcFont::FUNKY, NO_FLAGS,
        { .mantissa = 11, .numOrder = NumOrder::HUN_THOUSAND, .flags = Langfg::IN_INDIA | Langfg::GREATER_THAN,
          .year = 2011, .locations { LOC_INDIA_E, LOC_BANGLADESH, LOC_MYANMAR } } },
    // Nandinagari OK, in Nov 2021 Google finally made Noto font
    { "Nand", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::CONSERVED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(8, StdNote::FIRST_KNOWN), EcFont::NANDINAGARI },
        // EWP: Some of the earliest inscriptions in Nandināgarī have been found in Tamil Nadu.
        // The 8th century Narasimha Pallava's stone inscriptions in Mamallapuram on Tamil Nadu's coast…
    // North Arabian OK, W10 none → installed Google Noto
    { "Narb", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::DECIPHERED, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::century(-3, StdNote::FIRST_KNOWN), EcFont::FRANKEN_SANS },
    // Nabataean OK, W10 none → installed Google Noto
    { "Nbat", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::DECIPHERED, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::century(-2), EcFont::FRANKEN_SANS },
    // Newa OK, W10 none → installed Google Noto
    { "Newa", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(1654, StdNote::CUSTOM), EcFont::NEWA, NO_FLAGS,
        { .mantissa = 880, .numOrder = NumOrder::THOUSAND, .year = 2021,
          .locations { LOC_NEPAL } } },
    // NKo ok, W10 “Ebrima”, W7 lacks a few chars → installed Google Noto
    { "Nkoo", QFontDatabase::Nko,
        EcScriptType::ALPHABET, EcLangLife::NEW, EcWritingDir::RTL, EcContinent::AFRICA,
        Dating::year(1949), EcFont::NKO, NO_FLAGS,
        { .mantissa = 9, .numOrder = NumOrder::MILLION, .year = 2021,
          .locations { LOC_AFRICA_W } } },
    // Nushu OK, W10 off, installed Google Noto
    { "Nshu", QFontDatabase::Any,
        EcScriptType::SYLLABLE, EcLangLife::DEAD, EcWritingDir::RTL_COL, EcContinent::CJK,
        Dating::special(), EcFont::NUSHU },
    // Ogham OK, Windows font is differently named in W7 and W10 → installed Google Noto
    { "Ogam", QFontDatabase::Ogham,
        EcScriptType::ALPHABET, EcLangLife::CONSERVED, EcWritingDir::LTR_BU, EcContinent::EUROPE,
        Dating::century(4, StdNote::FIRST_KNOWN), EcFont::FRANKEN_SANS_110 },
    // Ol Chiki OK, W10 “Nirmala UI”, W7 none → installed Google Noto
    { "Olck", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::NEW, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(1925), EcFont::OLCHIKI, NO_FLAGS,
        { .mantissa = 73, .numOrder = NumOrder::HUN_THOUSAND, .flags = Langfg::GREATER_THAN, .year = 2011,
          .locations { LOC_INDIA_E, LOC_BANGLADESH, LOC_NEPAL } } },
    // Ol Onal OK, made in Funky
    { "Onao", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::NEW, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::decade(1980), EcFont::FUNKY, NO_FLAGS,
        { .mantissa = 27, .numOrder = NumOrder::THOUSAND, .year = 2011,
          .locations { LOC_INDIA_E } } },
    // Orkhon runes OK, took Noto
    { "Orkh", QFontDatabase::Any,
        EcScriptType::ALPHASYLLABLE, EcLangLife::DECIPHERED, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::century(6), EcFont::FRANKEN_SANS },
    // Oriya OK, took Noto + everything bad manually fixed
    { "Orya", QFontDatabase::Oriya,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(14, StdNote::MODERN_FORM), EcFont::ORIYA, NO_FLAGS,
        { .mantissa = 34, .numOrder = NumOrder::MILLION, .flags = Langfg::AS_NATIVE, .year = 2011,
          .locations { LOC_INDIA_E } } },
    // Osage OK, W10 Gadugi
    { "Osge", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::FOR_REVIVAL, EcWritingDir::LTR, EcContinent::AMERICA,
        Dating::year(2006), EcFont::GADUGI },
    // Osmanya OK, W10 Ebrima
    { "Osma", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::NEW, EcWritingDir::LTR, EcContinent::AFRICA,
        Dating::yapprox(1920), EcFont::VAI },
    // Old Uyghur OK, installed Noto font
    { "Ougr", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::DECIPHERED, EcWritingDir::LTR_COL, EcContinent::ASIA,
        Dating::century(8), EcFont::OLD_UYGHUR, Sfg::STUB_RTL },
    // Plamyrene OK, W10 none, installed Google Noto
    { "Palm", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::DECIPHERED, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::year(-44, StdNote::FIRST_KNOWN), EcFont::FRANKEN_SANS },
    // Pau Cin Hau OK, W10 none, installed Google Noto
    { "Pauc", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::COMPETING, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::special(), EcFont::FRANKEN_SANS, NO_FLAGS,
        { .mantissa = 340, .numOrder = NumOrder::THOUSAND, .flags = Langfg::NO_AUTO, .year = 1990 } },
    // Old Permic OK, W10 none, installed Google Noto
    { "Perm", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::DECIPHERED, EcWritingDir::LTR, EcContinent::EUROPE,
        Dating::year(1372), EcFont::PERMIC_OLD },
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
        Dating::century(6, StdNote::FIRST_KNOWN), EcFont::FRANKEN_SANS },
    // Phoenician OK, because of importance installed Google Noto
    { "Phnx", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::DECIPHERED, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::century(-11), EcFont::FRANKEN_SANS },
    // Miao/Pollard OK, W10 none, installed Google Noto
    { "Plrd", QFontDatabase::Any,
        EcScriptType::ABUGIDA, EcLangLife::COMPETING, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(1936), EcFont::POLLARD },
    // Inscriptional Parthian OK, W10 Historic
    { "Prti", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::DECIPHERED, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::yapprox(-250), EcFont::FRANKEN_SANS_BIGGER },
    // Rejang OK, installed Google Noto font
    { "Rjng", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::DEAD, EcWritingDir::LTR, EcContinent::OCEAN,
        Dating::yapprox(1750, StdNote::FIRST_KNOWN), EcFont::REJANG, NO_FLAGS,
        { .mantissa = 200, .hiMantissa = 350, .numOrder = NumOrder::THOUSAND, .year = 0,
          .locations { LOC_SUMATRA } } },
    // Rohingya, installed Google Noto font
    { "Rohg", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::NEW, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::decade(1980), EcFont::ROHINGYA, NO_FLAGS,
        { .mantissa = 2, .numOrder = NumOrder::MILLION, .flags = Langfg::GREATER_THAN, .year = 2017,
          .locations { LOC_BANGLADESH, LOC_MYANMAR } } },
    // Runes OK, several tofu → installed Google Noto font
    { "Runr", QFontDatabase::Runic,
        EcScriptType::ALPHABET, EcLangLife::CONSERVED, EcWritingDir::LTR, EcContinent::EUROPE,
        Dating::century(2, StdNote::FIRST_KNOWN), EcFont::FRANKEN_SANS },
    // Samaritan OK, installed Google Noto font, added dotted circle
    { "Samr", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::ENDANGERED, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::yrange(-600, -200), EcFont::SAMARITAN },
    // South Arabian OK, W10 Segoe UI Historic
    { "Sarb", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::DECIPHERED, EcWritingDir::RTL_MOSTLY, EcContinent::ASIA,
        Dating::century(-9, StdNote::FIRST_KNOWN), EcFont::HISTORIC },
    // Saurashtra OK, W10 none, installed Google Noto
    { "Saur", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::COMPETING, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(19, StdNote::MAYBE_EARLIER), EcFont::SAURASHTRA, NO_FLAGS,
        { .mantissa = 250, .numOrder = NumOrder::THOUSAND, .year = 2011,
          .locations { LOC_INDIA_S } } },
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
        Dating::century(16), EcFont::KHUDAWADI, NO_FLAGS,
        { .mantissa = 32, .numOrder = NumOrder::MILLION, .flags = Langfg::DECADE, .year = 2010,
          .locations { LOC_PAKISTAN, LOC_INDIA_N } } },
    // Sinhala OK, W10 obviously has no 2020 extension → installed Google Noto
    { "Sinh", QFontDatabase::Sinhala,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::OCEAN,
        Dating::century(13, StdNote::MODERN_FORM), EcFont::SINHALA, NO_FLAGS,
        { .mantissa = 18, .numOrder = NumOrder::MILLION, .year = 2021, .locations { LOC_SRI_LANKA } } },
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
        Dating::year(1936), EcFont::TAMIL, NO_FLAGS,
        { .mantissa = 400, .numOrder = NumOrder::THOUSAND, .year = 2011,
          .locations { LOC_INDIA_SE } } },
    // Soyombo OK, W10 none → installed Google Noto
    { "Soyo", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::SACRED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(1686), EcFont::SOYOMBO },
    // Sundanese OK, even W10 off → installed Google Noto
    { "Sund", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::REVIVED, EcWritingDir::LTR, EcContinent::OCEAN,
        Dating::century(14), EcFont::SUNDANESE, NO_FLAGS,
        { .mantissa = 32, .numOrder = NumOrder::MILLION, .flags = Langfg::AS_NATIVE | Langfg::NO_AUTO, .year = 2015 } }, // UNSD
    // Sunuwar OK, though font is REALLY bad
    { "Sunu", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(1942), EcFont::SUNUWAR, NO_FLAGS,
        { .mantissa = 38, .numOrder = NumOrder::THOUSAND, .year = 2011, .locations { LOC_NEPAL } } }, // Census
    // Syloti Nagri OK, W10 none → installed Google Noto
    { "Sylo", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::REVIVED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(15), EcFont::SYLOTI_NAGRI, NO_FLAGS,
        { .mantissa = 11, .numOrder = NumOrder::MILLION, .year = 2017,
          .locations { LOC_BANGLADESH, LOC_INDIA_E } }},  // Ethnologue
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
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::decade(1950), EcFont::TAILUE },
    // Tamil BMP OK, different fonts in W10 and W7
    { "Taml", QFontDatabase::Tamil,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::crange(6, 9), EcFont::TAMIL, NO_FLAGS,
        { .mantissa = 75, .numOrder = NumOrder::MILLION, .flags = Langfg::GREATER_THAN, .year = 2011,
          .locations { LOC_INDIA_S, LOC_SRI_LANKA } } },  // Census
    // Tangut OK, installed Google Noto
    { "Tang", QFontDatabase::Any,
        EcScriptType::HIEROGLYPH, EcLangLife::DECIPHERED, EcWritingDir::RTL_COL, EcContinent::CJK,
        Dating::year(1036), EcFont::TANGUT },
    // Tai Viet OK, installed Google Noto font
    { "Tavt", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(16), EcFont::TAI_VIET },
    // Telugu OK, W7 lacks recent extensions → installed Google Noto
    { "Telu", QFontDatabase::Telugu,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(14, StdNote::CUSTOM), EcFont::TELUGU, NO_FLAGS,
        { .mantissa = 90, .numOrder = NumOrder::MILLION, .flags = Langfg::GREATER_THAN, .year = 2011,
          .locations { LOC_INDIA_SE } } }, // Census
    // Tifinagh OK, W7 “Ebrima” of course has no 2012 chars → installed Google Noto
    { "Tfng", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::REVIVED, EcWritingDir::LTR, EcContinent::AFRICA,
        Dating::special(), EcFont::FRANKEN_SANS },
    // Tagalog OK, W10 off → installed Google Noto
    { "Tglg", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::REVIVED, EcWritingDir::LTR, EcContinent::OCEAN,
        Dating::yrange(1300, 1500), EcFont::TAGALOG },
    // Thaana OK, Monotype/M$ “MV Boli” font
    { "Thaa", QFontDatabase::Thaana,
        EcScriptType::ARGUABLE, EcLangLife::ALIVE, EcWritingDir::RTL, EcContinent::OCEAN,
        Dating::century(18), EcFont::THAANA, NO_FLAGS,
        { .mantissa = 490, .numOrder = NumOrder::THOUSAND, .year = 2022 } }, // Census
    // Thai OK, W7 and 10 fonts are different, but managed to
    { "Thai", QFontDatabase::Thai,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(1283), EcFont::THAI, NO_FLAGS,
        { .mantissa = 60, .numOrder = NumOrder::MILLION,
          .flags = Langfg::GREATER_THAN | Langfg::DECADE, .year = 2020 } }, // No reliable source
    // Tibetan OK, W10 did not have swastika, installed BabelStone
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
        Dating::year(1990), EcFont::TANGSA, NO_FLAGS,
        { .mantissa = 100, .numOrder = NumOrder::THOUSAND, .year = 2010,
          .locations { LOC_MYANMAR, LOC_INDIA_NE } } }, // Ethnologue
    // Todhri OK, drew in Funky
    { "Todr", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::EXPERIMENTAL, EcWritingDir::LTR, EcContinent::EUROPE,
        Dating::yapprox(1780), EcFont::FUNKY },
    // Toto OK, U14, moved that font to FunkySample
    { "Toto", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(2015), EcFont::FUNKY, NO_FLAGS,
        { .mantissa = 1400, .numOrder = NumOrder::UNIT, .year = 2014, .locations { LOC_INDIA_E } } },
    /// @todo [U16, big tofu] Tulu Tigalari
    { "Tutg", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::SACRED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(9), EcFont::TULU, NO_FLAGS,
        { .mantissa = 18, .numOrder = NumOrder::HUN_THOUSAND, .year = 2011, .locations { LOC_INDIA_SW } } },
    // Ugaritic OK
    { "Ugar", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::DECIPHERED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::yapprox(-1400), EcFont::FRANKEN_SANS },
    // Vai OK, have good M$ font Ebrima
    { "Vaii", QFontDatabase::Any,
        EcScriptType::SYLLABLE, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::AFRICA,
        Dating::decade(1830), EcFont::VAI },
    // Vithkuqi OK, installed Google Noto
    { "Vith", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::EXPERIMENTAL, EcWritingDir::LTR, EcContinent::EUROPE,
        Dating::year(1844, StdNote::FIRST_KNOWN), EcFont::VITHKUQI },
    // Warang Citi OK, W10 none → installed Google Noto
    { "Wara", QFontDatabase::Any,
        EcScriptType::ARGUABLE, EcLangLife::NEW, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::decade(1950), EcFont::FRANKEN_SANS, NO_FLAGS,
        { .mantissa = 14, .numOrder = NumOrder::HUN_THOUSAND, .year = 2011,
          .locations = { LOC_INDIA_E } } },
    // Wancho OK, W10 none, installed Google Noto
    { "Wcho", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::NEW, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::yrange(2001, 2012), EcFont::FRANKEN_SANS_BIGGER, NO_FLAGS,
        { .mantissa = 59, .numOrder = NumOrder::THOUSAND, .year = 2011, .locations { LOC_INDIA_NE } } },
    // Old Persian OK. Small font, let’s install!!
    { "Xpeo", QFontDatabase::Any,
        EcScriptType::ALPHASYLLABLE, EcLangLife::DECIPHERED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::yapprox(-520), EcFont::FRANKEN_SANS },
    // Sumerian cuneiform OK. Partly Segoe, partly Noto (they are the same)
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
        Dating::year(1974, StdNote::CUSTOM), EcFont::YI, NO_FLAGS,
        { .mantissa = 2, .numOrder = NumOrder::MILLION, .year = 2000,
          .locations = { LOC_CHINA } } }, // Census
    // Zanabazar square OK, installed Google Noto
    { "Zanb", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::CONSERVED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::ybefore(1700), EcFont::ZANABAZAR },
    { "Zinh", QFontDatabase::Any,
        EcScriptType::NONE, EcLangLife::NOMATTER, EcWritingDir::NOMATTER, EcContinent::NONE,
        Dating::none(), EcFont::NORMAL, Sfg::NO_LANGS | Sfg::NO_STATS },
};

static_assert(std::size(uc::scriptInfo) == static_cast<size_t>(uc::EcScript::NN));

const uc::Script* uc::findScript(std::string_view x) { return findInArray(x, scriptInfo); }


extern const uc::SideLang uc::sideLangInfo[] {
    { .id = "as",   // Assamese ← Beng
      .lang { .mantissa = 15, .numOrder = NumOrder::MILLION, .year = 2011 } },
                    // 2011 census in India
    { .id = "es",   // Spanish ← Latn
      .lang { .mantissa = 600, .numOrder = NumOrder::MILLION, .year = 2023 } },
                    // Estimate by Cervantes institute
    { .id = "kkh",  // Khün ← Lana
      .lang { .mantissa = 100, .numOrder = NumOrder::THOUSAND, .year = 1990 } },
                    // Ethnologue ← Diller and Juntanamalaga 1990
    { .id = "mnk",  // Mandinka ← Gara
      .lang { .mantissa = 21, .numOrder = NumOrder::HUN_THOUSAND, .year = 2017 } },
                    // Ethnologue
    { .id = "mnw",  // Mon ← Mymr
      .lang { .mantissa = 1, .numOrder = NumOrder::MILLION, .flags = Langfg::LESS_THAN, .year = 2012,
              .locations { LOC_MYANMAR } } },
                    // The greatest of known estimates (UNSD 2012)
    { .id = "uk",   // Ukrainian ← Cyrl
      .lang { .mantissa = 39, .numOrder = NumOrder::MILLION, .year = 2024 } },
                    // Ethnologue, counted diasporas (millions of L1 Ukrainians),
                    //    refugees (number really reliable but who knows L1 or L2)
                    //    and switchers (switcher is L2 by definition but…)
                    // 25M L1 + 5M L2 in Ukraine as of 2016 is believable
    { .id = "xmf",  // Megrelian ← Geor
      .lang { .mantissa = 290, .numOrder = NumOrder::THOUSAND, .flags = Langfg::GREATER_THAN, .year = 2020 } },
                    // Ethnologue + lack of reliable info
};
const uc::Lang* uc::findSideLang(std::string_view x) {
    if (auto q = findInArray(x, sideLangInfo)) {
        return &q->lang;
    }
    return nullptr;
}
