#ifndef UC_AUTO_DEFINES_H
#define UC_AUTO_DEFINES_H

// C++
#include <cstdint>
#include <string_view>
#include <span>
#include <unordered_set>

// Qt
#include <QString>
#include <QRect>

// Lib
#include "u_EcArray.h"  // used e.g. for EcNumType!

// Project-local
#include "UcFlags.h"
#include "UcAutoCount.h"

namespace uc {

    constexpr int N_PLANES = 17;

    enum class EcScript : unsigned char {
        Zyyy,
        ZARR,   // Pseudo-script: arrows
        ZMAT,   // Pseudo-script: mathemetical
        ZSYM,   // Pseudo-script: symbols and pictographs
        ZDIN,   // Pseudo-script: dingbats
        ZDIA,   // Pseudo-script: diacritical marks
        ZKXI,   // Pseudo-script: Kangxi
        ZOLD,   // Pseudo-script: legacy computing
        ZVAR,   // Pseudo-script: variation selectors
        ZSHP,   // Pseudo-script: shapes
        ZPUN,   // Pseudo-script: punctuation

#ifndef Q_MOC_RUN
    // MOC creates CPPs rather than parses this enum, so probably OK
    #include "UcAutoScripts.h"
#endif

        Zinh,   // Unicode technical
        NN,
        NONE = Zyyy,
        NO_VALUE = ZARR,   // Undefined in queries
    };

    constexpr auto N_SCRIPTS = static_cast<unsigned>(EcScript::NN);

    enum class Month : unsigned char {
        NUL = 0,
        JAN = 1,  FEB = 2,  MAR = 3,  APR = 4,  MAY = 5,  JUN = 6,
        JUL = 7,  AUG = 8,  SEP = 9,  OCT = 10, NOV = 11, DEC = 12
    };

    enum class LatestState : unsigned char {
        ALPHA, BETA, RELEASE
    };

    constexpr LatestState LATEST_STATE = LatestState::ALPHA;

    enum class EcVersion : unsigned char
    {
        NOT_EMOJI,
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
        E2_0,
        V_9_0,
        E4_0,
        V_10_0,
        V_11_0,
        V_12_0,
        V_12_1,
        V_13_0,
        E13_1,
        V_14_0,
        V_15_0,
        V_15_1,
        V_16_0,
        V_17_0,
        NN,
        TOO_HIGH = NN,
        FIRST = 0,
        FIRST_MEANING = V_1_1,
        NO_VALUE = NOT_EMOJI,
        LAST = NN - 1,
        E0_6 = V_6_0,
        E0_7 = V_7_0,
        E1_0 = V_8_0,
        E3_0 = V_9_0,
        E5_0 = V_10_0,
        E11_0 = V_11_0, ///< 11+: Unicode and emoji are released together
        E12_0 = V_12_0,
        E12_1 = V_12_1,
        E13_0 = V_13_0,
        E14_0 = V_14_0,
        E15_0 = V_15_0,
        E15_1 = V_15_1,
        E16_0 = V_16_0,        
        E17_0 = V_17_0,
    };
    inline EcVersion& operator ++ (EcVersion& x) noexcept
    {
        static_assert(sizeof(EcVersion) == sizeof(unsigned char), "Strange underlying type");
        ++reinterpret_cast<unsigned char&>(x);
        return x;
    }

    enum class EcBidiStrength : unsigned char {
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
        NONSPACING_MARK,            ///<  NSM    Haček = caron
        OTHER_NEUTRAL,              ///<  ON     ©
        MIRRORED,                   ///<  ON + 'Y', [
        RTL,                        ///<  R      ←
        SEGMENT_SEPARATOR,          ///<  S      Tab sign
        WHITE_SPACE,                ///<  WS     Space sign
        NN,
        NO_VALUE = NN,
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

    DEFINE_ENUM_TYPE_IN_NS(uc, EcNumType, unsigned char,
        NONE,               ///< No numeric value
        DIGIT,              ///< Digit (Indian 1)
        SPECIAL_DIGIT,      ///< Special digit (superscript ³)
        NUMBER,             ///< Number (Roman II)
        CJK_RARE,           ///< CJK rare meaning
        CJK_ACCOUNTING,     ///< CJK accounting meaning (to prevent counterfeits)
        CJK_ZHUANG,         ///< CJK Zhuang meaning
        CJK_VIETNAMESE      ///< CJK Vietnamese meaning
    )

