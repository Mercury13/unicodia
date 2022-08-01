#pragma once

// C++
#include <cstdint>
#include <string_view>

// Qt
#include <QString>

// Libs
#include "u_TypedFlags.h"
#include "u_Vector.h"

// Project-local
#include "UcAutoCount.h"

namespace uc {

    constexpr int N_PLANES = 17;

    enum class EcScript : unsigned char {
        Zyyy,
        ZARR,   // Pseudo-script: Arrows
        ZMAT,   // Pseudo-script: Mathemetical
        ZSUX,   // Pseudo-script: Cuneiform
        ZSYM,   // Pseudo-script: Symbols and pictographs
        Adlm,   // Adlam
        Aghb,   // Caucasian Albanian
        Ahom,   // Ahom
        Arab,   // Arabic
        Armi,   // Imperial Aramaic
        Armn,   // Armenian
        Avst,   // Avestan
        Bali,   // Balinese
        Bamu,   // Bamum
        Bass,   // Bassa Vah
        Batk,   // Batak
        Beng,   // Bengali
        Bhks,   // Bhaiksuki
        Bopo,   // Bopomofo
        Brah,   // Brahmi
        Brai,   // Braille dots
        Bugi,   // Buginese
        Buhd,   // Buhid
        Cakm,   // Chakma
        Cari,   // Carian
        Cans,   // Canadian syllabic
        Cham,   // Cham
        Cher,   // Cherokee
        Chrs,   // Chorasmian
        Copt,   // Coptic
        Cpmn,   // Cypro-Minoan
        Cprt,   // Cypriot
        Cyrl,   // Cyrillic
        Deva,   // Devanagari
        Diak,   // Dives Akuru
        Dogr,   // Dogra
        Dsrt,   // Deseret (Mormon)
        Dupl,   // Duployan shorthand
        Egyp,   // Egyptian hieroglyphs
        Elba,   // Elbasan Albanian
        Elym,   // Elymaic
        Ethi,   // Ethiopic
        Geor,   // Georgian
        Glag,   // Glagolitic
        Gong,   // Gondi / Gunjala
        Gonm,   // Gondi / Masaram
        Goth,   // Gothic
        Gran,   // Grantha
        Grek,   // Greek
        Gujr,   // Gujarati
        Guru,   // Gurmukhi
        Hang,   // Hangul
        Hani,   // Chinese hieroglyphs
        Hano,   // Hanunoo
        Hatr,   // Hatran
        Hebr,   // Hebrew
        Hent,   // Hentaigana
        Hira,   // Hiragana
        Hluw,   // Anatolian (Luwian, Hittite) hieroglyphs
        Hmng,   // Pahawh Hmong
        Hmnp,   // NP hmong (American script for White/Green Hmongs)
        Hung,   // Old Hungarian runes
        Ital,   // Old Italic
        Java,   // Javanese
        Kali,   // Kayah Li
        Kana,   // Katakana
        Kawi,   // Kawi (proto-Javanese)
        Khar,   // Kharoshthi
        Khmr,   // Khmer
        Khoj,   // Khojki
        Kits,   // Khitan small
        Knda,   // Kannada
        Kthi,   // Kaithi
        Lana,   // Tai-tham = Lanna
        Laoo,   // Lao
        Latn,   // Latin
        Lepc,   // Lepcha
        Limb,   // Limbu
        Lina,   // Linear A
        Linb,   // Linear B
        Lisu,   // Lisu
        Lyci,   // Lycian
        Lydi,   // Lydian
        Mahj,   // Mahajani
        Maka,   // Makasar
        Mand,   // Mandaic
        Mani,   // Manichean
        Marc,   // Marchen
        Medf,   // Medefaidrin
        Mend,   // Mende Kikakui
        Merc,   // Meroitic cursive
        Mero,   // Meroitic hieroglyphs
        Mlym,   // Malayalam
        Modi,   // Modi
        Mong,   // Mongolian (old)
        Mroo,   // Mro
        Mtei,   // Meitei Mayek
        Mult,   // Multani
        Mymr,   // Myanmar
        Nagm,   // Nag alphabet (Mundari)
        Nand,   // Nandinagari
        Narb,   // North Arabian
        Nbat,   // Nabataean
        Newa,   // Newa
        Nkoo,   // NKo
        Nshu,   // Nushu
        Ogam,   // Ogham
        Olck,   // Ol Chiki
        Orkh,   // Turkic runes
        Orya,   // Oriya
        Osge,   // Osage
        Osma,   // Osmanya
        Ougr,   // Old Uyghur, U14
        Palm,   // Palmyrene
        Pauc,   // Pau Cin Hau
        Perm,   // Old Permic
        Phag,   // Phags-Pa
        Phli,   // Inscriptional Pahlavi
        Phlp,   // Psalter Pahlavi
        Phnx,   // Phoenician
        Plrd,   // Miao / Pollard
        Prti,   // Inscriptional Parthian
        Rjng,   // Rejang
        Rohg,   // Hanifi Rohingya
        Runr,   // European runes
        Samr,   // Samaritan
        Sarb,   // South Arabian
        Saur,   // Saurashtra
        Sgnw,   // Sutton SignWriting
        Shaw,   // Shavian
        Shrd,   // Sharada
        Sidd,   // Siddham / Siddhamatrika
        Sind,   // Sindhi (Khudawadi)
        Sinh,   // Sinhala
        Sogd,   // Sogdian
        Sogo,   // Old Sogdian
        Sora,   // Sora Sompeng
        Soyo,   // Soyombo
        Sund,   // Sundanese
        Sylo,   // Syloti Nagri
        Syrc,   // Syriac
        Tagb,   // Tagbanwa
        Takr,   // Takri
        Tale,   // Tai Le (old)
        Talu,   // New Tai Lue
        Taml,   // Tamil
        Tang,   // Tangut
        Tavt,   // Tai Viet
        Telu,   // Telugu
        Tfng,   // Tifinagh
        Tglg,   // Tagalog
        Thaa,   // Thaana
        Thai,   // Thai
        Tibt,   // Tibetan
        Tirh,   // Tirhuta
        Tnsa,   // Tangsa
        Toto,   // Toto
        Ugar,   // Ugaritic
        Vaii,   // Vai
        Vith,   // Vithkuqi Albanian
        Wcho,   // Wancho
        Wara,   // Warang Citi
        Xpeo,   // Old Persian cuneiform
        Xsux,   // Cuneiform
        Yezi,   // Yezidi
        Yiii,   // Yi
        Zanb,   // Zanabazar square
        Zinh,   // Unicode technical
        NN,

