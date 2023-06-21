#pragma once

// STL
#include <string>

// Qt
#include <QFont>
#include <QFontDatabase>
#include <QPixmap>

// Misc
#include "u_TypedFlags.h"
#include "u_DumbSp.h"

// Unicode data
#include "UcAutoDefines.h"
#include "UcDating.h"

constexpr char32_t NO_TRIGGER = 0xDEADBEEF;


template <class T, size_t N>
const T* findInArray(std::string_view needle, const T (&haystack)[N])
{
    for (auto& v : haystack) {
        if (v.id == needle) {
            return &v;
        }
    }
    return nullptr;
}

template <class T>
concept Locable = requires(const T& x) { x.loc; };

namespace fst {
    constexpr auto TOFU = static_cast<QFont::StyleStrategy>(
                QFont::PreferAntialias | QFont::ForceOutline | QFont::NoFontMerging
                | QFont::PreferQuality);
    constexpr auto COMPAT = static_cast<QFont::StyleStrategy>(
                QFont::PreferAntialias | QFont::ForceOutline | QFont::PreferMatch);
    constexpr auto NO_AA = static_cast<QFont::StyleStrategy>(
                QFont::NoAntialias | QFont::ForceOutline | QFont::PreferMatch);
}

class Percent
{
public:
    constexpr explicit Percent() noexcept = default;
    constexpr explicit Percent(int x) noexcept : fValue(x) {}
    constexpr int value() const noexcept { return fValue; }
    constexpr int apply(int x) const noexcept { return x * fValue / 100; }
private:
    int fValue = 100;
};

constexpr Percent operator "" _pc (unsigned long long x) { return Percent(x); }

namespace uc {

    constexpr unsigned NO_CHAR = std::numeric_limits<unsigned>::max();
    //const QString STUB_CIRCLE = "A \u25CC";
    constexpr QChar STUB_CIRCLE { 0x25CC };
    constexpr std::u8string_view STUB_RTL_CIRCLE  = u8"\u200F\u25CC";   // 200F RLM  25CC dotted circle
    constexpr std::u8string_view STUB_ALM_CIRCLE  = u8"\u061C\u25CC";   // 061C ALM  25CC dotted circle
        // PUA = Private user area of Unicode, they are non-standard chars mapped to these codepoints
    constexpr QChar STUB_PUA_VIRAMA { 0xE001 };
    constexpr QChar STUB_PUA_TONE_RIGHT { 0xE002 };
    constexpr QChar STUB_PUA_TONE_TOPRIGHT { 0xE003 };
    constexpr QChar STUB_PUA_TONE_ABOVE { 0xE004 };
    constexpr QChar STUB_PUA_TONE_BELOW { 0xE005 };
    constexpr QChar STUB_PUA_OVERLAP { 0xE006 };
    constexpr QChar STUB_PUA_CONTINUING_OVERLAP { 0xE007 };
    constexpr QChar STUB_PUA_BIG_STAR { 0xE008 };
    constexpr QChar STUB_PUA_TAG { 0xE009 };
    constexpr QChar STUB_PUA_BIG_CIRCLE { 0xE00A };
    constexpr QChar STUB_PUA_ENCLOSURE_START { 0xE00B };
    constexpr QChar STUB_PUA_ENCLOSURE_END { 0xE00C };
    constexpr QChar STUB_PUA_WALL_START { 0xE00D };
    constexpr QChar STUB_PUA_WALL_END { 0xE00E };
    constexpr QChar STUB_PUA_DUPLOYAN { 0xE00F };
    constexpr QChar STUB_PUA_CJK_APPROX { 0xE010 };

    enum class EcLangLife
    {
        NOMATTER,       ///< Symbols (language’s life does not matter)
        ALIVE,          ///< UNESCO safe (Ukrainian)
        ENDANGERED,     ///< UNESCO vulnerable (Belorussian)
                        ///< UNESCO endangered (Walloon)
                        ///< UNESCO highly endangered (Crimean Tatar)
        COMPETING,      ///< Loses competition to another script
        DEAD,           ///< UNESCO elders only (Votic)
                        ///< UNESCO dead (Yugh)
        CONSERVED,      ///< Historical, writing tradition conserved (Glagolitic)
        UNDECIPHERED,   ///< Was silent and still remains undeciphered (Linear A)
        PARTLY_DECIPHERED, ///< Was silent, then partly deciphered (Carian)
        DECIPHERED,     ///< Was silent, then deciphered (cuneiform)
        SACRED,         ///< Used nowadays as sacred (Coptic, Avestan)
        NEW,            ///< Newly-created (Adlam)
        REVIVED,        ///< Revived (Ahom)
        EXPERIMENTAL,   ///< Experimental script for existing languages (Deseret)
        //ARTIFICIAL,     ///< Artificial languages (Esperanto, Ithkuil); unencoded right now
        //FICTIONAL       ///< Fictional languages (Quenya, Klingon); unencoded right now
        NN
    };

