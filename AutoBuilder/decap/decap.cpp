// My header
#include "decap.h"

// C++
#include <vector>

// Common libs
#include "u_Strings.h"
#include "u_Vector.h"

using namespace std::string_view_literals;

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
    { { "GEOMANTIC"sv, "FIGURE"sv }, IsPrefix::YES, IdiomAction::NEXT_CAP },
    // Rest idioms
    { { "VOWEL"sv, "LENGTH"sv, "MARK"sv }, IsPrefix::NO, IdiomAction::SECOND_THIRD_SMALL },
    { { "VEDIC"sv, "TONE"sv}, IsPrefix::NO, IdiomAction::NEXT_CAP },
    { { "AI"sv, "LENGTH"sv, "MARK"sv}, IsPrefix::NO, IdiomAction::FIRST_CAP },
    { { "SYLLABLE"sv, "LENGTHENER"sv }, IsPrefix::NO, IdiomAction::SECOND_THIRD_SMALL },
    { { "SYLLABLE"sv, "ITERATION"sv, "MARK"sv }, IsPrefix::NO, IdiomAction::SECOND_THIRD_SMALL },
    { { "YANGQIN"sv, "SIGN"sv }, IsPrefix::NO, IdiomAction::NEXT_CAP },
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
    { 0x113D1, uc::m::SAMPLED_CONTROL }, // Tutg conjoiner
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
    { 0x021C, 0x021D },     // Yogh: Cambria lazy
    { 0x02CE, 0x02CF },     // Misrender in Cambria
    { 0x0333 },             // Basic umlauts: misrenders in Cambria
    { 0x1D66, 0x1D6A },     // Clipped below in Cambria
    { 0x1E00, 0x1E01 },     // Clipped below in Cambria
    { 0x1E5C, 0x1E5D },     // Misplaced umlaut in Cambria
    { 0x1E68, 0x1E69 },     // Same
    { 0x2010, 0x203B },     // Several chars from Punctuation…
    { 0x203D, 0x203E },     // …Skip emoji and go on
    { 0x23B4, 0x23B6 },     // Technical — horz brackets, misrender in Cambria
    { 0x23DC, 0x23E0 },     // Same
    { 0x25CC },             // Dotted circle
    { 0x266D, 0x266F },     // Misc — use Funky font on musical symbols
    { 0x2C7C },             // Clipped below in Cambria
    { 0x3021, 0x3029 },     // U16 glyph changes, BabelStone’s here!
                            // Actually 26…29, the rest are for uniformness
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
    { 0x298B, 0x2990 },     // Square brackets
    { 0x2A0A, 0x2A1C },     // Integrals bunch 2

    // Mathematical: Cambria → Noto
    { 0x2201 },         // set C
    { 0x2205 },         // empty set
    { 0x22DA, 0x22DB },
    { 0x27EC, 0x27ED },
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
    { { 0x1E6C0, 0x1E6FF }, TWO_STYLES },   // Tayo
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
        if (cp || !itEx->second.flags.have(decap::Exf::CPONLY) ) {
            // Mixcase flag: in upcase mode leave, in locase mode to lower
            if (itEx->second.flags.have(decap::Exf::MIXCASE) && flags.have(Dcfg::LOCASE)) {
                return str::toLower(itEx->second.r);
            }
            if (itEx->second.flags.have(decap::Exf::LEAVE_BY_CONDITION)
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