    struct NumType;

    enum class FracType : unsigned char {
        NONE,       ///< Not a number
        INTEGER,    ///< Integer
        VULGAR,     ///< Vulgar fraction
        DECIMAL,    ///< Top-heavy (>1) convertible to decimal
                    ///< present in Tibetan
    };

    /// @warning  All fractions are reduced!
    struct Numeric
    {
        long long num = 0, denom = 0, altInt = 0;
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
        NN,
        NO_VALUE = NN
    };

    struct Version;
    struct NumType;
    struct Script;
    struct Category;
    struct BidiClass;
    struct Block;
    struct Font;

    struct StyleSheet {
        short topPc = 0, botPc = 0;

        ///  @warning  This is QSS way
        QString toQ() const;
        operator QString() const { return toQ(); }

        ///  @warning  This is GRAPHIC_SAMPLE’s way, normal QSS is another
        QRectF applyToGraphicSample(const QRect& rect, qreal pointSize) const;
    };

    struct SampleProxy {
        QString text;
        uc::StyleSheet styleSheet;

        SampleProxy() = default;
        SampleProxy(QString x, const uc::StyleSheet y) : text(std::move(x)), styleSheet(y) {}
    };

    enum class DrawMethod : unsigned char;

    enum class TofuState : unsigned char {
        PRESENT,    ///< Not a tofu, have char
        TOFU,       ///< Tofu, have no char
        NO_FONT };  ///< Have so custom method of drawing that no font needed

    enum class TofuPlace : unsigned char {
        CJK,        ///< In CJK chars
        REST        ///< In rest of chars
    };

    struct TofuInfo {
        TofuState state = TofuState::NO_FONT;
        TofuPlace place = TofuPlace::REST;
        const uc::Block* block = nullptr;
    };

    enum class Action : unsigned char { CONTINUE = 0, STOP = 1 };

    class TextSink {
    public:
        virtual ~TextSink() = default;
        /// @warning  Those s_v’s are NOT null-terminated!
        virtual Action onText(TextRole role, std::u8string_view text) const = 0;
    };

    class FontMatcher { // interface
    public:
        virtual ~FontMatcher() = default;
        virtual bool check(char32_t cp, const uc::Font& font) const = 0;

        const uc::Font* lastHopeMatch(char32_t cp, const uc::Font& font) const;
    };

    enum class EcGlyphStyleChannel : unsigned char {
        NONE,
        GLAGOLITIC,
        VERTICAL,
        ARABIC,
        NN,
    };

    struct GlyphStyleChannel;
    struct GlyphStyleSets;

    enum class ProxyType : unsigned char {
        TABLE_DEFAULT,  ///< What we write in table with default method
        EXTENDED        ///< What we draw with other font-related methods
    };

    enum class AutoName : unsigned char { NO, YES };

    struct Cp   // code point
    {
        Int3 subj = 0;              // 3
        struct Name {
            Int3 iTech;                 // +3 = 6
        } name;
        EcCategory ecCategory;          // +1 = 8
        EcVersion ecVersion;            // +1 = 9
        EcBidiClass ecBidiClass;        // +1 = 10
        EcScript ecScript;              // +1 = 11
        uint8_t iNumeric;               // +1 = 12
        mutable Cfgs flags;             // +2 = 14
        struct ScriptSpecific {
            unsigned char v1;
            signed char v2;             // +2 = 16

            // For Hani
            unsigned char haniRadical() const { return v1; }
            signed char haniPlusStrokes () const { return v2; }
            constexpr operator bool() const noexcept { return v1; }
        } scriptSpecific;

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
        const Font* font(const uc::FontMatcher& matcher) const;
        SampleProxy sampleProxy(
                ProxyType proxyType,
                EmojiDraw emojiDraw,
                const uc::GlyphStyleSets& glyphSets) const;
        QString osProxy() const;
        inline const Block& block() const;

        ///  @return [+] it is a true space, really white
        bool isTrueSpace() const;
        ///  @return [+] whether the character has graphical glyph according to complex rules
        bool hasGlyph() const;
        constexpr int plane() const { return subj.val() >> 16; }