    enum class EcScriptType
    {
        NONE,
        ALPHABET,       ///< Pure alphabet script (Latin, Cyrillic)
        ARGUABLE,       ///< Alphabet scripts that use syllables as units
        CONSONANT,      ///< Consonant script (Hebrew)
        SYLLABLE,       ///< Syllable script (Hiragana)
        ALPHASYLLABLE,  ///< Partly alphabet, partly syllable (Iberian)
        ABUGIDA,        ///< Syllable script where similar syllables have
                            ///<   similar symbols (Canadian syllable)
        ABUGIDA_BRAHMI, ///< … + based on Brahmi: K=ka, K*=ki, K’=K (Kannada, Thai)
        HIEROGLYPH,     ///< Pictures of things (CJK)
        SYLLABOHIEROGLYPH, ///< Syllable + hieroglyph (Linear A/B)
        CONSONANTHIEROGLYPH, ///< Consonant + hieroglyph (Egyptian)
        CODE,           ///< Code/shorthand
        NONTEXT,        ///< Non-text
        //SYMBOL,         ///< Miscellaneous symbols
        //GAME,           ///< Tabletop games
        //EMOJI,          ///< Emoji pictures
        NN,
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

    enum class EcWritingDir
    {
        NOMATTER,
        LTR,
        RTL,
        LTR_CJK,
        LTR_CUNEIFORM,
        LTR_BU,
        LTR_COL,
        TD,
        BOTH,
        TANGUT,
        RTL_COL,
        RTL_MOSTLY,
        SOGDIAN,
        NN,
    };

    enum class EcContinent {
        NONE,
        TECH,       ///< For Unicode’s own needs
        EUROPE,     ///< Europe, incl. Georgia and Mediterranean
        ASIA,       ///< Mainland Asia
        CJK,        ///< Hani and derived scripts
        OCEAN,      ///< Indian and Pacific Ocean
        AFRICA,
        AMERICA,
        NN
    };

    struct GlyphStyleChannel {
        unsigned count;
        std::string_view name;

        constexpr operator bool() const noexcept { return count; }
    };
    extern const GlyphStyleChannel glyphStyleChannelInfo[];

    struct GlyphStyleSets {
        unsigned vals[static_cast<int>(EcGlyphStyleChannel::NN)] { 0 };
        constexpr unsigned& operator[](EcGlyphStyleChannel i) { return vals[static_cast<int>(i)]; }
        constexpr const unsigned& operator[](EcGlyphStyleChannel i) const { return vals[static_cast<int>(i)]; }

        static const GlyphStyleSets EMPTY;
    };

