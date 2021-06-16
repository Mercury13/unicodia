#pragma once

#include "UcAutoCount.h"

#include <limits>

namespace uc {

    constexpr unsigned NO_CHAR = std::numeric_limits<unsigned>::max();

    enum class EcLangLife
    {
        NOMATTER,       ///< Symbols (language’s life does not matter)
        ALIVE,          ///< UNESCO safe (Ukrainian)
                        ///< UNESCO vulnerable (Belorussian)
        ENDANGERED,     ///< UNESCO endangered (Walloon)
                        ///< UNESCO highly endangered (Crimean Tatar)
        DEAD,           ///< UNESCO elders only (Votic)
                        ///< UNESCO dead (Yugh)
        HISTORICAL,     ///< Dead long ago (Latin, Coptic)
        NEW,            ///< Newly-created (Adlam)
        REVIVED,        ///< Revived (Ahom)
        //ARTIFICIAL,     ///< Artificial languages (Esperanto); unencoded right now
        //FICTIONAL       ///< Fictional languages (Quenya, Klingon); unencoded right now
        NN
    };

    enum class EcScriptType
    {
        NONE,
        UNKNOWN,
        ALPHABET,       ///< Pure alphabet script (Latin, Cyrillic)
        PSEUDOALPHABET, ///< Alphabet scripts that use syllables as units
        CONSONANT,      ///< Consonant script (Hebrew)
        SYLLABLE,       ///< Syllable script (Hiragana)
        ALPHASYLLABLE,  ///< Partly alphabet, partly syllable (Iberian)
        ABUGIDA_MONOLITH, ///< Syllable script where similar syllables have
                        ///<   similar symbols (Canadian syllable)
        ABUGIDA_COMBINING, ///< … + characters are combined from consonant and vowel (Thaana)
        ABUGIDA_BRAHMI, ///< … + based on Brahmi: K=ka, K*=ki, K’=K (Kannada, Thai)
        HIEROGLYPH,     ///< Pictures of things (CJK, ancient Egyptian)
        SYLLABOHEROGLYPH, ///< Syllable + hieroglyph (Linear A/B)
        //SHORTHAND,      ///< Mostly alphabet, but with some simplifications
        //                ///<   (English stenography)
        CODE,           ///< System for encoding other scripts (Braille)
        SYMBOL,         ///< Miscellaneous symbols
        GAME,           ///< Tabletop games
        EMOJI,          ///< Emoji pictures
        NN,
    };

    enum class EcScript : unsigned char {
        Zyyy,
        Adlm,
        Aghb,
        Ahom,
        Arab,
        Armi,
        Armn,
        Bali,
        Bamu,
        Batk,
        Beng,
        Bopo,
        Brai,
        Bugi,
        Buhd,
        Cans,
        Cham,
        Cher,
        Copt,
        Cyrl,
        Deva,
        Ethi,
        Geor,
        Glag,
        Grek,
        Gujr,
        Guru,
        Hang,
        Hani,
        Hano,
        Hebr,
        Hira,
        Java,
        Kali,
        Kana,
        Khmr,
        Knda,
        Lana,
        Laoo,
        Latn,
        Lepc,
        Limb,
        Lisu,
        Mand,
        Mlym,
        Mong,
        Mtei,
        Mymr,
        Nkoo,
        Ogam,
        Olck,
        Orya,
        Runr,
        Samr,
        Sinh,
        Syrc,
        Tagb,
        Taml,
        Telu,
        Tglg,
        Thaa,
        Thai,
        Tibt,
        Vaii,
        Zinh,
        NN,

