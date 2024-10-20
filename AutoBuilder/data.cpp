// My header
#include "data.h"

// C++
#include <vector>

// Common libs
#include "u_Strings.h"
#include "u_Vector.h"

using namespace std::string_view_literals;

const std::unordered_map<std::string_view, DicEntry> dictionary {
    // Language names
    { "ABKHASIAN",      Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE },
    { "ADLAM",          Dicf::TRIG_SCRIPT },
    { "AFAKA",          Dicf::TRIG_SCRIPT },
    { "AHOM",           Dicf::TRIG_SCRIPT },
    { "ALBANIAN",       Dicf::TRIG_SCRIPT },
      { "CAUCASIAN",    Dicf::TRIG_SCRIPT },
    { "ALEUT"sv,        Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE },
    { "ANATOLIAN",      Dicf::TRIG_SCRIPT },
    { "APL",            Dicf::CAP_ALL | Dicf::TRIG_SCRIPT },
    { "ARABIC",         Dicf::TRIG_SCRIPT },
    { "ARABIAN",        Dicf::TRIG_SCRIPT },
    { "ARABIC-INDIC",   { Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE, "Arabic-Indic"sv } },
    { "ASSAMESE",       Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE | Dicf::TRIG_FORCE_FULL_DECAP },
    { "IMPERIAL",       Dicf::TRIG_SCRIPT },
    { "ARAMAIC",        Dicf::TRIG_SCRIPT },
    { "ARMENIAN",       Dicf::TRIG_SCRIPT },
    { "AVESTAN",        Dicf::TRIG_SCRIPT },
    { "BACTRIAN",       Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE },
    { "BALINESE",       Dicf::TRIG_SCRIPT },
    { "BAMUM",          Dicf::TRIG_SCRIPT },
    { "BASSA",          Dicf::TRIG_SCRIPT_IF_FIRST }, // Music: bassa = low
      { "VAH",          Dicf::TRIG_SCRIPT },
    { "BATAK",          Dicf::TRIG_SCRIPT },
    { "BENGALI",        Dicf::TRIG_SCRIPT },
    { "VEDIC",          Dicf::CAP_TITLE | Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE },
    { "BHAIKSUKI",      Dicf::TRIG_SCRIPT },
    { "BOPOMOFO",       Dicf::TRIG_SCRIPT },
    { "BRAHMI",         Dicf::TRIG_SCRIPT },
    { "BRAILLE",        Dicf::TRIG_SCRIPT },
    { "BUGINESE",       Dicf::TRIG_SCRIPT },
    { "BUHID",          Dicf::TRIG_SCRIPT },
    { "CANADIAN",       Dicf::TRIG_SCRIPT },
    { "CARIAN",         Dicf::TRIG_SCRIPT },
    { "CHAKMA",         Dicf::TRIG_SCRIPT },
    { "CHAM",           Dicf::TRIG_SCRIPT },
    { "CHEROKEE",       Dicf::TRIG_SCRIPT },
    { "CHINESE",        Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE },
    { "CHORASMIAN",     Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE },
    { "CIRTH",          Dicf::TRIG_SCRIPT },
    { "CJK",            Dicf::CAP_ALL | Dicf::TRIG_SCRIPT },
    { "COPTIC",         Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE },
    { "CUNEIFORM",      Dicf::TRIG_SCRIPT | Dicf::CAP_SMALL },
    { "CYPRO-MINOAN",   { Dicf::TRIG_SCRIPT, "Cypro-Minoan"sv } },
    { "CYPRIOT",        Dicf::TRIG_SCRIPT },
    { "CYRILLIC",       Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE },
      { "KOMI"sv,       Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE },
      { "UKRAINIAN",    Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE },
      { "BYELORUSSIAN-UKRAINIAN", { Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE, "Byelorussian-Ukrainian"sv } },
      { "BASHKIR",      Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE },
    { "DEVANAGARI",     Dicf::TRIG_SCRIPT },
    { "DOGRA",          Dicf::TRIG_SCRIPT },
    { "DESERET",        Dicf::TRIG_SCRIPT },
    { "DUPLOYAN",       Dicf::TRIG_SCRIPT },
    { "DIVES",          Dicf::TRIG_SCRIPT },
     { "AKURU",         Dicf::TRIG_SCRIPT },
    { "EGYPTIAN",       Dicf::TRIG_SCRIPT },
    { "ELBASAN",        Dicf::TRIG_SCRIPT },
    { "ELYMAIC",        Dicf::TRIG_SCRIPT },
    { "ETHIOPIC",       Dicf::TRIG_SCRIPT },
      { "GURAGE",       Dicf::TRIG_SCRIPT | Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "KHUTSURI",       Dicf::TRIG_SCRIPT },    // Unused, but…
    { "FRAKTUR",        Dicf::TRIG_SCRIPT },
    { "GARAY",          Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE },
    { "GEORGIAN",       Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE },
     { "MTAVRULI",      Dicf::TRIG_SCRIPT },
    { "GONDI",          Dicf::TRIG_SCRIPT },
      { "GUNJALA",      Dicf::TRIG_SCRIPT },
      { "MASARAM",      Dicf::TRIG_SCRIPT },
    { "GOTHIC",         Dicf::TRIG_SCRIPT },
    { "GLAGOLITIC",     Dicf::TRIG_SCRIPT },
    { "GRANTHA",        Dicf::TRIG_SCRIPT },
    { "GREEK",          Dicf::TRIG_SCRIPT_IF_FIRST | Dicf::PART_ADJECTIVE | Dicf::CAP_TITLE },
      { "ATTIC",          Dicf::TRIG_SCRIPT },  // NOT adjective → we use treasure of ignorance here
      { "CARYSTIAN",      Dicf::TRIG_SCRIPT },
      { "CYRENAIC",       Dicf::TRIG_SCRIPT },
      { "DEPLHIC",        Dicf::TRIG_SCRIPT },
      { "EPIDAUREAN",     Dicf::TRIG_SCRIPT },
      { "HERAEUM",        Dicf::TRIG_SCRIPT },
      { "HERMIONIAN",     Dicf::TRIG_SCRIPT },
      { "MESSENIAN",      Dicf::TRIG_SCRIPT },
      { "NAXIAN",         Dicf::TRIG_SCRIPT },
      { "PAMPHYLIAN",     Dicf::TRIG_SCRIPT },
      { "STRATIAN",       Dicf::TRIG_SCRIPT },
      { "THESPIAN",       Dicf::TRIG_SCRIPT },
      { "TROEZENIAN",     Dicf::TRIG_SCRIPT },
    { "GUJARATI",       Dicf::TRIG_SCRIPT },
    { "GURMUKHI",       Dicf::TRIG_SCRIPT },
    { "GURUNG",         Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE },
      { "KHEMA",        Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE },
    { "HANGUL",         Dicf::TRIG_SCRIPT },
    { "HANIFI",         Dicf::TRIG_SCRIPT },
     { "ROHINGYA",      Dicf::TRIG_SCRIPT },
    { "HANGZHOU",       Dicf::TRIG_SCRIPT },
    { "HANUNOO",        Dicf::TRIG_SCRIPT },
    { "HATRAN",         Dicf::TRIG_SCRIPT },
    { "HEBREW",         Dicf::TRIG_SCRIPT },
    { "HIRAGANA",       Dicf::TRIG_SCRIPT },
    { "HENTAIGANA",     Dicf::TRIG_SCRIPT },
    { "HUNGARIAN",      Dicf::TRIG_SCRIPT },
    { "INDIC",          Dicf::TRIG_SCRIPT },
    { "INDUS",          Dicf::TRIG_SCRIPT },
    { "JAPANESE",       Dicf::TRIG_SCRIPT },
    { "JAVANESE",       Dicf::TRIG_SCRIPT },
    { "JUDEO-SPANISH",  { Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE, "Judeo-Spanish" } },
    { "KAKTOVIK",       Dicf::TRIG_SCRIPT },
    { "KANGXI",         Dicf::TRIG_SCRIPT },
    { "KANNADA",        Dicf::TRIG_SCRIPT },
    { "KATAKANA",       Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE },
    { "KATAKANA-HIRAGANA", { Dicf::TRIG_SCRIPT, "Katakana-Hiragana"sv } },
    { "KAWI",           Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE },
    { "KAYAH",          { Dicf::TRIG_TWO_WORD_SCRIPT | Dicf::PART_ADJECTIVE | Dicf::CAP_TITLE, {}, "LI"sv } },
    { "KHAROSHTHI",     Dicf::TRIG_SCRIPT },
    { "KHMER",          Dicf::TRIG_SCRIPT },
    { "KHOJKI",         Dicf::TRIG_SCRIPT },
    { "KHITAN",         Dicf::TRIG_SCRIPT },
    { "KHUDAWADI",      Dicf::TRIG_SCRIPT },
    { "KIRAT",          Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE },
      { "RAI",          Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE },
    { "KLINGON",        Dicf::TRIG_SCRIPT },
    { "KOREAN",         Dicf::TRIG_SCRIPT },
    { "KPELLE",         Dicf::TRIG_SCRIPT },
    { "KAITHI",         Dicf::TRIG_SCRIPT },
    { "LAO",            Dicf::TRIG_SCRIPT },
    { "LATIN",          Dicf::TRIG_SCRIPT },
    { "LEKE",           Dicf::TRIG_SCRIPT },
    { "LEPCHA",         Dicf::TRIG_SCRIPT },
    { "LIMBU",          Dicf::TRIG_SCRIPT },
    { "LINEAR",         { Dicf::TRIG_TWO_WORD_SCRIPT, {}, "A"sv, "B"sv } },
    { "LISU",           Dicf::TRIG_SCRIPT },
    { "LOMA",           Dicf::TRIG_SCRIPT },
    { "LYCIAN",         Dicf::TRIG_SCRIPT },
    { "LYDIAN",         Dicf::TRIG_SCRIPT },
    { "MAHAJANI",       Dicf::TRIG_SCRIPT },
    { "MAKASAR",        Dicf::TRIG_SCRIPT },
    { "MALAYALAM",      Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE },
    { "MALTESE",        Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE },
    { "MANDAIC",        Dicf::TRIG_SCRIPT },
    { "MANDAEAN",       Dicf::TRIG_SCRIPT },
    { "MANICHAEAN",     Dicf::TRIG_SCRIPT },
    { "MARCHEN",        Dicf::TRIG_SCRIPT },
    { "MAYAN",          Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE },
    { "MEDEFAIDRIN",    Dicf::TRIG_SCRIPT },
    { "MEETEI",         Dicf::TRIG_SCRIPT },
      { "MAYEK",        Dicf::TRIG_SCRIPT },
    { "MENDE",          Dicf::TRIG_SCRIPT | Dicf::TRIG_DIGIT_PROPAGATES_RIGHT },
      { "KIKAKUI",      Dicf::TRIG_SCRIPT | Dicf::TRIG_DIGIT_PROPAGATES_RIGHT },
    { "MEROITIC",       Dicf::TRIG_SCRIPT },
    { "MIAO",           Dicf::TRIG_SCRIPT },
    { "MIDDLE-WELSH",   { Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE, "Middle-Welsh" } },
    { "MODI",           Dicf::TRIG_SCRIPT },
    { "MONGOLIAN",      Dicf::TRIG_SCRIPT },
    { "MRO",            Dicf::TRIG_SCRIPT },
    { "MULTANI",        Dicf::TRIG_SCRIPT },
    { "MYANMAR",        Dicf::TRIG_SCRIPT },
    { "NABATAEAN",      Dicf::TRIG_SCRIPT },
    { "NANDINAGARI",    Dicf::TRIG_SCRIPT },
    { "NEWA",           Dicf::TRIG_SCRIPT },
    { "NKO",            { Dicf::TRIG_SCRIPT, "NKo" } },
    { "NUBIAN",         Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "NUSHU",          Dicf::TRIG_SCRIPT },
    { "OGHAM",          Dicf::TRIG_SCRIPT },
    { "OL",             { Dicf::TRIG_TWO_WORD_SCRIPT, {}, "CHIKI"sv, "ONAL"sv } },
    { "OTTOMAN",        Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE },
    { "TURKIC",         Dicf::TRIG_SCRIPT },
    { "ORIYA",          Dicf::TRIG_SCRIPT },
    { "OSAGE",          Dicf::TRIG_SCRIPT },
    { "OSMANYA",        Dicf::TRIG_SCRIPT },
    { "PAHAWH",         Dicf::TRIG_SCRIPT },
      { "HMONG",          Dicf::TRIG_SCRIPT },
      { "NYIAKENG",      Dicf::TRIG_SCRIPT },
      { "PUACHUE",       Dicf::TRIG_SCRIPT },
    { "PAHLAVI",        Dicf::TRIG_SCRIPT },
    { "PALI",           Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE },
    { "PALMYRENE",      Dicf::TRIG_SCRIPT },
    { "PARTHIAN",       Dicf::TRIG_SCRIPT },
    { "PAU",            { Dicf::TRIG_THREE_WORD_SCRIPT, {}, "CIN"sv, "HAU"sv } },
    { "PERSIAN",        Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE },
    { "PHAISTOS",       { Dicf::TRIG_TWO_WORD_SCRIPT, {}, "DISC" } },
    { "PHAGS-PA",       Dicf::TRIG_SCRIPT },
    { "PHOENICIAN",     Dicf::TRIG_SCRIPT },
    { "PHILIPPINE",     Dicf::TRIG_SCRIPT },
    { "REJANG",         Dicf::TRIG_SCRIPT },
    { "ROMAN",          Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE },
    { "RONGORONGO",     Dicf::TRIG_SCRIPT },
    { "RUNIC",          Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE | Dicf::CAP_SMALL },
    { "SAMARITAN",      Dicf::TRIG_SCRIPT },
    { "SANSKRIT",       Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE },
    { "SARATI",         Dicf::TRIG_SCRIPT },
    { "SAURASHTRA",     Dicf::TRIG_SCRIPT },
    { "SCOTS",          Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE | Dicf::CAP_TITLE },
    { "SHAVIAN",        Dicf::TRIG_SCRIPT },
    { "SHARADA",        Dicf::TRIG_SCRIPT },
    { "SHUISHU",        Dicf::TRIG_SCRIPT },
    { "SIDDHAM",        Dicf::TRIG_SCRIPT },
    { "SIGNWRITING",    { Dicf::PART_NOUN | Dicf::TRIG_SCRIPT, "SignWriting" } },
    { "SINHALA",        Dicf::TRIG_SCRIPT },
    { "SIYAQ",          Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE },
    { "SORA",           { Dicf::TRIG_TWO_WORD_SCRIPT, {}, "SOMPENG"sv } },
    { "SOYOMBO",        Dicf::TRIG_SCRIPT },
    { "SUNDANESE",      Dicf::TRIG_SCRIPT },
    { "SUNUWAR",        Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE },
    { "SYLOTI",         Dicf::TRIG_SCRIPT },
    { "NAGRI",          Dicf::TRIG_SCRIPT },
    { "SOGDIAN",        Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE },
    { "SYRIAC",         Dicf::TRIG_SCRIPT },
    { "TAGBANWA",       Dicf::TRIG_SCRIPT },
    { "TAKRI",          Dicf::TRIG_SCRIPT },
    { "TAMIL",          Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE },
    { "TANGSA",         Dicf::TRIG_SCRIPT },
    { "TANGUT",         Dicf::TRIG_SCRIPT },
    { "TELUGU",         Dicf::TRIG_SCRIPT },
    { "TENGWAR",        Dicf::TRIG_SCRIPT },
    { "TIFINAGH",       Dicf::TRIG_SCRIPT },
     { "AHAGGAR",       Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE },
     { "TUAREG",        Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE },
     { "TAWELLEMET",    Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE },
     { "AYER",          Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE },
    { "TAGALOG",        Dicf::TRIG_SCRIPT },
    { "THAANA",         Dicf::TRIG_SCRIPT },
    { "THAI",           Dicf::TRIG_SCRIPT },
    { "TIBETAN",        Dicf::TRIG_SCRIPT },
    { "TIRHUTA",        Dicf::TRIG_SCRIPT },
    { "TODHRI",         Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE },
    { "TOTO",           Dicf::TRIG_SCRIPT },
    { "TULU-TIGALARI",  { Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE, "Tulu-Tigalari" } },
    { "UIGHUR",         Dicf::TRIG_SCRIPT | Dicf::CAP_TITLE },
    { "UYGHUR",         Dicf::TRIG_SCRIPT | Dicf::CAP_TITLE },
    { "UGARITIC",       Dicf::TRIG_SCRIPT },
    { "VAI",            Dicf::TRIG_SCRIPT },
    { "VIETNAMESE",     Dicf::TRIG_SCRIPT | Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "VITHKUQI",       Dicf::TRIG_SCRIPT },
    { "WARANG",         { Dicf::TRIG_TWO_WORD_SCRIPT, {}, "CITI"sv } },
    { "WANCHO",         Dicf::TRIG_SCRIPT },
    { "YEZIDI"sv,       Dicf::TRIG_SCRIPT },
    { "YI"sv,           Dicf::TRIG_SCRIPT_IF_FIRST },
    { "ZANABAZAR",      Dicf::TRIG_SCRIPT },
    { "ZNAMENNY",       Dicf::TRIG_SCRIPT },

    { "OCR",            Dicf::CAP_ALL },
    { "MICR",           Dicf::CAP_ALL },
    { "ARIB",           Dicf::CAP_ALL },
    { "STD",            Dicf::CAP_ALL },
    { "B24",            Dicf::CAP_ALL },
    { "MIDI",           Dicf::CAP_ALL },
    { "ATM",            Dicf::CAP_ALL },
    { "ROFL",           Dicf::CAP_ALL },
    { "ROTFL",          Dicf::CAP_ALL },
    { "HDTV",           Dicf::CAP_ALL },
    { "SDTV",           Dicf::CAP_ALL },
    { "UFO",            Dicf::CAP_ALL },

    // Special adjectives
    { "EASTERN",        { Dicf::TRIG_TWO_WORD_SCRIPT | Dicf::PART_ADJECTIVE,
                          {}, "PWO" } },
    // There are several NAGs → it’s script only with Mundari
    { "NAG",            { Dicf::TRIG_TWO_WORD_SCRIPT | Dicf::PART_ADJECTIVE,
                          {}, "MUNDARI" } },
    // Interesting case: ITALIC is script with OLD only
    // For ARABIAN we can use just SCRIPT_ADJECTIVE, it’s enough to identify script
    { "OLD",            { Dicf::PART_ADJECTIVE | Dicf::TRIG_TWO_WORD_SCRIPT | Dicf::TRIG_SCRIPT_ADJECTIVE,
                          {}, "ITALIC", "PERMIC", "PERSIAN" } },
    { "TAI",            { Dicf::TRIG_TWO_WORD_SCRIPT, {}, "LE"sv, "VIET"sv, "THAM"sv, "LAING"sv } },
      { "LAING",        Dicf::IF_SCRIPT | Dicf::TRIG_CAP_NEXT | Dicf::IF_NEXT_NOT_NOUN },

    // Capitalize next
    { "AFFIX",          Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
    { "ACROPHONIC",     Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
    { "CHARACTER",      Dicf::IF_SCRIPT | Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
    { "CHOSEONG",       Dicf::IF_SCRIPT | Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
      { "JUNGSEONG",    Dicf::IF_SCRIPT | Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
      { "JONGSEONG",    Dicf::IF_SCRIPT | Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
    { "DENOMINATOR",    Dicf::PART_NOUN | Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
    { "DIGIT",          Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL  },
    { "FRACTION",       Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
    { "HIEROGLYPH",     Dicf::IF_SCRIPT | Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
    { "IDEOGRAM",       Dicf::IF_SCRIPT | Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
    { "LETTER",         Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
    { "DIGRAPH",        Dicf::TRIG_CAP_NEXT | Dicf::PART_NOUN | Dicf::CAP_SMALL },
    { "LIGATURE",       Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
    { "LOGICAL",        Dicf::TRIG_CAP_NEXT },
    { "LOGOGRAM",       Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
    { "MARK",           Dicf::PART_NOUN | Dicf::IF_SCRIPT | Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
    { "MODIFIER",       Dicf::TRIG_CAP_NEXT | Dicf::PART_NOUN | Dicf::CAP_SMALL },
    { "NEUME",          Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
    { "NUMBER",         Dicf::PART_NOUN | Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
    { "NUMERAL",        Dicf::IF_SCRIPT | Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
    { "NUMERATOR",      Dicf::PART_NOUN | Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
    { "PADA",           Dicf::IF_SCRIPT | Dicf::PART_NOUN | Dicf::TRIG_CAP_NEXT },
    { "PATTERN",        Dicf::IF_SCRIPT | Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
    { "PUNCTUATION",    Dicf::IF_SCRIPT | Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
    { "RADICAL",        Dicf::IF_SCRIPT | Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
    { "SIGN",           Dicf::PART_NOUN | Dicf::IF_SCRIPT | Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
    { "SYMBOL",         Dicf::PART_NOUN | Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
    { "SYLLABICS",      Dicf::IF_SCRIPT | Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
    { "SYLLABLE",       Dicf::IF_SCRIPT | Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
    { "VOWEL",          Dicf::IF_SCRIPT | Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },

    // Mixed
    { "TONE",           Dicf::PART_MIXED | Dicf::TRIG_CAP_NEXT | Dicf::IF_SCRIPT | Dicf::IF_NEXT_NOT_NOUN },
    { "REPETITION",     Dicf::PART_MIXED | Dicf::TRIG_CAP_NEXT | Dicf::IF_SCRIPT | Dicf::IF_NEXT_NOT_NOUN },
    { "WORD",           Dicf::PART_MIXED | Dicf::TRIG_CAP_NEXT | Dicf::IF_SCRIPT | Dicf::IF_NEXT_NOT_NOUN },

    // American tribes
    { "AIVILIK",        Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "ATHAPASCAN",     Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "BLACKFOOT",      Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
      // Who knows what to do with Carrier (Canadian tribe)
    { "CARRIER",        Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE | Dicf::TRIG_SCRIPT },
      // CREE here
      { "BIBLE-CREE",   { Dicf::PART_ADJECTIVE, "Bible-Cree" } },
      { "MOOSE-CREE",   { Dicf::PART_ADJECTIVE, "Moose-Cree" } },
      { "N-CREE",       { Dicf::PART_ADJECTIVE, "N-Cree" } },
      { "R-CREE",       { Dicf::PART_ADJECTIVE, "R-Cree" } },
      { "TH-CREE",      { Dicf::PART_ADJECTIVE, "Th-Cree" } },
      { "WEST-CREE",    { Dicf::PART_ADJECTIVE, "West-Cree" } },
      { "WOODS-CREE",   { Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE, "Woods-Cree" } },
      { "Y-CREE",       { Dicf::PART_ADJECTIVE, "Y-Cree" } },
    { "DENE",           Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE | Dicf::TRIG_SCRIPT },
    { "NASKAPI",        Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "NATTILIK",       Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "NUNAVIK",        Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "OJIBWAY",        Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "SAYISI",         Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "SOUTH-SLAVEY",   { Dicf::PART_ADJECTIVE, "South-Slavey" } },

    // Myanmar nations
    { "AITON",          Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "KAREN",          Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
      // KAYAH (above)
    { "KHAMTI",         Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "MON",            Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "PALAUNG",        Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "PAO",            Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "PWO",            Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "RUMAI",          Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "SGAW",           Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "SHAN",           Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
      { "COUNCIL",      Dicf::CAP_TITLE | Dicf::PART_MIXED },
    { "SHWE",           Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },

    // Mongolian nations/alphabets
    { "ALI",            Dicf::PART_ADJECTIVE },
    { "GALI",           Dicf::CAP_SMALL | Dicf::TRIG_CAP_NEXT },
    { "MANCHU",         Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "SIBE",           Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "TODO",           Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },

    // Indonesian nations
    { "KARO",           Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "MANDAILING",     Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "PAKPAK",         Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "SIMALUNGUN",     Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },

    // Other nations
    { "AEGEAN",         Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "AFRICAN",        Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "AKHMIMIC",       Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "AMERICAN",       Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "ASSYRIAN",       Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "BOHAIRIC",       Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "BYZANTINE",      Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "CORNISH",        Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "ELAMITE",        Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "EUROPEAN",       Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "FRENCH",         Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "GERMAN",         Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "HARKLEAN",       Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "INDIAN",         Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "JAIN",           Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "KASHMIRI",       Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "KAZAKH",         Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "KHAKASSIAN",     Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "KIRGHIZ",        Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "NORDIC",         Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "ORKHON",         Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "RUSSIAN",        Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "SAKHA",          Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "SEBATBEIT",      Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "SLAVONIC",       Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "TURKISH",        Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "VULCAN",         Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },

    // Nouns
    { "ANUSVARA",       Dicf::PART_NOUN },
    { "APOSTROPHE",     Dicf::PART_NOUN },
    { "ARDHAVISARGA",   Dicf::PART_NOUN },
    { "ASPIRATION",     Dicf::PART_NOUN },
    { "AXE",            Dicf::PART_NOUN },  // For Phaistos Disc
    { "ARROW",          Dicf::PART_NOUN },
    { "ARROWHEAD",      Dicf::PART_NOUN },
    { "BALUDA",         Dicf::PART_NOUN },
    { "BAR",            Dicf::PART_NOUN },
    { "BARB",           Dicf::PART_NOUN },
    { "BEAT",           Dicf::PART_NOUN },
    { "BHALE",          Dicf::PART_NOUN },
    { "BIRD",           Dicf::PART_NOUN },  // For some CJK hiero
    { "BOX",            Dicf::PART_NOUN },
    { "BRDA",           Dicf::PART_NOUN },
    { "CANDRABINDU",    Dicf::PART_NOUN },
    { "CARET",          Dicf::PART_NOUN },  // For APL not really, but OK
    { "CIRCLE",         Dicf::PART_NOUN },
    { "CLICK",          Dicf::PART_NOUN },
    { "COLON",          Dicf::PART_NOUN },
     { "TRICOLON",      Dicf::PART_NOUN },
     { "QUADCOLON",     Dicf::PART_NOUN },
    { "COMMA",          Dicf::PART_NOUN },
    { "CROSS",          Dicf::PART_NOUN },
    { "DAMARU",         Dicf::PART_NOUN },
    { "DANDA",          Dicf::PART_NOUN },
    { "DIVIDER",        Dicf::PART_NOUN },
    { "DNA",            Dicf::PART_NOUN | Dicf::CAP_ALL },
    { "DOT",            Dicf::PART_NOUN },
    { "DOTS",           Dicf::PART_NOUN },
    { "DVD",            Dicf::PART_NOUN | Dicf::CAP_ALL },
    { "FORM",           Dicf::PART_NOUN },
    { "HAMZA",          Dicf::PART_NOUN },  // Arabic, not really letter
    { "HASANTA",        Dicf::PART_NOUN },
    { "HOOK",           Dicf::PART_NOUN },
    { "HYPHEN",         Dicf::PART_NOUN },
    { "LINE",           Dicf::PART_NOUN },
    { "MACRON",         Dicf::PART_NOUN },
    { "MCHU",           Dicf::PART_NOUN },
    { "MUCAAD",         Dicf::PART_NOUN },
    { "NUKTA",          Dicf::PART_NOUN },
    { "PLUS",           Dicf::PART_NOUN },
    { "PRIME",          Dicf::PART_NOUN },
    { "RANA",           Dicf::PART_NOUN },
    { "REDUPLICATION",  Dicf::PART_NOUN },
    { "RIKRIK",         Dicf::PART_NOUN },
    { "RING",           Dicf::PART_NOUN },
    { "SECANT",         Dicf::PART_NOUN },
    { "SECTION",        Dicf::PART_NOUN },
    { "SEPARATOR",      Dicf::PART_NOUN },
    { "SHAD",           Dicf::PART_NOUN },
    { "SHELF",          Dicf::PART_NOUN },
    { "SHOE",           Dicf::PART_NOUN },  // For APL not really, but OK
    { "SHRII",          Dicf::PART_NOUN | Dicf::CAP_TITLE }, // That’s for Indic, near miss but OK in Cans
    { "SPIRAL",         Dicf::PART_NOUN },
    { "SPIRANT",        Dicf::PART_NOUN },  // One time, probably OK
    { "STEM",           Dicf::PART_NOUN },
    { "STOP",           Dicf::PART_NOUN },
    { "STROKE",         Dicf::PART_NOUN },
    { "STROKES",        Dicf::PART_NOUN },
    { "SVARITA",        Dicf::PART_NOUN },
    { "T-REX",          Dicf::PART_NOUN | Dicf::CAP_TITLE },
    { "T-SHIRT",        Dicf::PART_NOUN | Dicf::CAP_TITLE },
    { "TACK",           Dicf::PART_NOUN },
    { "TREE",           Dicf::PART_NOUN },
    { "TSHEG",          Dicf::PART_NOUN },
    { "UBADAMA",        Dicf::PART_NOUN },
    { "UNDERBAR",       Dicf::PART_NOUN },
    { "VARIKA",         Dicf::PART_NOUN },
    { "VIRAMA",         Dicf::PART_NOUN },
    { "VISARGA",        Dicf::PART_NOUN },
    { "VOICING",        Dicf::PART_NOUN },
    { "WAVE",           Dicf::PART_NOUN },
    { "X-RAY",          Dicf::PART_NOUN | Dicf::CAP_TITLE },
    { "X-X",            Dicf::PART_NOUN | Dicf::CAP_ALL },
    { "YANG",           Dicf::PART_MIXED },  // for Chinese tones, need more things
    //{ "YAT",            Dicf::PART_NOUN }, // Yat is normal (Capitalized)…
    { "YIG",            Dicf::PART_NOUN },
    { "YIN",            Dicf::PART_NOUN },   // for Chinese tones, seems to be OK
    { "YUS",            Dicf::PART_NOUN },   // …but yus is big & little → do not cap

    // Greek
#define GREK(x) { x, Dicf::PART_NOUN | Dicf::CAP_TITLE },
    GREK("ALPHA")
    GREK("BETA")
    GREK("GAMMA")
      GREK("DIGAMMA")
    GREK("DELTA")
    GREK("EPSILON")
    GREK("ZETA")
    GREK("ETA")
    GREK("THETA")
    GREK("IOTA")
    GREK("KAPPA")
    GREK("LAMDA")
    GREK("LAMBDA")
    GREK("MU")
    GREK("NU")
    GREK("XI")
    GREK("OMICRON")
    GREK("PI")
    GREK("RHO")
    GREK("THETA")
    GREK("SIGMA")
    GREK("TAU")
    GREK("UPSILON")
    GREK("PHI")
    GREK("CHI")
    GREK("PSI")
    GREK("OMEGA")
    GREK("NABLA")
#undef GREK

    // Tricky but work as noun
    { "PERCUSSIVE",     Dicf::PART_NOUN },

    // Adverbs
    { "ABOVE",          Dicf::PART_NOUN },
    { "BELOW",          Dicf::PART_NOUN },

    // Control abbreviations
    { "(FF)",           Dicf::CAP_ALL },
    { "(LF)",           Dicf::CAP_ALL },
    { "(CR)",           Dicf::CAP_ALL },

    // Numerals
    { "ONE",            Dicf::PART_NUMERAL },
    { "TWO",            Dicf::PART_NUMERAL },
    { "THREE",          Dicf::PART_NUMERAL },
    { "FOUR",           Dicf::PART_NUMERAL },
    { "FIVE",           Dicf::PART_NUMERAL },
    { "SIX",            Dicf::PART_NUMERAL },
    { "SEVEN",          Dicf::PART_NUMERAL },
    { "EIGHT",          Dicf::PART_NUMERAL },
    { "NINE",           Dicf::PART_NUMERAL },
    { "TEN",            Dicf::PART_NUMERAL },
    { "ELEVEN",         Dicf::PART_NUMERAL },
    { "TWELVE",         Dicf::PART_NUMERAL },
    { "FORTY",          Dicf::PART_NUMERAL },
    { "FIFTY",          Dicf::PART_NUMERAL },

    // Numeric tones are nouns
    { "TONE-1",         Dicf::PART_NOUN },
    { "TONE-2",         Dicf::PART_NOUN },
    { "TONE-3",         Dicf::PART_NOUN },
    { "TONE-4",         Dicf::PART_NOUN },
    { "TONE-5",         Dicf::PART_NOUN },
    { "TONE-6",         Dicf::PART_NOUN },
    { "TONE-7",         Dicf::PART_NOUN },
    { "TONE-8",         Dicf::PART_NOUN },
    { "TONE-9",         Dicf::PART_NOUN },
    { "TONE-B",         Dicf::PART_NOUN },
    { "TONE-D",         Dicf::PART_NOUN },
    { "TONE-G",         Dicf::PART_NOUN },
    { "TONE-J",         Dicf::PART_NOUN },
    { "TONE-M",         Dicf::PART_NOUN },
    { "TONE-S",         Dicf::PART_NOUN },

    // Numeric marks are nouns
    { "MARK-1",         Dicf::PART_NOUN },
    { "MARK-2",         Dicf::PART_NOUN },
    { "MARK-3",         Dicf::PART_NOUN },
    { "MARK-4",         Dicf::PART_NOUN },
    { "MARK-5",         Dicf::PART_NOUN },
    { "MARK-6",         Dicf::PART_NOUN },
    { "MARK-7",         Dicf::PART_NOUN },
    { "MARK-8",         Dicf::PART_NOUN },
    { "MARK-9",         Dicf::PART_NOUN },

    // Musical nouns
    { "ACCENT",         Dicf::PART_NOUN },
    { "ACCENT-STACCATO", Dicf::PART_NOUN },
    { "BARLINE",        Dicf::PART_NOUN },
    { "BOW",            Dicf::PART_NOUN },
    { "DOIT",           Dicf::PART_NOUN },
    { "FLAG-1",         Dicf::PART_NOUN },
    { "FLAG-2",         Dicf::PART_NOUN },
    { "FLAG-3",         Dicf::PART_NOUN },
    { "FLAG-4",         Dicf::PART_NOUN },
    { "FLAG-5",         Dicf::PART_NOUN },
    { "FLIP",           Dicf::PART_NOUN },
    { "HARMONIC",       Dicf::PART_NOUN },      // OK, music only
    { "LOURE",          Dicf::PART_NOUN },
    { "MARCATO",        Dicf::PART_NOUN },
    { "MARCATO-STACCATO", Dicf::PART_NOUN },
    { "NOTE",           Dicf::PART_NOUN },
    { "PIZZICATO",      Dicf::PART_NOUN },
    { "REST",           Dicf::PART_NOUN },
    { "RIP",            Dicf::PART_NOUN },
    { "STACCATISSIMO",  Dicf::PART_NOUN },
    { "STACCATO",       Dicf::PART_NOUN },
    { "TENUTO",         Dicf::PART_NOUN },
    { "TONGUE",         Dicf::PART_NOUN },      // OK, never used as adjective
    { "TREMOLO-1",      Dicf::PART_NOUN },
    { "TREMOLO-2",      Dicf::PART_NOUN },
    { "TREMOLO-3",      Dicf::PART_NOUN },
    { "TREMOLO-4",      Dicf::PART_NOUN },
    { "TREMOLO-5",      Dicf::PART_NOUN },
    { "TURN",           Dicf::PART_NOUN },

    // Adjectives
    { "ACUTE",          Dicf::PART_ADJECTIVE },
    { "ALTERNATE",      Dicf::PART_ADJECTIVE },
    { "ALTERNATIVE",    Dicf::PART_ADJECTIVE },
    { "ALVEOLAR",       Dicf::PART_ADJECTIVE },
    { "ARCHAIC",        Dicf::PART_ADJECTIVE },
    { "ARKTIKO",        Dicf::PART_ADJECTIVE },
    { "ASPIRATED",      Dicf::PART_ADJECTIVE },
    { "ATACHED",        Dicf::PART_ADJECTIVE },
    { "BARRED",         Dicf::PART_ADJECTIVE },
    { "BIG",            Dicf::PART_ADJECTIVE },
    { "BIDENTAL",       Dicf::PART_ADJECTIVE },
    { "BILABIAL",       Dicf::PART_ADJECTIVE },
    { "BREATHY",        Dicf::PART_ADJECTIVE },
    { "BOOK",           Dicf::PART_ADJECTIVE },
    { "BOTTOM",         Dicf::PART_ADJECTIVE },
    { "CAPITAL",        Dicf::PART_ADJECTIVE },
    { "CASKET",         Dicf::PART_ADJECTIVE }, // OK here, runes
    { "CHILLU",         Dicf::PART_ADJECTIVE }, // used for Malayalam
    { "CIRCLED",        Dicf::PART_ADJECTIVE | Dicf::CAP_SMALL },
    { "CLOSED",         Dicf::PART_ADJECTIVE },
    { "CLOSING",        Dicf::PART_ADJECTIVE },
    { "COMBINING",      Dicf::PART_ADJECTIVE },
    { "CROSSED",        Dicf::PART_ADJECTIVE },
    { "CRYPTOGRAMMIC",  Dicf::PART_ADJECTIVE },
    { "CURLED",         Dicf::PART_ADJECTIVE },
    { "DENTAL",         Dicf::PART_ADJECTIVE },
    { "DIGRAMMA",       Dicf::PART_ADJECTIVE }, // Not really an adj, but works so
    { "DOUBLE",         Dicf::PART_ADJECTIVE },
    { "DOUBLE-STRUCK",  Dicf::PART_ADJECTIVE },
    { "DOTTED",         Dicf::PART_ADJECTIVE },
    { "DOTLESS",        Dicf::PART_ADJECTIVE },
    { "DOWN",           Dicf::PART_ADJECTIVE },
    { "DIAERESIZED",    Dicf::PART_ADJECTIVE },
    { "DIAGONAL",       Dicf::PART_ADJECTIVE },
    { "DIALECT-P",      { Dicf::PART_ADJECTIVE, "dialect-P" } },
    { "EGYPTOLOGICAL",  Dicf::PART_ADJECTIVE | Dicf::CAP_TITLE },
    { "EPENTHETIC",     Dicf::PART_ADJECTIVE },
    { "EXCLAMATION",    Dicf::PART_ADJECTIVE },
    { "FINGERED",       Dicf::PART_ADJECTIVE },
    { "FIVE-LIKE",      Dicf::PART_ADJECTIVE },
    { "FIXED-FORM",     Dicf::PART_ADJECTIVE },
    { "FINAL",          Dicf::PART_ADJECTIVE },
    { "FLAT",           Dicf::PART_ADJECTIVE },
    { "FLATTENED",      Dicf::PART_ADJECTIVE },
    { "FRICATIVE",      Dicf::PART_ADJECTIVE },
    { "GLOTTAL",        Dicf::PART_ADJECTIVE },
    { "GRAVE",          Dicf::PART_ADJECTIVE },
    { "GREAT",          Dicf::PART_ADJECTIVE },
    { "HALF",           Dicf::PART_ADJECTIVE },
    { "HARD",           Dicf::PART_ADJECTIVE },
    { "HEAVY",          Dicf::PART_ADJECTIVE },
    { "HIGH",           Dicf::PART_ADJECTIVE },
    { "HORIZONTAL",     Dicf::PART_ADJECTIVE },
    { "IOTATED",        Dicf::PART_ADJECTIVE },
    { "IOTIFIED",       Dicf::PART_ADJECTIVE },
    { "INHERENT",       Dicf::PART_ADJECTIVE },
    { "INITIAL",        Dicf::PART_ADJECTIVE },
    { "INSCRIPTIONAL",  Dicf::PART_ADJECTIVE | Dicf::TRIG_SCRIPT_ADJECTIVE },
    { "INSULAR",        Dicf::PART_ADJECTIVE },
    { "INVERTED",       Dicf::PART_ADJECTIVE },
    { "JONA",           Dicf::PART_ADJECTIVE },     // Nkoo, probably small
    { "KINNA",          Dicf::PART_ADJECTIVE },     // Rohg, kinna = small
    { "LARYNGEAL",      Dicf::PART_ADJECTIVE },
    { "LATERAL",        Dicf::PART_ADJECTIVE },
    { "LATINATE",       Dicf::PART_ADJECTIVE },
    { "LAZY",           Dicf::PART_ADJECTIVE },
    { "LEFT",           Dicf::PART_ADJECTIVE },
    { "LEFT-STEM",      Dicf::PART_ADJECTIVE },
    { "LIGATING",       Dicf::PART_ADJECTIVE },
    { "LONG",           Dicf::PART_ADJECTIVE },
    { "LONG-LEGGED",    Dicf::PART_ADJECTIVE },
    { "LOOPED",         Dicf::PART_ADJECTIVE },
    { "LOST",           Dicf::PART_ADJECTIVE },
    { "LOW",            Dicf::PART_ADJECTIVE },
    { "LUNATE",         Dicf::PART_ADJECTIVE },
    { "LITTLE",         Dicf::PART_ADJECTIVE },
    { "MARWARI",        Dicf::PART_ADJECTIVE },
    { "MEDIAL",         Dicf::PART_ADJECTIVE },
    { "MID",            Dicf::PART_ADJECTIVE },
    { "MIDDLE",         Dicf::PART_ADJECTIVE },
    { "MONOCULAR",      Dicf::PART_ADJECTIVE },     // Cyrl
    { "MONOGRAPH",      Dicf::PART_ADJECTIVE },
    { "MULTIOCULAR",    Dicf::PART_ADJECTIVE },     // Cyrl
    { "NASAL",          Dicf::PART_ADJECTIVE },
    { "NDOLE",          Dicf::PART_ADJECTIVE | Dicf::CAP_TITLE },   // Vai
    { "NEUTRAL",        Dicf::PART_ADJECTIVE },
    { "NEW",            { Dicf::PART_ADJECTIVE | Dicf::TRIG_THREE_WORD_SCRIPT, {}, "TAI"sv, "LUE"sv, }},
    { "NINE-LIKE",      Dicf::PART_ADJECTIVE },
    // Old NORTH Arabian
    { "NORTH",          Dicf::PART_ADJECTIVE | Dicf::TRIG_SCRIPT_ADJECTIVE },
    { "NORTHERN",       Dicf::PART_ADJECTIVE },
    { "OBLIQUE",        Dicf::PART_ADJECTIVE },
    { "OPEN",           Dicf::PART_ADJECTIVE },
    { "OVERLONG",       Dicf::PART_ADJECTIVE },
    { "PALATAL",        Dicf::PART_ADJECTIVE },
    { "PHARYNGEAL",     Dicf::PART_ADJECTIVE },
    { "PHASE-A",        { Dicf::PART_ADJECTIVE, "phase-A" } },
    { "PHASE-B",        { Dicf::PART_ADJECTIVE, "phase-B" } },
    { "PHASE-C",        { Dicf::PART_ADJECTIVE, "phase-C" } },
    { "PHASE-D",        { Dicf::PART_ADJECTIVE, "phase-D" } },
    { "PHASE-E",        { Dicf::PART_ADJECTIVE, "phase-E" } },
    { "PHASE-F",        { Dicf::PART_ADJECTIVE, "phase-F" } },
    { "PSALTER",        Dicf::PART_ADJECTIVE | Dicf::TRIG_SCRIPT_ADJECTIVE },
    { "QUARTER",        Dicf::PART_ADJECTIVE }, // Not really OK, but works somehow
    { "REFORMED",       Dicf::PART_ADJECTIVE },
    { "REVERSE",        Dicf::PART_ADJECTIVE },
    { "REVERSED",       Dicf::PART_ADJECTIVE },
    { "RETROFLEX",      Dicf::PART_ADJECTIVE },
    { "RIGHT",          Dicf::PART_ADJECTIVE },
    { "RIGHT-STEM",     Dicf::PART_ADJECTIVE },
    { "ROMANIAN",       Dicf::PART_ADJECTIVE },
    { "ROTATED",        Dicf::PART_ADJECTIVE },
    { "ROUNDED",        Dicf::PART_ADJECTIVE },
    { "SARA",           Dicf::PART_ADJECTIVE }, // Used for Thai
    { "SCRIPT",         Dicf::PART_ADJECTIVE }, // “Khitan small script” OK
    { "SEMISOFT",       Dicf::PART_ADJECTIVE },
    { "SHARP",          Dicf::PART_ADJECTIVE },
    { "SHORT",          Dicf::PART_ADJECTIVE },
    /// @todo [decap] Arabic mark Sideways Noon gnunna — is it right?
    { "SIDEWAYS",       Dicf::PART_ADJECTIVE },
    { "SIGMOID",        Dicf::PART_ADJECTIVE },
    { "SLOAN",          Dicf::PART_ADJECTIVE },
    { "SMALL",          Dicf::PART_ADJECTIVE },
    { "SNAP",           Dicf::PART_ADJECTIVE }, // OK, music only
    { "SPACING",        Dicf::PART_ADJECTIVE },
    { "SPIDERY",        Dicf::PART_ADJECTIVE },
    { "SQUAT",          Dicf::PART_ADJECTIVE },
    { "SOFT",           Dicf::PART_ADJECTIVE },
    // Old SOUTH Arabian
    { "SOUTH",          Dicf::PART_ADJECTIVE | Dicf::TRIG_SCRIPT_ADJECTIVE  },
    { "SOUTHERN",       Dicf::PART_ADJECTIVE },
    { "SPRECHGESANG",   Dicf::PART_ADJECTIVE },
    { "STRAIGHT",       Dicf::PART_ADJECTIVE },
    { "STRETCHED",      Dicf::PART_ADJECTIVE },
    { "SUPERSCRIPT",    Dicf::PART_ADJECTIVE },
    { "SYLLABIC",       Dicf::PART_ADJECTIVE },
    { "TAILED",         Dicf::PART_ADJECTIVE },
    { "TAILLESS",       Dicf::PART_ADJECTIVE },
    { "TALL",           Dicf::PART_ADJECTIVE },
    { "THAM",           Dicf::PART_ADJECTIVE }, // not really correct, but let’s leave hora/tham lowcase
    { "THIRD-STAGE",    Dicf::PART_ADJECTIVE },
    { "THREE-D",        { Dicf::PART_ADJECTIVE, "three-D" } },
    { "THREE-LEGGED",   Dicf::PART_ADJECTIVE },
    { "TIGHT",          Dicf::PART_ADJECTIVE },
    { "TOP",            Dicf::PART_ADJECTIVE },
    { "TRIANGULAR",     Dicf::PART_ADJECTIVE },
    { "TRIPLE",         Dicf::PART_ADJECTIVE },
    { "TROKUTASTI",     Dicf::PART_ADJECTIVE },
    { "TRADITIONAL",    Dicf::PART_ADJECTIVE },
    { "TRUNCATED",      Dicf::PART_ADJECTIVE },
    { "TURNED",         Dicf::PART_ADJECTIVE },
    { "TWO-CIRCLE",     Dicf::PART_ADJECTIVE },
    { "UNBLENDED",      Dicf::PART_ADJECTIVE },
    { "UP",             Dicf::PART_ADJECTIVE },
    { "VERTICAL",       Dicf::PART_ADJECTIVE },
    { "VOCALIC",        Dicf::PART_ADJECTIVE },
    { "VOICED",         Dicf::PART_ADJECTIVE },
    { "VOICELESS",      Dicf::PART_ADJECTIVE },
    { "WESTERN",        Dicf::PART_ADJECTIVE },
    { "WIDE",           Dicf::PART_ADJECTIVE },

    // Starting with letter: sort by main word, then by letter
    { "N-ARY",          Dicf::PART_ADJECTIVE | Dicf::CAP_TITLE },
    { "L-SHAPED",       Dicf::PART_ADJECTIVE | Dicf::CAP_TITLE },
    { "S-SHAPED",       Dicf::PART_ADJECTIVE | Dicf::CAP_TITLE },
    { "U-SHAPED",       Dicf::PART_ADJECTIVE | Dicf::CAP_TITLE },
    { "H-TYPE",         Dicf::PART_ADJECTIVE | Dicf::CAP_TITLE },
    { "L-TYPE",         Dicf::PART_ADJECTIVE | Dicf::CAP_TITLE },

    // Sinhala
    { "ALPAPRAANA",     Dicf::PART_ADJECTIVE },
    { "AMBA",           Dicf::PART_ADJECTIVE },
    { "DANTAJA",        Dicf::PART_ADJECTIVE },
    { "DIGA",           Dicf::PART_ADJECTIVE },
    { "GAETTA-PILLA",   Dicf::PART_NOUN },
    { "GAYANUKITTA",    Dicf::PART_NOUN },
    { "KANTAJA",        Dicf::PART_ADJECTIVE },
    { "KETTI",          Dicf::PART_ADJECTIVE },
    { "KOMBUVA",        Dicf::PART_ADJECTIVE },
    { "LITH",           Dicf::PART_ADJECTIVE | Dicf::CAP_TITLE },
    { "MAHAAPRAANA",    Dicf::PART_ADJECTIVE },
    { "MUURDHAJA",      Dicf::PART_ADJECTIVE },
    { "SANYAKA",        Dicf::PART_ADJECTIVE },
    { "SANYOOGA",       Dicf::PART_ADJECTIVE },
    { "TAALUJA",        Dicf::PART_ADJECTIVE },

    // People’s names
    { "CELSIUS",        Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "CLAUS",          Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "DAVID",          Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "EULER",          Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "FAHRENHEIT",     Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "FERRIS",         Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "FRANKS",         Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "FITZPATRICK",    Dicf::CAP_TITLE },
    { "HERMITIAN",      Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "MARY",           Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "MX",             Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "PERNIN",         Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },   // Helen Pernin, author of shorthand
    { "PETRI",          Dicf::CAP_TITLE },
    { "PLANCK",         Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "TIRONIAN",       Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE | Dicf::TRIG_SCRIPT },
    { "WEIERSTRASS",    Dicf::CAP_TITLE | Dicf::PART_NOUN },

    // Myth/religion characters’ names
    { "AESCULAPIUS",    Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "ALLAAH",         Dicf::CAP_TITLE },
    { "ALLAAHU",        Dicf::CAP_TITLE },
    { "ALLAHOU",        Dicf::CAP_TITLE },
    { "HERMES",         Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "HYGIEIA",        Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "JOVE",           Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "SELENA",         Dicf::CAP_TITLE | Dicf::PART_NOUN },

    // Astronomy — 8 planets of Sol and Pluto
    { "JUPITER",        Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "SATURN",         Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "URANUS",         Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "NEPTUNE",        Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "PLUTO",          Dicf::CAP_TITLE | Dicf::PART_NOUN },

    // Astronomy — zodiac
    { "ARIES",          Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "TAURUS",         Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "GEMINI",         Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "LEO",            Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "VIRGO",          Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "LIBRA",          Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "SCORPIUS",       Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "SAGITTARIUS",    Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "CAPRICORN",      Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "AQUARIUS",       Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "PISCES",         Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "OPHIUCHUS",      Dicf::CAP_TITLE | Dicf::PART_NOUN },

    // Astronomy — rest
    { "ADMETOS",        Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "APOLLON",        Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "ASTRAEA",        Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "CERES",          Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "CHIRON",         Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "CUPIDO",         Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "ERIS",           Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "GONGGONG",        Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "HADES",          Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "HAUMEA",         Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "HYGIEA",         Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "JUNO",           Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "KRONOS",         Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "LILITH",         Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "MAKEMAKE",       Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "NESSUS",         Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "ORCUS",          Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "PALLAS",         Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "PHOLUS",         Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "POSEIDON",       Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "PROSERPINA",     Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "QUAOAR",         Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "SEDNA",          Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "TRANSPLUTO",     Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "VESTA",          Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "VULCANUS",       Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "ZEUS",           Dicf::CAP_TITLE | Dicf::PART_NOUN },

    // Other names
    { "ARTA",           Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "CHRISTMAS",      Dicf::CAP_TITLE },
    { "FARSI",          Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    // { "GARSHUNI",       Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE }, not really correct, but small is nicer
    { "HELLSCHREIBER",  Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "HINDU",          Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "JAPAN",          Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "JERUSALEM",      Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "KAABA",          Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "KORANIC",        Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "LORRAINE",       Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "ORTHODOX",       Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "RUMI",           Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "SHINTO",         Dicf::CAP_TITLE | Dicf::PART_MIXED },
    { "VOLAPUK",        Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },   // OK here
    { "YENISEI",        Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },   // OK here

    // Months
    { "JANUARY",        Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "FEBRUARY",       Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "MARCH",          Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "APRIL",          Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "MAY",            Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "JUNE",           Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "JULY",           Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "AUGUST",         Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "SEPTEMBER",      Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "OCTOBER",        Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "NOVEMBER",       Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "DECEMBER",       Dicf::CAP_TITLE | Dicf::PART_NOUN },

    // Znamenny
    { "CHELYUSTKA",     Dicf::PART_NOUN },
    { "ZAPYATAYA",      Dicf::PART_NOUN },

    // Some letters
    { "A",              Dicf::PART_MAYBE_LETTER },
    { "-A",             Dicf::CAP_ALL },
    { "ALEF",           Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "AA",             Dicf::CAP_TITLE },
    { "AE",             Dicf::CAP_TITLE },
    { "AIN",            Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "AU",             Dicf::CAP_TITLE },
    { "BEH",            Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "BET",            Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "B",              Dicf::CAP_TITLE },
    { "C",              Dicf::CAP_TITLE },
    { "D",              Dicf::CAP_TITLE },
    { "DAD",            Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "DAL",            Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "DALET",          Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "E",              Dicf::CAP_TITLE },
    { "EE",             Dicf::CAP_TITLE },
    { "EH",             Dicf::CAP_TITLE },
    { "ESH",            Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "EU",             Dicf::CAP_TITLE },
    { "F",              Dicf::CAP_TITLE },
    { "FEH",            Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "G",              Dicf::CAP_TITLE },
    { "GHAIN",          Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "GIMEL",          Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "H",              Dicf::CAP_TITLE },
    { "HAH",            Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "HE",             Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "HEH",            Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "I",              Dicf::PART_MAYBE_LETTER },  // Because of Znamenny
    { "J",              Dicf::CAP_TITLE },
    { "JEEM",           Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "K",              Dicf::CAP_TITLE },
    { "KAF",            Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "KHAH",           Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "L",              Dicf::CAP_TITLE },
    { "LLA",            Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "LAM",            Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "LAMED",          Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "M",              Dicf::CAP_TITLE },
    { "MEEM",           Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "N",              Dicf::CAP_TITLE },
    { "NOON",           Dicf::CAP_TITLE | Dicf::PART_NOUN },    // OK here, no English noon (12:00)
    //{ "NI",             Dicf::CAP_TITLE },    // widely used in cuneiform, IDK what to do
    { "O",              Dicf::CAP_TITLE },
    { "OE",             Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "OM",             Dicf::CAP_TITLE },
    { "OU",             Dicf::CAP_TITLE },
    { "OPEN-O",         { Dicf::PART_NOUN, "open-O" } },
    { "P",              Dicf::CAP_TITLE },
    { "Q",              Dicf::CAP_TITLE },
    { "QAF",            Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "R",              Dicf::CAP_TITLE },
    { "REH",            Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "RESH",           Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "REVERSED-SCHWA", { Dicf::PART_NOUN, "reversed-Schwa" } },
    { "RISH",           Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "S",              Dicf::PART_MAYBE_LETTER },  // Because of Znamenny
    { "SA",             Dicf::CAP_TITLE | Dicf::PART_NOUN },
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
    { "UE",             Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "V",              Dicf::CAP_TITLE },
    { "VAV",            Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "W",              Dicf::CAP_TITLE },
    { "WAW",            Dicf::CAP_TITLE },
    { "X",              Dicf::CAP_TITLE },
    { "Y",              Dicf::CAP_TITLE },
    { "YER",            Dicf::CAP_TITLE | Dicf::IF_SCRIPT | Dicf::PART_NOUN },
    { "YU",             Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "YEH",            Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "Z",              Dicf::CAP_TITLE },
    { "ZAH",            Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "ZAIN",           Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "J-SIMPLIFIED",   Dicf::CAP_TITLE },
    { "WAW-AYIN-RESH",  { Dicf::PART_NOUN, "Waw-Ayin-Resh" } },
    { "RESH-AYIN-DALETH", { Dicf::PART_NOUN, "Resh-Ayin-Daleth" } },
    { "AYIN-DALETH",    { Dicf::PART_NOUN, "Ayin-Daleth" } },
    { "RESH-AYIN",      { Dicf::PART_NOUN, "Resh-Ayin" } },
    { "ZAYIN-YODH",     { Dicf::PART_NOUN, "Zayin-Yodh" } },
    { "GIMEL-HETH",     { Dicf::PART_NOUN, "Gimel-Heth" } },
    { "MEM-QOPH",       { Dicf::PART_NOUN, "Mem-Qoph" } },
    { "DALETH-RESH",    { Dicf::PART_NOUN, "Daleth-Resh" } },

    // Byzantine notes
    //{ "PA",             Dicf::CAP_TITLE }, intersects
    { "NANA",           Dicf::CAP_TITLE },
    { "DI",             Dicf::CAP_TITLE },
    { "KE",             Dicf::CAP_TITLE },
    { "ZO",             Dicf::CAP_TITLE },
    { "VOU",            Dicf::CAP_TITLE },
    { "NI",             Dicf::CAP_TITLE },

    // Some prepositions
    { "WITH",           Dicf::CAP_SMALL },
    //{ "AND",            Dicf::CAP_SMALL },    // Tricky word, all And’s are tested manually
    { "JOINER",         Dicf::CAP_SMALL },
    //{ "ABOVE",          Dicf::CAP_SMALL },    // Tricky word, all above’s are tested manually
    { "FOR",            Dicf::CAP_SMALL },
};


enum class Exf {
    CPONLY = 1,     ///< Works if codepoint is present (Sun=star, sun=weather)
    MIXCASE = 2,    ///< Mixed case: for lowercase letter convert to small
    LEAVE_BY_CONDITION = 4,  ///< Leave as is special condition (see charsConditionalLeave)
};

struct Exception {
    std::string_view r;
    Flags<Exf> flags;
};

#define EX(x) { TOUPPER_SV(x), Exception{ .r{x}, .flags {} } },
#define EX2(x, fgs) { TOUPPER_SV(x), Exception{ .r{x}, .flags{fgs} } },

const std::unordered_map<std::string_view, Exception> exceptions{
    // EUROPEAN SCRIPTS
        // Cyrl
    EX("Cyrillic capital ligature En Ghe")   // All “ligature” are hand-checked
    EX("Cyrillic small ligature En Ghe")
    EX("Cyrillic capital ligature Te Tse")
    EX("Cyrillic small ligature Te Tse")
    EX("Cyrillic capital ligature A Ie")
    EX("Cyrillic small ligature A Ie")
    EX("Cyrillic \"beautiful Omega\"")  // Letter is BIG
    EX("voiceless l")               // letter is SMALL
    EX("voiceless r")               // letter is SMALL
    EX("palatalized l")             // letter is SMALL
    EX("palatalized n")             // letter is SMALL
        // Old Cyrl
    EX("old Cyrillic yest")         // letter is SMALL
    EX("old Cyrillic i")            // letter is SMALL
        // Dupl
    EX("Duployan affix Attached I hook")            // Same
    EX("Duployan thick letter selector")            // A clear miss of my algo, IDK what to do
    EX("Shorthand format Letter overlap")           // Letter is not keyword
        // Glag
    EX("Glagolitic capital letter Caudate Chrivi")  // Don’t want to encode Chrivi
    EX("Glagolitic small letter Caudate Chrivi")    // Same
        // Grek
    EX("Greek lunate epsilon symbol")   // Epsilon is small here
    EX("Greek reversed lunate epsilon symbol") // Same
    EX("Greek beta symbol")             // Beta is small here
    EX("Greek capital Kai symbol")      // Kai is capital here
    EX("Greek kai symbol")              // Kai is small here
    EX("Greek lunate sigma symbol")     // Sigma is small here
    EX("Greek rho with stroke symbol")  // Rho is small here
    EX("Greek one half sign alternate form")    // sign, alternate form
    EX("Greek non-spacing iota below")  // This letter is SMALL
    EX("iota subscript")                // This letter is SMALL
    EX("gamma function")                // Letter is big, function is small
    EX("Iota adscript")                 // Letter is BIG
    EX2("Lambda", Exf::MIXCASE)
    EX("curled beta")                   // Letter is SMALL
    EX("script theta")                  // Letter is SMALL
    EX("omega pi")                      // Letter is SMALL
    EX("script kappa")                  // Letter is SMALL
    EX("tailed rho")                    // Letter is SMALL
    EX("straight epsilon")              // Letter is SMALL
    EX("reversed straight epsilon")     // Letter is SMALL
    EX("Antisigma")                     // Letter is BIG
    EX("Antisigma periestigmenon")      // Letter is BIG
    EX("Claudian Antisigma")            // Letter is BIG
    EX("Epidaurean acrophonic symbol Three")    // Vertical ellipsis
            // Greek capital reversed lunate Sigma symbol — OK, Sigma is cap
        // Latn
    EX2("I dot", Exf::MIXCASE)
    EX2("I bar", Exf::MIXCASE)
    EX2("O bar", Exf::MIXCASE)
    EX2("Lambda bar", Exf::MIXCASE)
    EX2("barred Lambda", Exf::MIXCASE)
    EX2("Z bar", Exf::MIXCASE)
    EX2("barred Z", Exf::MIXCASE)
    EX("script f")              // This letter is SMALL
    EX("reversed Polish-hook o")    // This letter is SMALL
    EX2("O slash", Exf::MIXCASE)
    EX2("barred L", Exf::MIXCASE)
    EX2("D retroflex hook", Exf::MIXCASE)
    EX2("Epsilon", Exf::MIXCASE)
    EX2("closed reversed Epsilon", Exf::MIXCASE)
    EX2("closed Epsilon", Exf::MIXCASE)
    EX2("reversed Epsilon", Exf::MIXCASE)
        // Runr
    EX("runic letter Dotted-N") // All are really tricky!!
    EX("runic letter Dotted-L")
    EX("runic letter Long-branch-Yr")
    EX("runic letter Short-twig-Yr")
    EX("runic letter Icelandic-Yr")
    EX("runic letter Dotted-P")
    EX("runic letter Open-P")
    EX("runic letter Sigel Long-branch-Sol S")
    EX("runic letter Short-twig-Sol S")
    EX("runic letter Long-branch-Ar Ae")
    EX("runic letter Short-twig-Ar A")
    EX("runic letter Long-branch-Hagall H")
    EX("runic letter Short-twig-Hagall H")
    EX("runic letter Short-twig-Bjarkan B")
    EX("runic letter Long-branch-Oss O")
    EX("runic letter Short-twig-Oss O")
    EX("runic letter Short-twig-Naud N")
    EX("runic letter Long-branch-Madr M")
    EX("runic letter Short-twig-Madr M")
    EX("runic letter Short-twig-Tyr T")
    EX("runic Arlaug symbol")
    EX("runic Tvimadur symbol")  // Same
    EX("runic Belgthor symbol")
        // Byzantine notes
    EX("Byzantine musical symbol Fthora diatoniki Pa")  // Pa is not just note

    // BRAHMIC SCRIPTS
        // Common-Indic
    EX("siddham")                               // siddham sign
    EX("vowel half-U")
        // Beng
    EX("Bengali letter Khanda Ta")  // Sword-like; because of Sikh character it’s tricky
    EX("Bengali Va")
        // Deva
    EX("Devanagari letter Candra A") // Both Candra and A are tricky, better to make an exception
    EX("Vedic tone Svarita")    // In NamesList.txt it’s in Mixed case → need
    EX("Vedic tone Anudatta")   // Same
        // Diak
    EX("Dives Akuru medial Ya")
    EX("Dives Akuru initial Ra")
    EX("Dives Akuru medial Ra")
        // Guru
    EX("Gurmukhi Iri")      // Same as Gurmukhi letter Uu
    EX("Gurmukhi Ura")      // Same
    EX("Gurmukhi Ek Onkar") // Ligature of phrase
        // Kthi
    EX("Kaithi number sign above")  // Number’s mark → number is not keyword
        // Lana
    EX("Tai Tham consonant sign High Ratha or Low Pa")
        // Mymr
    EX("Myanmar sign Pao Karen tone")  // What to do, “tone” is overloaded
        // Sinh
    EX("Sinhala vowel sign Kombuva haa Aela-pilla")     // Haa is also a letter
    EX("Sinhala vowel sign Kombuva haa diga Aela-pilla")
    EX("Sinhala vowel sign Kombuva haa gayanukitta")
        // Tglg
    EX("Zambales Ra")
        // Thai
    EX("Thai character No nu")      // Conflict with Greek Nu
    EX("mai may")                   // IDK what triggers May
        // Tibt
    EX("Tibetan astrological sign -Khyud pa")   // Because of hyphen (=apostrophe)
    EX("reversed Sha")
    EX("reversed subjoined Sha")
    EX("A-chung")
    EX("Ya-btags (Ya ta)")
    EX("Ra-btags (Ra ta)")
        // Zanb
    EX("Zanabazar square final consonant mark") // Some bugs

    // NON-BRAHMIC ASIAN AND OCEAN SCRIPTS
        // Arab
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
    EX("Arabic pound mark above")    // Mark is not special word here. You may call “above” an adverb, but no
    EX("Arabic piastre mark above")
    EX("Arabic letter Lam with small Arabic letter Tah above")  // Some bug
    EX("Arabic letter Teh with small Teh above")    // Tricky
    EX("Arabic letter Reh with small Noon above")   // Same
    EX("Arabic letter Yeh with two dots below and small Noon above")
    EX("Arabic letter Peh with small Meem above")
    EX("Arabic letter Beh with small Meem above")
    EX("Arabic-Indic Per mille sign")               // Tricky
    EX("Arabic-Indic Per ten thousand sign")        // Tricky
    EX("Arabic number mark above")              // Number’s mark
    EX("Arabic ligature Salla used as Koranic stop sign isolated form") // Sign is not a keyword
    EX("Arabic ligature Qala used as Koranic stop sign isolated form")  // Sign is not a keyword
    EX("Arabic letter Hamzah on Ha")        // Shall we make Ha a keyword?
        // Armn
    EX("Armenian small ligature Ech Yiwn")  // All “ligature” are manually checked
    EX("Armenian small ligature Men Now")
    EX("Armenian small ligature Men Ech")
    EX("Armenian small ligature Men Ini")
    EX("Armenian small ligature Vew Now")
    EX("Armenian small ligature Men Xeh")
        // Hebr
    EX("Hebrew Yod triangle")                       // IDK what to do
    EX("Hebrew ligature Yiddish Vav Yod")           // Same
    EX("Hebrew ligature Yiddish double Yod")
    EX("Hebrew ligature Yiddish double Vav")
    EX("Hebrew ligature Yiddish Yod Yod Patah")
    EX("Hebrew ligature Alef Lamed")
        // Mand
    EX("pharyngeal Hu")
    EX("I")
        // Mani
    EX("Manichaean abbreviation mark above")    // Abbreviation’s mark → abbreviation/mark are not keywords
    EX("Manichaean abbreviation mark below")    // The same
        // Mong
    EX("Mongolian Sibe syllable boundary marker")   // Syllable is not keyword
        // Pauc
    EX("Pau Cin Hau rising tone long")          // Tone is not keyword
    EX("Pau Cin Hau rising tone long final")    // Same
    EX("Pau Cin Hau rising tone final")
    EX("Pau Cin Hau sandhi tone long")
    EX("Pau Cin Hau sandhi tone long final")
    EX("Pau Cin Hau sandhi tone final")
    EX("Pau Cin Hau mid-level tone long final")
    EX("Pau Cin Hau mid-level tone final")
    EX("Pau Cin Hau low-falling tone long")
    EX("Pau Cin Hau low-falling tone long final")
    EX("Pau Cin Hau low-falling tone final")
        // Sogd
    EX("Sogdian independent Shin")          // Hebrew Shin / shin dot
        // Syrc
    EX("SAM")             // Syriac abbreviation mark
        // Thaa
    EX("Thaana Abafili")  // I think they should be capitalized
    EX("Thaana Aabaafili")
    EX("Thaana Ibifili")
    EX("Thaana Eebeefili")
    EX("Thaana Ubufili")
    EX("Thaana Ooboofili")
    EX("Thaana Ebefili")
    EX("Thaana Eybeyfili")
    EX("Thaana Obofili")
    EX("Thaana Oaboafili")

    // MODERN ASIAN SCRIPTS
        // Hmnp
    EX("Nyiakeng Puachue Hmong circled Ca") // IDK what to do
        // Mong
    EX("Mongolian vowel separator")     // Vowel is not keyword
        // Olck
    EX("Ol Chiki mu ttuddag")           // Conflict with Greek Mu
        // Plrd
    EX("Miao sign Consonant modifier bar")  // Modifier is not keyword here
    EX("Miao tone right")     // Tone is not special here
    EX("Miao tone top right")
    EX("Miao tone above")
    EX("Miao tone below")
        // Hmng
    EX("Pahawh Hmong sign Xyeem rho")   // Conflict with Greek Rho

    // AMERICAN SCRIPTS
    EX("Canadian syllabics hyphen")     // What to do?

    // AFRICAN SCRIPTS
        // Adlm
    EX("Adlam Alif lengthener")     // Alif is a letter
    EX("Adlam vowel lengthener")    // Vowel is not keyword
        // Bamu
    EX("Bamum letter Phase-A Mon nggeuaet")     // Mon is also Myanmar nation
        // Copt
    EX("Coptic combining Ni above") // Ni is widely used in cuneiform, and IDK what to do
    EX("Coptic symbol kai")             // Seemingly small
    EX("Coptic symbol Mi Ro")
    EX("Coptic symbol Pi Ro")
    EX("Coptic symbol Tau Ro")
    EX("Coptic symbol Khi Ro")
    EX("Coptic symbol Shima Sima")
        // Medf
    EX("Medefaidrin exclamation Oh")  // Cannot make a rule for “exclamation”, too hard
        // Tfng
    EX("Adrar Yaj")
    EX("harpoon Yaz")
    EX("Tifinagh letter Berber Academy Yaj")
    EX("Tuareg Yab")
    EX("Tuareg Yaw")            

    // Misc letters
    EX2("A", Exf::MIXCASE)
    EX2("B", Exf::MIXCASE)
    EX("Ai")
    EX("Bb")
    EX("Bha")
    EX("Bs")
    EX2("C", Exf::MIXCASE)
    EX("Ca")
    EX("Cha")
    EX2("Chi", Exf::LEAVE_BY_CONDITION)
    EX("Chha")
    EX("soft Da")
    EX("soft Dda")
    EX("hard Da")
    EX("hard Dda")
    EX2("D", Exf::MIXCASE)
    EX("Dd")
    EX("Dda")
    EX("Ddha")
    EX("Dha")
    { "ĕ", Exception{ "Ĕ", {} } },  // A9BC
    EX("Ei")
    EX2("F", Exf::MIXCASE)
    EX("Fu")
    EX2("G", Exf::MIXCASE)
    EX("Gg")
    EX("Gha")
    EX("Gs")
    EX2("H", Exf::MIXCASE)
    EX("Ha")
    EX("Ii")
    EX2("J", Exf::MIXCASE)
    EX("Ja")
    EX("Jha")
    EX("Ji (not unique)")
    EX("Jj")
    EX("Jnya")
    EX2("K", Exf::MIXCASE)
    EX("Ka")
    EX("Kaam")
    EX("Kai")
    EX("Kaim")
    EX("Kam")
    EX("Kau")
    EX("Kaum")
    EX("Kaw")
    EX("Kee")
    EX("Kem")
    EX("Keem")
    EX("Kha")
    EX("Kia")
    EX("Kim")
    EX("Ki")
    EX("Kog")
    EX("Kom")
    EX("Kua")
    EX("Kuam")
    EX("Kum")
    EX("Kwm")
    EX2("L", Exf::MIXCASE)
    EX("La")
    EX("Lb")
    EX("Lg")
    EX("Lha")
    EX("Lm")
    EX("Ls")
    EX("Lt")
    EX("Lp")
    EX("Lh")
    EX("vocalic L")
    EX("vocalic Ll")
    EX2("M", Exf::MIXCASE)
    EX("Ma")
    EX("subjoined Ma")
    EX2("Maa", Exf::LEAVE_BY_CONDITION)
    EX("logosyllabic Muwa")
    EX2("N", Exf::MIXCASE)
    EX("hard Na")
    EX("Ng")
    EX("Nga")
    EX("Nh")
    EX("Nha")
    EX("Nj")
    EX("Nna")
    EX2("P", Exf::MIXCASE)
    EX("Pha")
    EX2("Pi", Exf::LEAVE_BY_CONDITION)
    EX2("Q", Exf::MIXCASE)
    EX("Qa")
    EX2("R", Exf::MIXCASE)
    EX("vocalic R")
    EX("Ra")
    EX("medial Ra")
    EX("vocalic Rr")
    EX2("S", Exf::MIXCASE)
    EX("Sad")
    EX("Sha")
    EX("Sheen")
    EX("Shi")
    EX("Ssa")
    EX("Ss")
    EX("soft Sha")
    EX2("T", Exf::MIXCASE)
    EX("Ta")
    EX("Tha")
    EX("syllabic Ti")
    EX("Tra")
    EX("Tta")
    EX("Ttha")
    EX("Tsu")
    EX("small Tsu")
    EX("syllabic Tu")
    EX2("V", Exf::MIXCASE)
    EX("Uu")
    EX("Wa")
    EX("subjoined Wa")
    EX("syllabic Wi(ya)")
    EX("Ya")
    EX("medial Ya")
    EX2("Z", Exf::MIXCASE)
    EX("Zha")
    EX("Zu (not unique)")

    // CJK
        // Hang
    EX("Hangul choseong filler")    // Simpler to add exception
    EX("Hangul jungseong filler")   // Same
        // Decorations etc
    EX("rounded symbol for Fu")
    EX("rounded symbol for Lu")
    EX("rounded symbol for Shou")
    EX("rounded symbol for Xi")
    EX("rounded symbol for Shuangxi")
    EX("rounded symbol for Cai")
    EX("square Hiragana Hoka")  // Better exception?
    EX("squared Katakana Koko")
    EX("squared Katakana Sa")
    EX("kaeriten Re")
    EX("symbol of unification")
    EX("di ren")
    EX("ren di")

    // SPECIALS
        // Misc
    EX("commercial At")                 // At is not a preposition
    EX("tag Commercial At")             // Same
    EX("fullwidth commercial At")       // Same
    EX("small commercial At")           // Same
    EX("symbol for type A electronics") // A is tricky
    EX("combining ligature left half")  // Other meaning of ligature — horz parenthesis
    EX("combining ligature right half") // Same
    EX("combining ligature left half below")
    EX("combining ligature right half below")
    EX("combining Almost equal to below") // Tricky
    EX("modifier breve with inverted breve")    // Modifier
    EX("modifier letter Low circumflex accent") // What to do with circumflex?
    EX("modifier letter Short equals sign")     // What to do with equals?
    EX("direct current symbol form two") // Symbol is not a keyword
    EX("Roman numeral Reversed one hundred") // IDK what happens with numerals
    EX("AC current")                    // Alternating current current :)
    EX("ligature tie below")            // ligature tie = smile
    EX("ligature tie")                  // Same
    EX("symbol of Iran")                // Just glitch
        // Religion
    EX("Adi Shakti")
    EX("West Syriac cross")
    EX("East Syriac cross")
        // Math
    EX("small Element of")              // small sign “Element of”
    EX("small Contains as member")      // Same
    EX("small Element of with vertical bar at end of horizontal stroke")
    EX("small Element of with overbar")
    EX("small Contains with vertical bar at end of horizontal stroke")
    EX("small Contains with overbar")
    EX("Or with dot inside")            // Still same
    EX("And with dot")
    EX("angle with S inside")           // Because of Znamenny, S is tricky
    EX("perpendicular with S")          // Same
    EX("fraction slash")                // Fraction’s slash
    EX("sloping large Or")              // Or is a tricky word
    EX("sloping large And")             // And is a tricky word
    EX("Planck constant over two pi")   // pi is small here
    EX("invisible Times")               // ???
    EX("S in triangle")                 // Because of Znamenny, S is tricky
    EX("radical symbol bottom")         // symbol’s bottom
    EX("XOR")
    EX("NAND")
    EX("NOR")
    EX("Riemann integral")
    EX("Z notation Cartesian product")  // Z???
    EX("EMF (electromotive force)")
    EX("N-ary Dijkstra choice")
    EX("alef symbol")                   // ???
    EX("bet symbol")
    EX("gimel symbol")
    EX("dalet symbol")
    EX("nabla")                         // Other nablas are capital
        // Astronomy
    EX("Earth")                         // Planet, not element
    EX2("Sun", Exf::CPONLY)             // CP: star; Emoji: weather
    EX("Mercury")                       // Planet, not metal
    EX("Cancer")                        // Constellation, not disease
        // Emoji
    EX("Earth globe Europe-Africa")     // Special proper name
    EX("Earth globe Americas")          // Special proper name
    EX("Earth globe Asia-Australia")    // Special proper name
    EX("Milky Way")                     // Special proper name
    EX("Tokyo Tower")                   // Special proper name
    EX("Mount Fuji")                    // Special proper name
    EX("Statue of Liberty")             // Special proper name
    EX("man with gua pi mao")           // Conflict with Greek Pi
    EX("face with No good gesture")     // Complex rules, better manually
    EX("face with OK gesture")          // Complex rules, better manually
    EX("OK hand sign")                  // OK is both a syllable and an emoji
    EX("turned OK hand sign")           // The same
    EX("Call me hand")                  // “Call me” hand
    EX("I love you hand sign")          // same + I
    EX("letter box")                    // Letter is a paper
    EX("ID")                            // Just two letters
        // Formatting
    EX("national digit shapes")   // Digit is not special here
    EX("nominal digit shapes")    // Same
        // Music
    EX("musical symbol With fingernails")       // With is not a preposition
    EX("musical symbol Quarter rest")           // Quarter is a tricky word
    EX("musical symbol Quarter note")           // The same
    EX("musical symbol Left repeat sign")       // Just manually, 3 chars
    EX("musical symbol Right repeat sign")      // The same
    EX("musical symbol Half pedal mark")        // The same
    EX("musical symbol Combining augmentation dot")  // Augmentation is a future landmine
        // Music/Znamenny
    EX("Znamenny neume Dva v chelnu")  // Better not to make exception from V
        // Games
    EX("mahjong tile back")
    // domino tile Horizontal/Vertical back probably OK
    EX("playing card back")

    // B-I-G  S-E-T-S

    // Mathematical alphabets
#define MATH_SYM(x) \
        EX("Greek " x " symbol") \
        EX("mathematical bold " x " symbol") \
        EX("mathematical italic " x " symbol") \
        EX("mathematical bold italic " x " symbol") \
        EX("mathematical sans-serif bold " x " symbol") \
        EX("mathematical sans-serif bold italic " x " symbol")

    MATH_SYM ("epsilon")    // Small epsilon etc here
    MATH_SYM ("theta")
    MATH_SYM ("kappa")
    MATH_SYM ("phi")
    MATH_SYM ("rho")
    MATH_SYM ("pi")
#undef MATH_SYM
    EX("Arabic mathematical Sad")    // Ambiguous
    EX("Arabic mathematical Sheen")  // Ambiguous

    // Legacy
    EX("NPN transistor")
    EX("PNP transistor")
    EX("logic gate Or")
    EX("logic gate And")

    // Hand-checked CJK Latin
    EX("squared HV")                            // Just manually, small block
    EX("squared MV")
    EX("squared SD")
    EX("squared SS")
    EX("squared PPV")
    EX("squared WC")
    EX("square Corporation")                    // IDK what’s happening
    EX("square hPa")                            // A whole cohort of Chinese units
    EX("square IU")
    EX("square da")
    EX("square AU")
    EX("square bar")
    EX("square oV")
    EX("square pc")
    EX("square dm squared")
    EX("square pA amps")
    EX("square nA")
    EX("square mu A")
    EX("square mA")
    EX("square kA")
    EX("square KB")
    EX("square MB")
    EX("square GB")
    EX("square cal")
    EX("square kcal")
    EX("square pF")
    EX("square nF")
    EX("square mu F")
    EX("square mu G")
    EX("square mg")
    EX("square kg")
    EX("square Hz")
    EX("square kHz")
    EX("square MHz")
    EX("square GHz")
    EX("square THz")
    EX("square mu l")
    EX("square ml")
    EX("square dl")
    EX("square kl")
    EX("square fm")
    EX("square nm")
    EX("square mu m")
    EX("square mm")
    EX("square cm")
    EX("square km")
    EX("square mm squared")
    EX("square cm squared")
    EX("square m squared")
    EX("square km squared")
    EX("square mm cubed")
    EX("square m cubed")
    EX("square km cubed")
    EX("square m over s")
    EX("square m over s squared")
    EX("square Pa")
    EX("square kPa")
    EX("square MPa")
    EX("square GPa")
    EX("square rad")
    EX("square rad over s")
    EX("square rad over s squared")
    EX("square ps")
    EX("square ns")
    EX("square mu s")
    EX("square ms")
    EX("square pV")
    EX("square nV")
    EX("square mu V")
    EX("square mV")
    EX("square kV")
    EX("square MV mega")
    EX("square pW")
    EX("square nW")
    EX("square mu W")
    EX("square mW")
    EX("square kW")
    EX("square MW mega")
    EX("square k Ohm")
    EX("square M Ohm")
    EX("square am")
    EX("square Bq")
    EX("square cc")
    EX("square cd")
    EX("square C over kg")
    EX("square Co")
    EX("square dB")
    EX("square Gy")
    EX("square ha")
    EX("square HP")
    EX("square in")
    EX("square KK")
    EX("square KM capital")
    EX("square kt")
    EX("square lm")
    EX("square ln")
    EX("square log")
    EX("square lx")
    EX("square mb small")
    EX("square mil")
    EX("square mol")
    EX("square pH")
    EX("square pm")
    EX("square PPM")
    EX("square PR")
    EX("square sr")
    EX("square Sv")
    EX("square Wb")
    EX("square V over m")   // Volt/metre
    EX("square A over m")   // Ampere/metre
    EX("square gal")
    EX("square Hg")
    EX("square erg")
    EX("square eV")
    EX("square DJ")

        // Enclosed Latin
    EX("circled CC")            // Closed Captions, hand-checked
    EX("circled CD")            // Compact disc? hand-checked
    EX("circled Wz")            // German Warenzeichen
    EX("negative squared IC")   // Hand-checked
    EX("negative squared PA")
    EX("negative squared SA")
    EX("negative squared AB")
    EX("negative squared WC")
    EX("raised MC sign")        // Hand-checked
    EX("raised MD sign")
    EX("raised MR sign")
    EX("squared OK")            // OK is both a syllable and an emoji
    EX("squared SOS")           // The same
    EX("squared CL")            // Squared is a tricky word
    EX("squared Cool")
    EX("squared New")
    EX("squared No good")
    EX("squared Up with exclamation mark")
    EX("squared Three D")
    EX("squared Second screen")
    EX("squared Two K")
    EX("squared Four K")
    EX("squared Eight K")
    EX("squared Five point one")
    EX("squared Seven point one")
    EX("squared Twenty-two point two")
    EX("squared Sixty P")
    EX("squared One hundred twenty P")
    EX("squared HC")
    EX("squared Vs")
    EX("squared New")
    EX("squared NG")
    EX("squared Free")
    EX("squared ID")
    EX("squared Hi-res")
    EX("squared HDR")
    EX("squared SHV")
    EX("squared VOD")
    EX("squared UHD")
    EX("squared Lossless")
    EX("Back with leftwards arrow above")
    EX("End with leftwards arrow above")
    EX("On with exclamation mark with left right arrow above")
    EX("Soon with rightwards arrow above")
    EX("Top with upwards arrow above")
    EX("keycap Ten")
};

/// @warning DO NOT REMOVE sv, w/o does not work and IDK how to ban
const std::multiset<IdiomEntry> idioms {
    // Prefixes
    { { "APL"sv, "FUNCTIONAL"sv, "SYMBOL"sv }, IsPrefix::YES, IdiomAction::REST_CAPSMALL },
    { { "QUADRANT"sv }, IsPrefix::YES, IdiomAction::NEXT_CAP },
    { { "BOX"sv, "DRAWINGS"sv }, IsPrefix::YES, IdiomAction::NEXT_CAP },
    { { "LARGE"sv, "TYPE"sv, "PIECE"sv }, IsPrefix::YES, IdiomAction::NEXT_CAP },
    { { "MAHJONG"sv, "TILE"sv }, IsPrefix::YES, IdiomAction::REST_CAPSMALL },
    { { "DOMINO"sv, "TILE"sv }, IsPrefix::YES, IdiomAction::REST_CAPSMALL },
    { { "PLAYING"sv, "CARD"sv }, IsPrefix::YES, IdiomAction::REST_CAPSMALL },
    { { "TIBETAN"sv, "DIGIT"sv, "HALF"sv }, IsPrefix::YES, IdiomAction::NEXT_SMALL },
    { { "SUPERSCRIPT"sv }, IsPrefix::YES, IdiomAction::NEXT_CAP },
    { { "SUBSCRIPT"sv }, IsPrefix::YES, IdiomAction::NEXT_CAP },
    { { "INDIC"sv, "SIYAQ"sv, "NUMBER"sv }, IsPrefix::YES, IdiomAction::REST_CAPSMALL },
    { { "MODIFIER"sv, "LETTER"sv, "CHINESE"sv, "TONE"sv, }, IsPrefix::YES, IdiomAction::NEXT_CAP },
    { { "RUNIC"sv, "LETTER"sv, "FRANKS"sv, "CASKET"sv }, IsPrefix::YES, IdiomAction::REST_CAP },      // prevent next!
    { { "RUNIC"sv, "LETTER"sv }, IsPrefix::YES, IdiomAction::REST_CAP },
    { { "CIRCLED"sv, "KATAKANA"sv }, IsPrefix::YES, IdiomAction::NEXT_CAP },  // IDK what to do, normal rules fail
    { { "CJK"sv, "STROKE"sv }, IsPrefix::YES, IdiomAction::REST_ALLCAP },
    { { "MIAO"sv, "LETTER"sv, "YI"sv }, IsPrefix::YES, IdiomAction::NEXT_CAP }, // Yi is ambiguous
    { { "SQUARE"sv, "ERA"sv, "NAME"sv }, IsPrefix::YES, IdiomAction::NEXT_CAP },
    { { "SQUARE"sv }, 0x3300, 0x3357, IsPrefix::YES, IdiomAction::NEXT_CAP },     // And what to do with those Katakana chars?
    { { "CYPRO-MINOAN"sv, "SIGN"sv }, IsPrefix::YES, IdiomAction::REST_ALLCAP },
    { { "LINEAR"sv, "A"sv, "SIGN"sv }, IsPrefix::YES, IdiomAction::REST_CAP },
    { { "LINEAR"sv, "B"sv, "SYLLABLE"sv }, IsPrefix::YES, IdiomAction::REST_CAP },
    { { "PAHAWH"sv, "HMONG"sv, "CONSONANT"sv }, IsPrefix::YES, IdiomAction::NEXT_CAP },   // Consonant is tricky
    { { "SHORTHAND"sv, "FORMAT"sv }, IsPrefix::YES, IdiomAction::REST_CAPSMALL },
    { { "ZNAMENNY"sv, "PRIZNAK"sv, "MODIFIER"sv }, IsPrefix::YES, IdiomAction::NEXT_CAP },
    { { "HEBREW"sv, "ACCENT"sv }, IsPrefix::YES, IdiomAction::NEXT_CAP },     // Accent is tricky, though seemingly good
    { { "HEBREW"sv, "POINT"sv }, IsPrefix::YES, IdiomAction::NEXT_CAP },      // Point is really tricky
    { { "TAG"sv }, IsPrefix::YES, IdiomAction::NEXT_CAP },                    // Tags
    { { "CLOCK"sv, "FACE"sv }, IsPrefix::YES, IdiomAction::NEXT_CAP },        // clock face XXX
    { { "CIRCLED"sv, "IDEOGRAPH"sv }, IsPrefix::YES, IdiomAction::NEXT_CAP }, // circled ideograph XXX
    { { "PARENTHESIZED"sv, "IDEOGRAPH"sv }, IsPrefix::YES, IdiomAction::NEXT_CAP }, // parenthesized ideograph XXX
    { { "CIRCLED"sv, "HANGUL"sv }, IsPrefix::YES, IdiomAction::REST_CAP },        // circled Hangul Xxx
    { { "PARENTHESIZED"sv, "HANGUL"sv }, IsPrefix::YES, IdiomAction::REST_CAP },  // parenthesized Hangul Xxx
    { { "BALINESE"sv, "MUSICAL"sv, "SYMBOL"sv }, IsPrefix::YES, IdiomAction::REST_CAPSMALL },
    { { "IDEOGRAPHIC"sv, "TALLY"sv, "MARK"sv }, IsPrefix::YES, IdiomAction::NEXT_CAP },   // ideographic tally mark One
    { { "TALLY"sv, "MARK"sv }, IsPrefix::YES, IdiomAction::NEXT_CAP },
    { { "EMOJI"sv, "COMPONENT"sv }, IsPrefix::YES, IdiomAction::NEXT_CAP },
    { { "IDEOGRAPHIC"sv, "DESCRIPTION"sv, "CHARACTER"sv }, IsPrefix::YES, IdiomAction::NEXT_CAP },
    { { "DUPLOYAN"sv, "LETTER"sv }, IsPrefix::YES, IdiomAction::REST_DETECT_LETTERS },
    // Rest idioms
    { { "VOWEL"sv, "LENGTH"sv, "MARK"sv }, IsPrefix::NO, IdiomAction::SECOND_THIRD_SMALL },
    { { "VEDIC"sv, "TONE"sv}, IsPrefix::NO, IdiomAction::NEXT_CAP },
    { { "AI"sv, "LENGTH"sv, "MARK"sv}, IsPrefix::NO, IdiomAction::FIRST_CAP },
    { { "SYLLABLE"sv, "LENGTHENER"sv }, IsPrefix::NO, IdiomAction::SECOND_THIRD_SMALL },
    { { "SYLLABLE"sv, "ITERATION"sv, "MARK"sv}, IsPrefix::NO, IdiomAction::SECOND_THIRD_SMALL },
};


const std::unordered_map<char32_t, std::string_view> abbrevs {
    { 9, "TAB" },       // TAB or HT
    { 10, "LF" },       // NL or LF
    { ' ', {} },        // SPACE has abbrev SP
    { 0x0019, "EOM" },  // EOM is better than EM
    { 0x00A0, "!" },    // NBSP bas abbreviations, turn them to aliases
    { 0x115F, "HC F" }, // Hangul starting filler
    { 0x1160, "HJ F" }, // Hangul medial filler
    { 0x17B4, "KIV AQ" }, // Khmer intependent vowel Aq
    { 0x17B5, "KIV AA" }, // Khmer intependent vowel Aa
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
    { 0x3164, "HF" },   // Hangul filler
    { 0xFEFF, "BOM" },  // BOM or ZWNBSP
    { 0xFFA0, "HWHF" }, // Halfwidth Hangul filler
    { 0xFFF9, "IA A" }, // Interlinear annotation anchor
    { 0xFFFA, "IA S" }, // Interlinear annotation separator
    { 0xFFFB, "IA T" }, // Interlinear annotation terminator
    { 0xFFFC, "OBJ" },  // Object replacement character
    { 0x1107F, "BNJ" }, // Brahmi number joiner
    { 0x13430, ":" },   // Egyptian hiero vertical joiner
    { 0x13436, "+" },   // Egyptian hiero overlay middle
    { 0x13437, "(" },   // Egyptian hiero begin segment
    { 0x13438, ")" },   // Egyptian hiero end segment
    { 0x13440, "↔" },   // Egyptian hiero mirror
    { 0x16FE4, "KSS F" }, // Khitan small filler
    { 0x1BC9D, "DTLS" }, // Duployan thick letter selector
    { 0x1BCA2, "↓" },   // Shorthand format Down step
    { 0x1BCA3, "↑" },   // Shorthand format Up step
    { 0x1D159, "NULL NOTE HEAD" }, // Music
    { 0x1D173, "BEGIN BEAM" },
    { 0x1D174, "END BEAM" },
    { 0x1D175, "BEGIN TIE" },
    { 0x1D176, "END TIE" },
    { 0x1D177, "BEGIN SLUR" },
    { 0x1D178, "END SLUR" },
    { 0x1D179, "BEGIN PHR." },
    { 0x1D17A, "END PHR." },
    { 0x1DA9B, "SW F2" },   // Sutton SignWriting fill
    { 0x1DA9C, "SW F3" },
    { 0x1DA9D, "SW F4" },
    { 0x1DA9E, "SW F5" },
    { 0x1DA9F, "SW F6" },
    { 0x1DAA1, "SW R2" },   // Sutton SignWriting rotation
    { 0x1DAA2, "SW R3" },
    { 0x1DAA3, "SW R4" },
    { 0x1DAA4, "SW R5" },
    { 0x1DAA5, "SW R6" },
    { 0x1DAA6, "SW R7" },
    { 0x1DAA7, "SW R8" },
    { 0x1DAA8, "SW R9" },
    { 0x1DAA9, "SW R10" },
    { 0x1DAAA, "SW R11" },
    { 0x1DAAB, "SW R12" },
    { 0x1DAAC, "SW R13" },
    { 0x1DAAD, "SW R14" },
    { 0x1DAAE, "SW R15" },
    { 0x1DAAF, "SW R16" },
    // Then Variation Selectors
};

const std::unordered_map<char32_t, uc::Cfgs> drawMethods {
    // Knda
    { 0x0CF1,  uc::m::SAMPLED_CONTROL }, // Knda vedic sign jihva
    { 0x0CF2,  uc::m::SAMPLED_CONTROL }, // Knda vedic sign upadh
    // Mlym
    { 0x0D4E,  uc::m::SAMPLED_CONTROL }, // Mlym dot reph
    // Tibt
    { 0x0F0C,  uc::m::SAMPLED_CONTROL }, // Tibt non-breaking tsheg
    // Mymr
    { 0x1039,  uc::m::VIRTUAL_VIRAMA  }, // Mymr virtual virama
    // Sund
    { 0x1BAB,  uc::m::VIRTUAL_VIRAMA  }, // Sund virtual virama
    // Khmr
    { 0x17D2,  uc::m::VIRTUAL_VIRAMA  }, // Khmr coeng (virtual virama)
    // Lana
    { 0x1A60,  uc::m::VIRTUAL_VIRAMA  }, // Lana sakot (virtual virama)
    // Vedic
    { 0x1CF5,  uc::m::SAMPLED_CONTROL }, // Upadh
    { 0x1CF6,  uc::m::SAMPLED_CONTROL }, // Jihva
    // Brai
    { 0x2800,  uc::m::SPACE           }, // Braille blank
    // CJK
    { 0x303E,  uc::m::CUSTOM_CONTROL  }, // ideographic variation indicator
    // Mtei
    { 0xAAF6,  uc::m::VIRTUAL_VIRAMA  }, // Mtei virtual virama
    // Khar
    { 0x10A3F, uc::m::VIRTUAL_VIRAMA }, // Khar virtual virama (does not fit)
    // Brah
    { 0x11003, uc::m::SAMPLED_CONTROL }, // Brah jihva
    { 0x11004, uc::m::SAMPLED_CONTROL }, // Brah upadh
    // Cakm
    { 0x11133, uc::m::VIRTUAL_VIRAMA  }, // Cakm virama
    // Shrd
    { 0x111C2, uc::m::SAMPLED_CONTROL }, // Shrd jihva
    { 0x111C3, uc::m::SAMPLED_CONTROL }, // Shrd upadh
    // Tutg
    { 0x113D0, uc::m::VIRTUAL_VIRAMA  }, // Tutg conjoiner
    // Diak
    { 0x1193E, uc::m::VIRTUAL_VIRAMA  }, // Diak virama
    { 0x1193F, uc::m::SAMPLED_CONTROL }, // Diak nasal sign
    { 0x11941, uc::m::SAMPLED_CONTROL }, // Diak repha
    // Zanb
    { 0x11A3A, uc::m::SAMPLED_CONTROL | uc::Cfg::M_NO_SHAPING }, // Zanb repha (cluster-initial Ra)
    { 0x11A47, uc::m::VIRTUAL_VIRAMA  }, // Zanb subjoiner (virtual virama)
    // Soyo
    { 0x11A84, uc::m::SAMPLED_CONTROL | uc::Cfg::M_NO_SHAPING }, // Soyo vedic sign jihva
    { 0x11A85, uc::m::SAMPLED_CONTROL | uc::Cfg::M_NO_SHAPING }, // Soyo vedic sign upadh
    { 0x11A86, uc::m::SAMPLED_CONTROL }, // Soyo Ra+
    { 0x11A87, uc::m::SAMPLED_CONTROL }, // Soyo La+
    { 0x11A88, uc::m::SAMPLED_CONTROL }, // Soyo Sha+
    { 0x11A89, uc::m::SAMPLED_CONTROL }, // Soyo Sa+
    { 0x11A99, uc::m::VIRTUAL_VIRAMA  }, // Soyo subjoiner
    // Gonm
    { 0x11D45, uc::m::VIRTUAL_VIRAMA  }, // Gonm virtual virama
    { 0x11D46, uc::m::SAMPLED_CONTROL }, // Gonm repha
    // Gong
    { 0x11D97, uc::m::VIRTUAL_VIRAMA  },  // Gong virtual virama
    // Kawi
    { 0x11F02, uc::m::SAMPLED_CONTROL }, // Kawi repha
    { 0x11F42, uc::m::VIRTUAL_VIRAMA  }, // Kawi subjoiner
    // Egyp
    { 0x13431, uc::m::CUSTOM_CONTROL },  // Egyptian hiero horiz joiner
    { 0x13432, uc::m::CUSTOM_CONTROL },  // Egyptian hiero insert at top start
    { 0x13433, uc::m::CUSTOM_CONTROL },  // Egyptian hiero insert at bottom start
    { 0x13434, uc::m::CUSTOM_CONTROL },  // Egyptian hiero insert at top end
    { 0x13435, uc::m::CUSTOM_CONTROL },  // Egyptian hiero insert at bottom end
    { 0x13439, uc::m::CUSTOM_CONTROL },  // Egyptian hieroglyph Insert at middle
    { 0x1343A, uc::m::CUSTOM_CONTROL },  // Egyptian hieroglyph Insert at top
    { 0x1343B, uc::m::CUSTOM_CONTROL },  // Egyptian hieroglyph Insert at bottom
    { 0x1343C, uc::m::CUSTOM_CONTROL },  // Egyptian hieroglyph Begin enclosure
    { 0x1343D, uc::m::CUSTOM_CONTROL },  // Egyptian hieroglyph End enclosure
    { 0x1343E, uc::m::CUSTOM_CONTROL },  // Egyptian hieroglyph Begin walled enclosure
    { 0x1343F, uc::m::CUSTOM_CONTROL },  // Egyptian hieroglyph End walled enclosure
    { 0x13441, uc::m::SPACE },           // Egyptian hiero Full blank
    { 0x13442, uc::m::SPACE },           // Egyptian hiero Half blank
    // Plrd (Miao)
    { 0x16F8F, uc::m::CUSTOM_CONTROL },  // Miao tone right
    { 0x16F90, uc::m::CUSTOM_CONTROL },  // Miao tone top right
    { 0x16F91, uc::m::CUSTOM_CONTROL },  // Miao tone above
    { 0x16F92, uc::m::CUSTOM_CONTROL },  // Miao tone below
    // Dupl
    { 0x1BCA0, uc::m::CUSTOM_CONTROL },  // shorthand format Letter overlap
    { 0x1BCA1, uc::m::CUSTOM_CONTROL },  // shorthand format Continuing overlap
};

// Just capitalize them!
const std::unordered_set<std::string_view> cuneiformSymbols {
    "BAR", "LAM", "MU", "NU", "PI", "SA"
};

const std::unordered_set<std::string_view> cuneiformSuffixes {
    "TENU", "GUNU", "SHESHIG", "NUTILLU"
};

const std::unordered_set<std::string_view> cuneiformKeywords {
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

struct RangeByEnd {
    char32_t a = 0, b = 0;
    constexpr RangeByEnd() = default;
    constexpr RangeByEnd(char32_t aa) : a(aa), b(aa) {}
    constexpr RangeByEnd(char32_t aa, char32_t bb) : a(aa), b(bb) {}
};

constexpr bool operator < (const RangeByEnd& x, const RangeByEnd& y) { return (x.b < y.b); }

///  BUG_AVOID / BUG_FIXUP are used
const std::set<RangeByEnd> alternateRanges {
    { 0x0126 },             // H bar: Strange bar in Cambria
    { 0x1D66, 0x1D6A },     // Clipped below in Cambria
    { 0x1E00, 0x1E01 },     // Clipped below in Cambria
    { 0x1E5C, 0x1E5D },     // Misplaced umlaut in Cambria
    { 0x1E68, 0x1E69 },     // Same
    { 0x02CE, 0x02CF },     // Misrender in Cambria
    { 0x0333 },             // Basic umlauts: misrenders in Cambria
    { 0x2010, 0x203B },     // Several chars from Punctuation…
    { 0x203D, 0x203E },     // …Skip emoji and go on
    { 0x23B4, 0x23B6 },     // Technical — horz brackets, misrender in Cambria
    { 0x23DC, 0x23E0 },     // Same
    { 0x266D, 0x266F },     // Misc — use Funky font on musical symbols
    { 0x2C7C },             // Clipped below in Cambria
    // CJK compatibles: SimSun → BabelStone
    { 0xFE31, 0xFE34 },     // Some dashes
    { 0xFE49, 0xFE4F },     // More dashes
    // BS = BabelStone, SimSun = built-in Windows font, G = mainland China, T = Taiwan?
    // As BS is now primary font, destroying everything
    { 0x34F0  },    // CJK A: BS drew a hypotethical Chinese version
    { 0x2561E },    // CJK B: BS drew a hypotethical Chinese version
    { 0x256A1 },    // CJK B: SimSun-B drew more canonical glyph    

    // Mathematical: Cambria → Funky
    { 0x220F, 0x2211 },     // Big sums and products
    { 0x222B, 0x2233 },     // Integrals bunch 1
    { 0x2320, 0x2321 },     // Multi-storey integrals part 1
    { 0x239B, 0x23AE },     // Multi-storey ()[]{}, integral part 2
    { 0x23B0, 0x23B1 },     // Two-storey {}
    { 0x2A0A, 0x2A1C },     // Integrals bunch 2

    // Mathematical: Cambria → Noto
    { 0x2201 },         // set C
    { 0x2205 },         // empty set
    { 0x22DA, 0x22DB },
    { 0x27EC, 0x27ED },
    { 0x298B, 0x298C },
    { 0x2997, 0x2998 },
    { 0x2A79, 0x2AAD },  // greater-less
};

const std::set<RangeByEnd> noAaRanges {
    { 0x133FA, 0x1340C },   // Egyptian — sticks 1…9
};

constexpr auto TWO_STYLES = uc::Cfg::G_STYLE_0 | uc::Cfg::G_STYLE_1;

const std::map<RangeByEnd, uc::Cfgs> styleRanges {
    // Glagolitic: Bulgaria or Croatia
    { { 0x2C00,  0x2C5F  }, TWO_STYLES },   // Glag main
    { { 0x1E000, 0x1E02F }, uc::Cfg::G_STYLE_0 }, // Glag ex
    // Sogdian etc: horizontal or vertical
    { { 0x1800,  0x180A  }, TWO_STYLES },   // Mong
    { { 0x180B,  0x180F  }, uc::Cfg::G_STYLE_0 }, // A small hole in Mong
    { { 0x1810,  0x18AF  }, TWO_STYLES },   // Mong
    { { 0x11660, 0x1167F }, TWO_STYLES },   // Mong ex
    { { 0xA840,  0xA87F  }, TWO_STYLES },   // Phag
    { { 0x10F30, 0x10F6F }, TWO_STYLES },   // Sogd
    { { 0x10F70, 0x10FAF }, TWO_STYLES },   // Ougr
};

/// One method of homonym disambig: these chars are left as-are
/// while the rest are decapped by dictionary under Exf::LEAVE_BY_CONDITION
const std::unordered_set<char32_t> charsConditionalLeave {
    0x11451,    // Newa digit One = chi
    0x11454,    // Newa digit Four = pi
    0x11fc8,    // Tamil fraction One twentieth = maa
};


const std::unordered_map<char32_t, DepInfo> deprecatedInfo {
    // Latin A
    { 0x0149, { .whatsInstead = u8"\u02BC" "\u006E" } },
    // Arabic
    { 0x0673, { .whatsInstead = u8"\u0627" "\u065F" } },
    // Tibetan
    { 0x0F77, { .whatsInstead = u8"\u0FB2" "\u0F71" "\u0F80" } },
    { 0x0F79, { .whatsInstead = u8"\u0FB3" "\u0F71" "\u0F80" } },
    // Khmer
    { 0x17A3, { .whatsInstead = u8"\u17A2" } },
    { 0x17A4, { .whatsInstead = u8"\u17A2" "\u17B6" } },
    // General punctuation
    { 0x206A, { .whatsInstead = u8"2" } },
    { 0x206B, { .whatsInstead = u8"1" } },
    { 0x206C, { .whatsInstead = u8"1" } },
    { 0x206D, { .whatsInstead = u8"3" } },
    { 0x206E, { .whatsInstead = u8"4" } },
    { 0x206F, { .whatsInstead = u8"1" } },
    // Misc tech
    { 0x2329, { .whatsInstead = u8"\u27E8", .whatsInstead2 = u8"\u3008" } },
    { 0x232A, { .whatsInstead = u8"\u27E9", .whatsInstead2 = u8"\u3009" } },
    // Tags
    { 0xE0001, { .whatsInstead = u8"5" } },
    // -- total 15
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
    "Basketo",           // баскето (Эфиопия)
    "Berber",
    "Belarusian",       ///< =Byelorussian
    "Bihari languages", // бихарские (Индия)
    "Bosnian",
    "Bulgarian",
    "Burushaski",       // бурушаски (Кашмир)
    "Byelorussian",
    "Carrier",          ///< кэрриер (Канада), better stop?
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
    "Inuktitut",    ///< better stop?
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
    "Kuvi",         // куви (Индия)
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
    "Shina",        // шина (Кашмир)
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

    enum class WordState { SMALL, TITLE, CUSTOM_TITLE, CUSTOM, ALL_CAP };

    struct Word {
        std::string_view original, customCap;
        bool isAllCap = false;
        bool isCapital = false;
        bool nextNoun = false;
        bool nextAdjective = false;
        bool nextNumeral = false;
        bool isAdjective = false;
        Flags<Dicf> dicFlags;
    };

    constexpr auto TRIG_ANY_SCRIPT = Dicf::TRIG_SCRIPT | Dicf::TRIG_SCRIPT_IF_FIRST
            | Dicf::TRIG_TWO_WORD_SCRIPT | Dicf::TRIG_THREE_WORD_SCRIPT
            | Dicf::TRIG_SCRIPT_ADJECTIVE;

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

    /// @param [in] wantWordAfter  [+] want at least one word after idiom
    bool doesIdiomFit(const SafeVector<Word>& words, size_t pos,
                    Buf1d<const std::string_view> idiom,
                    bool wantWordAfter)
    {
        size_t wantedSize = idiom.size() + pos + static_cast<int>(wantWordAfter);
        return words.size() >= wantedSize
                && std::equal(idiom.begin(), idiom.end(), words.begin() + pos,
                            [](std::string_view x, const Word& y) {
                                return x == y.original;
                            });
    }

    void doIdiomAction(SafeVector<Word>& words,
                       size_t iStart, size_t iEndPlus,
                       IdiomAction action)
    {
        // Prefix actions remove appropriate dictionary flags,
        // as they are more important than dic. properties
        switch (action) {
        case IdiomAction::REST_SMALL:
            for (size_t i = iEndPlus; i < words.size(); ++i)
                words[i].isCapital = false;
            break;
        case IdiomAction::SECOND_THIRD_SMALL:
            for (size_t i = iStart + 1; i < iEndPlus; ++i) {
                auto& w = words[i];
                w.isCapital = false;
            } break;
        case IdiomAction::REST_CAP:
            for (size_t i = iEndPlus; i < words.size(); ++i) {
                auto& w = words[i];
                w.isCapital = true;
                w.dicFlags.remove(Dicf::CAP_SMALL);
            }
            break;
        case IdiomAction::REST_DETECT_LETTERS:
            for (size_t i = iEndPlus; i < words.size(); ++i) {
                auto& w = words[i];
                if (w.original.length() == 1) {
                    w.isCapital = true;
                }
            }
            break;
        case IdiomAction::REST_ALLCAP:
            for (size_t i = iEndPlus; i < words.size(); ++i) {
                auto& w = words[i];
                w.isAllCap = true;
                w.dicFlags.remove(Dicf::CAP_SMALL);
            }
            break;
        case IdiomAction::REST_CAPSMALL:
            for (size_t i = iEndPlus + 1; i < words.size(); ++i)
                words[i].isCapital = false;
            [[fallthrough]];
        case IdiomAction::NEXT_CAP: {
                auto& wp = words[iEndPlus];
                wp.isCapital = true;
                wp.dicFlags.remove(Dicf::CAP_SMALL);
            } break;
        case IdiomAction::NEXT_SMALL:
            words[iEndPlus].isCapital = false;
            break;
        case IdiomAction::FIRST_CAP:
            words[iStart].isCapital = true;
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

    [[maybe_unused]] void dump(const SafeVector<Word>& words)
    {
        for (auto& v : words) {
            std::cout << v.original << " ";
            dump(v.dicFlags);
            std::cout << '\n';
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

    bool isFullDecap(const SafeVector<std::string_view>& words)
    {
        for (auto& word : words) {
            auto itWord = dictionary.find(word);
            if (itWord != dictionary.end()) {
                if (itWord->second.flags.have(Dicf::TRIG_FORCE_FULL_DECAP))
                    return true;
            }
        }
        return false;
    }

}   // anon namespace


size_t nExceptions() { return exceptions.size(); }


std::string decapitalize(
        std::string_view x, char32_t cp,
        Flags<Dcfg> flags, DecapDebug debug)
{
    // Detect exceptions
    std::string upper = str::toUpper(x);
    auto itEx = exceptions.find(upper);
    if (itEx != exceptions.end()) {
        if (cp || !itEx->second.flags.have(Exf::CPONLY) ) {
            // Mixcase flag: in upcase mode leave, in locase mode to lower
            if (itEx->second.flags.have(Exf::MIXCASE) && flags.have(Dcfg::LOCASE)) {
                return str::toLower(itEx->second.r);
            }
            if (itEx->second.flags.have(Exf::LEAVE_BY_CONDITION)
                    && charsConditionalLeave.contains(cp)) {
                return std::string{x};
            }
            return std::string(itEx->second.r);
        }
    }

    // Get words
    auto rawWords = str::splitSv(upper, ' ');
    if (rawWords.empty())
        return {};

    // Check: do we need to trigger normal decapitalization?
    if (flags.have(Dcfg::SHORTCUT)) {
        if (!isFullDecap(rawWords))
            return std::string{x};
    }

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

        if (str::isIndex(word.original))
            word.isCapital = true;

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
                }
                // Works together with other flags
                if (word.dicFlags.have(Dicf::TRIG_SCRIPT_ADJECTIVE)) {
                    // Propagate adjective that are script parts
                    // OLD SOUTH Arabian
                    if (nextWord) {
                        // Cascade capitalization: OLD North Arabian
                        if ((nextWord->isCapital && nextWord->dicFlags.have(Dicf::TRIG_SCRIPT_ADJECTIVE))
                                    // First-party capitalization: NORTH Arabian
                                 || (nextWord->dicFlags.have(Dicf::TRIG_SCRIPT))) {
                            word.isCapital = true;
                            hasScript = true;
                        }
                    }
                }
            }
            // Check whether digits propagate right
            if (word.dicFlags.have(Dicf::TRIG_DIGIT_PROPAGATES_RIGHT)) {
                digitPropagatesRight = true;
            }
            // Check for custom capitalization
            if (!wordInfo.customCap.empty()) {
                word.customCap = wordInfo.customCap;
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
                word.customCap = w;
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

    // Check for idioms
    for (size_t posIdiom = 0; posIdiom < words.size(); ++posIdiom) {
        IdiomEntry idiomKey { words[posIdiom].original, IsPrefix::NO, IdiomAction::NOMATTER };
        auto range = idioms.equal_range(idiomKey);
        for (auto it = range.first; it != range.second; ++it) {
            auto& idiom = *it;
            if ((!idiom.isPrefix() || posIdiom == 0)           // Prefix?
                    && cp >= idiom.lo && cp <= idiom.hi      // Code point OK?
                    && doesIdiomFit(words, posIdiom, idiom.toBuf(), idiom.doesNeedWordAfter())) {
                doIdiomAction(words, posIdiom, posIdiom + idiom.nTriggers, idiom.action);
                break;
            }
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
            } else if (q != dictionary.end() && q->second.flags.haveAny(TRIG_ANY_SCRIPT
                        | Dicf::PART_ADJECTIVE | Dicf::PART_NOUN | Dicf::PART_NUMERAL)) {
                // do nothing
            // 3. Suffix — initial capital
            } else if (cuneiformSuffixes.contains(word.original)) {
                word.isCapital = true;
            // 4. Keyword — YES
            } else if (!cuneiformKeywords.contains(word.original)) {
                word.isAllCap = true;
            }
        }
    }

    // Compose
    std::string r;
    r.reserve(newLen);
    for (auto& word : words) {
        if (!r.empty())
            r.append(" "sv);

        WordState state = WordState::SMALL;        
        if (word.isAllCap) {
            state = WordState::ALL_CAP;
        } else if (word.dicFlags.have(Dicf::CAP_SMALL)) {
            state = WordState::SMALL;
        } else if (word.dicFlags.have(Dicf::CAP_ALL)) {
            state = WordState::ALL_CAP;
        } else if (word.dicFlags.have(Dicf::CAP_TITLE)) {
            state = WordState::TITLE;
        } else if (!word.customCap.empty()) {
            state = word.isCapital ? WordState::CUSTOM_TITLE : WordState::CUSTOM;
        } else if (word.isCapital) {
            state = WordState::TITLE;
        }

        switch (state) {
        case WordState::CUSTOM:
            r.append(word.customCap);
            break;
        case WordState::CUSTOM_TITLE:
            appendAsIsTitle(r, word.customCap);
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


bool isAlternate(char32_t x)
{
    auto it = alternateRanges.lower_bound(RangeByEnd{x,x});
    if (it == alternateRanges.end())
        return false;
    // so x <= it->b
    // upper_bound gives x < it->b
    return (it->a <= x);
}


bool isNoAa(char32_t x)
{
    auto it = noAaRanges.lower_bound(RangeByEnd{x,x});
    if (it == noAaRanges.end())
        return false;
    // so x <= it->b
    // upper_bound gives x < it->b
    return (it->a <= x);
}


std::string decapitalizeEmoji(
        std::string_view x,
        DecapDebug debug)
{
    auto list = str::splitSv(x, ": ", false);
    if (list.size() == 1) {
        return decapitalize(x, 0, debug);
    }
    std::string r;
    for (auto v : list) {
        if (!r.empty())
            r += ": ";
        auto decap = decapitalize(v, 0, debug);
        r += decap;
    }
    return r;
}


std::string decapitalizeByTable(std::string_view x)
{
    std::string upper = str::toUpper(x);
    auto itEx = exceptions.find(upper);
    if (itEx != exceptions.end())
        return std::string(itEx->second.r);
    return std::string{x};
}


Flags<uc::Cfg> styleFlags(char32_t x)
{
    auto it = styleRanges.lower_bound(RangeByEnd{x,x});
    if (it != styleRanges.end()) {
        // so x <= it->b
        // upper_bound gives x < it->b
        if (it->first.a <= x) {
            return it->second;
        }
    }
    return {};
}