    enum class EcFont
    {
        NORMAL,
          Z_NOR_1,
          Z_NOR_2,
          Z_NOR_3,
          Z_NOR_4,
          Z_NOR_5,
          Z_NOR_6,
          Z_NOR_7,
         NOTO,
          Z_NOT_1,
         NOTO_SYMBOL2,
         NOTO_SYMBOL2_BIGGER,
         SYMBOL,
         HISTORIC,
         PUNCTUATION,
           Z_PUN_1,
           Z_PUN_2,
           Z_PUN_3,
         MATH,
           NOTO_MATH,
         MUSIC,
           Z_MUS_1,
         MUSIC_BIGGER,
         MUSIC_NORMAL,
         GADUGI,
         DEJAVU,
         FUNKY,
          Z_FUN_1,
         FUNKY_RTL,
        ADLAM,
        AHOM,
        ANATOLIAN,
        ARABIC,
        ARABIC_NOTO,
          Z_ARN_1,
          Z_ARN_2,
        ARAMAIC,
        ARMENIAN,
          Z_ARM_1,
        AVESTAN,
        BALINESE,
          Z_BAL_1,
        BAMUM,
        BASSA_VAH,
        BATAK,
        VEDIC,      // Devanagari → Nadinagari → Bengali
          Z_VED_1,
        BENGALI,
        BHAIKSUKI,
        BRAHMI,
          Z_BRA_1,
          Z_BRA_2,
        BUGINESE,   // Here we use Lelewadee UI → specialized W7 font
          Z_BUG_1,
        BUHID,
        CANADIAN_ABORIGINAL,
        CAUCASIAN_ALBANIAN,
        CHAKMA,
        CHAM,
        CHEROKEE,
        CHORASMIAN,
        CJK,
          Z_CJK_1,
          Z_CJK_2,
          Z_CJK_3,
          Z_CJK_4,
          Z_CJK_5,
          Z_CJK_6,
          Z_CJK_7,
          Z_CJK_8,
         CJK_COMPAT,
           Z_CJC_1,
           Z_CJC_2,
         CJK_KANBUN,
         CJK_STRUCTURE,
           Z_CJS_1,
           Z_CJS_2,
         CJK_NEWHAN,
           Z_HNA_1,
        COPTIC,
        CUNEIFORM,
        DEVANAGARI,
        DINGBAT,
          Z_DIN_1,
          Z_DIN_2,
          Z_DIN_3,
          Z_DIN_4,
        DINGBAT2,       // Dingbats of BMP — need main font Cambria
          Z_DI2_1,
          Z_DI2_2,
          Z_DI2_3,
          Z_DI2_4,
        DIVES_AKURU,
        DOGRA,
        DUPLOYAN,
        EGYPTIAN,
          Z_EGY_1,
          Z_EGY_2,
        ELBASAN,
        ELYMAIC,
        ETHIOPIC,
        GEORGIAN_NUSKHURI,  // Special bhv for Nuskhuri W7/10. First Sylfaen that covers all Nuskuri in W10.
          Z_NUS_1,      // Then DejaVu, a REALLY WONDERFUL Nuskhuri → a few chars are missing, and FALL DOWN to…
        GEORGIAN,       // …Noto Serif that surely covers the entire Georgian
        GLAGOLITIC,
        GRANTHA,
        GUJARATI,
        GUNJALA_GONDI,
        GURMUKHI,
        HANUNOO,
        HATRAN,
        HEBREW,
        HENTAIGANA,
          Z_HEN_1,
        JAVANESE,
        KAITHI,
          Z_KAI_1,
        KANNADA,
        KAWI,
        KAYAH_LI,
        KHAROSHTHI,
        KHITAN_SMALL,
        KHMER,
        KHOJKI,
        KHUDAWADI,
        KOREAN,
          Z_KOR_1,
          Z_KOR_2,
          Z_KOR_3,
        LANNA,
        LAO,
        LATIN,
          Z_LAT_1,
          Z_LAT_2,
        LEPCHA,
        LIMBU,
        LINEAR_A,
        LINEAR_B,
        LISU,
        MAHAJANI,
        MAKASAR,
        MALAYALAM,
        MANDAIC,
        MANICHAEAN,
        MARCHEN,
        MASARAM_GONDI,
        MEDEFAIDRIN,
        MEETEI,
        MENDE_KIKAKUI,
        MEROITIC,
        MODI,
        MONGOLIAN,
        MRO,
        MULTANI,
        MYANMAR,
        NABATAEAN,
        NANDINAGARI,
        NEWA,
        NKO,
        NORTH_ARABIAN,
        NP_HMONG,
        NUSHU,
        OGHAM,
        OLCHIKI,
        OLD_HUNGARIAN,
        OLD_ITALIC,
        OLD_PERMIC,
        OLD_PERSIAN,
        OLD_SOGDIAN,
        OLD_UYGHUR,
        ORIYA,
        ORKHON,
          Z_ORK_1,
        PAHAWH_HMONG,
        PALMYRENE,
        PARTHIAN,
        PAU_CIN_HAU,
        PHAGS_PA,
        PHOENICIAN,
        MAHJONG_TILES,      // Fall down to Phaistos Disc
         PHAISTOS_DISC,
        POLLARD,
        PSALTER_PAHLAVI,
        REJANG,
        ROHINGYA,
        RUNIC,
        SAMARITAN,
        SAURASHTRA,
        SHARADA,
        SIDDHAM,
        SIGNWRITING,
        SINHALA,
        SIYAQ_INDIC,
        SIYAQ_OTTOMAN,
        SOGDIAN,
        SOYOMBO,
        SUNDANESE,
        SYLOTI_NAGRI,
        SYRIAC,
        TAGALOG,
        TAGBANWA,
        TAKRI,
          Z_TAK_1,
        TAILE,
        TAILUE,
        TAI_VIET,
        TAMIL,
        TAMIL_SUPPLEMENT,
        TANGSA,
        TANGUT,
        TECHNICAL,
         Z_TEC_1,
         Z_TEC_2,
         Z_TEC_3,
         Z_TEC_4,
         Z_TEC_5,
        TELUGU,
         Z_TEL_1,
        THAANA,
        THAI,
        TIBETAN,
        TIFINAGH,
        TIRHUTA,
        UMLAUT_SYMBOL,
          Z_USY_1,
          Z_USY_2,
          Z_USY_3,
        VAI,
        VITHKUQI,
        WANCHO,
        WARANG_CITI,
        YEZIDI,
        YI,
        ZANABAZAR,
        ZNAMENNY,
        NN
    };

    enum class Ffg {
        BOLD              = 1<<0,   ///< Weight: bold
        SEMIBOLD          = 1<<1,   ///< Weight: semibold
        LIGHT             = 1<<2,   ///< Weight: light
        STUB_OFF          = 1<<3,   ///< Disable circle stub
        DESC_STD          = 1<<4,   ///< Use standard font in descriptions, not this
        DESC_BIGGER       = 1<<5,   ///< Use bigger font in descriptions
        DESC_SMALLER      = 1<<6,   ///< Use smaller font in descriptions
        FALL_TO_NEXT      = 1<<7,   ///< Also use the next font if failed to find
        BUG_FIXUP         = 1<<8,   ///< Use only for chars flagged as “render bug”
        BUG_AVOID         = 1<<9,   ///< Avoid for chars flagged as “render bug”
        CELL_SMALLER      = 1<<10,  ///< Make cell text a bit smaller
        CELL_BIGGER       = 1<<11,  ///< Make cell text a bit smaller
        STUB_FINEGRAINED  = 1<<12,  ///< Stub on/off is controlled on finer level
        STUB_RTL          = 1<<13,  ///< Use “RtL mark” char in stub
        STUB_ALM          = 1<<14,  ///< Use “Arabic letter mark” char in stub
        STUB_INTERCHAR    = 1<<15,  ///< Debug: test inter-character interval
        NOHINT_TINY       = 1<<16,  ///< Unglitch: no anti-aliasing in tiny sizes
        FALL2_IF_ACCEPTED = 1<<17,  ///< Fall over one font if not rejected;
                                    ///<   used under “Main (usually system) → backup → patch” triad (Brahmi)
        MARK_AVOID        = 1<<18,  ///< If char’s type is mark → avoid!
        DESC_BADLY_HINTED = DESC_BIGGER, ///< Not just bigger but confession that the font is badly hinted
    };