        //Afak  no chars — Afaka (Suriname)
        //Aran  no chars — Arabic Nastaliq
        //Blis  no chars — Blissymbols
        //Cirt  no chars — Cirth
        //Cyrs  no chars — Cyrillic Old Church
        //Egyd  no chars — Egyptian demotic
        //Egyh  no chars — Egyptian hieratic
        //Geok  no chars
        //Hanb  no chars — Han + Bopomofo
        //Hans  no chars — Han simplified
        //Hant  no chars — Han traditional
        //Hrkt  no chars — Hiragana + Katakana
        //Inds  no chars — Indus
        //Jamo  no chars — Hangul jamo
        //Jpan  no chars — Japanese
        //Jurc  no chars — Jurchen
        //Kitl  no chars — Khitan large
        //Kore  no chars — Hangul+Han
        //Kpel  no chars — Kpelle (Africa)
        //Latf  no chars — Latin Fraktur
        //Latg  no chars — Latin Gaelic
        //Leke  no chars — Leke (Pwo Karen)
        //Loma  no chars — Loma (Africa)
        //Maya  no chars — Maya
        //Moon  no chars — Moon’s embossing
        //Nkdb  no chars — Naxi Dongba (Naxi hieroglyphs)
        //Nkgb  no chars — Naxi Geba
        //Phlv  no chars — Book Pahvali
        //Piqd  no chars — Klingon :)
        //Roro  no chars — Rongorongo (Easter Island)
        //Sara  no chars — Sarati (Tolkien)
        //Shui  no chars — Shuishu hieroglyphs (Sui language, China)
        //Syre  no chars — Syriac Estrangelo
        //Syrj  no chars — Syriac Western
        //Syrn  no chars — Syriac Eastern
        //Teng  no chars — Tengwar
        //Visp  no chars — Visible speech
        //Wole  no chars
        //Zmth  no chars — math notation
        //Zsye  no chars — emoji
        //Zsym  no chars — misc symbols
        //Zxxx  no chars — unwritten documents
        //Zzzz  no chars — unencoded script
        NONE = Zyyy
    };