        QString viewableName() const;
        DrawMethod drawMethod(EmojiDraw emojiMode, const uc::GlyphStyleSets& glyphSets) const;
        TofuInfo tofuInfo(SvgChecker& svgChecker) const;
        constexpr bool isAbbreviated() const noexcept { return ((flags & uc::m::ALL) == uc::m::ABBREVIATION); }
        constexpr bool isNoAa() const noexcept { return ((flags & uc::m::ALL) == uc::m::NO_AA); }
        std::u8string_view abbrev() const;
        constexpr bool isDeprecated() const noexcept { return flags.have(Cfg::U_DEPRECATED); }
        constexpr bool isDefaultIgnorable() const noexcept { return flags.have(Cfg::U_DEF_IGNORABLE); }
        constexpr bool isVs16Emoji() const noexcept { return flags.have(Cfg::U_VS16_EMOJI); }
        /// @return [+] single-char or VS16 emoji
        constexpr bool isEmoji() const noexcept
            { return isVs16Emoji() || ((flags & m::ALL) == m::SVG_EMOJI); }
        bool isVs15Emoji() const noexcept;
        bool isMark() const noexcept;
        constexpr bool hasStyle() const { return flags.haveAny(STYLE_ALL); }
        EcGlyphStyleChannel ecStyleChannel() const;
        inline const GlyphStyleChannel styleChannel() const;

        /// @return  [+] the very text we called STOP on
        ///          [0] we traversed everything and did not stop
        std::u8string_view traverseTexts(
                AutoName autoName, const TextSink& sink) const;
        template <class Body> inline std::u8string_view traverseTextsT(
                AutoName autoName, const Body& body) const;

        /// @warning  This s_v is NOT null-terminated!
        std::u8string_view techName() const;

        /// @warning  This s_v is NOT null-terminated!
        std::u8string_view explicitMainName() const;

        /// @return  [+] the 1st text if that role
        ///          [0] no text of that role
        std::u8string_view getText(TextRole role) const;
    private:
        QString markProxy(char32_t code) const;
        std::u8string_view autoName() const;
    };

    size_t sprintPlus(char* buf, size_t n, std::u32string_view text);

    template<size_t N>
    inline size_t sprintPlus(char (&buf)[N], std::u32string_view text)
        { return sprintPlus(buf, N, text); }

    // Check your calculations once again
    static_assert(sizeof(Cp) == 16, "Cp size wrong");

    extern Cp cpInfo[N_CPS];
    extern const char8_t allStrings[];
    extern const Numeric allNumerics[N_NUMERICS];

    enum class TitleMode : unsigned char { SHORT, LONG };

    struct LibNode {
        std::u32string_view value;
        std::u8string_view text;
        short iParent, nChildren, iFirstChild;
        Lfgs flags;
        uc::EcVersion ecEmojiVersion = uc::EcVersion::NOT_EMOJI;

        EmojiDraw emojiDraw() const;
        inline const uc::Version& emojiVersion() const;
        const uc::Version& emojiPrevVersion() const;

        QString viewableTitle(TitleMode mode) const;

        /// Prints “U+0000+1111+2222”
        /// @return new pos
        size_t sprintUPLUS(char* data, size_t n) const;

        /// @return new pos
        template<size_t N>
        size_t sprintUPLUS(char (&data)[N]) const { return sprintUPLUS(data, N); }

        /// Prints “0000+1111+2222”
        /// @return new pos
        size_t sprintPlus(char* data, size_t n) const
            { return uc::sprintPlus(data, n, value); }

        /// @return new pos
        template<size_t N>
        size_t sprintPlus(char (&data)[N]) const { return sprintPlus(data, N); }
    };

    constexpr auto ILIB_EMOJI_ROOT = 1;   ///< index of Emoji node
    extern const LibNode libNodes[];
    size_t nLibNodes();
    std::span<const LibNode> allLibNodes();

    struct OldCompSpan {
        std::span<const Flags<uc::OldComp>> span;
        char32_t firstCp;

    };
    inline bool operator < (char32_t x, const OldCompSpan& y) { return (x < y.firstCp); }

    extern const uc::OldCompSpan oldCompSpans[];

    /// We sync interface and implementation here, but let it be.
    /// If becomes complex → decouple
    using SetOfChar = std::unordered_set<char32_t>;

}   // namespace uc

#endif