    DEFINE_ENUM_OPS(Ffg)

    struct LoadedFont;
}

extern template class dumb::Sp<uc::LoadedFont>;

namespace uc {
    struct StyleSheet {
        std::string_view v;
        constexpr operator const std::string_view& () const { return v; }

        explicit constexpr StyleSheet() = default;
        explicit constexpr StyleSheet(std::string_view x) : v(x) {}
    };

    enum class FontPlace { CELL, SAMPLE, PROBE };

    enum class Fafg {
        RAW_FONT = 1<<0, ///< [+] use RawFont structure, not QFont for checking coverage
    };

    using EvRecode = char32_t (*)(char32_t unicode);

    struct Family
    {
        std::string_view text;
        Flags<Fafg> flags {};
        EvRecode recode = nullptr;

        constexpr Family(std::string_view aText) : text(aText) {}
        constexpr Family(std::string_view aText, Fafg aFlag)
            : text(aText), flags(aFlag) {}
        constexpr Family(std::string_view aText, Flags<Fafg> aFlags)
            : text(aText), flags(aFlags) {}
        constexpr Family(std::string_view aText, EvRecode aRecode)
            : text(aText), recode(aRecode) {}
    };

    struct ProbeChar {
        char32_t value = 0;
        constexpr ProbeChar() noexcept = default;
        explicit constexpr ProbeChar(char32_t x) noexcept : value(x) {}
        explicit operator bool() const noexcept { return value; }
    };

    struct StyleChange {
        int delta = 0;
    };

    struct Font
    {
        static const QString qempty;

        Family family;
        Flags<Ffg> flags {};
        std::string_view styleSheet {};
        Percent sizeAdjust {};
        ProbeChar probeChar {};
        StyleChange styleChange {};

        mutable struct Q {
            dumb::Sp<LoadedFont> loaded {};
            /// Currently have one reason for rejection: probed and failed
            /// (Brahmi only)
            /// Dubbed as “fall to next” for description font
            bool isRejected = false;
            consteval Q() = default;
            consteval Q(const Q&) {};
        } q {};
        void load(char32_t trigger) const;

        int computeSize(FontPlace place, int size) const;
        QFont get(FontPlace place, int size, bool noAa, char32_t trigger) const;
        bool doesSupportChar(char32_t x) const;
        const QString& familiesComma() const;

        consteval Font(
                std::string_view aFamily,
                Flags<Ffg> aFlags = {},
                StyleSheet aStylesheet = StyleSheet{},
                Percent aSizeAdjust = Percent())
            : family(aFamily), flags(aFlags), styleSheet(aStylesheet),
              sizeAdjust(aSizeAdjust) {}
        consteval Font(
                std::string_view aFamily,
                Flags<Ffg> aFlags,
                ProbeChar aProbeChar)
            : family(aFamily), flags(aFlags), probeChar(aProbeChar) {}
        consteval Font(
                const Family& aFamily,
                Flags<Ffg> aFlags = {},
                StyleSheet aStylesheet = StyleSheet{},
                Percent aSizeAdjust = Percent())
            : family(aFamily), flags(aFlags), styleSheet(aStylesheet),
              sizeAdjust(aSizeAdjust) {}
        consteval Font(
                std::string_view aFamily,
                Flags<Ffg> aFlags,
                Percent aSizeAdjust)
            : family(aFamily), flags(aFlags), sizeAdjust(aSizeAdjust) {}
        consteval Font(
                const Family& aFamily,
                Flags<Ffg> aFlags,
                Percent aSizeAdjust)
            : family(aFamily), flags(aFlags), sizeAdjust(aSizeAdjust) {}
        consteval Font(
                const Family& aFamily,
                Percent aSizeAdjust)
            : family(aFamily), sizeAdjust(aSizeAdjust) {}
        consteval Font(
                std::string_view aFamily,
                Percent aSizeAdjust)
            : family(aFamily), sizeAdjust(aSizeAdjust) {}
        consteval Font(
                std::string_view aFamily,
                StyleSheet aStylesheet,
                Percent aSizeAdjust = Percent())
            : family(aFamily), styleSheet(aStylesheet),
              sizeAdjust(aSizeAdjust) {}
        consteval Font(
                std::string_view aFamily,
                Percent aSizeAdjust,
                StyleChange aVariation)
            : family(aFamily), sizeAdjust(aSizeAdjust), styleChange(aVariation) {}
        Font(const Font&) = delete;
    private:
        void newLoadedStruc() const;
    };
    extern const Font fontInfo[];

