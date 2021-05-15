#pragma once

#include <limits>

namespace uc {

    constexpr unsigned NO_CHAR = std::numeric_limits<unsigned>::max();

    enum class LangLife
    {
        NOMATTER,       ///< Symbols (language’s life does not matter)
        ALIVE,          ///< UNESCO safe (Ukrainian)
                        ///< UNESCO vulnerable (Belorussian)
        ENDANGERED,     ///< UNESCO endangered (Walloon)
                        ///< UNESCO highly endangered (Crimean Tatar)
        DEAD,           ///< UNESCO elders only (Votic)
                        ///< UNESCO dead (Yugh)
        HISTORICAL,     ///< Dead long ago (Latin, Coptic)
        //ARTIFICIAL,     ///< Artificial languages (Esperanto); unencoded right now
        //FICTIONAL       ///< Fictional languages (Quenya, Klingon); unencoded right now
    };


    enum class ScriptType
    {
        UNKNOWN,
        ALPHABET,       ///< Pure alphabet script (Latin, Cyrillic)
        CONSONANT,      ///< Consonant script (Hebrew)
        SYLLABLE,       ///< Syllable script (Hiragana)
        SEMISYLLABLE,   ///< Partly alphabet, partly syllable (Iberian)
        ALPHASYLLABLE,  ///< Syllable script where similar syllables have
                        ///<   similar symbols (Devanagari, Thai)
        HIEROGLYPH,     ///< Pictures of things (CJK, ancient Egyptian)
        SYLLABOHEROGLYPH, ///< Syllable + hieroglyph (Linear A/B)
        //SHORTHAND,      ///< Mostly alphabet, but with some simplifications
        //                ///<   (English stenography)
        CODE,           ///< System for encoding other scripts (Braille)
        MISC,           ///< Miscellaneous symbols
        EMOJI,          ///< Emoji pictures
        NN,
        ABUGIDA = ALPHASYLLABLE
    };


    enum class EcVersion
    {
        //V_1_0,        unused
        //V_1_0_1,      unused
        V_1_1,
        V_2_0,
        V_2_1,
        V_3_0,
        V_3_1,
        V_3_2,
        V_4_0,
        V_4_1,
        V_5_0,
        V_5_1,
        V_5_2,
        V_6_0,
        V_6_1,
        V_6_2,
        V_6_3,
        V_7_0,
        V_8_0,
        V_9_0,
        V_10_0,
        V_11_0,
        V_12_0,
        V_12_1,
        V_13_0,
        NN
    };

    enum class EcBidiClass
    {                               //   Abbr.   Icon
        BIDI_CONTROL,               ///<  FSI, LRE, LRI, LRO, PDF, PDI, RLE, RLI, RLO
                                    ///<         →|←  (bar is dashed)
        ARABIC_LETTER,              ///<  AL     Some Arabic letter
        ARABIC_NUMBER,              ///<  AN     Arabic-Indic 3 ٣
        PARAGRAPH_SEPARATOR,        ///<  B      Enter sign
        BOUNDARY_NEUTRAL,           ///<  BN     Bell
        COMMON_SEPARATOR,           ///<  CS     Slash
        EUROPEAN_NUMBER,            ///<  EN     Digit 2
        EUROPEAN_SEPARATOR,         ///<  ES     +
        EUROPEAN_TERMINATOR,        ///<  ET     %
        LTR,                        ///<  L      →
        NONSPACING_MARK,            ///<  NSM    Haček = caron + circle
        OTHER_NEUTRAL,              ///<  ON     ©
        RTL,                        ///<  R      ←
        SEGMENT_SEPARATOR,          ///<  S      Tab sign
        WHITE_SPACE,                ///<  WS     Space sign
        NN
    };

    enum class UpCategory
    {
        CONTROL,
        FORMAT,
        LETTER,
        MARK,
        NUMBER,
        PUNCTUATION,
        SYMBOL,
        SEPARATOR
    };

    enum class EcCategory
    {                           //  Abbr.  Icon
        CONTROL,                ///< Cc   Tab sign
        FORMAT,                 ///< Cf   Caret sign
        // PRIVATE_USE,     -- unused as Unicodia has nothing to tell
        // SURROGATE,       -- unused as Unicodia has nothing to tell
        LETTER_LOWERCASE,       ///< Ll   Small a
        LETTER_MODIFIER,        ///< Lm   Prime = ′
        LETTER_OTHER,           ///< Lo   Hebrew alef
        LETTER_TITLECASE,       ///< Lt   Dz ligature
        LETTER_UPPERCASE,       ///< Lu   Capital A
        MARK_SPACING,           ///< Mc   Devanagari? + circle
        MARK_ENCLOSING,         ///< Me   Cyrillic 100k (U+0488) + circle
        MARK_NONSPACING,        ///< Mn   Haček = caron + circle
        NUMBER_DECIMAL,         ///< Nd   Digit 2
        NUMBER_LETTER,          ///< Nl   Roman Ⅳ
        NUMBER_OTHER,           ///< No   Fraction 1/2
        PUNCTUATION_CONNECTOR,  ///< Pc   Character tie (word root)
        PUNCTUATION_DASH,       ///< Pd   Dash
        PUNCTUATION_CLOSE,      ///< Pe   Right )
        PUNCTUATION_FINAL,      ///< Pf   Right »
        PUNCTUATION_INITIAL,    ///< Pi   Left «
        PUNCTUATION_OTHER,      ///< Po   !
        PUNCTUATION_OPEN,       ///< Ps   Left (
        SYMBOL_CURRENCY,        ///< Sc   $
        SYMBOL_MODIFIER,        ///< Sk   Same caron, but w/o circle
        SYMBOL_MATH,            ///< Sm   ∞
        SYMBOL_OTHER,           ///< So   ©
        SEPARATOR_LINE,         ///< Zl   Enter sign
        SEPARATOR_PARAGRAPH,    ///< Zp   Paragraph sign
        SEPARATOR_SPACE,        ///< Zs   Space sign
        NN
    };

    enum class EcIndustry
    {
        NONE,
        APPLIANCES,             ///< power, play, volume…
        ASTROLOGY,
        ASTRONOMY,
        BUSINESS,               ///< @, telephone, fax…
        CHEMISTRY,              ///< incl. alchemy
        COMPUTING,              ///< incl. \, box-drawing
        ELECTRIC,
        MACHINEBUILDING,
        MATHS,
        MATHS_MULTILINE,        ///< multiline formulae and so on
        MEDICINE,               ///< incl. dentistry
        MUSIC,
        NAVIGATION_TRAFFIC,     ///< traffic sings, detours…
        NAVIGATION_PEDESTRIAN,  ///< navigation in airports, tourist maps…
        POETRY,
        RELIGION,
        SPORT,
        NN
    };

    enum class EcWritingDir
    {
        UNKNOWN,
        LTR,
        RTL,
        LTR_CJK,
        NN,
    };

    enum class EcNumType {
        NONE,               ///< No numeric value
        DIGIT,              ///< Digit (Indian 1)
        SPECIAL_DIGIT,      ///< Special digit (superscript ³)
        NUMBER,             ///< Number (Roman II)
        NN
    };

}   // namespace uc