    enum class EcVersion : unsigned char
    {
        NONE,
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
        V_14_0,
        V_15_0,
        NN,
        UNKNOWN = NN,
        FIRST = 0,
        LAST = NN - 1,
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

    class Int3
    {
    public:
        consteval Int3(int32_t x) : lo(x), med(x >> 8), hi(x >> 16) {}
        constexpr int32_t val() const
            { return (static_cast<int32_t>(hi) << 16) | (med << 8) | lo; }
        constexpr operator int32_t() const { return val(); }
        constexpr uint32_t uval() const
            { return (static_cast<uint32_t>(hi) << 16) | (med << 8) | lo; }
        constexpr operator uint32_t() const { return uval(); }

        constexpr operator char32_t() const { return val(); }
        constexpr char32_t ch32() const { return val(); }
    private:
        uint8_t lo = 0, med = 0;
        int8_t hi;
    };

    enum class EcNumType : unsigned char {
        NONE,               ///< No numeric value
        DIGIT,              ///< Digit (Indian 1)
        SPECIAL_DIGIT,      ///< Special digit (superscript ³)
        NUMBER,             ///< Number (Roman II)
        NN
    };

    struct NumType;

    enum class FracType {
        NONE,       ///< Not a number
        INTEGER,    ///< Integer
        VULGAR,     ///< Vulgar fraction
        DECIMAL,    ///< Top-heavy (>1) convertible to decimal
                    ///< present in Tibetan
    };

    /// @warning  All fractions are reduced!
    struct Numeric
    {
        long long num = 0, denom = 0;
        EcNumType ecType = EcNumType::NONE;

        bool isPresent() const { return (ecType != EcNumType::NONE); }
        const NumType& type() const;
        FracType fracType() const;
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

    struct Version;
    struct NumType;
    struct Script;
    struct Category;
    struct BidiClass;
    struct Block;
    struct Font;

    struct SampleProxy {
        QString text;
        std::string_view styleSheet;

        SampleProxy() = default;
        SampleProxy(QString x, std::string_view y) : text(std::move(x)), styleSheet(y) {}
    };

    enum class Cfg : unsigned short {
        M_ABBREVIATION = 1,     ///< [+] Method: 1st synonym is abbreviation
        U_DEPRECATED = 2,       ///< [+] UC feature: char is deprecated
        RENDER_BUG = 4,         ///< [+] use font BUG_PREFER, or drop BUG_AVOID
        M_CUSTOM_CONTROL = 8,   ///< [+] Method: custom-drawn control char
        NO_AA = 16,             ///< [+] Submethod of SAMPLE: temporarily disable anti-aliasing
        U_DEF_IGNORABLE = 32,   ///< [+] UC feature: default-ignorable
        U_VS16_EMOJI = 64,      ///< [+] UC feature: to surely make this char graphic, use VS16
        M_SVG_EMOJI = 128,      ///< [+] SVG emoji
        M_SPACE = 256,          ///< [+] Method: draw as space, even if it is not space
        M_EGYPTIAN_HATCH = 512, ///< [+] Method: draw Egyptian hatch (not really a hatch, but OK)
        DYN_SYSTEM_TOFU = 32768,///< cached in runtime; [+] the char is tofu in system fonts
        M_ALL = M_ABBREVIATION | M_CUSTOM_CONTROL | M_SVG_EMOJI | M_SPACE | M_EGYPTIAN_HATCH
    };