    struct LangLife
    {
        std::string_view locKey;
    };
    extern const LangLife langLifeInfo[];


    struct ScriptType
    {
        std::string_view locKey;
    };
    extern const ScriptType scriptTypeInfo[];


    struct WritingDir
    {
        std::string_view locKey;
    };
    extern const WritingDir writingDirInfo[static_cast<int>(EcWritingDir::NN)];

    struct CoarseDate {
        short year;
        Month month;
    };

    struct Version
    {
        std::u8string_view name;
        CoarseDate date;

        //unsigned nNewChars = 0;
    };
    extern const Version versionInfo[];

    constexpr int PLANE_BASE = 0;
    constexpr int PLANE_UNKNOWN = -1;

    enum class Sfg {
        NONSCRIPT = 1,
        NO_LANGS = 2,
        DESC_FROM_PREV = 4,
        SORT_KEY = 8,       ///< [+] Pseudo-script made specially for sorting
    };
    DEFINE_ENUM_OPS(Sfg)

    struct Script
    {
        std::string_view id;
        QFontDatabase::WritingSystem qtCounterpart;
        EcScriptType ecType;
        EcLangLife ecLife;
        EcWritingDir ecDir;
        EcContinent ecContinent;
        Dating time;
        EcFont ecFont;
        Flags<Sfg> flags {};

        mutable unsigned nChars = 0;
        mutable int plane = -1;
        mutable EcVersion ecVersion = EcVersion::UNKNOWN;
        mutable const Block* mainBlock = nullptr;
        struct Loc {
            std::u8string_view name, timeComment, langs, description;
        } mutable loc {};

        inline const ScriptType& type() const { return scriptTypeInfo[static_cast<int>(ecType)]; }
        inline const LangLife& life() const { return langLifeInfo[static_cast<int>(ecLife)]; }
        inline const WritingDir& dir() const { return writingDirInfo[static_cast<int>(ecDir)]; }
        inline const Font& font() const { return fontInfo[static_cast<int>(ecFont)]; }
        const Version& version() const { return versionInfo[static_cast<int>(ecVersion)]; }
        void printfLocKey(char* buf, size_t n, const char* suffix) const;

        template <size_t N>
        void printfLocKey(char (&buf)[N], const char* suffix) const
            { printfLocKey(buf, N, suffix); }
    };
    extern const Script scriptInfo[];
    const Script* findScript(std::string_view x);

    struct Cp;

    ///  Flags of Unicode block
    ///
    enum class Bfg {
        COLLAPSIBLE     = 1<<0,     ///< [+] collapsible CJK
        HAS_32_NONCHARS = 1<<1,     ///< [+] block has 32 non-characters
        FORCE_FONT      = 1<<2,     ///< [+] force NORMAL font
        UNGLITCH_MARKS  = 1<<3,     ///< [+] every combining will be from NOTO
        SCRIPTLIKE      = 1<<4,     ///< [+] keywords like LETTER behave as in script
        CJK             = 1<<5,     ///< [+] CJK for tofu counting (see remark below)
        NO_EMOJI        = 1<<6,     ///< [+] No graphic emoji
        HAS_DESCRIPTION = 1<<7,     ///< [+] Has script but also has description (no script — MUST have)
        HIPRIO_NUMBERS  = 1<<8,     ///< [+] Place its numeric search b4 the rest
        EXPERIMENT      = 1<<15,    ///< Left for experiments
    };
    ///< @warning  Bfg::COLLAPSIBLE ≠ Bfg::CJK ≠ SynthIcon.continent
    ///   Bfg::COLLAPSIBLE is for expanding/collapsing CJK blocks, and contains
    ///     • Big (32+) hand-picked CJK blocks, incl. Tang, Nshu,
    ///       CJK punctuation and special chars
    ///     !!! Kana/Hira main blocks are NOT collapsible because of importance
    ///   Bfg::CJK is for tofu counting
    ///     • Blocks from CJK encodings that are too small to be collapsible,
    ///       and cannot identify CJK by script
    ///     !!! CJK scripts and Bfg::COLLAPSIBLE are automatically CJK
    ///     !!! See also exceptions below
    ///   SynthIcon.continent is for synthesized icon drawing, and contains
    ///      all CJK incl. implicit, plus:
    ///     • Ideograph-based emoji like TV, baseball
    ///     • Non-text characters of Chinese origin: Yijing and Tai Xuan

    DEFINE_ENUM_OPS(Bfg)

    struct Continent
    {
        // Colors used on 16×16 icons: BG and FG
        // We use the same colors for synthesized icons
        struct Icon {
            QColor bgColor, fgColor, frameColor;

            Icon() = delete;
            consteval Icon(const QColor& aBg, const QColor& aFg) noexcept
                : bgColor(aBg), fgColor(aFg), frameColor(aFg) {}
            consteval Icon(const QColor& aBg, const QColor& aFg, const QColor& aFrame) noexcept
                : bgColor(aBg), fgColor(aFg), frameColor(aFrame) {}
        } icon;
    };
    extern const Continent continentInfo[];