        //Afak  no chars
        //Aran  no chars
        Avst = Zyyy,        // beyond BMP
        Bass = Zyyy,        // beyond BMP
        Bhks = Zyyy,        // beyond BMP
        //Blis  no chars
        Brah = Zyyy,        // historical, beyond BMP
        Cakm = Zyyy,        // beyond BMP
        Cari = Zyyy,        // beyond BMP
        Chrs = Zyyy,        // Chorasmian, beyond BMP
        //Cirt no chars
        //Cpmn  no chars
        Cprt = Zyyy,        // beyond BMP
        //Cyrs  no chars
        Diak = Zyyy,        // Dives Akuru, beyond BMP
        Dogr = Zyyy,        // beyond BMP
        Dsrt = Zyyy,        // beyond BMP
        Dupl = Zyyy,        // beyond BMP
        //Egyd  no chars
        //Egyh  no chars
        Egyp = Zyyy,        // beyond BMP
        Elba = Zyyy,        // beyond BMP
        Elym = Zyyy,        // Elymaic, beyond BMP
        //Geok  no chars
        Gong = Zyyy,        // beyond BMP
        Gonm = Zyyy,        // beyond BMP
        Goth = Zyyy,        // beyond BMP
        Gran = Zyyy,        // beyond BMP
        //Hanb  no chars
        //Hans  no chars
        //Hant  no chars
        Hatr = Zyyy,        // beyond BMP
        Hluw = Zyyy,        // beyond BMP
        Hmng = Zyyy,        // beyond BMP
        Hmnp = Zyyy,        // beyond BMP
        //Hrkt  no chars
        Hung = Zyyy,        // beyond BMP
        //Inds  no chars
        Ital = Zyyy,        // beyond BMP
        //Jamo  no chars
        //Jpan  no chars
        //Jurc  no chars
        Khar = Zyyy,        // beyond BMP
        Khoj = Zyyy,        // beyond BMP
        //Kitl  no chars
        Kits = Zyyy,        // beyond BMP
        //Kore  no chars
        //Kpel  no chars
        Kthi = Zyyy,        // beyond BMP
        //Latf  no chars
        //Latg  no chars
        //Leke  no chars
        Lina = Zyyy,        // beyond BMP
        Linb = Zyyy,        // beyond BMP
        //Loma  no chars
        Lyci = Zyyy,        // beyond BMP
        Lydi = Zyyy,        // beyond BMP
        Mahj = Zyyy,        // Mahajani, beyond BMP
        Maka = Zyyy,        // beyond BMP
        Mani = Zyyy,        // beyond BMP
        Marc = Zyyy,        // beyond BMP
        //Maya  no chars
        Medf = Zyyy,        // beyond BMP
        Mend = Zyyy,        // beyond BMP
        Merc = Zyyy,        // beyond BMP
        Mero = Zyyy,        // beyond BMP
        Modi = Zyyy,        // beyond BMP
        //Moon  no chars
        Mroo = Zyyy,        // beyond BMP
        Mult = Zyyy,        // beyond BMP
        Nand = Zyyy,        // Nandinagari, beyond BMP
        Narb = Zyyy,        // beyond BMP
        Nbat = Zyyy,        // beyond BMP
        Newa = Zyyy,        // beyond BMP
        //Nkdb  no chars
        //Nkgb  no chars
        Nshu = Zyyy,        // beyond BMP
        Orkh = Zyyy,        // Turkic runes, beyond BMP
        Osge = Zyyy,        // beyond BMP
        Osma = Zyyy,        // beyond BMP
        Palm = Zyyy,        // beyond BMP
        Pauc = Zyyy,        // beyond BMP
        Perm = Zyyy,        // beyond BMP
        Phag = Zyyy,        /// @todo [bmp] Phags-pa
        Phli = Zyyy,        // beyond BMP
        Phlp = Zyyy,        // beyond BMP
        //Phlv  no chars
        Phnx = Zyyy,        // beyond BMP
        //Piqd  no chars — Klingon :)
        Plrd = Zyyy,        // beyond BMP
        Prti = Zyyy,        // beyond BMP
        Rjng = Zyyy,        /// @todo [bmp] Rejang
        Rohg = Zyyy,        // Hanifi Rohingya, beyond BMP
        //Roro  no chars
        //Sara  no chars
        Sarb = Zyyy,        // beyond BMP
        Saur = Zyyy,        /// @todo [bmp] Saurashtra
        Sgnw = Zyyy,        // SignWriting, beyond BMP
        Shaw = Zyyy,        // beyond BMP
        Shrd = Zyyy,        // beyond BMP
        //Shui  no chars
        Sidd = Zyyy,        // beyond BMP
        Sind = Zyyy,        // beyond BMP
        Sora = Zyyy,        // beyond BMP
        Sogd = Zyyy,        // beyond BMP
        Sogo = Zyyy,        // beyond BMP
        Soyo = Zyyy,        // beyond BMP
        Sund = Zyyy,        /// @todo [bmp] Sundanese
        Sylo = Zyyy,        /// @todo [bmp] Syloti Nagri
        //Syre  no chars
        //Syrj  no chars
        //Syrn  no chars
        Takr = Zyyy,        // beyond BMP
        Tale = Zyyy,        /// @todo [bmp] Tai Le
        Talu = Zyyy,        /// @todo [bmp] New Tai Lue
        Tang = Zyyy,        // beyond BMP
        Tavt = Zyyy,        /// @todo [bmp] Tai Viet
        //Teng  no chars
        Tfng = Zyyy,        /// @todo [bmp] Tifinagh
        Tirh = Zyyy,        // Tirhuta beyond BMP
        Ugar = Zyyy,        // Ugaritic beyond BMP
        //Visp  no chars
        Wara = Zyyy,        // Warang Citi beyond BMP
        Wcho = Zyyy,        // Wancho beyond BMP
        //Wole  no chars
        Xpeo = Zyyy,        // Old Persian beyond BMP
        Xsux = Zyyy,        // Cuneiform beyond BMP
        Yezi = Zyyy,        // Yezidi beyond BMP
        Yiii = Zyyy,        /// @todo [bmp] Yi
        Zanb = Zyyy,        // Zanabazar beyond BMP
        //Zmth  no chars
        //Zsye  no chars
        //Zsym  no chars
        //Zxxx  no chars
        //Zzzz  no chars
        NONE = Zyyy
    };

