#pragma once

// C++
#include <cstdint>
#include <string_view>

// Qt
#include <QString>


namespace uc {

    enum class EcScript : unsigned char {
        Zyyy,
        Adlm,   // Adlam
        Aghb,   // Caucasian Albaniam
        Ahom,   // Ahom
        Arab,   // Arabic
        Armi,   // Imperial Aramaic
        Armn,   // Armenian
        Bali,   // Balinese
        Bamu,   // Bamum
        Batk,   // Batak
        Beng,   // Bengali
        Bopo,   // Bopomofo
        Brai,   // Braille dots
        Bugi,   // Buginese
        Buhd,   // Buhid
        Cans,   // Canadian syllabic
        Cham,   // Cham
        Cher,   // Cherokee
        Copt,   // Coptic
        Cyrl,   // Cyrillic
        Deva,   // Devanagari
        Ethi,   // Ethiopic
        Geor,   // Georgian
        Glag,   // Glagolitic
        Grek,   // Greek
        Gujr,   // Gujarati
        Guru,   // Gurmukhi
        Hang,   // Hangul
        Hani,   // Chinese hieroglyphs
        Hano,   // Hanunoo
        Hebr,   // Hebrew
        Hira,   // Hiragana
        Java,   // Javanese
        Kali,   // Kayah Li
        Kana,   // Katakana
        Khmr,   // Khmer
        Knda,   // Kannada
        Lana,   // Tai-tham = Lanna
        Laoo,   // Lao
        Latn,   // Latin
        Lepc,   // Lepcha
        Limb,   // Limbu
        Lisu,   // Lisu
        Mand,   // Mandaic
        Mlym,   // Malayalam
        Mong,   // Mongolian (old)
        Mtei,   // Meitei Mayek
        Mymr,   // Myanmar
        Nkoo,   // NKo
        Ogam,   // Ogham
        Olck,   // Ol Chiki
        Orya,   // Oriya
        Runr,   // European runes
        Samr,   // Samaritan
        Sinh,   // Sinhala
        Sund,   // Sundanese
        Syrc,   // Syriac
        Tagb,   // Tagbanwa
        Tale,   // Tai Le (old)
        Talu,   // New Tai Lue
        Taml,   // Tamil
        Telu,   // Telugu
        Tglg,   // Tagalog
        Thaa,   // Thaana
        Thai,   // Thai
        Tibt,   // Tibetan
        Vaii,   // Vai
        Yiii,   // Yi
        Zinh,   // Unicode technical
        ZDeva,  // “Unicodia” technical
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
        Sylo = Zyyy,        /// @todo [bmp] Syloti Nagri
        //Syre  no chars
        //Syrj  no chars
        //Syrn  no chars
        Takr = Zyyy,        // beyond BMP
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

    class Int3
    {
    public:
        consteval Int3(int32_t x) : lo(x), med(x >> 8), hi(x >> 16) {}
        constexpr int32_t val() const
            { return (static_cast<int32_t>(hi) << 16) | (med << 8) | lo; }
        constexpr operator int32_t() const { return val(); }

        constexpr operator char32_t() const { return val(); }
        constexpr char32_t ch32() const { return val(); }
    private:
        uint8_t lo = 0, med = 0;
        int8_t hi;
    };

    struct Fraction
    {
        long long num = 0, denom = 0;
    };

    class CompressedFraction
    {
    public:
        constexpr CompressedFraction() = default;
        consteval CompressedFraction(long long aNum, unsigned long long aDenom)
                    : order(0) {
            if (aNum > 100 || aNum < -100 || aDenom > 200) {
                while (aNum % 10 == 0) {
                    aNum /= 10;
                    ++order;
                }
                while (aDenom % 10 == 0) {
                    aDenom /= 10;
                    --order;
                }
            }
            num = aNum;
            denom = aDenom;
        }
        Fraction val() const;
        bool isFull() const { return (denom != 0); }
        explicit operator bool () const { return isFull(); }
    private:
        signed char num = 0;
        unsigned char denom = 0;
        signed char order = 0;
    };

    enum class EcNumType : unsigned char {
        NONE,               ///< No numeric value
        DIGIT,              ///< Digit (Indian 1)
        SPECIAL_DIGIT,      ///< Special digit (superscript ³)
        NUMBER,             ///< Number (Roman II)
        NN
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

    struct Cp   // code point
    {
        Int3 subj = 0;              // 3
        struct Name {
            Int3 iTech, iLoc;            // +6 = 9
            const char8_t* tech() const;
        } name;
        EcCategory ecCategory;          // +1 = 10
        EcVersion ecVersion;            // +1 = 11
        EcBidiClass ecBidiClass;        // +1 = 12
        EcScript ecScript;              // +1 = 13
        struct Numeric {
            CompressedFraction val;
            EcNumType ecType = EcNumType::NONE;
            const NumType& type() const;
            bool isPresent() const { return (ecType != EcNumType::NONE); }
            consteval Numeric() = default;
            consteval Numeric(long long num, unsigned long long denom, EcNumType aType)
                : val(num, denom), ecType(aType) {}
        } numeric;                      // +4 = 17
        const Version& version() const;
        const Category& category() const;
        const BidiClass& bidiClass() const;
        const Script& script() const;
        EcScript ecScriptEx(const Block*& hint) const;
        const Script& scriptEx(const Block*& hint) const;
        const Font& font(const Block*& hint) const;
        SampleProxy sampleProxy(const Block*& hint) const;
        QString osProxy() const;

        ///  @return [+] it is a true space, really white
        bool isTrueSpace() const;
        ///  @return [+] is graphical, even space [-] is control/formatting
        bool isGraphical() const;
        constexpr int plane() const { return subj.val() >> 16; }
    };

}   // namespace uc