    enum class Ifg {
        CONTINENT_OK      = 1<<0,   ///< [+] disable auto-check, continent is really OK
        MISSING           = 1<<1,   ///< [+] red icon, missing block
        CUSTOM_ENGINE     = 1<<2,   ///< [+] use custom engine in lo-res
        FORMAT            = 1<<3,   ///< [+] format char is on icon
        // These flags are merely informational and do nothing,
        // and certify that the icon is synthesized approximately because of…
        APPROX_2_CHARS    = 0,      ///< [+] 2 chars on icon:
                                    ///<      sub/super, legacy computers, Kali…
        APPROX_ROTATED    = 0,      ///< [+] rotated text on icon:
                                    ///<      Mong, Phag
        APPROX_SQUARE     = 0,      ///< [+] square of main char on icon:
                                    ///<      Latin ex F, Cyr ex D
        APPROX_OTHER_LINES = 0,     ///< [+] other lines on icon: Georian Mtavruli
        APPROX_COLLECTIVE = 0,      ///< [+] graphic icon contains collective image, not specific char:
                                    ///<      variation selectors, tags
        APPROX_COLOR      = 0,      ///< [+] graphic icon is coloured/colourless:
                                    ///<      Psalter Pahlavi, board games, CJK emoji
        APPROX_HISTORICAL = 0,      ///< [+] icon is from historical font with © issues:
                                    ///<      Cpmn, Tang, Kits
        // Synthesized icon is BIG, at least 39px, and we CAN afford drawing
        // dotted circle completely → no flag for such approximation
    };
    DEFINE_ENUM_OPS(Ifg)

    ///
    /// \brief The SynthIcon class
    ///    Initially was a description of synthesized icon in Search.
    ///    Currently it describes normal icon in Blocks too.
    ///
    struct SynthIcon
    {
        char32_t subj;
        EcContinent ecContinent;
        Flags<Ifg> flags {};
        SvgHint svgHint { 0, 0 };

        inline const Cp& cp() const;
        const Continent& continent() const;
    };

    enum class MyName { INST };

    ///
    /// \brief The BlockAlpha struct
    ///
    struct AlphaKey {
        EcScript ecScript = EcScript::NONE;
        char subKey = 0;

        const Script& script() const
            { return scriptInfo[static_cast<int>(ecScript)]; }

        /// Use block’s name as alphabetic key
        constexpr AlphaKey(MyName) {}
        /// Use (pseudo)script’s name
        constexpr AlphaKey(EcScript sc, char sk) : ecScript(sc), subKey(sk) {}
    };

    // Such a limitation: sort by first N meaning chars
    using LocSortKey = signed short[40];

    struct Block
    {
        char32_t startingCp, endingCp;
        SynthIcon synthIcon;
        std::string_view name;
        AlphaKey alphaKey;
        EcScript ecScript = EcScript::NONE;
        EcFont ecFont = EcFont::NORMAL;
        Flags<Bfg> flags {};
        EcGlyphStyleChannel ecStyleChannel = EcGlyphStyleChannel::NONE;

        /// @warning We do not delete that icon, strange constinit problems, but OK
        mutable QIcon* icon = nullptr;
        mutable const Cp* firstAllocated = nullptr;
        mutable const Cp* lastAllocated = nullptr;
        mutable int nChars = 0;
        mutable unsigned cachedIndex = 0;
        mutable EcVersion ecVersion = EcVersion::NN;
        mutable EcVersion ecLastVersion = EcVersion::FIRST;

        mutable struct Loc {
            std::u8string_view name {};
            std::u8string_view description {};
            LocSortKey sortKey {};
            bool hasEllipsis = false;
        } loc {};

        size_t permanentIndex() const;
        const Version& version() const { return versionInfo[static_cast<int>(ecVersion)]; }
        const Version& lastVersion() const { return versionInfo[static_cast<int>(ecLastVersion)]; }
        const Script& script() const { return scriptInfo[static_cast<int>(ecScript)]; }
        const Font& font() const
        {
            return (ecFont != EcFont::NORMAL)
                ? fontInfo[static_cast<int>(ecFont)]
                : script().font();
        }
        unsigned nNonChars() const;
        unsigned nTotalPlaces() const { return endingCp - startingCp + 1; }
        unsigned nEmptyPlaces() const { return nTotalPlaces() - nChars - nNonChars(); }
        bool hasDescription() const
            { return (ecScript == EcScript::NONE || flags.have(Bfg::HAS_DESCRIPTION)); }
        void printfLocKey(char* buf, size_t n, const char* suffix) const;

        template <size_t N>
        void printfLocKey(char (&buf)[N], const char* suffix) const
            { printfLocKey(buf, N, suffix); }
    };

    enum class Graphical { NO, YES };

    struct Category
    {
        UpCategory upCat;
        std::string_view id;
        Graphical isGraphical;
        mutable unsigned nChars = 0;

