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
    { "ABKHASIAN",      Dicf::TRIG_SCRIPT },
    { "ADLAM",          Dicf::TRIG_SCRIPT },
    { "AFAKA",          Dicf::TRIG_SCRIPT },
    { "ANATOLIAN",      Dicf::TRIG_SCRIPT },
    { "ALEUT"sv,        Dicf::TRIG_SCRIPT },
    { "ALBANIAN",       Dicf::TRIG_SCRIPT },
      { "CAUCASIAN",    Dicf::TRIG_SCRIPT },
    { "AHOM",           Dicf::TRIG_SCRIPT },
    { "APL",            Dicf::CAP_ALL | Dicf::TRIG_SCRIPT },
    { "ARABIC",         Dicf::TRIG_SCRIPT },
    { "ARABIAN",        Dicf::TRIG_SCRIPT },
    { "ARABIC-INDIC",   { Dicf::TRIG_SCRIPT, "Arabic-Indic"sv } },
    { "IMPERIAL",       Dicf::TRIG_SCRIPT },
    { "ARAMAIC",        Dicf::TRIG_SCRIPT },
    { "ARMENIAN",       Dicf::TRIG_SCRIPT },
    { "AVESTAN",        Dicf::TRIG_SCRIPT },
    { "BALINESE",       Dicf::TRIG_SCRIPT },
    { "BAMUM",          Dicf::TRIG_SCRIPT },
    { "BASSA",          Dicf::TRIG_SCRIPT },
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
    { "CIRTH",          Dicf::TRIG_SCRIPT },
    { "CJK",            Dicf::CAP_ALL | Dicf::TRIG_SCRIPT },
    { "COPTIC",         Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE },
    { "CUNEIFORM",      Dicf::TRIG_SCRIPT | Dicf::CAP_SMALL },
    { "CYPRO-MINOAN",   { Dicf::TRIG_SCRIPT, "Cypro-Minoan"sv } },
    { "CYPRIOT",        Dicf::TRIG_SCRIPT },
    { "CYRILLIC",       Dicf::TRIG_SCRIPT },
      { "KOMI"sv,       Dicf::TRIG_SCRIPT },
      { "UKRAINIAN",    Dicf::TRIG_SCRIPT },
      { "BYELORUSSIAN-UKRAINIAN", { Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE, "Byelorussian-Ukrainian"sv } },
      { "BASHKIR",      Dicf::TRIG_SCRIPT },
    { "DEVANAGARI",     Dicf::TRIG_SCRIPT },
    { "DOGRA",          Dicf::TRIG_SCRIPT },
    { "DESERET",        Dicf::TRIG_SCRIPT },
    { "DUPLOYAN",       Dicf::TRIG_SCRIPT },
    { "DIVES",          Dicf::TRIG_SCRIPT },
     { "AKURU",         Dicf::TRIG_SCRIPT },
    { "EGYPTIAN",       Dicf::TRIG_SCRIPT },
    { "ELBASAN",        Dicf::TRIG_SCRIPT },
    { "ETHIOPIC",       Dicf::TRIG_SCRIPT },
      { "GURAGE",       Dicf::TRIG_SCRIPT | Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "KHUTSURI",       Dicf::TRIG_SCRIPT },    // Unused, but…
    { "FRAKTUR",        Dicf::TRIG_SCRIPT },
    { "GEORGIAN",       Dicf::TRIG_SCRIPT },
     { "MTAVRULI",      Dicf::TRIG_SCRIPT },
    { "GUNJALA",        Dicf::TRIG_SCRIPT },
    { "GONDI",          Dicf::TRIG_SCRIPT },
    { "MASARAM",        Dicf::TRIG_SCRIPT },
    { "GOTHIC",         Dicf::TRIG_SCRIPT },
    { "GLAGOLITIC",     Dicf::TRIG_SCRIPT },
    { "GRANTHA",        Dicf::TRIG_SCRIPT },
    { "GREEK",          Dicf::TRIG_SCRIPT_IF_FIRST | Dicf::PART_ADJECTIVE | Dicf::CAP_TITLE },
      { "PAMPHYLIAN",     Dicf::TRIG_SCRIPT },
      { "ATTIC",          Dicf::TRIG_SCRIPT },
      { "TROEZENIAN",     Dicf::TRIG_SCRIPT },
      { "THESPIAN",       Dicf::TRIG_SCRIPT },
      { "EPIDAUREAN",     Dicf::TRIG_SCRIPT },
      { "HERMIONIAN",     Dicf::TRIG_SCRIPT },
    { "GUJARATI",       Dicf::TRIG_SCRIPT },
    { "GURMUKHI",       Dicf::TRIG_SCRIPT },
    { "HANGUL",         Dicf::TRIG_SCRIPT },
    { "HANIFI",         Dicf::TRIG_SCRIPT },
     { "ROHINGYA",      Dicf::TRIG_SCRIPT },
    { "HANGZHOU",       Dicf::TRIG_SCRIPT },
    { "HANUNOO",        Dicf::TRIG_SCRIPT },
    { "HATRAN",         Dicf::TRIG_SCRIPT },
    { "HEBREW",         Dicf::TRIG_SCRIPT },
    { "HIRAGANA",       Dicf::TRIG_SCRIPT },
    { "HENTAIGANA",     Dicf::TRIG_SCRIPT },
    { "PAHAWH",         Dicf::TRIG_SCRIPT },
    { "HMONG",          Dicf::TRIG_SCRIPT },
     { "NYIAKENG",      Dicf::TRIG_SCRIPT },
     { "PUACHUE",       Dicf::TRIG_SCRIPT },
    { "HUNGARIAN",      Dicf::TRIG_SCRIPT },
    { "INDIC",          Dicf::TRIG_SCRIPT },
    { "INDUS",          Dicf::TRIG_SCRIPT },
    { "OLD",            { Dicf::PART_ADJECTIVE | Dicf::TRIG_TWO_WORD_SCRIPT, {}, "ITALIC"sv } },
    { "JAMO",           Dicf::TRIG_SCRIPT },
    { "JAVANESE",       Dicf::TRIG_SCRIPT },
    { "JAPANESE",       Dicf::TRIG_SCRIPT },
    { "KANGXI",         Dicf::TRIG_SCRIPT },
    { "KANNADA",        Dicf::TRIG_SCRIPT },
    { "KATAKANA",       Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE },
    { "KATAKANA-HIRAGANA", { Dicf::TRIG_SCRIPT, "Katakana-Hiragana"sv } },
    { "KAYAH",          { Dicf::TRIG_TWO_WORD_SCRIPT | Dicf::PART_ADJECTIVE | Dicf::CAP_TITLE, {}, "LI"sv } },
    { "KHAROSHTHI",     Dicf::TRIG_SCRIPT },
    { "KHMER",          Dicf::TRIG_SCRIPT },
    { "KHOJKI",         Dicf::TRIG_SCRIPT },
    { "KHITAN",         Dicf::TRIG_SCRIPT },
    { "KHUDAWADI",      Dicf::TRIG_SCRIPT },
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
    { "MANDAIC",        Dicf::TRIG_SCRIPT },
    { "MANDAEAN",       Dicf::TRIG_SCRIPT },
    { "MANICHAEAN",     Dicf::TRIG_SCRIPT },
    { "MARCHEN",        Dicf::TRIG_SCRIPT },
    { "MEDEFAIDRIN",    Dicf::TRIG_SCRIPT },
    { "MEETEI",         Dicf::TRIG_SCRIPT },
      { "MAYEK",        Dicf::TRIG_SCRIPT },
    { "MENDE",          Dicf::TRIG_SCRIPT | Dicf::TRIG_DIGIT_PROPAGATES_RIGHT },
      { "KIKAKUI",      Dicf::TRIG_SCRIPT | Dicf::TRIG_DIGIT_PROPAGATES_RIGHT },
    { "MEROITIC",       Dicf::TRIG_SCRIPT },
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
    { "NUBIAN",         Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE },
    { "NUSHU",          Dicf::TRIG_SCRIPT },
    { "OGHAM",          Dicf::TRIG_SCRIPT },
    { "OL",             { Dicf::TRIG_TWO_WORD_SCRIPT, {}, "CHIKI"sv } },
    { "TURKIC",         Dicf::TRIG_SCRIPT },
    { "ORIYA",          Dicf::TRIG_SCRIPT },
    { "OSAGE",          Dicf::TRIG_SCRIPT },
    { "OSMANYA",        Dicf::TRIG_SCRIPT },
    { "PALI",           Dicf::TRIG_SCRIPT | Dicf::PART_ADJECTIVE },
    { "PALMYRENE",      Dicf::TRIG_SCRIPT },
    { "PAU",            { Dicf::TRIG_THREE_WORD_SCRIPT, {}, "CIN"sv, "HAU"sv } },
    { "PERMIC",         Dicf::TRIG_SCRIPT },
    { "PERSIAN",        Dicf::TRIG_SCRIPT },
    { "PHAISTOS",       { Dicf::TRIG_TWO_WORD_SCRIPT, {}, "DISC" } },
    { "PHAGS-PA",       Dicf::TRIG_SCRIPT },
    { "PAHLAVI",        Dicf::TRIG_SCRIPT },
    { "PHOENICIAN",     Dicf::TRIG_SCRIPT },
    { "KLINGON",        Dicf::TRIG_SCRIPT },
    { "MIAO",           Dicf::TRIG_SCRIPT },
    { "PARTHIAN",       Dicf::TRIG_SCRIPT },
    { "REJANG",         Dicf::TRIG_SCRIPT },
    { "RONGORONGO",     Dicf::TRIG_SCRIPT },
    { "RUNIC",          Dicf::TRIG_SCRIPT },
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
    { "SIYAQ",          Dicf::TRIG_SCRIPT },
    { "SORA",           { Dicf::TRIG_TWO_WORD_SCRIPT, {}, "SOMPENG"sv } },
    { "SOYOMBO",        Dicf::TRIG_SCRIPT },
    { "SUNDANESE",      Dicf::TRIG_SCRIPT },
    { "SYLOTI",         Dicf::TRIG_SCRIPT },
    { "NAGRI",          Dicf::TRIG_SCRIPT },
    { "SOGDIAN",        Dicf::TRIG_SCRIPT },
    { "SYRIAC",         Dicf::TRIG_SCRIPT },
    { "TAGBANWA",       Dicf::TRIG_SCRIPT },
    { "TAKRI",          Dicf::TRIG_SCRIPT },
    { "TAI",            { Dicf::TRIG_TWO_WORD_SCRIPT, {}, "LE"sv, "LUE"sv, "VIET"sv, "THAM"sv, "LAING"sv } },
      { "LAING",        Dicf::IF_SCRIPT | Dicf::TRIG_CAP_NEXT | Dicf::IF_NEXT_NOT_NOUN },
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
    { "TOTO",           Dicf::TRIG_SCRIPT },
    { "UIGHUR",         Dicf::TRIG_SCRIPT | Dicf::CAP_TITLE },
    { "UYGHUR",         Dicf::TRIG_SCRIPT | Dicf::CAP_TITLE },
    { "UGARITIC",       Dicf::TRIG_SCRIPT },
    { "VAI",            Dicf::TRIG_SCRIPT },
    { "VIETNAMESE",     Dicf::TRIG_SCRIPT | Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "VITHKUQI",       Dicf::TRIG_SCRIPT },
    { "WARANG",         { Dicf::TRIG_TWO_WORD_SCRIPT, {}, "CITI"sv } },
    { "WANCHO",         Dicf::TRIG_SCRIPT },
    { "YI"sv,           Dicf::TRIG_SCRIPT_IF_FIRST },
    { "ZANABAZAR",      Dicf::TRIG_SCRIPT },
    { "ZNAMENNY",       Dicf::TRIG_SCRIPT },

    { "OCR"sv,          Dicf::CAP_ALL },
    { "MICR"sv,         Dicf::CAP_ALL },

    // Capitalize next
    { "AFFIX",          Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
    { "CHARACTER",      Dicf::IF_SCRIPT | Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
    { "CHOSEONG",       Dicf::IF_SCRIPT | Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
      { "JUNGSEONG",    Dicf::IF_SCRIPT | Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
      { "JONGSEONG",    Dicf::IF_SCRIPT | Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
    { "DIGIT",          Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL  },
    { "FRACTION",       Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
    { "HIEROGLYPH",     Dicf::IF_SCRIPT | Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
    { "IDEOGRAM",       Dicf::IF_SCRIPT | Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
    { "LETTER",         Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
    { "DIGRAPH",        Dicf::TRIG_CAP_NEXT | Dicf::PART_NOUN | Dicf::CAP_SMALL },
    /// @todo [spelling] Ligature Alpha Bravo — how to kick?
    { "LIGATURE"sv,     Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
    { "LOGICAL"sv,      Dicf::TRIG_CAP_NEXT },
    { "LOGOGRAM"sv,     Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
    { "MARK"sv,         Dicf::PART_NOUN | Dicf::IF_SCRIPT | Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
    { "NEUME",          Dicf::TRIG_CAP_NEXT | Dicf::CAP_SMALL },
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
    { "AIVILIK",        Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "ATHAPASCAN",     Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "BLACKFOOT",      Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
      // Who knows what to do with Carrier (Canadian tribe)
    { "CARRIER",        Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE | Dicf::TRIG_SCRIPT },
      // CREE here
      { "BIBLE-CREE",   { Dicf::PART_ADJECTIVE, "Bible-Cree" } },
      { "MOOSE-CREE",   { Dicf::PART_ADJECTIVE, "Moose-Cree" } },
      { "N-CREE"sv,     { Dicf::PART_ADJECTIVE, "N-Cree" } },
      { "R-CREE"sv,     { Dicf::PART_ADJECTIVE, "R-Cree" } },
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
    { "AKHMIMIC",       Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "ASSYRIAN",       Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "BOHAIRIC",       Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "ELAMITE",        Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "JAIN",           Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
    { "SEBATBEIT",      Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },

    // Nouns
    { "ANUSVARA",       Dicf::PART_NOUN },
    { "ASPIRATION",     Dicf::PART_NOUN },
    { "APOSTROPHE",     Dicf::PART_NOUN },
    { "AXE",            Dicf::PART_NOUN },  // For Phaistos Disc
    { "ARROW",          Dicf::PART_NOUN },
    { "ARROWHEAD",      Dicf::PART_NOUN },
    { "BALUDA",         Dicf::PART_NOUN },
    { "BAR",            Dicf::PART_NOUN },
    { "BEAT",           Dicf::PART_NOUN },
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
    { "FORM",           Dicf::PART_NOUN },
    { "HAMZA",          Dicf::PART_NOUN },  // Arabic, not really letter
    { "HASANTA",        Dicf::PART_NOUN },
    { "HOOK",           Dicf::PART_NOUN },
    { "LINE",           Dicf::PART_NOUN },
    { "MCHU",           Dicf::PART_NOUN },
    { "PLUS",           Dicf::PART_NOUN },
    { "REDUPLICATION",  Dicf::PART_NOUN },
    { "RIKRIK",         Dicf::PART_NOUN },
    { "RING",           Dicf::PART_NOUN },
    { "SECANT",         Dicf::PART_NOUN },
    { "SECTION",        Dicf::PART_NOUN },
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
    { "UBADAMA",        Dicf::PART_NOUN },
    { "UNDERBAR",       Dicf::PART_NOUN },
    { "VIRAMA",         Dicf::PART_NOUN },
    { "VISARGA",        Dicf::PART_NOUN },
    { "VOICING",        Dicf::PART_NOUN },
    { "WAVE",           Dicf::PART_NOUN },
    { "X-X",            Dicf::PART_NOUN | Dicf::CAP_ALL },
    { "YANG",           Dicf::PART_MIXED },  // for Chinese tones, need more things
    //{ "YAT",            Dicf::PART_NOUN }, // Yat is normal (Capitalized)…
    { "YIG",            Dicf::PART_NOUN },
    { "YIN",            Dicf::PART_NOUN },   // for Chinese tones, seems to be OK
    { "YUS",            Dicf::PART_NOUN },   // …but yus is big & little → do not cap

    // Tricky but work as noun
    { "PERCUSSIVE",     Dicf::PART_NOUN },

    // Adverbs
    { "ABOVE",          Dicf::PART_NOUN },
    { "BELOW",          Dicf::PART_NOUN },

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
    { "TURN",           Dicf::PART_NOUN },

    // Adjectives
    { "ACUTE",          Dicf::PART_ADJECTIVE },
    { "ALTERNATE",      Dicf::PART_ADJECTIVE },
    { "ALVEOLAR",       Dicf::PART_ADJECTIVE },
    { "ARCHAIC",        Dicf::PART_ADJECTIVE },
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
    { "DOUBLE",         Dicf::PART_ADJECTIVE },
    { "DOUBLE-STRUCK",  Dicf::PART_ADJECTIVE },
    { "DOTTED",         Dicf::PART_ADJECTIVE },
    { "DOTLESS",        Dicf::PART_ADJECTIVE },
    { "DOWN",           Dicf::PART_ADJECTIVE },
    { "DIAERESIZED",    Dicf::PART_ADJECTIVE },
    { "DIAGONAL",       Dicf::PART_ADJECTIVE },
    { "DIALECT-P",      { Dicf::PART_ADJECTIVE, "dialect-P" } },
    { "EXCLAMATION",    Dicf::PART_ADJECTIVE },
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
    { "INSCRIPTIONAL",  Dicf::PART_ADJECTIVE },
    { "INSULAR",        Dicf::PART_ADJECTIVE },
    { "INVERTED",       Dicf::PART_ADJECTIVE },
    { "L-SHAPED",       Dicf::PART_ADJECTIVE | Dicf::CAP_TITLE },
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
    { "LOW",            Dicf::PART_ADJECTIVE },
    { "LUNATE",         Dicf::PART_ADJECTIVE },
    { "LITTLE",         Dicf::PART_ADJECTIVE },
    { "MAI",            Dicf::PART_ADJECTIVE },
    { "MALAYALAM",      Dicf::PART_ADJECTIVE },
    { "MARWARI",        Dicf::PART_ADJECTIVE },
    { "MEDIAL",         Dicf::PART_ADJECTIVE },
    { "MID",            Dicf::PART_ADJECTIVE },
    { "MIDDLE",         Dicf::PART_ADJECTIVE },
    { "MONOGRAPH",      Dicf::PART_ADJECTIVE },
    { "NASAL",          Dicf::PART_ADJECTIVE },
    { "NEW",            Dicf::PART_ADJECTIVE },
    { "NORTH",          Dicf::PART_ADJECTIVE },
    { "NORTHERN",       Dicf::PART_ADJECTIVE },
    { "OBLIQUE",        Dicf::PART_ADJECTIVE },
    { "OPEN",           Dicf::PART_ADJECTIVE },
    { "OVERLONG",       Dicf::PART_ADJECTIVE },
    { "PALATAL",        Dicf::PART_ADJECTIVE },
    { "PERNIN",         Dicf::PART_ADJECTIVE | Dicf::CAP_TITLE },
    { "PHARYNGEAL",     Dicf::PART_ADJECTIVE },
    { "PHASE-A",        { Dicf::PART_ADJECTIVE, "phase-A" } },
    { "PHASE-B",        { Dicf::PART_ADJECTIVE, "phase-B" } },
    { "PHASE-C",        { Dicf::PART_ADJECTIVE, "phase-C" } },
    { "PHASE-D",        { Dicf::PART_ADJECTIVE, "phase-D" } },
    { "PHASE-E",        { Dicf::PART_ADJECTIVE, "phase-E" } },
    { "PHASE-F",        { Dicf::PART_ADJECTIVE, "phase-F" } },
    { "PSALTER",        Dicf::PART_ADJECTIVE },
    { "QUARTER",        Dicf::PART_ADJECTIVE }, // Not really OK, but works somehow
    { "REFORMED",       Dicf::PART_ADJECTIVE },
    { "REVERSE",        Dicf::PART_ADJECTIVE },
    { "REVERSED",       Dicf::PART_ADJECTIVE },
    { "RETROFLEX",      Dicf::PART_ADJECTIVE },
    { "RIGHT",          Dicf::PART_ADJECTIVE },
    { "RIGHT-STEM",     Dicf::PART_ADJECTIVE },
    { "ROMANIAN",       Dicf::PART_ADJECTIVE },
    { "ROUNDED",        Dicf::PART_ADJECTIVE },
    { "SARA",           Dicf::PART_ADJECTIVE }, // Used for Thai
    { "SCRIPT",         Dicf::PART_MIXED },
    { "SEMISOFT",       Dicf::PART_ADJECTIVE },
    { "SHARP",          Dicf::PART_ADJECTIVE },
    { "SHORT",          Dicf::PART_ADJECTIVE },
    { "SHORT-TWIG-AR",  Dicf::PART_ADJECTIVE }, // used for runes
    /// @todo [decap] Arabic mark Sideways Noon gnunna — is it right?
    { "SIDEWAYS",       Dicf::PART_ADJECTIVE },
    { "SIGMOID",        Dicf::PART_ADJECTIVE },
    { "SLOAN",          Dicf::PART_ADJECTIVE },
    { "SMALL",          Dicf::PART_ADJECTIVE },
    { "SNAP",           Dicf::PART_ADJECTIVE }, // OK, music only
    { "SPACING",        Dicf::PART_ADJECTIVE },
    { "SOFT",           Dicf::PART_ADJECTIVE },
    { "SOUTH",          Dicf::PART_ADJECTIVE },
    { "SOUTHERN",       Dicf::PART_ADJECTIVE },
    { "STRAIGHT",       Dicf::PART_ADJECTIVE },
    { "STRETCHED",      Dicf::PART_ADJECTIVE },
    { "SYLLABIC",       Dicf::PART_ADJECTIVE },
    { "TAILED",         Dicf::PART_ADJECTIVE },
    { "TALL",           Dicf::PART_ADJECTIVE },
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
    { "KAZAKH",         Dicf::CAP_TITLE | Dicf::PART_ADJECTIVE },
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

    // Znamennoye
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
    { "I",              Dicf::PART_MAYBE_LETTER },  // Because of Znamenny
    { "J",              Dicf::CAP_TITLE },
    { "JEEM",           Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "K",              Dicf::CAP_TITLE },
    { "KAF",            Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "KHAH",           Dicf::CAP_TITLE | Dicf::PART_NOUN },
    { "L",              Dicf::CAP_TITLE },
    { "LLA",            Dicf::CAP_TITLE | Dicf::PART_NOUN },
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
    { "WITH",           Dicf::CAP_SMALL },
    //{ "AND",            Dicf::CAP_SMALL },    // Tricky word, all And’s are tested manually
    { "JOINER",         Dicf::CAP_SMALL },
    //{ "ABOVE",          Dicf::CAP_SMALL },    // Tricky word, all above’s are tested manually
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
    EX("Hangul choseong filler")                    // Simpler to add exception
    EX("Hangul jungseong filler")                   // Same
    EX("Circled CC")                                // Would be Cc
    EX("Raised MC sign")                            // Same
    EX("Raised MD sign")
    EX("Raised MR sign")
    EX("Square Hiragana Hoka")                      // Better exception
    EX("Squared Katakana Koko")
    EX("Squared Katakana Sa")
    // U14
    EX("Arabic pound mark above")                   // No adverbs here in data
    EX("Arabic piastre mark above")                 // Same
    EX("Glagolitic capital letter Caudate Chrivi")  // Don’t want to encode Chrivi
    EX("Glagolitic small letter Caudate Chrivi")    // Same
    EX("Angle with S inside")                       // Because of Znamennoye, S is tricky
    EX("Perpendicular with S")                      // Same
    EX("Duployan letter D S")                       // Same
    EX("Duployan letter R S")                       // Same
    EX("Duployan letter M S")                       // Same
    EX("Duployan letter N S")                       // Same
    EX("Duployan letter J S")                       // Same
    EX("Duployan letter S S")                       // Same
    EX("Duployan letter M N S")                     // Same
    EX("Duployan letter N M S")                     // Same
    EX("Duployan letter J M S")                     // Same
    EX("Duployan letter S J S")                     // Same
    EX("Duployan letter J S with dot")              // Same
    EX("Duployan letter J N S")                     // Same
    EX("Duployan letter T S")                       // Same
    EX("Duployan letter T R S")                     // Same
    EX("Duployan letter K R S")                     // Same
    EX("Duployan letter G R S")                     // Same
    EX("Duployan affix Attached I hook")            // Same
    EX("Duployan thick letter selector")            // A clear miss of my algo, IDK what to do
    EX("Bamum letter Phase-A Mon nggeuaet")         // Mon is also Myanmar nation
    EX("Miao tone right")                           // A clear miss of my algorithm, actually a semi-format tone combiner
    EX("Miao tone top right")                       // Same
    EX("Miao tone above")                           // Same
    EX("Miao tone below")                           // Same
    EX("Shorthand format Letter overlap")           // Letter is not low-case!
    EX("Znamenny neume Dva v chelnu")               // Better not to make exception from V
};

/// @warning DO NOT REMOVE sv, w/o does not work and IDK how to ban
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
    { { "RUNIC"sv, "LETTER"sv, "FRANKS"sv, "CASKET"sv }, PrefixAction::REST_CAP },      // prevent next!
    { { "RUNIC"sv, "LETTER"sv }, PrefixAction::REST_CAP },
    { { "CIRCLED"sv, "KATAKANA"sv }, PrefixAction::NEXT_CAP },  // IDK what to do, normal rules fail
    { { "CJK"sv, "STROKE"sv }, PrefixAction::REST_ALLCAP },
    { { "MIAO"sv, "LETTER"sv, "YI"sv }, PrefixAction::NEXT_CAP }, // Yi is ambiguous
    { { "SQUARE"sv, "ERA"sv, "NAME"sv }, PrefixAction::NEXT_CAP },
    { { "SQUARE"sv }, 0x3300, 0x3357, PrefixAction::NEXT_CAP },     // And what to do with those Katakana chars?
    { { "CYPRO-MINOAN"sv, "SIGN"sv }, PrefixAction::REST_ALLCAP },
    { { "LINEAR"sv, "A"sv, "SIGN"sv }, PrefixAction::REST_CAP },
    { { "LINEAR"sv, "B"sv, "SYLLABLE"sv }, PrefixAction::REST_CAP },
    { { "PAHAWH"sv, "HMONG"sv, "CONSONANT"sv }, PrefixAction::NEXT_CAP },   // Consonant is tricky
    { { "SHORTHAND"sv, "FORMAT"sv }, PrefixAction::REST_CAPSMALL },
    { { "ZNAMENNY"sv, "PRIZNAK"sv, "MODIFIER"sv }, PrefixAction::NEXT_CAP },
};


const std::map<char32_t, std::string_view> abbrevs {
    { 9, "TAB" },       // TAB or HT
    { 10, "LF" },       // NL or LF
    { ' ', {} },        // SPACE has abbrev SP
    { 0x00A0, "!" },    // NBSP bas abbreviations, turn them to aliases
    { 0x115F, "HC F" }, // Hangul starting filler
    { 0x1160, "HJ F" }, // Hangul medial filler
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

// Just capitalize them!
const std::set<std::string_view> cuneiformSymbols {
    "BAR", "LAM", "SA"
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

struct RangeByEnd {
    char32_t a = 0, b = 0;
    constexpr RangeByEnd() = default;
    constexpr RangeByEnd(char32_t aa, char32_t bb) : a(aa), b(bb) {}
};

constexpr bool operator < (const RangeByEnd& x, const RangeByEnd& y) { return (x.b < y.b); }

/// First max, then min!
const std::set<RangeByEnd> alternateRanges {
    { 0x2336, 0x237A },     // Technical — APL
    { 0x23B7, 0x23BD },     // Technical — several chars missing in Noto’s
};

const std::set<char32_t> virtualViramas {
    0x11D45,        // Masaram Gondi
    0x11D97,        // Gunjala Gondi
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


std::string decapitalize(std::string_view x, char32_t cp, DecapDebug debug)
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

    // Check for prefixes
    PrefixEntry prefixKey { words[0].original, PrefixAction::NOMATTER };
    auto range = prefixes.equal_range(prefixKey);
    for (auto it = range.first; it != range.second; ++it) {
        auto& entry = *it;
        if (cp >= entry.lo && cp <= entry.hi && doWordsFit(words, entry.toBuf())) {
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
        } else if (!word.customCap.empty()) {
            state = word.isCapital ? WordState::CUSTOM_TITLE : WordState::CUSTOM;
        } else if (word.isCapital) {
            state = WordState::TITLE;
        }

        if (iWord == 0) {
            switch (state) {
            case WordState::SMALL: state = WordState::TITLE; break;
            case WordState::CUSTOM: state = WordState::CUSTOM_TITLE; break;
            case WordState::TITLE:
            case WordState::CUSTOM_TITLE:
            case WordState::ALL_CAP:;
            }
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