    enum class EcVersion : unsigned char
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
        NN,
        UNKNOWN = NN,
    };

    enum class EcBidiStrength
    {
        NEUTRAL, WEAK, STRONG, EXPLICIT, NN
    };

    enum class EcBidiClass : unsigned char
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
        MIRRORED,                   ///<  ON + 'Y', [
        RTL,                        ///<  R      ←
        SEGMENT_SEPARATOR,          ///<  S      Tab sign
        WHITE_SPACE,                ///<  WS     Space sign
        NN,
        z_FSI = BIDI_CONTROL,
        z_LRE = BIDI_CONTROL,
        z_LRI = BIDI_CONTROL,
        z_LRO = BIDI_CONTROL,
        z_PDF = BIDI_CONTROL,
        z_PDI = BIDI_CONTROL,
        z_RLE = BIDI_CONTROL,
        z_RLI = BIDI_CONTROL,
        z_RLO = BIDI_CONTROL,
        z_AL = ARABIC_LETTER,
        z_AN = ARABIC_NUMBER,
        z_B = PARAGRAPH_SEPARATOR,
        z_BN = BOUNDARY_NEUTRAL,
        z_CS = COMMON_SEPARATOR,
        z_EN = EUROPEAN_NUMBER,
        z_ES = EUROPEAN_SEPARATOR,
        z_ET = EUROPEAN_TERMINATOR,
        z_L = LTR,
        z_NSM = NONSPACING_MARK,
        z_ON = OTHER_NEUTRAL,
        z_R = RTL,
        z_S = SEGMENT_SEPARATOR,
        z_WS = WHITE_SPACE,
        z_MIR = MIRRORED
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

    enum class EcCategory : unsigned char
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
        NOMATTER,
        LTR,
        RTL,
        LTR_CJK,
        LTR_BU,
        LTR_COL,
        NN,
    };

    enum class EcNumType : unsigned char {
        NONE,               ///< No numeric value
        DIGIT,              ///< Digit (Indian 1)
        SPECIAL_DIGIT,      ///< Special digit (superscript ³)
        NUMBER,             ///< Number (Roman II)
        NN
    };

}   // namespace uc