        struct Loc {
            std::u8string_view name;
            std::u8string_view description;
        } mutable loc {};

        void printfLocKey(char* buf, size_t n, const char* suffix) const;

        template <size_t N>
        void printfLocKey(char (&buf)[N], const char* suffix) const
            { printfLocKey(buf, N, suffix); }

        bool isIndependent() const noexcept;
    };

    extern const Category categoryInfo[static_cast<int>(EcCategory::NN)];
    inline const Category* findCategory(std::string_view x) { return findInArray(x, categoryInfo); }

    struct NumType
    {
        std::string_view locKey;
    };
    extern const NumType numTypeInfo[static_cast<int>(EcNumType::NN)];

    struct BidiClass
    {
        std::string_view id;
        std::u8string_view tech;
        EcBidiStrength strength;
        mutable unsigned nChars = 0;
        struct Loc {
            std::u8string_view name;
            std::u8string_view shortName;
            std::u8string_view description;
        } mutable loc {};

        void printfLocKey(char* buf, size_t n, const char* suffix) const;

        template <size_t N>
        void printfLocKey(char (&buf)[N], const char* suffix) const
            { printfLocKey(buf, N, suffix); }
    };
    extern const BidiClass bidiClassInfo[static_cast<int>(EcBidiClass::NN)];
    inline const BidiClass* findBidiClass(std::string_view x) { return findInArray(x, bidiClassInfo); }

    extern const Block blocks[];
    inline Buf1d<const Block> allBlocks() noexcept { return { N_BLOCKS, blocks }; }

    constexpr int CAPACITY = 65536 * N_PLANES;
    extern const Cp* cpsByCode[CAPACITY];
    extern short blocksByCode16[CAPACITY >> 4];

    // We’ll use this WS for Hani, we could take Japanese as well
    static constexpr auto WS_HANI = QFontDatabase::SimplifiedChinese;

    void completeData();
    const Block* blockOf(char32_t subj);
    inline const uc::Block& uc::Cp::block() const { return *blockOf(subj); }

    enum class EcTermCat {
        ENCODING, SERIALIZATION, SCRIPT_CLASS, CHAR_CLASS, ALGORITHM,
        PRINCIPLES, WRITING_STYLE, INPUT, OTHER, NN,
        HIDDEN = NN
    };

    enum class SortTerms { NO, YES };

    struct TermCat
    {
        std::string_view locKey;
        SortTerms sort;
    };

    extern const TermCat termCats[];

    struct Term
    {
        std::string_view key;
        EcTermCat ecCat;
        std::u8string_view engName;
        std::string_view borrowedDesc {};
        EcFont ecFont = EcFont::NORMAL;
        struct Loc {
            std::u8string_view name;
            std::u8string_view description;
            LocSortKey sortKey;
        } mutable loc {};

        const TermCat& cat() const { return termCats[static_cast<int>(ecCat)]; }

        const uc::Font& font() const { return fontInfo[static_cast<int>(ecFont)]; }

        void printfLocKey(char* buf, size_t n, const char* suffix) const;

        template <size_t N>
        void printfLocKey(char (&buf)[N], const char* suffix) const
            { printfLocKey(buf, N, suffix); }
    };

    extern const Term terms[];
    extern const Term* sortedTerms[];
    size_t nTerms();

    const Term* findTerm(std::string_view id);

    enum class OldComp {
        AMSTRAD     = 1 << 0,
        APPLE       = 1 << 1,
        AQUARIUS    = 1 << 2,
        ATARI_ST    = 1 << 3,
        ATARI_8BIT  = 1 << 4,
        MINITEL     = 1 << 5,
        MSX         = 1 << 6,
        ORIC        = 1 << 7,
        COMMODORE   = 1 << 8,
        RISC_OS     = 1 << 9,
        SINCLAIR    = 1 << 10,
        TELETEXT    = 1 << 11,
        TANDY_TRS80 = 1 << 12,
        TANDY_COCO  = 1 << 13,
        KORVET      = 1 << 14,
        LAST = KORVET
    };
    DEFINE_ENUM_OPS(OldComp)

    struct OldCompInfo {
        std::u8string_view fixedName;
        std::string_view locKey {};
        std::u8string locName() const;
    };
    extern const OldCompInfo oldCompInfo[];

    Flags<OldComp> cpOldComps(char32_t cp);

    struct AltCode {
        unsigned char dosCommon = 0, dosEn = 0, dosRu = 0, win = 0;
        unsigned short unicode = 0;
        bool hasLocaleIndependent() const
            { return (dosCommon != 0 || win != 0); }
        constexpr bool operator == (const AltCode& x) const = default;
    };

    struct AltgrKey {
        char key = 0, letter = 0;
        bool isTwice = false;
        constexpr bool operator == (const AltgrKey& x) const = default;
    };

    struct InputMethods {
        AltCode alt;
        AltgrKey birman;
        std::u8string_view sometimesKey;
        constexpr bool operator == (const InputMethods& x) const = default;
        static const uc::InputMethods NONE;
        bool hasSmth() const { return (*this != NONE); }
        bool hasAltCode() const { return (alt != NONE.alt); }
        bool hasBirman() const { return (birman != NONE.birman); }
    };

