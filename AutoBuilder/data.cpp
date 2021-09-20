// My header
#include "data.h"

// C++
#include <vector>

// Common libs
#include "u_Strings.h"
#include "u_Vector.h"

using namespace std::string_view_literals;

const std::map<std::string_view, DicEntry> dictionary {
    // Language names
    { "ADLAM"sv,        Dicf::TRIG_SCRIPT },
    { "AFAKA"sv,        Dicf::TRIG_SCRIPT },
    { "CAUCASIAN"sv,    Dicf::TRIG_SCRIPT },
    { "ALBANIAN"sv,     Dicf::TRIG_SCRIPT },
    { "AHOM"sv,         Dicf::TRIG_SCRIPT },
    { "APL"sv,          Dicf::CAP_ALL | Dicf::TRIG_SCRIPT },
    { "ARABIC"sv,       Dicf::TRIG_SCRIPT },
    { "ARABIAN"sv,      Dicf::TRIG_SCRIPT },
    { "ARABIC-INDIC"sv, { Dicf::TRIG_SCRIPT, "Arabic-Indic"sv } },
    { "IMPERIAL"sv,     Dicf::TRIG_SCRIPT },
    { "ARAMAIC"sv,      Dicf::TRIG_SCRIPT },
    { "ARMENIAN"sv,     Dicf::TRIG_SCRIPT },
    { "AVESTAN"sv,      Dicf::TRIG_SCRIPT },
    { "BALINESE"sv,     Dicf::TRIG_SCRIPT },
    { "BAMUM"sv,        Dicf::TRIG_SCRIPT },
    { "BASSA"sv,        Dicf::TRIG_SCRIPT },
    { "VAH"sv,          Dicf::TRIG_SCRIPT },
    { "BATAK"sv,        Dicf::TRIG_SCRIPT },
    { "BENGALI"sv,      Dicf::TRIG_SCRIPT },
    { "VEDIC"sv,        Dicf::TRIG_SCRIPT },
    { "BHAIKSUKI"sv,    Dicf::TRIG_SCRIPT },
    { "BOPOMOFO"sv,     Dicf::TRIG_SCRIPT },
    { "BRAHMI"sv,       Dicf::TRIG_SCRIPT },
    { "BRAILLE"sv,      Dicf::TRIG_SCRIPT },
    { "BUGINESE"sv,     Dicf::TRIG_SCRIPT },
    { "BUHID"sv,        Dicf::TRIG_SCRIPT },
    { "CANADIAN"sv,     Dicf::TRIG_SCRIPT },
     { "WOODS-CREE"sv,  Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE },
    { "CARIAN"sv,       Dicf::TRIG_SCRIPT },
    { "CHAKMA"sv,       Dicf::TRIG_SCRIPT },
    { "CHAM"sv,         Dicf::TRIG_SCRIPT },
    { "CHEROKEE"sv,     Dicf::TRIG_SCRIPT },
    { "CHINESE"sv,      Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE },
    { "CIRTH"sv,        Dicf::TRIG_SCRIPT },
    { "COPTIC"sv,       Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE },
    { "NUBIAN"sv,       Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE },
    { "CYPRO-MINOAN"sv, { Dicf::TRIG_SCRIPT, "Cypro-Minoan"sv } },
    { "CYPRIOT"sv,      Dicf::TRIG_SCRIPT },
    { "CYRILLIC"sv,     Dicf::TRIG_SCRIPT },
    { "ABKHASIAN"sv,    Dicf::TRIG_SCRIPT },
    { "KOMI"sv,         Dicf::TRIG_SCRIPT },
    { "ALEUT"sv,        Dicf::TRIG_SCRIPT },
    { "UKRAINIAN"sv,    Dicf::TRIG_SCRIPT },
    { "BYELORUSSIAN-UKRAINIAN"sv, { Dicf::TRIG_SCRIPT, "Byelorussian-Ukrainian"sv } },
    { "BASHKIR"sv,      Dicf::TRIG_SCRIPT },
    { "DEVANAGARI"sv,   Dicf::TRIG_SCRIPT },
    { "DOGRA"sv,        Dicf::TRIG_SCRIPT },
    { "DESERET"sv,      Dicf::TRIG_SCRIPT },
    { "DUPLOYAN"sv,     Dicf::TRIG_SCRIPT },
    { "DIVES"sv,        Dicf::TRIG_SCRIPT },
     { "AKURU"sv,       Dicf::TRIG_SCRIPT },
    { "EGYPTIAN"sv,     Dicf::TRIG_SCRIPT },
    { "ELBASAN"sv,      Dicf::TRIG_SCRIPT },
    { "ETHIOPIC"sv,     Dicf::TRIG_SCRIPT },
    { "KHUTSURI"sv,     Dicf::TRIG_SCRIPT },    // Unused, but…
    { "FRAKTUR"sv,      Dicf::TRIG_SCRIPT },
    { "GEORGIAN"sv,     Dicf::TRIG_SCRIPT },
     { "MTAVRULI"sv,    Dicf::TRIG_SCRIPT },
    { "GUNJALA"sv,      Dicf::TRIG_SCRIPT },
    { "GONDI"sv,        Dicf::TRIG_SCRIPT },
    { "MASARAM"sv,      Dicf::TRIG_SCRIPT },
    { "GOTHIC"sv,       Dicf::TRIG_SCRIPT },
    { "GLAGOLITIC"sv,   Dicf::TRIG_SCRIPT },
    { "GRANTHA"sv,      Dicf::TRIG_SCRIPT },
    { "GREEK"sv,        Dicf::TRIG_SCRIPT },
      { "PAMPHYLIAN"sv,   Dicf::TRIG_SCRIPT },
      { "ATTIC"sv,        Dicf::TRIG_SCRIPT },
      { "TROEZENIAN"sv,   Dicf::TRIG_SCRIPT },
      { "THESPIAN"sv,     Dicf::TRIG_SCRIPT },
      { "EPIDAUREAN"sv,   Dicf::TRIG_SCRIPT },
      { "HERMIONIAN"sv,   Dicf::TRIG_SCRIPT },
    { "GUJARATI"sv,     Dicf::TRIG_SCRIPT },
    { "GURMUKHI"sv,     Dicf::TRIG_SCRIPT },
    { "HANGUL"sv,       Dicf::TRIG_SCRIPT },
    { "HANIFI"sv,       Dicf::TRIG_SCRIPT },
     { "ROHINGYA"sv,    Dicf::TRIG_SCRIPT },
    { "KANGXI"sv,       Dicf::TRIG_SCRIPT },
    { "CJK"sv,          Dicf::CAP_ALL | Dicf::TRIG_SCRIPT },
    { "HANGZHOU"sv,     Dicf::TRIG_SCRIPT },
    { "HANUNOO"sv,      Dicf::TRIG_SCRIPT },
    { "HATRAN"sv,       Dicf::TRIG_SCRIPT },
    { "HEBREW"sv,       Dicf::TRIG_SCRIPT },
    { "HIRAGANA"sv,     Dicf::TRIG_SCRIPT },
    { "HENTAIGANA"sv,   Dicf::TRIG_SCRIPT },
    { "ANATOLIAN"sv,    Dicf::TRIG_SCRIPT },
    { "PAHAWH"sv,       Dicf::TRIG_SCRIPT },
    { "HMONG"sv,        Dicf::TRIG_SCRIPT },
     { "NYIAKENG"sv,    Dicf::TRIG_SCRIPT },
     { "PUACHUE"sv,     Dicf::TRIG_SCRIPT },
    { "HUNGARIAN"sv,    Dicf::TRIG_SCRIPT },
    { "INDIC"sv,        Dicf::TRIG_SCRIPT },
    { "INDUS"sv,        Dicf::TRIG_SCRIPT },
    { "OLD"sv,          { Dicf::PART_ADJECTIVE | Dicf::TRIG_TWO_WORD_SCRIPT, {}, "ITALIC"sv } },
    { "JAMO"sv,         Dicf::TRIG_SCRIPT },
    { "JAVANESE"sv,     Dicf::TRIG_SCRIPT },
    { "JAPANESE"sv,     Dicf::TRIG_SCRIPT },
    { "KANNADA"sv,      Dicf::TRIG_SCRIPT },
    { "KATAKANA"sv,     Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE },
    { "KATAKANA-HIRAGANA"sv, { Dicf::TRIG_SCRIPT, "Katakana-Hiragana"sv } },
    { "KAYAH"sv,        { Dicf::TRIG_TWO_WORD_SCRIPT, {}, "LI"sv } },
    { "KHAROSHTHI"sv,   Dicf::TRIG_SCRIPT },
    { "KHMER"sv,        Dicf::TRIG_SCRIPT },
    { "KHOJKI"sv,       Dicf::TRIG_SCRIPT },
    { "KHITAN"sv,       Dicf::TRIG_SCRIPT },
    { "KHUDAWADI"sv,    Dicf::TRIG_SCRIPT },
    { "KOREAN"sv,       Dicf::TRIG_SCRIPT },
    { "KPELLE"sv,       Dicf::TRIG_SCRIPT },
    { "KAITHI"sv,       Dicf::TRIG_SCRIPT },
    { "LAO"sv,          Dicf::TRIG_SCRIPT },
    { "LATIN"sv,        Dicf::TRIG_SCRIPT },
    { "LEKE"sv,         Dicf::TRIG_SCRIPT },
    { "LEPCHA"sv,       Dicf::TRIG_SCRIPT },
    { "LIMBU"sv,        Dicf::TRIG_SCRIPT },
    { "LINEAR"sv,       { Dicf::TRIG_TWO_WORD_SCRIPT, {}, "A"sv, "B"sv } },
    { "LISU"sv,         Dicf::TRIG_SCRIPT },
    { "LOMA"sv,         Dicf::TRIG_SCRIPT },
    { "LYCIAN"sv,       Dicf::TRIG_SCRIPT },
    { "LYDIAN"sv,       Dicf::TRIG_SCRIPT },
    { "MAHAJANI"sv,     Dicf::TRIG_SCRIPT },
    { "MAKASAR"sv,      Dicf::TRIG_SCRIPT },
    { "MALAYALAM"sv,    Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE },
    { "MANDAIC"sv,      Dicf::TRIG_SCRIPT },
    { "MANDAEAN"sv,     Dicf::TRIG_SCRIPT },
    { "MANICHAEAN"sv,   Dicf::TRIG_SCRIPT },
    { "MARCHEN"sv,      Dicf::TRIG_SCRIPT },
    { "MEDEFAIDRIN"sv,  Dicf::TRIG_SCRIPT },
    { "MEETEI"sv,       Dicf::TRIG_SCRIPT },
      { "MAYEK"sv,      Dicf::TRIG_SCRIPT },
    { "MENDE"sv,        Dicf::TRIG_SCRIPT | Dicf::TRIG_DIGIT_PROPAGATES_RIGHT },
      { "KIKAKUI"sv,    Dicf::TRIG_SCRIPT | Dicf::TRIG_DIGIT_PROPAGATES_RIGHT },
    { "MEROITIC",       Dicf::TRIG_SCRIPT },
    { "MIDDLE-WELSH",   { Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE, "Middle-Welsh" } },
    { "MODI",           Dicf::TRIG_SCRIPT },
    { "MONGOLIAN",      Dicf::TRIG_SCRIPT },
    { "MRO"sv,          Dicf::TRIG_SCRIPT },
    { "MULTANI"sv,      Dicf::TRIG_SCRIPT },
    { "MYANMAR"sv,      Dicf::TRIG_SCRIPT },
     { "KHAMTI"sv,      Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE },
    { "NABATAEAN"sv,    Dicf::TRIG_SCRIPT },
    { "NANDINAGARI",    Dicf::TRIG_SCRIPT },
    { "NEWA"sv,         Dicf::TRIG_SCRIPT },
    { "NKO"sv,          { Dicf::TRIG_SCRIPT, "NKo" } },
    { "NUBIAN"sv,       Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE },
    { "NUSHU"sv,        Dicf::TRIG_SCRIPT },
    { "OGHAM"sv,        Dicf::TRIG_SCRIPT },
    { "OL"sv,           { Dicf::TRIG_TWO_WORD_SCRIPT, {}, "CHIKI"sv } },
    { "TURKIC"sv,       Dicf::TRIG_SCRIPT },
    { "ORIYA"sv,        Dicf::TRIG_SCRIPT },
    { "OSAGE"sv,        Dicf::TRIG_SCRIPT },
    { "OSMANYA"sv,      Dicf::TRIG_SCRIPT },
    { "PALI"sv,         Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE },
    { "PALMYRENE"sv,    Dicf::TRIG_SCRIPT },
    { "PAU"sv,          { Dicf::TRIG_THREE_WORD_SCRIPT, {}, "CIN"sv, "HAU"sv } },
    { "PERMIC"sv,       Dicf::TRIG_SCRIPT },
    { "PERSIAN"sv,      Dicf::TRIG_SCRIPT },
    { "PHAISTOS",       { Dicf::TRIG_TWO_WORD_SCRIPT, {}, "DISC" } },
    { "PHAGS-PA"sv,     Dicf::TRIG_SCRIPT },
    { "PAHLAVI"sv,      Dicf::TRIG_SCRIPT },
    { "PHOENICIAN"sv,   Dicf::TRIG_SCRIPT },
    { "KLINGON"sv,      Dicf::TRIG_SCRIPT },
    { "MIAO"sv,         Dicf::TRIG_SCRIPT },
    { "PARTHIAN"sv,     Dicf::TRIG_SCRIPT },
    { "REJANG"sv,       Dicf::TRIG_SCRIPT },
    { "RONGORONGO"sv,   Dicf::TRIG_SCRIPT },
    { "RUNIC"sv,        Dicf::TRIG_SCRIPT },
    { "SAMARITAN",      Dicf::TRIG_SCRIPT },
    { "SANSKRIT",       Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE },
    { "SARATI"sv,       Dicf::TRIG_SCRIPT },
    { "SAURASHTRA"sv,   Dicf::TRIG_SCRIPT },
    { "SHAVIAN"sv,      Dicf::TRIG_SCRIPT },
    { "SHARADA"sv,      Dicf::TRIG_SCRIPT },
    { "SHUISHU"sv,      Dicf::TRIG_SCRIPT },
    { "SIDDHAM"sv,      Dicf::TRIG_SCRIPT },
    { "SIGNWRITING",    { Dicf::PART_NOUN | Dicf::TRIG_SCRIPT, "SignWriting" } },
    { "SINHALA"sv,      Dicf::TRIG_SCRIPT },
    { "SIYAQ"sv,        Dicf::TRIG_SCRIPT },
    { "SORA"sv,         { Dicf::TRIG_TWO_WORD_SCRIPT, {}, "SOMPENG"sv } },
    { "SOYOMBO"sv,      Dicf::TRIG_SCRIPT },
    { "SUNDANESE"sv,    Dicf::TRIG_SCRIPT },
    { "SYLOTI"sv,       Dicf::TRIG_SCRIPT },
    { "NAGRI"sv,        Dicf::TRIG_SCRIPT },
    { "SOGDIAN"sv,      Dicf::TRIG_SCRIPT },
    { "SYRIAC"sv,       Dicf::TRIG_SCRIPT },
    { "TAGBANWA"sv,     Dicf::TRIG_SCRIPT },
    { "TAKRI"sv,        Dicf::TRIG_SCRIPT },
    { "TAI"sv,          { Dicf::TRIG_TWO_WORD_SCRIPT, {}, "LE"sv, "LUE"sv, "VIET"sv, "THAM"sv, "LAING"sv } },
      { "LAING"sv,     Dicf::IF_SCRIPT | Dicf::TRIG_CAP_NEXT | Dicf::IF_NEXT_NOT_NOUN },
    { "TAMIL"sv,        Dicf::TRIG_SCRIPT },
    { "TANGUT"sv,       Dicf::TRIG_SCRIPT },
    { "TELUGU"sv,       Dicf::TRIG_SCRIPT },
    { "TENGWAR"sv,      Dicf::TRIG_SCRIPT },
    { "TIFINAGH"sv,     Dicf::TRIG_SCRIPT },
     { "AHAGGAR"sv,     Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE },
     { "TUAREG"sv,      Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE },
     { "TAWELLEMET"sv,  Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE },
     { "AYER"sv,        Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE },
    { "TAGALOG"sv,      Dicf::TRIG_SCRIPT },
    { "THAANA"sv,       Dicf::TRIG_SCRIPT },
    { "THAI"sv,         Dicf::TRIG_SCRIPT },
    { "TIBETAN"sv,      Dicf::TRIG_SCRIPT },
    { "TIRHUTA"sv,      Dicf::TRIG_SCRIPT },
    { "URARITIC"sv,     Dicf::TRIG_SCRIPT },
    { "VAI"sv,          Dicf::TRIG_SCRIPT },
    { "WARANG"sv,       { Dicf::TRIG_TWO_WORD_SCRIPT, {}, "CITI"sv } },
    { "WANCHO"sv,       Dicf::TRIG_SCRIPT },
    { "CUNEIFORM"sv,    Dicf::TRIG_SCRIPT | Dicf::CAP_SMALL },
    { "YI"sv,           Dicf::TRIG_SCRIPT_IF_FIRST },
    { "ZANABAZAR"sv,    Dicf::TRIG_SCRIPT },
    { "ZANZABAR"sv,     Dicf::TRIG_SCRIPT },

    { "OCR"sv,          Dicf::CAP_ALL },
    { "MICR"sv,         Dicf::CAP_ALL },

    // Capitalize next
    { "CHARACTER"sv,    Dicf::IF_SCRIPT | Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
    { "CHOSEONG"sv,     Dicf::IF_SCRIPT | Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
      { "JUNGSEONG"sv,  Dicf::IF_SCRIPT | Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
      { "JONGSEONG"sv,  Dicf::IF_SCRIPT | Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
    { "DIGIT"sv,        Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL  },
    { "FRACTION"sv,     Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
    { "HIEROGLYPH"sv,   Dicf::IF_SCRIPT | Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
    { "IDEOGRAM"sv,     Dicf::IF_SCRIPT | Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
    { "LETTER"sv,       Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
    /// @todo [spelling] Ligature Alpha Bravo — how to kick?
    { "LIGATURE"sv,     Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
    { "LOGICAL"sv,      Dicf::TRIG_CAP_NEXT },
    { "LOGOGRAM"sv,     Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
    { "MARK"sv,         Dicf::PART_NOUN | Dicf::IF_SCRIPT | Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
    { "NUMBER"sv,       Dicf::PART_NOUN | Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
    { "NUMERAL"sv,      Dicf::IF_SCRIPT | Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
    { "NUMERATOR"sv,    Dicf::PART_NOUN | Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
    { "PADA"sv,         Dicf::IF_SCRIPT | Dicf::PART_NOUN | Dicf::TRIG_CAP_NEXT },
    { "PATTERN"sv,      Dicf::IF_SCRIPT | Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
    { "PUNCTUATION"sv,  Dicf::IF_SCRIPT | Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
    { "RADICAL"sv,      Dicf::IF_SCRIPT | Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
    { "SIGN"sv,         Dicf::PART_NOUN | Dicf::IF_SCRIPT | Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
    { "SYMBOL"sv,       Dicf::PART_NOUN | Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
    { "SYLLABICS"sv,    Dicf::IF_SCRIPT | Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
    { "SYLLABLE"sv,     Dicf::IF_SCRIPT | Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
    { "VOWEL"sv,        Dicf::IF_SCRIPT | Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },

    // Mixed
    { "TONE"sv,         Dicf::PART_MIXED | Dicf::TRIG_CAP_NEXT | Dicf::IF_SCRIPT | Dicf::IF_NEXT_NOT_NOUN },
    { "WORD"sv,         Dicf::PART_MIXED | Dicf::TRIG_CAP_NEXT | Dicf::IF_SCRIPT | Dicf::IF_NEXT_NOT_NOUN },
    { "REPETITION"sv,   Dicf::PART_MIXED | Dicf::TRIG_CAP_NEXT | Dicf::IF_SCRIPT | Dicf::IF_NEXT_NOT_NOUN },

    // American tribes
    { "TH-CREE"sv,      { Dicf::PART_ADJECTIVE, "Th-Cree" } },
    { "WEST-CREE"sv,    { Dicf::PART_ADJECTIVE, "West-Cree" } },
    { "BIBLE-CREE",     { Dicf::PART_ADJECTIVE, "Bible-Cree" } },
    { "R-CREE"sv,       { Dicf::PART_ADJECTIVE, "R-Cree" } },
    { "N-CREE"sv,       { Dicf::PART_ADJECTIVE, "N-Cree" } },
    { "WOODS-CREE",     { Dicf::PART_ADJECTIVE, "Woods-Cree" } },
    { "MOOSE-CREE",     { Dicf::PART_ADJECTIVE, "Moose-Cree" } },
    { "Y-CREE",         { Dicf::PART_ADJECTIVE, "Y-Cree" } },
    { "SOUTH-SLAVEY",   { Dicf::PART_ADJECTIVE, "South-Slavey" } },
    { "BLACKFOOT"sv,    Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "SAYISI"sv,       Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    // Who knows what to do with Carrier (Canadian tribe)
    { "CARRIER"sv,      Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE | Dicf::TRIG_SCRIPT },
    { "NASKAPI"sv,      Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "NUNAVIK"sv,      Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "AIVILIK"sv,      Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "ATHAPASCAN"sv,   Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "OJIBWAY",        Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },

    // Other nations
    { "AKHMIMIC",       Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "ASSYRIAN",       Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "BOHAIRIC",       Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "ELAMITE",        Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "JAIN",           Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "SEBATBEIT",      Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },

    // Nouns
    { "ANUSVARA",       Dicf::PART_NOUN },
    { "APOSTROPHE",     Dicf::PART_NOUN },
    { "AXE",            Dicf::PART_NOUN },  // For Phaistos Disc
    { "ARROW",          Dicf::PART_NOUN },
    { "ARROWHEAD",      Dicf::PART_NOUN },
    { "BAR",            Dicf::PART_NOUN },
    { "BEAT",           Dicf::PART_NOUN },
    { "BIRD",           Dicf::PART_NOUN },  // For some CJK hiero
    { "BOX",            Dicf::PART_NOUN },
    { "CANDRABINDU",    Dicf::PART_NOUN },
    { "CARET",          Dicf::PART_NOUN },  // For APL not really, but OK
    { "COLON",          Dicf::PART_NOUN },
     { "TRICOLON",      Dicf::PART_NOUN },
     { "QUADCOLON",     Dicf::PART_NOUN },
    { "COMMA",          Dicf::PART_NOUN },
    { "CIRCLE",         Dicf::PART_NOUN },
    { "DANDA",          Dicf::PART_NOUN },
    { "DIVIDER",        Dicf::PART_NOUN },
    { "DNA",            Dicf::PART_NOUN | Dicf::CAP_ALL },
    { "DOT",            Dicf::PART_NOUN },
    { "DOTS",           Dicf::PART_NOUN },
    { "FORM",           Dicf::PART_NOUN },
    { "HAMZA",          Dicf::PART_NOUN },  // Arabic, not really letter
    { "HASANTA",        Dicf::PART_NOUN },
    { "HOOK",           Dicf::PART_NOUN },
    { "LINE",           Dicf::PART_NOUN },
    { "MCHU",           Dicf::PART_NOUN },
    { "PLUS",           Dicf::PART_NOUN },
    { "REDUPLICATION",  Dicf::PART_NOUN },
    { "RIKRIK",         Dicf::PART_NOUN },
    { "RING"sv,         Dicf::PART_NOUN },
    { "SEPARATOR",      Dicf::PART_NOUN },
    { "SHAD",           Dicf::PART_NOUN },
    { "SHELF",          Dicf::PART_NOUN },
    { "SHOE",           Dicf::PART_NOUN },  // For APL not really, but OK
    { "SPIRANT",        Dicf::PART_NOUN },  // One time, probably OK
    { "STOP",           Dicf::PART_NOUN },
    { "STROKE",         Dicf::PART_NOUN },
    { "STROKES",        Dicf::PART_NOUN },
    { "SVARITA",        Dicf::PART_NOUN },
    { "TACK",           Dicf::PART_NOUN },
    { "TREE",           Dicf::PART_NOUN },
    { "TSHEG",          Dicf::PART_NOUN },
    { "UNDERBAR",       Dicf::PART_NOUN },
    { "VIRAMA",         Dicf::PART_NOUN },
    { "VISARGA",        Dicf::PART_NOUN },
    { "YANG",           Dicf::PART_MIXED },  // for Chinese tones, need more things
    //{ "YAT",            Dicf::PART_NOUN }, // Yat is normal (Capitalized)…
    { "YIN",            Dicf::PART_NOUN },   // for Chinese tones, seems to be OK
    { "YUS",            Dicf::PART_NOUN },   // …but yus is big & little → do not cap

    // Adverbs
    { "ABOVE",          Dicf::PART_NOUN },
    { "BELOW",          Dicf::PART_NOUN },

    // Numerals
    { "ONE"sv,          Dicf::PART_NUMERAL },
    { "TWO"sv,          Dicf::PART_NUMERAL },
    { "THREE"sv,        Dicf::PART_NUMERAL },
    { "FOUR"sv,         Dicf::PART_NUMERAL },
    { "FIVE"sv,         Dicf::PART_NUMERAL },
    { "SIX"sv,          Dicf::PART_NUMERAL },
    { "SEVEN"sv,        Dicf::PART_NUMERAL },
    { "EIGHT"sv,        Dicf::PART_NUMERAL },
    { "NINE"sv,         Dicf::PART_NUMERAL },
    { "TEN"sv,          Dicf::PART_NUMERAL },
    { "ELEVEN"sv,       Dicf::PART_NUMERAL },
    { "TWELVE"sv,       Dicf::PART_NUMERAL },
    { "FORTY"sv,        Dicf::PART_NUMERAL },
    { "FIFTY"sv,        Dicf::PART_NUMERAL },

    // Numeric tones are nouns
    { "TONE-1"sv,       Dicf::PART_NOUN },
    { "TONE-2"sv,       Dicf::PART_NOUN },
    { "TONE-3"sv,       Dicf::PART_NOUN },
    { "TONE-4"sv,       Dicf::PART_NOUN },
    { "TONE-5"sv,       Dicf::PART_NOUN },
    { "TONE-6"sv,       Dicf::PART_NOUN },
    { "TONE-7"sv,       Dicf::PART_NOUN },
    { "TONE-8"sv,       Dicf::PART_NOUN },
    { "TONE-9"sv,       Dicf::PART_NOUN },
    { "TONE-B"sv,       Dicf::PART_NOUN },
    { "TONE-D"sv,       Dicf::PART_NOUN },
    { "TONE-G"sv,       Dicf::PART_NOUN },
    { "TONE-J"sv,       Dicf::PART_NOUN },
    { "TONE-M"sv,       Dicf::PART_NOUN },
    { "TONE-S"sv,       Dicf::PART_NOUN },

    // Numeric marks are nouns
    { "MARK-1"sv,       Dicf::PART_NOUN },
    { "MARK-2"sv,       Dicf::PART_NOUN },
    { "MARK-3"sv,       Dicf::PART_NOUN },
    { "MARK-4"sv,       Dicf::PART_NOUN },
    { "MARK-5"sv,       Dicf::PART_NOUN },
    { "MARK-6"sv,       Dicf::PART_NOUN },
    { "MARK-7"sv,       Dicf::PART_NOUN },
    { "MARK-8"sv,       Dicf::PART_NOUN },
    { "MARK-9"sv,       Dicf::PART_NOUN },

    // Musical nouns
    { "ACCENT"sv,       Dicf::PART_NOUN },
    { "ACCENT-STACCATO"sv, Dicf::PART_NOUN },
    { "BARLINE"sv,      Dicf::PART_NOUN },
    { "BOW"sv,          Dicf::PART_NOUN },
    { "DOIT"sv,         Dicf::PART_NOUN },
    { "FLIP"sv,         Dicf::PART_NOUN },
    { "HARMONIC"sv,     Dicf::PART_NOUN },      // OK, music only
    { "LOURE"sv,        Dicf::PART_NOUN },
    { "MARCATO"sv,      Dicf::PART_NOUN },
    { "MARCATO-STACCATO"sv, Dicf::PART_NOUN },
    { "NOTE"sv,         Dicf::PART_NOUN },
    { "PIZZICATO"sv,    Dicf::PART_NOUN },
    { "REST"sv,         Dicf::PART_NOUN },
    { "RIP"sv,          Dicf::PART_NOUN },
    { "STACCATISSIMO"sv, Dicf::PART_NOUN },
    { "STACCATO"sv,     Dicf::PART_NOUN },
    { "TENUTO"sv,       Dicf::PART_NOUN },
    { "TONGUE"sv,       Dicf::PART_NOUN },      // OK, never used as adjective
    { "TURN"sv,         Dicf::PART_NOUN },

    // Adjectives
    { "ACUTE",          Dicf::PART_ADJECTIVE },
    { "ALTERNATE",      Dicf::PART_ADJECTIVE },
    { "ARCHAIC",        Dicf::PART_ADJECTIVE },
    { "ASPIRATED",      Dicf::PART_ADJECTIVE },
    { "BARRED",         Dicf::PART_ADJECTIVE },
    { "BIG",            Dicf::PART_ADJECTIVE },
    { "BOOK",           Dicf::PART_ADJECTIVE },
    { "BOTTOM",         Dicf::PART_ADJECTIVE },
    { "CAPITAL",        Dicf::PART_ADJECTIVE },
    { "CASKET",         Dicf::PART_ADJECTIVE },  // OK here, runes
    { "CIRCLED",        Dicf::PART_ADJECTIVE | Dicf::CAP_SMALL },
    { "CLOSED",         Dicf::PART_ADJECTIVE },
    { "COMBINING",      Dicf::PART_ADJECTIVE },
    { "CROSSED",        Dicf::PART_ADJECTIVE },
    { "CRYPTOGRAMMIC",  Dicf::PART_ADJECTIVE },
    { "CURLED",         Dicf::PART_ADJECTIVE },
    { "DOUBLE",         Dicf::PART_ADJECTIVE },
    { "DOUBLE-STRUCK",  Dicf::PART_ADJECTIVE },
    { "DOTTED",         Dicf::PART_ADJECTIVE },
    { "DOTLESS",        Dicf::PART_ADJECTIVE },
    { "DOWN",           Dicf::PART_ADJECTIVE },
    { "DIAERESIZED",    Dicf::PART_ADJECTIVE },
    { "DIAGONAL"sv,     Dicf::PART_ADJECTIVE },
    { "DIALECT-P"sv,    { Dicf::PART_ADJECTIVE, "dialect-P" } },
    { "EXCLAMATION"sv,  Dicf::PART_ADJECTIVE },
    { "FIXED-FORM"sv,   Dicf::PART_ADJECTIVE },
    { "FINAL"sv,        Dicf::PART_ADJECTIVE },
    { "FLAT"sv,         Dicf::PART_ADJECTIVE },
    { "FLATTENED"sv,    Dicf::PART_ADJECTIVE },
    { "FRICATIVE"sv,    Dicf::PART_ADJECTIVE },
    { "GLOTTAL"sv,      Dicf::PART_ADJECTIVE },
    { "GRAVE"sv,        Dicf::PART_ADJECTIVE },
    { "GREAT"sv,        Dicf::PART_ADJECTIVE },
    { "HALF"sv,         Dicf::PART_ADJECTIVE },
    { "HARD"sv,         Dicf::PART_ADJECTIVE },
    { "HIGH"sv,         Dicf::PART_ADJECTIVE },
    { "HEAVY"sv,        Dicf::PART_ADJECTIVE },
    { "HORIZONTAL"sv,   Dicf::PART_ADJECTIVE },
    { "IOTATED"sv,      Dicf::PART_ADJECTIVE },
    { "IOTIFIED"sv,     Dicf::PART_ADJECTIVE },
    { "INHERENT"sv,     Dicf::PART_ADJECTIVE },
    { "INITIAL"sv,      Dicf::PART_ADJECTIVE },
    { "INSCRIPTIONAL"sv, Dicf::PART_ADJECTIVE },
    { "INSULAR"sv,      Dicf::PART_ADJECTIVE },
    { "INVERTED"sv,     Dicf::PART_ADJECTIVE },
    { "L-SHAPED"sv,     Dicf::PART_ADJECTIVE | Dicf::CAP_TITLE },
    { "LATINATE"sv,     Dicf::PART_ADJECTIVE },
    { "LEFT"sv,         Dicf::PART_ADJECTIVE },
    { "LEFT-STEM"sv,    Dicf::PART_ADJECTIVE },
    { "LARYNGEAL",      Dicf::PART_ADJECTIVE },
    { "LONG",           Dicf::PART_ADJECTIVE },
    { "LONG-LEGGED",    Dicf::PART_ADJECTIVE },
    { "LOOPED",         Dicf::PART_ADJECTIVE },
    { "LOW",            Dicf::PART_ADJECTIVE },
    { "LUNATE",         Dicf::PART_ADJECTIVE },
    { "LITTLE",         Dicf::PART_ADJECTIVE },
    { "MAI"sv,          Dicf::PART_ADJECTIVE },
    { "MALAYALAM"sv,    Dicf::PART_ADJECTIVE },
    { "MARWARI"sv,      Dicf::PART_ADJECTIVE },
    { "MEDIAL"sv,       Dicf::PART_ADJECTIVE },
    { "MID"sv,          Dicf::PART_ADJECTIVE },
    { "MIDDLE"sv,       Dicf::PART_ADJECTIVE },
    { "MONOGRAPH"sv,    Dicf::PART_ADJECTIVE },
    { "NEW"sv,          Dicf::PART_ADJECTIVE },
    { "NORTH"sv,        Dicf::PART_ADJECTIVE },
    { "OBLIQUE"sv,      Dicf::PART_ADJECTIVE },
    { "OPEN"sv,         Dicf::PART_ADJECTIVE },
    { "OVERLONG"sv,     Dicf::PART_ADJECTIVE },
    { "PALATAL"sv,      Dicf::PART_ADJECTIVE },
    { "PHARYNGEAL"sv,   Dicf::PART_ADJECTIVE },
    { "PHASE-A"sv,      { Dicf::PART_ADJECTIVE, "phase-A" } },
    { "PHASE-B"sv,      { Dicf::PART_ADJECTIVE, "phase-B" } },
    { "PHASE-C"sv,      { Dicf::PART_ADJECTIVE, "phase-C" } },
    { "PHASE-D"sv,      { Dicf::PART_ADJECTIVE, "phase-D" } },
    { "PHASE-E"sv,      { Dicf::PART_ADJECTIVE, "phase-E" } },
    { "PHASE-F"sv,      { Dicf::PART_ADJECTIVE, "phase-F" } },
    { "PSALTER",        Dicf::PART_ADJECTIVE },
    { "QUARTER",        Dicf::PART_ADJECTIVE },     // Not really OK, but works somehow
    { "REVERSE",        Dicf::PART_ADJECTIVE },
    { "REVERSED",       Dicf::PART_ADJECTIVE },
    { "RETROFLEX",      Dicf::PART_ADJECTIVE },
    { "RIGHT",          Dicf::PART_ADJECTIVE },
    { "RIGHT-STEM",     Dicf::PART_ADJECTIVE },
    { "ROUNDED",        Dicf::PART_ADJECTIVE },
    { "SARA",           Dicf::PART_ADJECTIVE },
    { "SEMISOFT",       Dicf::PART_ADJECTIVE },
    { "SHARP",          Dicf::PART_ADJECTIVE },
    { "SHORT",          Dicf::PART_ADJECTIVE },
    { "SHORT-TWIG-AR",  Dicf::PART_ADJECTIVE },
    /// @todo [decap] Arabic mark Sideways Noon gnunna — is it right?
    { "SIDEWAYS",       Dicf::PART_ADJECTIVE },
    { "SMALL",          Dicf::PART_ADJECTIVE },
    { "SNAP",           Dicf::PART_ADJECTIVE },     // OK, music only
    { "SPACING",        Dicf::PART_ADJECTIVE },
    { "SOFT",           Dicf::PART_ADJECTIVE },
    { "SOUTH",          Dicf::PART_ADJECTIVE },
    { "STRAIGHT",       Dicf::PART_ADJECTIVE },
    { "STRETCHED",      Dicf::PART_ADJECTIVE },
    { "SYLLABIC",       Dicf::PART_ADJECTIVE },
    { "TAILED",         Dicf::PART_ADJECTIVE },
    { "TALL",           Dicf::PART_ADJECTIVE },
    { "THREE-D",        { Dicf::PART_ADJECTIVE, "three-D" } },
    { "THREE-LEGGED",   Dicf::PART_ADJECTIVE },
    { "TOP",            Dicf::PART_ADJECTIVE },
    { "TRIANGULAR",     Dicf::PART_ADJECTIVE },
    { "TRIPLE",         Dicf::PART_ADJECTIVE },
    { "TROKUTASTI",     Dicf::PART_ADJECTIVE },
    { "TRADITIONAL",    Dicf::PART_ADJECTIVE },
    { "TRUNCATED",      Dicf::PART_ADJECTIVE },
    { "TURNED",         Dicf::PART_ADJECTIVE },
    { "UNBLENDED",      Dicf::PART_ADJECTIVE },
    { "UP",             Dicf::PART_ADJECTIVE },
    { "VERTICAL",       Dicf::PART_ADJECTIVE },
    { "VOCALIC",        Dicf::PART_ADJECTIVE },
    { "VOICED",         Dicf::PART_ADJECTIVE },
    { "VOICELESS",      Dicf::PART_ADJECTIVE },
    { "WIDE",           Dicf::PART_ADJECTIVE },

    // Names
    { "AESCULAPIUS",    Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "AFRICAN",        Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "ARTA",           Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "CHRISTMAS",      Dicf::CAP_TITLE },
    { "DAVID",          Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "FARSI",          Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "FITZPATRICK",    Dicf::CAP_TITLE },
    { "FRANKS",         Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "FUJI",           Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "HERMES",         Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "HYGIEIA",        Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "JAPAN",          Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "JERUSALEM",      Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "JOVE",           Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "KASHMIRI",       Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "KIRGHIZ",        Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "KORANIC",        Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "LILITH",         Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "LORRAINE",       Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "MARY",           Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "ORKHON",         Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "SELENA",         Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "URANUS",         Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "VOLAPUK",        Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },   // OK here
    { "YENISEI",        Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },   // OK here

    // Some letters
    { "A",              Dicf::PART_MAYBE_LETTER },
    { "-A",             Dicf::CAP_ALL },
    { "ALEF",           Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "AA",             Dicf::CAP_TITLE },
    { "AE",             Dicf::CAP_TITLE },
    { "AIN",            Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "AU",             Dicf::CAP_TITLE },
    { "BEH",            Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "B",              Dicf::CAP_TITLE },
    { "C",              Dicf::CAP_TITLE },
    { "D",              Dicf::CAP_TITLE },
    { "DAD",            Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "DAL",            Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "E",              Dicf::CAP_TITLE },
    { "EE",             Dicf::CAP_TITLE },
    { "EH",             Dicf::CAP_TITLE },
    { "F",              Dicf::CAP_TITLE },
    { "FEH",            Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "G",              Dicf::CAP_TITLE },
    { "GHAIN",          Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "H",              Dicf::CAP_TITLE },
    { "HAH",            Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "HEH",            Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "I",              Dicf::CAP_TITLE },
    { "J",              Dicf::CAP_TITLE },
    { "JEEM",           Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "K",              Dicf::CAP_TITLE },
    { "KAF",            Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "KHAH",           Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "L",              Dicf::CAP_TITLE },
    { "LAM",            Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "M",              Dicf::CAP_TITLE },
    { "MEEM",           Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "N",              Dicf::CAP_TITLE },
    { "NOON",           Dicf::CAP_TITLE | Dicf::PART_NOUN },    // OK here, no English noon (12:00)
    //{ "NI",             Dicf::CAP_TITLE },    // widely used in cuneiform, IDK what to do
    { "O",              Dicf::CAP_TITLE },
    { "OM",             Dicf::CAP_TITLE },
    { "OU",             Dicf::CAP_TITLE },
    { "P",              Dicf::CAP_TITLE },
    { "Q",              Dicf::CAP_TITLE },
    { "QAF",            Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "R",              Dicf::CAP_TITLE },
    { "REH",            Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "S",              Dicf::CAP_TITLE },
    { "SAD",            Dicf::PART_MAYBE_LETTER },
    { "SEEN",           Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "SHEEN",          Dicf::PART_MAYBE_LETTER },
    { "T",              Dicf::CAP_TITLE },
    { "TAH",            Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "TEH",            Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "THAL",           Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "THEH",           Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "TH",             Dicf::CAP_TITLE },
    { "U",              Dicf::CAP_TITLE },
    { "V",              Dicf::CAP_TITLE },
    { "W",              Dicf::CAP_TITLE },
    { "WAW",            Dicf::CAP_TITLE },
    { "X",              Dicf::CAP_TITLE },
    { "Y",              Dicf::CAP_TITLE },
    { "YU",             Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "YEH",            Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "Z",              Dicf::CAP_TITLE },
    { "ZAH",            Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "ZAIN",           Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "J-SIMPLIFIED",   Dicf::CAP_TITLE },

    // Some prepositions
    { "WITH"sv,         Dicf::CAP_SMALL },
    //{ "AND"sv,          Dicf::CAP_SMALL },    // Tricky word, all And’s are tested manually
    { "JOINER"sv,       Dicf::CAP_SMALL },
    //{ "ABOVE"sv,        Dicf::CAP_SMALL },    // Tricky word, all above’s are tested manually
    { "FOR"sv,          Dicf::CAP_SMALL },
};


#define EX(x) { TOUPPER_SV(x), x },

const std::map<std::string_view, std::string_view> exceptions{
    EX("Commercial At")                         // At is not a preposition
    EX("Fraction slash")                        // Fraction’s slash
    EX("Musical symbol With fingernails")       // With is not a preposition
    EX("Devanagari letter Candra A")            // Both Candra and A are tricky, better to make an exception
    EX("Vedic tone Dot below")                  // Not tone’s dot
    EX("Vedic tone Ring above")                 // Not tone’s ring
    EX("Zanabazar square final consonant mark") // Some bugs
    EX("Musical symbol Quarter rest")           // Quarter is a tricky word
    EX("Musical symbol Quarter note")           // The same
    EX("Arabic number mark above")              // Number’s mark
    EX("Manichaean abbreviation mark above")    // Abbreviation’s mark
    EX("Manichaean abbreviation mark below")    // The same
    EX("Kaithi number sign above")              // The same
    EX("Squared HV")                            // Just manually, small block
    EX("Squared MV")
    EX("Squared SD")
    EX("Squared SS")
    EX("Squared PPV")
    EX("Squared WC")
    EX("Miao tone Above")                       // Adverb!
    EX("Miao tone Below")                       // Adverb!
    EX("Musical symbol Left repeat sign")       // Just manually, 3 chars
    EX("Musical symbol Right repeat sign")      // The same
    EX("Musical symbol Half pedal mark")        // The same
    EX("Square hPa")                            // A whole cohort of Chinese units
    EX("Square IU")
    EX("Square da")
    EX("Square AU")
    EX("Square bar")
    EX("Square oV")
    EX("Square pc")
    EX("Square dm squared")
    EX("Square pA amps")
    EX("Square nA")
    EX("Square mu A")
    EX("Square mA")
    EX("Square kA")
    EX("Square KB")
    EX("Square MB")
    EX("Square GB")
    EX("Square cal")
    EX("Square kcal")
    EX("Square pF")
    EX("Square nF")
    EX("Square mu F")
    EX("Square mu G")
    EX("Square mg")
    EX("Square kg")
    EX("Square Hz")
    EX("Square kHz")
    EX("Square MHz")
    EX("Square GHz")
    EX("Square THz")
    EX("Square mu l")
    EX("Square ml")
    EX("Square dl")
    EX("Square kl")
    EX("Square fm")
    EX("Square nm")
    EX("Square mu m")
    EX("Square mm")
    EX("Square cm")
    EX("Square km")
    EX("Square mm squared")
    EX("Square cm squared")
    EX("Square m squared")
    EX("Square km squared")
    EX("Square mm cubed")
    EX("Square m cubed")
    EX("Square km cubed")
    EX("Square m over s")
    EX("Square m over s squared")
    EX("Square Pa")
    EX("Square kPa")
    EX("Square MPa")
    EX("Square GPa")
    EX("Square rad")
    EX("Square rad over s")
    EX("Square rad over s squared")
    EX("Square ps")
    EX("Square ns")
    EX("Square mu s")
    EX("Square ms")
    EX("Square pV")
    EX("Square nV")
    EX("Square mu V")
    EX("Square mV")
    EX("Square kV")
    EX("Square MV mega")
    EX("Square pW")
    EX("Square nW")
    EX("Square mu W")
    EX("Square mW")
    EX("Square kW")
    EX("Square MW mega")
    EX("Square k Ohm")
    EX("Square M Ohm")
    EX("Square am")
    EX("Square Bq")
    EX("Square cc")
    EX("Square cd")
    EX("Square C over kg")
    EX("Square Co")
    EX("Square dB")
    EX("Square Gy")
    EX("Square ha")
    EX("Square HP")
    EX("Square in")
    EX("Square KK")
    EX("Square KM capital")
    EX("Square kt")
    EX("Square lm")
    EX("Square ln")
    EX("Square log")
    EX("Square lx")
    EX("Square mb small")
    EX("Square mil")
    EX("Square mol")
    EX("Square pH")
    EX("Square pm")
    EX("Square PPM")
    EX("Square PR")
    EX("Square sr")
    EX("Square Sv")
    EX("Square Wb")
    EX("Square v over m")
    EX("Square a over m")
    EX("Square gal")
    EX("Square Hg")
    EX("Square erg")
    EX("Square eV")
    EX("Square DJ")
    EX("Earth globe Europe-Africa")     // Rare names
    EX("Earth globe Americas")
    EX("Earth globe Asia-Australia")
    EX("Milky Way")                     // Special proper name
    EX("Tokyo Tower")                   // Special proper name
    EX("Statue of Liberty")             // Special proper name
    EX("Face with No good gesture")     // Complex rules, better manually
    EX("Face with OK gesture")          // Complex rules, better manually
    EX("Squared OK")                    // OK is both a syllable and an emoji
    EX("Squared SOS")                   // The same
    EX("Squared CL")                    // Squared is a tricky word
    EX("Squared Cool")
    EX("Squared New")
    EX("Squared No good")
    EX("Squared Up with exclamation mark")
    EX("Squared Three D")
    EX("Squared Second screen")
    EX("Squared Two K")
    EX("Squared Four K")
    EX("Squared Eight K")
    EX("Squared Five point one")
    EX("Squared Seven point one")
    EX("Squared Twenty-two point two")
    EX("Squared Sixty P")
    EX("Squared One hundred twenty P")
    EX("Squared HC")
    EX("Squared Vs")
    EX("Squared New")
    EX("Squared NG")
    EX("Squared Free")
    EX("Squared ID")
    EX("Squared Hi-res")
    EX("Squared HDR")
    EX("Squared SHV")
    EX("Squared VOD")
    EX("Squared UHD")
    EX("Squared Lossless")
    EX("Negative squared IC")
    EX("Negative squared PA")
    EX("Negative squared SA")
    EX("Negative squared AB")
    EX("Negative squared WC")
    EX("OK hand sign")                  // OK is both a syllable and an emoji
    EX("Turned OK hand sign")           // The same
    EX("Sloping large Or")              // Or is a tricky word
    EX("Sloping large And")             // And is a tricky word
    EX("Arabic small low Waw")          // IDK what to do
    EX("Arabic small high Sad")
    EX("Arabic small high Ain")
    EX("Arabic small high Qaf")
    EX("Arabic small high Noon with kasra")
    EX("Arabic small low Noon with kasra")
    EX("Arabic small high Waw")
    EX("Arabic small high word Ar-rub") // “Word” is a tricky word
    EX("Arabic small high word Ath-thalatha")
    EX("Arabic small high word As-sajda")
    EX("Arabic small high word An-nisf")
    EX("Arabic small high word Sakta")
    EX("Arabic small high word Qif")
    EX("Arabic small high word Waqfa")
    EX("Runic letter Dotted-N")         // Tricky
    EX("Runic letter Dotted-L")         // The same
    EX("Runic letter Long-branch-Yr")   // Really tricky
    EX("Runic letter Short-twig-Yr")    // The same
    EX("Runic letter Icelandic-Yr")
    EX("Runic letter Dotted-P")
    EX("Runic letter Open-P")
    EX("Runic letter Sigel Long-branch-Sol S")
    EX("Runic letter Short-twig-Sol S")
    EX("Runic letter Long-branch-Ar Ae")
    EX("Runic letter Short-twig-Ar A")
    EX("Runic letter Long-branch-Hagall H")
    EX("Runic letter Short-twig-Hagall H")
    EX("Runic letter Short-twig-Bjarkan B")
    EX("Runic letter Long-branch-Oss O")
    EX("Runic letter Short-twig-Oss O")
    EX("Runic letter Short-twig-Naud N")
    EX("Runic letter Long-branch-Madr M")
    EX("Runic letter Short-twig-Madr M")
    EX("Runic letter Short-twig-Tyr T")
    EX("Yi syllable iteration mark")    // Maybe so
    EX("Symbol for type A electronics") // A is tricky
    EX("Coptic combining Ni above") // Ni is widely used in cuneiform, and IDK what to do
    EX("Nyiakeng Puachue Hmong circled Ca") // IDK what to do
    EX("Dives Akuru medial Ya")             // The same
    EX("Dives Akuru initial Ra")
    EX("Dives Akuru medial Ra")
    EX("Arabic letter Lam with small Arabic letter Tah above")  // Some bug
    EX("Arabic letter Teh with small Teh above")    // Tricky
    EX("Arabic letter Reh with small Noon above")   // Same
    EX("Arabic letter Yeh with two dots below and small Noon above")
    EX("Arabic letter Peh with small Meem above")
    EX("Arabic letter Beh with small Meem above")
    EX("Arabic-Indic Per mille sign")               // Tricky
    EX("Arabic-Indic Per ten thousand sign")        // Tricky
    EX("Arabic mathematical Sad")                   // Ambiguous
    EX("Arabic mathematical Sheen")                 // Ambiguous
};

const std::multiset<PrefixEntry> prefixes {
    { { "APL"sv, "FUNCTIONAL"sv, "SYMBOL"sv }, PrefixAction::REST_CAPSMALL },
    { { "QUADRANT"sv }, PrefixAction::NEXT_CAP },
    { { "BOX"sv, "DRAWINGS"sv }, PrefixAction::NEXT_CAP },
    { { "MAHJONG"sv, "TILE"sv }, PrefixAction::REST_CAPSMALL },
    { { "DOMINO"sv, "TILE"sv }, PrefixAction::REST_CAPSMALL },
    { { "PLAYING"sv, "CARD"sv }, PrefixAction::REST_CAPSMALL },
    { { "TIBETAN"sv, "DIGIT"sv, "HALF"sv }, PrefixAction::NEXT_SMALL },
    { { "SUPERSCRIPT"sv }, PrefixAction::NEXT_CAP },
    { { "SUBSCRIPT"sv }, PrefixAction::NEXT_CAP },
    { { "INDIC"sv, "SIYAQ"sv, "NUMBER"sv }, PrefixAction::REST_CAPSMALL },
    { { "MODIFIER"sv, "LETTER"sv, "CHINESE"sv, "TONE"sv, }, PrefixAction::NEXT_CAP },
    { { "SIGNWRITING"sv }, PrefixAction::REST_SMALL },
    { { "RUNIC"sv, "LETTER"sv, "FRANKS"sv, "CASKET"sv }, PrefixAction::REST_CAP },      // prevent next!
    { { "RUNIC"sv, "LETTER"sv }, PrefixAction::REST_CAP },
    { { "CIRCLED"sv, "KATAKANA"sv }, PrefixAction::NEXT_CAP },  // IDK what to do, normal rules fail
    { { "CJK"sv, "STROKE"sv }, PrefixAction::REST_ALLCAP },
};


const std::map<char32_t, std::string_view> abbrevs {
    { 9, "TAB" },       // TAB or HT
    { 10, "LF" },       // NL or LF
    { ' ', {} },        // SPACE has abbrev SP
    { 0x00A0, "!" },    // NBSP bas abbreviations, turn them to aliases
    { 0x2028, "LSEP" }, // Line separator, somehow has no abbrev
    { 0x2029, "PSEP" }, // Paragraph separator, same
    { 0x202F, "!" },    // Narrow NBSP has abbrevs, turn them to aliases
    { 0x205F, "!" },    // Medium math space same
    { 0x2061, "f()" },  // Function application
    { 0x2062, "✕" },   // Invisible times
    { 0x2063, "," },    // Invisible separator
    { 0x2064, "+" },    // Invisible plus
    { 0x206A, "ISS" },  // Inhibit symmetric swapping
    { 0x206B, "ASS" },  // Activate symmetric swapping
    { 0x206C, "IAFS" }, // Inhibit Arabic form shaping
    { 0x206D, "AAFS" }, // Activate Arabic form shaping
    { 0x206E, "NADS" }, // National digit shapes
    { 0x206F, "NODS" }, // Nominal digit shapes
    { 0xFEFF, "BOM" },  // BOM or ZWNBSP
    { 0xFFF9, "IA A" }, // Interlinear annotation anchor
    { 0xFFFA, "IA S" }, // Interlinear annotation separator
    { 0xFFFB, "IA T" }, // Interlinear annotation terminator
    { 0xFFFC, "OBJ" },  // Object replacement character
    { 0x1107F, "BNJ" }, // Brahmi number joiner
    { 0x13430, ":" },   // Egyptian hiero vertical joiner
    { 0x13431, "✻" },   // Egyptian hiero horiz joiner
    { 0x13432, "`" },   // Egyptian hiero insert at top start
    { 0x13433, "`" },   // Egyptian hiero insert at bottom start
    { 0x13434, "`" },   // Egyptian hiero insert at top end
    { 0x13435, "`" },   // Egyptian hiero insert at bottom end
    { 0x13436, "+" },   // Egyptian hiero overlay middle
    { 0x13437, "(" },   // Egyptian hiero begin segment
    { 0x13438, ")" },   // Egyptian hiero end segment
    // Then Variation Selectors
};

// Just capitalize them!
const std::set<std::string_view> cuneiformSymbols {
    "BAR", "LAM"
};

const std::set<std::string_view> cuneiformKeywords {
    "CUNEIFORM",
    "SIGN",
    "TIMES",
    "OVER",
    "CROSSING",
    "PLUS",
    "NUMERIC",
    "INVERTED",
    "ROTATED",
    "NINETY",
    "DEGREES",
    "PUNCTUATION",
    "DIAGONAL",
    "QUADCOLON",
    "OPPOSING",
    "SQUARED",
    "VARIANT",
    "ASTERISK",
    "WORD",
    "DIVIDER",
    "THIRD",
    "THIRDS",
    "FOURTH",
    "FOURTHS",
    "FIFTH",
    "FIFTHS",
    "SIXTH",
    "SIXTHS",
};



/// @todo [langs] Stopped at Canadian syllabics
const std::set<std::string_view> langNames {
    "also Cornish",     // корнский (Великобритания)
    "also used in Americanist linguistics",
    "used in transcription of Mandarin Chinese",
    "used by linguists working on Chinese and other Sino-Tibetan languages",
    "used in some Polynesian orthographies for glottal stop",
    "also used for Enets",      // энецкий (Россия)
    "also used for Itelmen"     // ительменский (Россия)
    "also used for Nenets"
    "also used for Ossetian"    // осетинский (Россия)
    "Middle Eastern Arabic for foreign words"
    "North African Arabic for foreign words"
        /// @todo [langs] Headers like “@→→Komi letters”, → = TAB

    "and many other languages",
    "...",
    "not in current use",

    "Abkhasian",
    "Adamawa Fulfulde", // фулá (Камерун)
    "Adighe",           // адыгейский (=западночеркесский)
    "African",
    "African languages",
    "Afrikaans",
    "Algonquin",
    "Algonquian",       ///< =algonquin
    "Altay",
    "Amazigh",          ///< =berber
    "Americanist phonetic usage",   // индейская фонетика
    "Arwi",             // Арви (арабская запись тамильского)
    "Asturian",
    "Athapascan",       // атабаскские (Канада)
    "Azerbaijani",
    "Baluchi",          // белуджский (Пакистан)
    "Bashkir",
    "Basketo"           // баскето (Эфиопия)
    "Berber",
    "Belarusian"        ///< =Byelorussian
    "Bihari languages"  // бихарские (Индия)
    "Bosnian",
    "Bulgarian",
    "Burushaski",       // бурушаски (Кашмир)
    "Byelorussian",
    "Carrier"           ///< кэрриер (Канада), better stop?
    "Crimean Tatar",
    "Chechen",
    "Chukchi",
    "Chuvash",
    "Croatian",
    "Czech",
    "Danish",
    "Dargwa",           // даргинский (Дагестан)
    "Dutch",
    "early Jawi",
    "early Persian",
    "East Cree",
    "Esperanto",
    "Estonian",
    "Ewe",          // эве (Того)
    "Faroese",
    "Finnish",
    "Finnish Romany",   // финский кало (цыганский)
    "French",
    "former-Soviet minority language scripts",
    "Galician",
    "German",
    "German dialectology",
    "Germanic",
    "Greenlandic (old orthography)",
    "Gojri",                    // гуджарский (Пакистан)
    "Gothic transliteration",   // готская транслитерация
    "Hausa",
    "Hittite transcription",    // транскрипция хеттского
    "Hungarian",
    "Huron",
    "Icelandic",
    "Igbo",         // игбо (Нигерия)
    "Ingush",
    "Indic transliteration",    // транслитерация санскрита
    "Inuktitut"     ///< better stop?
    "Irish Gaelic (old orthography)",
    "Italian",
    "IPA",
    "IPA and many languages",
    "Iroquoian",
    "Jawi",         // джави (Бруней)
    "Kalami",       // гаври (=калами, Пакистан)
    "Kashmiri",
    "Kazakh",
    "Kharoshthi transliteration",   // транслитерация кхароштхи (Индия, Ср. Азия, −I…I тыс.)
    "Khoisan tradition",    // койсанские (бушменские)
    "Khwarazmian",  // хорезмийский
    "Kikuyu",       // кикуйю (Кения)
    "Kildin Sami",  // кильдинский саамский
    "Kurdish",
    "Kuvi"          // куви (Индия)
    "Labrador Inuttut", // лабрадорский инуитский
    "Lahnda",   // лахнда (западнопенджабские)
    "Lak",      // лакский (Дагестан)
    "Lakota",   // лакота (индейцы США)
        /// @todo [langs] Remove text in parentheses, after:
    "Latin",
    "Latin abbreviation",
    "Latvian",
    "Lithuanian",
    "Lithuanian phonetics and medievalist transcriptions",
    "Livonian",     // ливский (Латвия)
    "Maba",         // маба (Чад)
    "Macedonian",
    "Macedonian and Serbian transliteration",
    "Maghrib Arabic",
    "Madurese",     // мадурский (Индонезия)
    "Malagasy",
    "Malay",        // малайский
    "Maltese",
    "Mari",
    "Marshallese",
    "medieval Hungarian orthography",
    "Mende",        // менде (Сьерра-Леоне)
    "Middle English",   // среднеанглийский
    "Middle High German",
    "Moldavian",
    "Moose Cree",   // мусский кри (Канада)
    "Moroccan Arabic",
    "Norwegian",
    "older orthographies for Abkhasian",
    "Old English",
    "Old Hausa",
    "Old Icelandic",
    "Old Norse",        // древнескандинавский
    "Old Saxon",
    "Old Urdu",
    "Ormuri",           // ормури (Пакистан)
    "Pan-Nigerian alphabet",    // паннигерийский алфавит
    "Pan-Turkic Latin alphabets",   // тюркские
    "Pan-Turkic Latin orthography",
    "Pegon",            // пегон (яванское арабское письмо)
    "Pinyin",                   // пиньинь (транскрипция китайского)
    "Pinyin first tone",
    "Pinyin second tone",
    "Pinyin third tone",
    "Pinyin fourth tone",
        "Mandarin Chinese first tone",
        "Mandarin Chinese second tone",
        "Mandarin Chinese third tone",
        "Mandarin Chinese fourth tone",
        "Mandarin Chinese fifth tone (light or neutral)",
    "phonetics",
    "phonetic use in Sinology",
    "Pashto",       // пуштунский
    "Persian",
    "Philippine languages",
    "Polish",
    "Portuguese",
    "Quranic Arabic",
    "Romanian",
    "Russian",
    "Sami",         // also Saami
    "Saraiki",      // сараики (Пакистан)
    "Sayisi",       // сайиси (Канада)
    "Scots",        // англо-шотландский
    "Semitic transliteration",  // транслитерация семитских языков
    "Serbian",
    "Serbian and Croatian phonetics",
    "Shina"         // шина (Кашмир)
    "Sindhi",       // синдхи (Пакистан)
    "Skolt Sami",   // колтта-саамский (Финляндия)
    "Slovak",
    "Slovenian",
    "Spanish",
    "Swedish",
    "Swedish grave accent",
    "Tajik",
    "Tatar",
    "Torwali",      // торвали (Пакистан)
    "Tunisian Arabic",
    "Turkish",
    "Twi",          // чви (Гана)
    "Uighur",
    "Ukrainian",
    "Uralicist usage",  // уральские языки
    "Urdu",
    "Uzbek",
    "Vietnamese",
    "Walloon",
    "Welsh",
    "West Cree",
    "Yakut",
    "Yoruba",
    "Yupik",        // юпикские (Чукотка)
    "Zhuang (old orthography)", // Чжуанский (старая орфография, Китай)
    "Zulu",
};


namespace {

    enum class WordState { SMALL, TITLE, AS_IS_TITLE, AS_IS, ALL_CAP };

    struct Word {
        std::string_view original;
        bool isAllCap = false;
        bool isCapital = false;
        bool isAsIs = false;
        bool nextNoun = false;
        bool nextAdjective = false;
        bool nextNumeral = false;
        bool isAdjective = false;
        Flags<Dicf> dicFlags;
    };

    constexpr auto TRIG_ANY_SCRIPT = Dicf::TRIG_SCRIPT | Dicf::TRIG_SCRIPT_IF_FIRST
            | Dicf::TRIG_TWO_WORD_SCRIPT | Dicf::TRIG_THREE_WORD_SCRIPT
            | Dicf::PART_ADJECTIVE;

    bool hasDigit(std::string_view x) {
        for (auto v : x) {
            if (std::isdigit(v))
                return true;
        }
        return false;
    }

    void appendLower(std::string& r, std::string_view x) {
        for (auto v : x)
            r += char(std::tolower(v));
    }

    void appendUpper(std::string& r, std::string_view x) {
        for (auto v : x)
            r += char(std::toupper(v));
    }

    std::string toUpper(std::string_view x) {
        std::string r;
        r.reserve(x.size());
        for (auto v : x)
            r += char(std::toupper(v));
        return r;
    }

    void appendTitle(std::string& r, std::string_view x) {
        if (x.empty())
            return;
        r += char(std::toupper(x[0]));
        for (size_t i = 1; i < x.length(); ++i) {
            r += char(std::tolower(x[i]));
        }
    }

    void appendAsIsTitle(std::string& r, std::string_view x) {
        if (x.empty())
            return;
        r += char(std::toupper(x[0]));
        r.append(x.data() + 1, x.length() - 1);
    }

    bool doWordsFit(const SafeVector<Word>& words, Buf1d<const std::string_view> prefix)
    {
        return words.size() > prefix.size()
                && std::equal(prefix.begin(), prefix.end(), words.begin(),
                            +[](std::string_view x, const Word& y) {
                                return x == y.original;
                            });
    }

    void doPrefixAction(SafeVector<Word>& words, size_t prefixSize, PrefixAction action)
    {
        switch (action) {
        case PrefixAction::REST_SMALL:
            for (size_t i = prefixSize; i < words.size(); ++i)
                words[i].isCapital = false;
            break;
        case PrefixAction::REST_CAP:
            for (size_t i = prefixSize; i < words.size(); ++i)
                words[i].isCapital = true;
            break;
        case PrefixAction::REST_ALLCAP:
            for (size_t i = prefixSize; i < words.size(); ++i)
                words[i].isAllCap = true;
            break;
        case PrefixAction::REST_CAPSMALL:
            for (size_t i = prefixSize + 1; i < words.size(); ++i)
                words[i].isCapital = false;
            [[fallthrough]];
        case PrefixAction::NEXT_CAP:
            words[prefixSize].isCapital = true;
            break;
        case PrefixAction::NEXT_SMALL:
            words[prefixSize].isCapital = false;
            break;
        }
    }

    class MultiPrinter
    {
    public:
        void print(const char* x);
        void printIf(bool cond, const char* x);
    private:
        bool wasPrinted = false;
    };

    void MultiPrinter::print(const char* x)
    {
        if (wasPrinted)
            std::cout << " ";
        std::cout << x;
        wasPrinted = true;
    }

    void MultiPrinter::printIf(bool cond, const char* x)
    {
        if (cond)
            print(x);
    }

    void dump(Flags<Dicf> x)
    {
        MultiPrinter mp;
        std::cout << "[";
        mp.printIf(x.have(Dicf::CAP_ALL), "ALL");
        mp.printIf(x.have(Dicf::CAP_SMALL), "small");
        mp.printIf(x.have(Dicf::CAP_TITLE), "Title");
        mp.printIf(x.have(Dicf::IF_SCRIPT), "ifscr");
        mp.printIf(x.have(Dicf::IF_NEXT_NOT_NOUN), "ifnnn");
        mp.printIf(x.have(Dicf::PART_ADJECTIVE), "adj");
        mp.printIf(x.have(Dicf::PART_MAYBE_LETTER), "let?");
        mp.printIf(x.have(Dicf::PART_NOUN), "noun");
        mp.printIf(x.have(Dicf::PART_NUMERAL), "num");
        mp.printIf(x.have(Dicf::PART_MIXED), "mixed");
        mp.printIf(x.have(Dicf::TRIG_CAP_NEXT), "capnext");
        mp.printIf(x.have(Dicf::TRIG_DIGIT_PROPAGATES_RIGHT), "dig->");
        mp.printIf(x.have(Dicf::TRIG_SCRIPT), "Script");
        mp.printIf(x.have(Dicf::TRIG_SCRIPT_IF_FIRST), "Script1");
        mp.printIf(x.have(Dicf::TRIG_THREE_WORD_SCRIPT), "3wScript");
        mp.printIf(x.have(Dicf::TRIG_TWO_WORD_SCRIPT), "2wScript");
        std::cout << "]";
    }

    [[maybe_unused]] void dump(const SafeVector<Word> words)
    {
        for (auto& v : words) {
            std::cout << v.original << " ";
            dump(v.dicFlags);
            std::cout << std::endl;
        }
    }

    constexpr auto STR_IDEOGRAPH = "IDEOGRAPH-"sv;

    bool isHex(std::string_view s) {
        for (char c : s) {
            if (!isxdigit(c))
                return false;
        }
        return true;
    }

}   // anon namespace


std::string decapitalize(std::string_view x, DecapDebug debug)
{
    // Tetect exceptions
    std::string upper = toUpper(x);
    auto itEx = exceptions.find(upper);
    if (itEx != exceptions.end())
        return std::string(itEx->second);

    // Get words
    auto rawWords = str::splitSv(upper, ' ');
    if (rawWords.empty())
        return {};
    SafeVector<Word> words;
    words.resize(rawWords.size());
    for (size_t iWord = 0; iWord < words.size(); ++iWord) {
        words[iWord].original = rawWords[iWord];
    }

    // Detect scripts, cap, small
    bool hasScript = false;
    bool digitPropagatesRight = false;
    Word* nextWord = nullptr;
    Word* nextNextWord = nullptr;
    std::vector<std::string> tempWords;

    for (auto iWord = words.size(); iWord != 0; ) { --iWord;
        auto& word = words[iWord];
        auto itWord = dictionary.find(word.original);
        if (itWord != dictionary.end()) {
            auto& wordInfo = itWord->second;
            word.dicFlags = wordInfo.flags;
            if (word.dicFlags.haveAny(TRIG_ANY_SCRIPT)) {
                if (word.dicFlags.have(Dicf::TRIG_SCRIPT)) {
                    // Script
                    word.isCapital = true;
                    hasScript = true;
                } else if (word.dicFlags.have(Dicf::TRIG_SCRIPT_IF_FIRST)) {
                    // Script only if 1st word
                    if (iWord == 0) {
                        word.isCapital = true;
                        hasScript = true;
                    }
                } else if (word.dicFlags.have(Dicf::TRIG_TWO_WORD_SCRIPT)) {
                    // Two-word script
                    if (nextWord &&
                            (nextWord->original == wordInfo.trigger1
                          || nextWord->original == wordInfo.trigger2
                          || nextWord->original == wordInfo.trigger3
                          || nextWord->original == wordInfo.trigger4
                          || nextWord->original == wordInfo.trigger5)) {
                        word.isCapital = true;
                        nextWord->isCapital = true;
                        hasScript = true;
                    }
                } else if (word.dicFlags.have(Dicf::TRIG_THREE_WORD_SCRIPT)) {
                    // Three-word script
                    if (nextNextWord && nextWord->original == wordInfo.trigger1
                            && nextNextWord->original == wordInfo.trigger2) {
                        word.isCapital = true;
                        nextWord->isCapital = true;
                        nextNextWord->isCapital = true;
                        hasScript = true;
                    }
                } else if (word.dicFlags.have(Dicf::PART_ADJECTIVE)) {
                    // Adjective
                    if (nextWord && nextWord->isCapital) {
                        word.isCapital = true;
                        hasScript = true;
                    }
                }
            }
            // Check whether digits propagate right
            if (word.dicFlags.have(Dicf::TRIG_DIGIT_PROPAGATES_RIGHT)) {
                digitPropagatesRight = true;
            }
            // Check for custom capitalization
            if (!wordInfo.customCap.empty()) {
                word.original = wordInfo.customCap;
                word.isAsIs = true;
            }
        }

        // Check for ideograph capitalization
        if (word.original.starts_with(STR_IDEOGRAPH)) {
            auto remainder = word.original.substr(STR_IDEOGRAPH.length());
            if (isHex(remainder)) {
                auto& w = tempWords.emplace_back(word.original);
                for (size_t i = 0; i < STR_IDEOGRAPH.length(); ++i) {
                    w[i] = tolower(w[i]);
                }
                word.original = w;
                word.isAsIs = true;
            }
        }

        if (nextWord) {
            if (nextWord->dicFlags.have(Dicf::PART_NOUN)) {
                word.nextNoun = true;
            } else if (nextWord->dicFlags.have(Dicf::PART_ADJECTIVE)) {
                word.nextAdjective = true;
            } else if (nextWord->dicFlags.have(Dicf::PART_NUMERAL)) {
                if (nextNextWord && nextNextWord->dicFlags.have(Dicf::PART_NOUN)) {     // tone Two dots → next noun “dots”, not numeral 2
                    //word.nextNoun = true;         // But do not register it, “tone” will also be a noun
                } else {
                    word.nextNumeral = true;
                }
            } else if (nextWord->dicFlags.have(Dicf::PART_MIXED)) {
                if (nextWord->nextNoun || nextWord->nextAdjective) {
                    word.nextAdjective = true;
                } else {
                    word.nextNoun = true;
                }
            }
        }
        nextNextWord = nextWord;  nextWord = &word;
    }

    if (debug != DecapDebug::NO)
        dump(words);

    // Propagate right
    int propagationLevel = 0;
    Word* prevWord = nullptr;
    for (auto& word : words) {
        bool wantPropagate = false;
        if (word.dicFlags.have(Dicf::PART_MIXED)) {
            if (word.nextNoun) {
                // Adjective
                word.isAdjective = true;
            }
        }
        if (propagationLevel != 0) {
            if (!prevWord)
                throw std::logic_error("prevWord should be non-null!");
            if (word.dicFlags.have(Dicf::PART_MIXED)) {
                if (word.isAdjective    // We already defined that it is adjective
                        || (word.nextAdjective && propagationLevel > 1)) {  // High tone thing
                    // Adjective
                    word.isAdjective = true;
                    wantPropagate = true;
                } else {
                    // Noun
                    word.isCapital = true;
                }
            } else if (word.dicFlags.have(Dicf::PART_ADJECTIVE)) {
                wantPropagate = true;
                word.isAdjective = true;
                // Adj + adj = no cap; “First second third Pi”
                if (!prevWord->isAdjective) {
                    word.isCapital = true;
                }
            } else if (word.dicFlags.have(Dicf::PART_NOUN)) {
                // Stop propagating;
                // Adj + noun = no cap; “Noun”, but “First noun”
                if (!prevWord->isAdjective) {
                    word.isCapital = true;
                }
            } else {
                word.isCapital = true;
            }
            if (digitPropagatesRight && hasDigit(word.original)) {
                // Adjectives propagate further
                // Digits too if we set this
                wantPropagate = true;
            }
        }
        bool wantTrigger = true;
        if (word.dicFlags.have(Dicf::IF_SCRIPT) && !hasScript)
            wantTrigger = false;
        if (word.dicFlags.have(Dicf::IF_NEXT_NOT_NOUN) && (word.nextNoun || word.nextNumeral))
            wantTrigger = false;
        if (wantTrigger) {
            if (word.dicFlags.have(Dicf::TRIG_CAP_NEXT) && !word.isAdjective)
                wantPropagate = true;
        }
        if (word.dicFlags.have(Dicf::PART_MAYBE_LETTER)
                && prevWord && prevWord->dicFlags.have(Dicf::PART_ADJECTIVE)) {
            word.isCapital = true;
        }
        prevWord = &word;
        if (wantPropagate) {
            ++propagationLevel;
        } else {
            propagationLevel = 0;
        }
    }

    // Check for prefixes
    PrefixEntry prefixKey { words[0].original, PrefixAction::NOMATTER };
    auto range = prefixes.equal_range(prefixKey);
    for (auto it = range.first; it != range.second; ++it) {
        auto& entry = *it;
        if (doWordsFit(words, entry.toBuf())) {
            doPrefixAction(words, entry.nTriggers, entry.action);
            break;
        }
    }

    // Find length capitalization
    size_t newLen = words.empty() ? 0 : words.size() - 1;
    for (auto& word : words) {
        newLen += word.original.length();
    }

    // Cuneiform recapitalization
    if (words[0].original == "CUNEIFORM"sv) {
        for (auto& word : words) {
            auto q = dictionary.find(word.original);
            // 1. Symbols — YES
            if (cuneiformSymbols.contains(word.original)) {
                word.isAllCap = true;
            // 2. Script/noun/adjective — NO
            } else if (q->second.flags.haveAny(TRIG_ANY_SCRIPT
                        | Dicf::PART_ADJECTIVE | Dicf::PART_NOUN | Dicf::PART_NUMERAL)) {
                // do nothing
            // 3. Keyword — YES
            } else if (!cuneiformKeywords.contains(word.original)) {
                    word.isAllCap = true;
            }
        }
    }

    // Compose
    std::string r;
    r.reserve(newLen);
    for (size_t iWord = 0; iWord < words.size(); ++iWord) {
        if (!r.empty())
            r.append(" "sv);

        WordState state = WordState::SMALL;
        auto& word = words[iWord];
        if (word.isAllCap) {
            state = WordState::ALL_CAP;
        } else if (word.dicFlags.have(Dicf::CAP_SMALL)) {
            state = WordState::SMALL;
        } else if (word.dicFlags.have(Dicf::CAP_ALL)) {
            state = WordState::ALL_CAP;
        } else if (word.dicFlags.have(Dicf::CAP_TITLE)) {
            state = WordState::TITLE;
        } else if (word.isAsIs) {
            state = word.isCapital ? WordState::AS_IS_TITLE : WordState::AS_IS;
        } else if (word.isCapital) {
            state = WordState::TITLE;
        }

        if (iWord == 0) {
            switch (state) {
            case WordState::SMALL: state = WordState::TITLE; break;
            case WordState::AS_IS: state = WordState::AS_IS_TITLE; break;
            case WordState::TITLE:
            case WordState::AS_IS_TITLE:
            case WordState::ALL_CAP:;
            }
        }

        switch (state) {
        case WordState::AS_IS:
            r.append(word.original);
            break;
        case WordState::AS_IS_TITLE:
            appendAsIsTitle(r, word.original);
            break;
        case WordState::SMALL:
            appendLower(r, word.original);
            break;
        case WordState::TITLE:
            appendTitle(r, word.original);
            break;
        case WordState::ALL_CAP:
            appendUpper(r, word.original);
            break;
        }
    }

    return r;
}