    using Cfgs = Flags<Cfg>;
    enum class DrawMethod {
            // Font-based methods
            SAMPLE,             ///< Take font and draw string
            CUSTOM_AA,          ///< Unused right now, no fonts have custom anti-aliasing right now
            SPACE,              ///< Take font, measure space width and draw 2 lines
            EGYPTIAN_HATCH,     ///< Draw special big circle, then hatch
            // Char-based methods
            ABBREVIATION,       ///< Draw dotted square and abbreviation
            CUSTOM_CONTROL,     ///< Draw dotted square and smth custom
            SVG_EMOJI,          ///< Draw SVG emoji
            LAST_FONT = EGYPTIAN_HATCH };            // Last using (loading, checking) font

    enum class TofuState {
        PRESENT,    ///< Not a tofu, have char
        TOFU,       ///< Tofu, have no char
        NO_FONT };  ///< Have so custom method of drawing that no font needed

    enum class TofuPlace {
        CJK,        ///< In CJK chars
        REST        ///< In rest of chars
    };

    struct TofuInfo {
        TofuState state = TofuState::NO_FONT;
        TofuPlace place = TofuPlace::REST;
        const uc::Block* block = nullptr;
    };

    enum class MatchLast { NO, YES };

    struct Cp   // code point
    {
        Int3 subj = 0;              // 3
        struct Name {
            Int3 iTech;                 // +3 = 6
            uint8_t alts;               // +1 = 7
            const char8_t* tech() const;
        } name;
        EcCategory ecCategory;          // +1 = 8
        EcVersion ecVersion;            // +1 = 9
        EcBidiClass ecBidiClass;        // +1 = 10
        EcScript ecScript;              // +1 = 11
        uint8_t iNumeric;               // +1 = 12
        mutable Cfgs flags;             // +2 = 14

        const Version& version() const;
        const Category& category() const;
        const BidiClass& bidiClass() const;
        const Script& script() const;
        const Numeric& numeric() const;
        EcScript ecScriptEx() const;
        const Script& scriptEx() const;
        const Font& firstFont() const;

        /// @param [in] matchLast  [+] match last font, can return null
        ///                        [-] take last font, never null
        const Font* font(MatchLast matchLast) const;
        SampleProxy sampleProxy(int dpi) const;
        QString osProxy() const;
        inline const Block& block() const;

        ///  @return [+] it is a true space, really white
        bool isTrueSpace() const;
        ///  @return [+] is graphical, even space [-] is control/formatting
        bool isGraphical() const;
        constexpr int plane() const { return subj.val() >> 16; }

        QString viewableName() const;
        SafeVector<std::u8string_view> allRawNames() const;
        DrawMethod drawMethod(int dpi) const;
        TofuInfo tofuInfo() const;
        constexpr bool isAbbreviated() const { return flags.have(Cfg::M_ABBREVIATION); }
        std::u8string_view abbrev() const;
        constexpr bool isDeprecated() const { return flags.have(Cfg::U_DEPRECATED); }
        constexpr bool isDefaultIgnorable() const { return flags.have(Cfg::U_DEF_IGNORABLE); }
        constexpr bool isVs16Emoji() const { return flags.have(Cfg::U_VS16_EMOJI); }
    };

    // Check your calculations once again
    static_assert(sizeof(Cp) == 14);

    extern Cp cpInfo[N_CPS];
    extern const char8_t allStrings[];
    extern const Numeric allNumerics[N_NUMERICS];

    struct LibNode {
        std::u32string_view value;
        std::u8string_view text;
        short iParent, nChildren, iFirstChild;
    };

    extern const LibNode libNodes[N_LIBNODES];

}   // namespace uc