    InputMethods cpInputMethods(char32_t cp);

    inline size_t sprintUPLUS(char* buf, size_t n, char32_t code)
        { return snprintf(buf, n, "U+%04X", static_cast<unsigned>(code)); }

    template<size_t N>
    inline size_t sprintUPLUS(char (&buf)[N], char32_t code)
        { return sprintUPLUS(buf, N, code); }

    /// @return new n1
    size_t appendUPLUS(char* buf, size_t n, size_t n1, char32_t code);

    /// @return new n1
    template<size_t N>
    inline size_t appendUPLUS(char (&buf)[N], size_t n1, char32_t code)
        { return appendUPLUS(buf, N, n1, code); }

    QFont funkyFont(FontPlace place, int size, char32_t trigger);

    void finishTranslation(
            const std::unordered_map<char32_t, int>& sortOrder,
            std::u32string_view ellipsisBlocks);


    inline std::strong_ordering operator <=> (char32_t x, const Cp& y)
        { return x <=> y.subj.ch32(); }
    inline std::strong_ordering operator <=> (const Cp& x, char32_t y)
        { return x.subj.ch32() <=> y; }
    // Need somehow for lower_bound
    inline bool operator < (const Cp& x, char32_t y)
        { return x.subj.ch32() < y; }

    template <class Body>
    class TextSinkT final : public TextSink {
    public:
        TextSinkT(const Body& aBody) noexcept : body(aBody) {}
        Action onText(TextRole role, std::u8string_view text) const override {
            using Result = std::invoke_result_t<Body, TextRole, std::u8string_view>;
            if constexpr (std::is_same_v<void, Result>) {
                body(role, text);
                return Action::CONTINUE;
            } else if constexpr (std::is_nothrow_convertible_v<Result, Action>) {
                return body(role, text);
            } else {
                static_assert(always_false_v<Result>, "Body should return void or Action");
            }
        }
    private:
        const Body& body;
        template<class> inline static constexpr bool always_false_v = false;
    };

    inline Action stopIf(bool x) noexcept { return static_cast<Action>(x); }

}   // namespace uc

namespace sw {

    extern const sw::Char SIMPLE_CHAR, EMPTY_CHAR;

    // Opaque structure that holds Sutton SignWriting character info.
    class Info {
    public:
        /// Pseudo-ctor
        explicit Info(const uc::Cp& cp);

        const uc::Cp& cp() const { return fCp; }
        char32_t subj() const { return fCp.subj; }
        /// @return [+] It is actually a SignWriting character, but has no variations
        bool isSimple() const { return (*fData == SIMPLE_CHAR); }
        bool hasSmth() const { return (fData != &EMPTY_CHAR); }
        /// @return [+] has at least one char of fill, 0-based
        bool hasFill0(int i) const;
        /// @return [+] has at least one char of rotation, 0-based
        bool hasRot0(int i) const;
        operator bool() const { return hasSmth(); }
        char32_t baseChar() const noexcept;
    private:
        const uc::Cp& fCp;
        const sw::Char* fData;
    };

}   // namespace sw


consteval uc::StyleSheet operator "" _sty (const char* data, size_t n)
    { return uc::StyleSheet{std::string_view { data, n }}; }


// Inline implementations
// …Numeric
inline const uc::NumType& uc::Numeric::type() const { return numTypeInfo[static_cast<int>(ecType)]; }

// …Cp
inline const uc::Numeric& uc::Cp::numeric() const { return allNumerics[iNumeric]; }
inline const uc::Version& uc::Cp::version() const { return versionInfo[static_cast<int>(ecVersion)]; }
inline const uc::Category& uc::Cp::category() const { return categoryInfo[static_cast<int>(ecCategory)]; }
inline const uc::BidiClass& uc::Cp::bidiClass() const { return bidiClassInfo[static_cast<int>(ecBidiClass)]; }
inline const uc::Script& uc::Cp::script() const { return scriptInfo[static_cast<int>(ecScript)]; }
inline const uc::Script& uc::Cp::scriptEx() const
    { return scriptInfo[static_cast<int>(ecScriptEx())]; }
inline bool uc::Cp::isTrueSpace() const
        { return (ecCategory == EcCategory::SEPARATOR_SPACE &&
                  ecScript != EcScript::Ogam); }    // Ogham space is a continuing line (edge of stick)
inline const uc::GlyphStyleChannel uc::Cp::styleChannel() const
    { return glyphStyleChannelInfo[static_cast<int>(ecStyleChannel())]; }

// Name
template <class Body>
inline std::u8string_view uc::Cp::Name::traverseAllT(const Body& body) const {
    TextSinkT sink(body);
    return traverseAll(sink);
}

// SynthIcon
inline const uc::Cp& uc::SynthIcon::cp() const { return *cpsByCode[subj]; }

// LibNode
inline const uc::Version& uc::LibNode::emojiVersion() const { return versionInfo[static_cast<int>(ecEmojiVersion)]; }
