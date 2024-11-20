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
#include "u_Array.h"
#include "u_EnumSize.h"
#include "u_EcArray.h"
#include "u_MicroList.h"

// Unicode data
#include "UcCommon.h"
#include "UcDating.h"
#include "UcContinents.h"

class QIcon;

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
    constexpr QChar STUB_PUA_VIRAMA { 0xE001 };             // Virtual viraam (=subjoiner, conjoiner)
    constexpr QChar STUB_PUA_TONE_RIGHT { 0xE002 };         // Plrd tone right
    constexpr QChar STUB_PUA_TONE_TOPRIGHT { 0xE003 };      // Plrd tone top-right
    constexpr QChar STUB_PUA_TONE_ABOVE { 0xE004 };         // Plrd tone above
    constexpr QChar STUB_PUA_TONE_BELOW { 0xE005 };         // Plrd tone below
    constexpr QChar STUB_PUA_OVERLAP { 0xE006 };            // Dupl overlap
    constexpr QChar STUB_PUA_CONTINUING_OVERLAP { 0xE007 }; // Dupl continuing overlap
    constexpr QChar STUB_PUA_BIG_STAR { 0xE008 };           // Egyp big asterisk
    constexpr QChar STUB_PUA_TAG { 0xE009 };                // Tag character
    constexpr QChar STUB_PUA_ZOOM { 0xE00A };               // Magnifying glass for zoom
    constexpr QChar STUB_PUA_ENCLOSURE_START { 0xE00B };    // Egyp enclosure start
    constexpr QChar STUB_PUA_ENCLOSURE_END { 0xE00C };      //               …end
    constexpr QChar STUB_PUA_WALL_START { 0xE00D };         // Egyp waled ecnclosure start
    constexpr QChar STUB_PUA_WALL_END { 0xE00E };           //                      …end
    constexpr QChar STUB_PUA_DUPLOYAN { 0xE00F };           // Dupl K+A+TS
    constexpr QChar STUB_PUA_CJK_APPROX { 0xE010 };         // Image of CJK 303E
    constexpr QChar STUB_PUA_PLUS { 0xE011 };               // Plus for synthesized virtual virama

    DEFINE_ENUM_TYPE_IN_NS(uc, EcLangLife, unsigned char,
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
        FOR_REVIVAL,    ///< Made for revival (Osage)
        EXPERIMENTAL,   ///< Experimental script for existing languages (Deseret)
        //ARTIFICIAL,     ///< Artificial languages (Esperanto, Ithkuil); unencoded right now
        //FICTIONAL       ///< Fictional languages (Quenya, Klingon); unencoded right now
    )

    enum class EcScriptType : unsigned char {
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

    enum class EcUpCategory : unsigned char {
        CONTROL,
        FORMAT,
        LETTER,
        MARK,
        NUMBER,
        PUNCTUATION,
        SYMBOL,
        SEPARATOR,
        NN,
        NO_VALUE = NN
    };

    DEFINE_ENUM_TYPE_IN_NS(uc, EcWritingDir, unsigned char,
        NOMATTER,
        LTR,
        RTL,
        LTR_CJK,
        LTR_CUNEIFORM,
        LTR_BU,
        LTR_COL,
        TD,
        BOTH_LTR,   ///< Both, encoded as →
        RTL_COL,
        RTL_MOSTLY,
        SOGDIAN
    )

    struct GlyphStyleChannel {
        unsigned count;
        std::string_view name;
        EcGlyphStyleChannel value;

        constexpr operator bool() const noexcept { return count; }
    };
    extern const GlyphStyleChannel glyphStyleChannelInfo[];

    struct GlyphStyleSets {
        unsigned vals[static_cast<int>(EcGlyphStyleChannel::NN)] { 0 };
        constexpr unsigned& operator[](EcGlyphStyleChannel i) { return vals[static_cast<int>(i)]; }
        constexpr const unsigned& operator[](EcGlyphStyleChannel i) const { return vals[static_cast<int>(i)]; }

        static const GlyphStyleSets EMPTY;
    };

    enum class EcFont : unsigned char
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
          Z_NOT_1,      ///< Funky, do not delete, needed for alias GREEK_COPTIC
         NOTO_SYMBOL2,
          Z_NS2_1,      ///< Backed by Pseudographica
          Z_NS2_2,      ///< Backed by Funky
         NOTO_SYMBOL2_BIGGER,
          Z_N2B_1,
         SYMBOL,
         HISTORIC,
         PUNCTUATION,
           Z_PUN_1,
           Z_PUN_2,
           Z_PUN_3,
         MATH,
           Z_MTH_1,
           Z_MTH_2,
         MUSIC,
           Z_MUS_1,
         MUSIC_BIGGER,
         MUSIC_NORMAL,
         GADUGI,
         DEJAVU,
         FUNKY,
          Z_FUN_1,
         FUNKY_BIGGER,
         FRANKEN_SANS,
         FRANKEN_SANS_BIGGER,  // Parthian, Wancho, Warang
         FRANKEN_SANS_110,     // Ogham
        ADLAM,
        AHOM,
        ANATOLIAN,
        ARABIC,
          Z_ARA_1,
          Z_ARA_2,
        ARMENIAN,
          Z_ARM_1,
        BALINESE,
          Z_BAL_1,
        BAMUM,
        BATAK,
        VEDIC,      ///< Devanagari fixup → Devanagari → Nadinagari → Bengali
          Z_VED_2,
          Z_VED_1,
        BENGALI,
        BHAIKSUKI,
        BRAHMI,
          Z_BRA_1,
        BUGINESE,   ///< Here we use Lelewadee UI → specialized W7 font
          Z_BUG_1,
        BUHID,
        CANADIAN_ABORIGINAL,
        CHAKMA,
        CHAM,
        CJK,
          Z_CJK_1,
          Z_CJK_2,
          Z_CJK_3,
          Z_CJK_4,
         CJK_COMPAT,
           Z_CJC_1,
           Z_CJC_2,
           Z_CJC_3,
         CJK_KANBUN,
         CJK_STRUCTURE,
           CJK_STROKES,  ///< also dubbed as CJK strokes
           Z_CJS_2,
         CJK_SIMSUN,
         CJK_NEWHAN,
           CJK_UHAN,    ///< Unicodia Han
         CJK_SYMBOLS,
           Z_CJY_1,
           Z_CJY_2,
           Z_CJY_3,
         CJK_FULLHALF,
           Z_CJF_1,
         CJK_ENCLOSED,
           Z_CJE_1,
        CUNEIFORM,
        DEVANAGARI,
          DEVANAGARI_SYSTEM,
        DEVANAGARI_BIGGER,  ///< Also use fixup font
          Z_DE1_1,
        DINGBAT,
          Z_DIN_1,
          Z_DIN_2,
          Z_DIN_3,
        DINGBAT2,       ///< Dingbats of BMP — need main font Cambria
          Z_DI2_1,
          Z_DI2_2,
          Z_DI2_3,
        DIVES_AKURU,
        DOGRA,
        DOMINOES,
        DUPLOYAN,
        EGYPTIAN,
          Z_EGY_1,
        ELBASAN,
        ELYMAIC,
        ENCLOSED_ALNUM, // Korean
          Z_EAN_1,      // Backed up by Symbol2
        ETHIOPIC,
        GEORGIAN_NUSKHURI,  // Special bhv for Nuskhuri W7/10. First Sylfaen that covers all Nuskuri in W10.
          Z_NUS_1,      // Then DejaVu, a REALLY WONDERFUL Nuskhuri → a few chars are missing, and FALL DOWN to…
        GEORGIAN,       // …Noto Serif that surely covers the entire Georgian
        GLAGOLITIC,
          Z_GLA_1,
        GRANTHA,
        GUJARATI,
        GUNJALA_GONDI,
        GURMUKHI,
        HANUNOO,
        HEBREW,
        HENTAIGANA,
          Z_HEN_1,
        JAVANESE,
        KAITHI,
        KANNADA,
          Z_KAN_1,
        KAWI,
          Z_KAW_1,
        KAYAH_LI,
        KHAROSHTHI,
        KHITAN_SMALL,
          Z_KHI_1,
        KHMER,
        KHOJKI,
        KHUDAWADI,
        KIRAT,
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
        MAHAJANI,
        MALAYALAM,
        MANDAIC,
        MANICHAEAN,
        MARCHEN,
        MASARAM_GONDI,
        MEETEI,
        MEROITIC,
        MODI,
        MONGOLIAN,
          Z_MNG_1,
        MULTANI,
        MYANMAR,
        NANDINAGARI,
        NEWA,
        NKO,
        NP_HMONG,
        NUSHU,
        OCR,
        OLCHIKI,
        OLD_HUNGARIAN,
        OLD_SOGDIAN,
        OLD_UYGHUR,
        ORIYA,
        PAHAWH_HMONG,
        PERMIC_OLD,
        PHAGS_PA,
        MAHJONG_TILES,      // Fall down to Phaistos Disc
         PHAISTOS_DISC,        
        POLLARD,
        PSEUDOGRAPHICS,
        REJANG,
        ROHINGYA,
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
        SUNUWAR,
        SYLOTI_NAGRI,
        SYRIAC,
        TAGALOG,
        TAGBANWA,
        TAKRI,
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
        TIRHUTA,
        TULU,
        UMLAUT_SYMBOL,
          Z_USY_1,
          Z_USY_2,
          Z_USY_3,
        VAI,
        VITHKUQI,
        YEZIDI,
        YI,
        ZANABAZAR,
        ZNAMENNY,
        NN,
        ARABIC_NOTO = ARABIC,  ///< Both Scheh and Noto chains have Scheh → Noto → Funky,
                               ///< but do not lose difference
        GREEK_COPTIC = NOTO,   ///< We rely here that Noto does not implement A SINGLE char of Coptic,
                               ///< and Noto is backed with Funky

    };

    enum class Ffg {
        BOLD              = 1<<0,   ///< Weight: bold
        SEMIBOLD          = 1<<1,   ///< Weight: semibold
        LIGHT             = 1<<2,   ///< Weight: light
        STUB_OFF          = 1<<3,   ///< Disable circle stub
        DESC_BIGGER       = 1<<4,   ///< Use bigger font in descriptions
        DESC_SMALLER      = 1<<5,   ///< Use smaller font in descriptions
        FALL_TO_NEXT      = 1<<6,   ///< Also use the next font if failed to find
        BUG_FIXUP         = 1<<7,   ///< Use only for chars flagged as “render bug”
        BUG_AVOID         = 1<<8,   ///< Avoid for chars flagged as “render bug”
        CELL_SMALLER      = 1<<9,   ///< Make cell text a bit smaller
        CELL_BIGGER       = 1<<10,  ///< Make cell text a bit smaller
        STUB_FINEGRAINED  = 1<<11,  ///< Stub on/off is controlled on finer level
        STUB_INTERCHAR    = 1<<12,  ///< Debug: test inter-character interval
        NOHINT_TINY       = 1<<13,  ///< Unglitch: no anti-aliasing in tiny sizes
        FALL2_IF_ACCEPTED = 1<<14,  ///< Fall over one font if not rejected;
                                    ///<   used under “Main (usually system) → backup → patch” triad (Brahmi)
        MARK_AVOID        = 1<<15,  ///< If char’s type is mark → avoid!
        DESC_AVOID        = 1<<16,  ///< Avoid in description, use next instead
        GRAPHIC_SAMPLE    = 1<<17,  ///< Draw characters graphically, killing font’s dimensions
        DESC_BADLY_HINTED = DESC_BIGGER, ///< Not just bigger but confession that the font is badly hinted
    };

    DEFINE_ENUM_OPS(Ffg)

    struct LoadedFont;

    enum class DrawMethod {
        // Font-based methods
        SAMPLE,             ///< Take font and draw string
        SPACE,              ///< Take font, measure space width and draw 2 lines
        MARCHEN,            ///< Special drawing method for Marchen
        SAMPLED_CONTROL,    ///< Draw dotted square and sample
        VERTICAL_CW,        ///< Draw Mong etc vertically, rotate CW
        VERTICAL_CCW,       ///< Draw Sogd etc vertically, rotate CCW
        // Char-based methods
        VIRTUAL_VIRAMA,     ///< Draw 25CC with that font, and then lower + with Funky
        ABBREVIATION,       ///< Draw dotted square and abbreviation
        CUSTOM_CONTROL,     ///< Draw dotted square and smth custom
        SVG_EMOJI,          ///< Draw SVG emoji
        LAST_FONT = VERTICAL_CCW };            // Last using (loading, checking) font

}   //namespace uc;

extern template class dumb::Sp<uc::LoadedFont>;

namespace uc {

    enum class FontPlace { CELL, SAMPLE, PROBE };

    enum class Fafg : unsigned char {
        // Now all fonts use
        DEHINT_DOTC = 1 << 0,  ///< Dehint dotted circle
    };

    using EvRecode = char32_t (*)(char32_t unicode);

    struct ProbeChar {
        char32_t value = 0;
        constexpr ProbeChar() noexcept = default;
        explicit constexpr ProbeChar(char32_t x) noexcept : value(x) {}
        explicit operator bool() const noexcept { return value; }
    };

    struct Family
    {
        std::string_view text;
        Flags<Fafg> flags {};
        EvRecode recode = nullptr;
        ProbeChar probeChar {};

        constexpr Family(std::string_view aText) : text(aText) {}
        constexpr Family(std::string_view aText, Fafg aFlag)
            : text(aText), flags(aFlag) {}
        constexpr Family(std::string_view aText, Flags<Fafg> aFlags)
            : text(aText), flags(aFlags) {}
        constexpr Family(std::string_view aText, EvRecode aRecode)
            : text(aText), recode(aRecode) {}
        constexpr Family(std::string_view aText, ProbeChar aProbeChar)
            : text(aText), probeChar(aProbeChar) {}
        constexpr Family(std::string_view aText, Fafg aFlag, ProbeChar aProbeChar)
            : text(aText), flags(aFlag), probeChar(aProbeChar) {}
    };

    /// Other set of styled characters is at alternate CPs → add/subtract some delta to code
    struct StyleChange {
        int delta = 0;
    };

    enum class FontGetFg {
        NO_AA = 1,
        KNOWN_TOFU = 2,
        FAST = KNOWN_TOFU,        ///< accent that it shows faster
    };

    struct Font
    {
        static const QString qempty;

        Family family;
        Flags<Ffg> flags {};
        StyleSheet styleSheet {};
        Percent sizeAdjust {};
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
        QFont get(FontPlace place, int size, Flags<FontGetFg> flags, const uc::Cp* subj) const;
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
                const Family& aFamily,
                StyleSheet aStylesheet,
                Percent aSizeAdjust = Percent())
            : family(aFamily), styleSheet(aStylesheet),
              sizeAdjust(aSizeAdjust) {}
        consteval Font(
                std::string_view aFamily,
                Flags<Ffg> aFlags,
                Percent aSizeAdjust,
                StyleChange aVariation)
            : family(aFamily), flags(aFlags), sizeAdjust(aSizeAdjust),
              styleChange(aVariation) {}
        consteval Font(
                const Family& aFamily,
                Flags<Ffg> aFlags,
                Percent aSizeAdjust,
                StyleChange aVariation)
            : family(aFamily), flags(aFlags), sizeAdjust(aSizeAdjust),
              styleChange(aVariation) {}
        Font(const Font&) = delete;
    private:
        void newLoadedStruc() const;
    };
    extern const Font fontInfo[];

    struct LangLife
    {
        std::string_view locKey;
    };
    extern const ec::Array<LangLife, EcLangLife> langLifeInfo;


    struct ScriptType
    {
        std::string_view locKey;
    };
    extern const ScriptType scriptTypeInfo[];


    struct WritingDir
    {
        std::string_view locKey;
    };
    extern const ec::Array<WritingDir, EcWritingDir> writingDirInfo;

    constexpr int PLANE_BASE = 0;
    constexpr int PLANE_UNKNOWN = -1;

    DEFINE_ENUM_TYPE_IN_NS(uc, NumOrder, unsigned char,
        NONE, UNIT, THOUSAND, HUN_THOUSAND, MILLION, DEC_MILLION, HUN_MILLION)

    struct NumOrderInfo {
        unsigned char nDigs;
        std::string_view locKey;
    };

    extern const ec::Array<NumOrderInfo, NumOrder> numOrderInfo;

    enum class Langfg : unsigned char {
        AS_NATIVE = 1<<0,       ///< [+] # of L1 (native) speakers [-] all speakers
        GREATER_THAN = 1<<1,    ///< [+] greater than (# is lower limit)
        LESS_THAN = 1<<2,       ///< [+] less than (# is upper limit)
        DECADE = 1<<3,          ///< Year = decade rather than exact year of census
        BURMESE = 1<<4,         ///< Special action for Burmese
        CUSTOM_PRENOTE = 1<<5,  ///< Custom (rather than stock) pre-note
        IN_INDIA = 1<<6,        ///< Special string “{1} in India as of {2}”
        NO_AUTO = 1<<7,         ///< [+] Avoid auto {{nspk}}, mainly for competing scripts
    };
    DEFINE_ENUM_OPS(Langfg)

    struct LangLoc {
        std::string_view locSubKey;
    };

    struct Lang {
        unsigned short mantissa;
        unsigned short hiMantissa = 0;
        NumOrder numOrder;
        Flags<Langfg> flags {};
        unsigned short year;
        MicroList<LangLoc, 3> locations {};

        constexpr bool hasValue() const noexcept { return (numOrder != NumOrder::NONE); }
        explicit constexpr operator bool() const noexcept { return hasValue(); }
    };

    constexpr Lang NO_LANG = {
        .mantissa = 0, .numOrder = NumOrder::NONE, .flags {}, .year = 0
    };

    /// Script’s flags
    enum class Sfg : unsigned char {
        NONSCRIPT      = 1<<0,  ///< [+] Non-script for search
        NO_LANGS       = 1<<1,  ///< [+] No “Languages” line
        DESC_FROM_PREV = 1<<2,  ///< [+] Description is taken from previous script (Meroitic, Sogdian)
        SORT_KEY       = 1<<3,  ///< [+] Pseudo-script made specially for sorting
        STUB_RTL       = 1<<4,  ///< Use “RtL mark” char in stub
        STUB_ALM       = 1<<5,  ///< Use “Arabic letter mark” char in stub        
        NO_STATS       = 1<<6,  ///< Do not count in # of scripts
                                ///< Hent = Hira, Zinh is scripted for search, but do not count
        DISUNIFIED     = 1<<7,  ///< Created by disunification, not merely
                                ///< introduction of new chars (Coptic only)
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
        Lang mainLang = NO_LANG;

        mutable unsigned nChars = 0;
        mutable int plane = -1;
        mutable EcVersion ecVersion = EcVersion::TOO_HIGH;
        mutable const Block* mainBlock = nullptr;
        struct Loc {
            std::u8string_view name, timeComment, langs, description;
        } mutable loc {};

        inline const ScriptType& type() const { return scriptTypeInfo[static_cast<int>(ecType)]; }
        inline const LangLife& life() const { return langLifeInfo[ecLife]; }
        inline const WritingDir& dir() const { return writingDirInfo[ecDir]; }
        inline const Font& font() const { return fontInfo[static_cast<int>(ecFont)]; }
        const Version& version() const { return versionInfo[static_cast<int>(ecVersion)]; }
        void printfLocKey(char* buf, size_t n, const char* suffix) const
            { printfLocKeyN(buf, n, suffix); }

        template <size_t N>
        void printfLocKey(char (&buf)[N], const char* suffix) const
            { printfLocKeyN(buf, N, suffix); }
    private:
        void printfLocKeyN(char* buf, size_t n, const char* suffix) const;
    };
    extern const Script scriptInfo[];
    const Script* findScript(std::string_view x);

    struct SideLang {
        std::string_view id;
        Lang lang;
    };
    extern const SideLang sideLangInfo[];
    const Lang* findSideLang(std::string_view x);

    struct Cp;

    ///  Flags of Unicode block
    ///
    enum class Bfg {
        COLLAPSIBLE     = 1<<0,     ///< [+] collapsible CJK/Egyp
        HAS_32_NONCHARS = 1<<1,     ///< [+] block has 32 non-characters
        FORCE_FONT      = 1<<2,     ///< [+] force NORMAL font
        UNGLITCH_MARKS  = 1<<3,     ///< [+] every combining will be from NOTO
        SCRIPTLIKE      = 1<<4,     ///< [+] keywords like LETTER behave as in script
        CJK             = 1<<5,     ///< [+] CJK for tofu counting (see remark below)
        NO_EMOJI        = 1<<6,     ///< [+] VS16 emoji are always text (Latin-1, arrows)
        HAS_DESCRIPTION = 1<<7,     ///< [+] Has script but also has description (no script — MUST have)
        HIPRIO_NUMBERS  = 1<<8,     ///< [+] Place its numeric search b4 the rest
        CCW             = 1<<9,     ///< [+] Rotate vertical text counter-clockwise rather than clockwise
        HIDE_STYLE      = 1<<10,    ///< [+] Hide from list: which blocks use this style channel
                                    ///<     (Glag supp has Bulgarian style only, so write just Glag)
        BIG_CONTROLS    = 1<<11,    ///< [+] Control chars use same size as normal
        VIRAMA_UP       = 1<<12,    ///< [+] Move dotted circle ¼r upwards in virtual virama
        VIRAMA_BIGGER   = 1<<13,    ///< [+] Bigger + in virtual virama
        VIRAMA_INSIDE   = 1<<14,    ///< [+] Plus inside virtual virama (Tutg)
        EMOJI_BY_CASE   = 1<<15,    ///< [+] Text/graphic VS16 emoji is case-by-case
        EMOJI_OVER_CHAR = 1<<16,    ///< [+] In VS16 blocks draw emoji over characters
        EXPERIMENT      = 1<<30,    ///< Left for experiments
    };
    ///< @warning  Bfg::COLLAPSIBLE ≠ Bfg::CJK ≠ SynthIcon.continent
    ///   Bfg::COLLAPSIBLE is for expanding/collapsing CJK blocks, and contains
    ///     • Big (32+) hand-picked CJK blocks, incl. Tang, Nshu,
    ///       CJK punctuation and special chars
    ///     • Egyptian hieros (U16 made 4k more)
    ///     !!! Kana/Hira main blocks are NOT collapsible because of importance
    ///   Bfg::CJK is for tofu counting
    ///     • Blocks from CJK encodings that cannot identify CJK by script,
    ///       but are surely related to CJK cells (e.g. ideograph structure,
    ///       half/fullwidth)
    ///     !!! CJK scripts are automatically CJK,
    ///          COLLAPSIBLE are not because of Egyp
    ///     !!! See also exceptions below
    ///   SynthIcon.continent is for synthesized icon drawing, and contains
    ///      all CJK incl. implicit, plus:
    ///     • Ideograph-based emoji like TV, baseball
    ///     • Non-text characters of Chinese origin: Yijing and Tai Xuan

    DEFINE_ENUM_OPS(Bfg)

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

    /// Subdivision of blocks for (future) plane maps
    /// @todo [future] Plane map
    enum class MapType : unsigned char {
        UNALLOCATED,    ///< unused in data, IDK what I do                      As white as possible
        PRIVATE,        ///< private-use, unused in data                        Light grey
        TECHNICAL,      ///< technical: VS, tags, specials, surrogate only      Green/acid
        EU_LATIN,       ///< Europe/Latin: Latn only                            Blue/deep
        EU_OTHER,       ///< Europe/other: Cyrl, Grek                           Blue
        EU_OTHER_DEAD,  ///< Europe/other/dead: Runr, Ogam                      Blue/pale
        AS_BRAH,        ///< Asia/Brahmic: Deva, Guru                           Orange
        AS_BRAH_DEAD,   ///< Asia/Brahmic/dead: Gran, pan-Indic Vedic           Orange/pale
        AS_OTHER,       ///< Asia/other: Arab, Armn                             Amber (like car blinkers)
        AS_OTHER_DEAD,  ///< Asia/other/dead: Armi, Khar                        Yellow
        CJ_HANI,        ///< CJK/Han: Hani only                                 Violet
        CJ_OTHER,       ///< CJK/other: Kana, Hang                              Less violet
        CJ_OTHER_DEAD,  ///< CJK/other/dead: Nshu, Kits                         Pink
        CJ_SYMBOL,      ///< CJK/symbols: punctuation, Tai Xuan                 Fuchsia
        OC,             ///< Ocean: Thaa, Sinh                                  Green
        OC_DEAD,        ///< Ocean/dead: Tglg, Kawi                             Green/pale
        AF,             ///< Africa: Tfng, Bamu                                 Chocolate
        AF_DEAD,        ///< Africa/dead: Copt, Mero                            Beige
        AM,             ///< America: Cans, Cher, Hmnp                          Red
        AM_DEAD,        ///< America/dead: Dsrt and Maya only                   Red/pale
        SYM_CODE,       ///< codes, shorthand, non-text, incl. notes            Aquamarine
        SYM_PUNCT,      ///< modifiers, punctuation, diacritics                 Black
        SYM_OTHER,      ///< maths, shapes, emoji                               Teal/steel
        //FICTIONAL,      ///< Unused for now: Cirt, Teng                       Blue-violet
    };

    class BlockResizeSink {     // interface
    public:
        virtual void act(uc::EcVersion version, char32_t before, char32_t after) const = 0;
        virtual ~BlockResizeSink() = default;
    };

    template <class Body>
    class BlockResizeSinkT : public BlockResizeSink {     // interface
    public:
        BlockResizeSinkT(const Body& x) : body(x) {}
        void act(uc::EcVersion version, char32_t before, char32_t after) const override
            { body(version, before, after); }
    private:
        const Body& body;
    };

    struct Block
    {
        char32_t startingCp, endingCp;
        SynthIcon synthIcon;
        std::string_view name;
        AlphaKey alphaKey;
        MapType mapType;
        EcScript ecScript = EcScript::NONE;
        EcFont ecFont = EcFont::NORMAL;
        Flags<Bfg> flags {};
        EcGlyphStyleChannel ecStyleChannel = EcGlyphStyleChannel::NONE;
        struct History {
            char32_t oldEndingCp = 0;
            EcVersion ecVersion = EcVersion::NO_VALUE;
        } history {};

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
        constexpr unsigned nTotalPlaces() const { return endingCp - startingCp + 1; }
        unsigned nEmptyPlaces() const { return nTotalPlaces() - nChars - nNonChars(); }
        constexpr bool hasDescription() const
            { return (ecScript == EcScript::NONE || flags.have(Bfg::HAS_DESCRIPTION)); }
        bool wasFilledIn(uc::EcVersion v) const
            { return nEmptyPlaces() == 0 && ecLastVersion == v; }
        bool wasExtendedIn(uc::EcVersion v) const
            { return (history.ecVersion == v && history.oldEndingCp < endingCp); }

        void printfLocKey(char* buf, size_t n, const char* suffix) const
            { printfLocKeyN(buf, n, suffix); }

        template <size_t N>
        void printfLocKey(char (&buf)[N], const char* suffix) const
            { printfLocKeyN(buf, N, suffix); }

        /// Traverses history of all resizes from earlier to later
        /// Only 1 right now, but do not rely
        /// 3 params: when, before, after
        void resizeHistory(const BlockResizeSink& x) const;

        /// Same, virtual template idiom
        template <class Body>
        void resizeHistoryT(const Body& body) const
            { resizeHistory(BlockResizeSinkT<Body>(body)); }
    private:
        void printfLocKeyN(char* buf, size_t n, const char* suffix) const;
    };

    enum class Graphical : unsigned char { NO, YES };
    enum class Independent : unsigned char { NO, YES };

    struct Category
    {
        EcUpCategory upCat;
        std::string_view id;
        /// [+] is graphical by Unicode’s rules, actually unused
        Graphical isGraphical;
        /// [+] is “independent” by my own rules, for Library
        Independent isIndependent;
        mutable unsigned nChars = 0;

        struct Loc {
            std::u8string_view name;
            std::u8string_view description;
        } mutable loc {};

        void printfLocKey(char* buf, size_t n, const char* suffix) const
            { printfLocKeyN(buf, n, suffix); }

        template <size_t N>
        void printfLocKey(char (&buf)[N], const char* suffix) const
            { printfLocKeyN(buf, N, suffix); }
    private:
        void printfLocKeyN(char* buf, size_t n, const char* suffix) const;
    };

    extern const Category categoryInfo[];
    const Category* findCategory(std::string_view x);

    struct NumType
    {
        std::string_view locKey;
        std::string_view searchLocKey;
    };
    extern const ec::Array<NumType, EcNumType> numTypeInfo;

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

        void printfLocKey(char* buf, size_t n, const char* suffix) const
            { printfLocKeyN(buf, n, suffix); }

        template <size_t N>
        void printfLocKey(char (&buf)[N], const char* suffix) const
            { printfLocKeyN(buf, N, suffix); }
    private:
        void printfLocKeyN(char* buf, size_t n, const char* suffix) const;
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

    struct TermSearch
    {
        uc::EcCategory ecCategory = uc::EcCategory::NO_VALUE;
        uc::EcUpCategory ecUpCat = uc::EcUpCategory::NO_VALUE;
        uc::Cfgs fgs {};
        bool isNumber = false;
    };

    struct Term
    {
        std::string_view key;
        EcTermCat ecCat;
        std::u8string_view engName;
        std::string_view borrowedDesc {};
        EcFont ecFont = EcFont::NORMAL;
        TermSearch search {};
        struct Loc {
            std::u8string_view name;
            std::u8string_view description;
            LocSortKey sortKey;
        } mutable loc {};

        const TermCat& cat() const { return termCats[static_cast<int>(ecCat)]; }

        const uc::Font& font() const { return fontInfo[static_cast<int>(ecFont)]; }

        void printfLocKey(char* buf, size_t n, const char* suffix) const
            { printfLocKeyN(buf, n, suffix); }

        template <size_t N>
        void printfLocKey(char (&buf)[N], const char* suffix) const
            { printfLocKeyN(buf, N, suffix); }
    private:
        void printfLocKeyN(char* buf, size_t n, const char* suffix) const;
    };

    extern const Term terms[];
    extern const Term* sortedTerms[];
    size_t nTerms();

    const Term* findTerm(std::string_view id);

    inline size_t sprint(char* buf, size_t n, char32_t code, unsigned length = 4)
        { return snprintf(buf, n, "%0*X", length, static_cast<unsigned>(code)); }

    template<size_t N>
    inline size_t sprint(char (&buf)[N], char32_t code, unsigned length = 4)
        { return sprint(buf, N, code, length); }

    inline size_t sprintUPLUS(char* buf, size_t n, char32_t code)
        { return snprintf(buf, n, "U+%04X", static_cast<unsigned>(code)); }

    template<size_t N>
    inline size_t sprintUPLUS(char (&buf)[N], char32_t code)
        { return sprintUPLUS(buf, N, code); }

    /// Appends a char.code to “U+0000+1111+2222”
    /// @param [in] buff   character data
    /// @param [in] n      data size
    /// @param [in] n1     position there
    /// @param [in] code   character code
    /// @return new n1
    size_t appendUPLUS(char* buf, size_t n, size_t n1, char32_t code);

    /// @return new n1
    template<size_t N>
    inline size_t appendUPLUS(char (&buf)[N], size_t n1, char32_t code)
        { return appendUPLUS(buf, N, n1, code); }

    /// Appends a char.code to “0000+1111+2222”
    /// @param [in] buff   character data
    /// @param [in] n      data size
    /// @param [in] n1     position there
    /// @param [in] code   character code
    /// @return new n1
    size_t appendPlus(char* buf, size_t n, size_t n1, char32_t code);

    /// @return new n1
    template<size_t N>
    inline size_t appendPlus(char (&buf)[N], size_t n1, char32_t code)
        { return appendPlus(buf, N, n1, code); }

    QFont funkyFont(FontPlace place, int size);

    void finishTranslation(
            const std::unordered_map<char32_t, int>& sortOrder,
            std::u32string_view ellipsisBlocks,
            const std::unordered_map<char32_t, std::u32string>& alphaFixup);


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


namespace cou {

    DEFINE_ENUM_TYPE_IN_NS(cou, Type, unsigned char,
        RECOGNIZED_STATE,
        NO_MANS_LAND,
        SUPRANATIONAL_UNION,
        INTERNATIONAL_ORGANIZATION,
        AUSTRALIA_EXTERNAL,  // 5
        BRITAIN_OVERSEAS,
        BRITAIN_OVERSEAS_ACTUALLY,
        BRITAIN_OVERSEAS_FORMER,
        BRITAIN_DEPENDENT,
        CHINA_SPECIAL,       // 10
        DENMARK_AUTONOMY,
        FINLAND_SPECIAL,
        FRANCE_OVERSEAS,
        FRANCE_UNINHABITED,
        MAURITIUS_GB,
        NETHERLANDS_DEPENDENT, // 15
        NETHERLANDS_OVERSEAS,
        NEWZEALAND_ASSOC,
        NORWAY_UNINHABITED,
        NORWAY_DEPENDENT,
        SPAIN_AUTONOMY,  // 20
        US_DEPENDENT,
        PARTLY_PALESTINE,
        PARTLY_WEST_SAHARA,
        PARTLY_TAIWAN,
        PARTLY_KOSOVO // 25
    )

    DEFINE_ENUM_TYPE_IN_NS(cou, Location, unsigned char,
        EUROPE,
        ASIA,
        AFRICA,
        NORTH_AMERICA,
        SOUTH_AMERICA,  // 5
        AUSTRALIA,
        SOUTH_POLE,
        EUROPE_ASIA,
        ATLANTIC_OCEAN,
        ATLANTIC_EUROPE, // 10
        ATLANTIC_BRITISH,
        ATLANTIC_AFRICA,
        ATLANTIC_NORTH_AMERICA,
        ATLANTIC_SOUTH_AMERICA,
        ATLANTIC_CARIBBEAN, // 15
        ATLANTIC_ANTARCTIC,
        ENGLISH_CHANNEL,
        BALTIC_SEA,
        MEDITERRANEAN_SEA,
        PACIFIC_POLYNESIA, // 20
        PACIFIC_MICRONESIA,
        PACIFIC_ASIA,
        PACIFIC_AUSTRALIA,
        PACIFIC_NORTH_AMERICA,
        MALAY_ARCHIPELAGO, // 25
        ASIA_MALAY_ARCHIPELAGO, ///< Malaysia
        INDIAN_OCEAN,
        INDIAN_AFRICA,
        INDIAN_ASIA,
        INDIAN_ANTARCTIC, // 30
        GULF_OF_PERSIA,
        ARCTIC_OCEAN,
        NEWYORK_HQ, // 33
    )

    DEFINE_ENUM_TYPE_IN_NS(cou, Popul, unsigned char,
        DFLT,      ///< Do not write; actually there were indigenous people
        SETTLERS,  ///< Relatively recent settlers
        SETTLERS_SVALBARD, ///< Settlers, Svalbard only
        MAINLY_MILITARY,
        MAINLY_SCIENTISTS, ///< Antarctica
        MAINLY_NOMADS,  ///< W.Sahara
        MAINLY_PRIESTS, ///< Vatican
        UNINHABITED,
        NATURE_RESERVE, ///< Bouvet
        ABANDONED_GS,   ///< South Georgia & South Sandwich Islands
        NOT_APPLICABLE, ///< UN
    )

    struct TwoLetters {
        uint16_t value = 0;

        constexpr TwoLetters() = default;
        constexpr TwoLetters(unsigned char hi, unsigned char lo) : value ((hi << 8) | lo) {}
        constexpr TwoLetters(const int  (&c)[2]) : TwoLetters(c[0], c[1]) {}
        constexpr TwoLetters(const char (&c)[3]) : TwoLetters(c[0], c[1]) {}
        constexpr TwoLetters(const char (&c)[2]) : TwoLetters(c[0], c[1]) {}
        friend std::strong_ordering operator <=> (TwoLetters x, TwoLetters y);
        friend bool operator == (TwoLetters x, TwoLetters y);

        constexpr explicit operator bool () const { return value; }
    };
    inline bool operator == (TwoLetters x, TwoLetters y) = default;
    inline std::strong_ordering operator <=> (TwoLetters x, TwoLetters y) = default;

    struct Country {
        TwoLetters name;
        Type type;
        Location location;
        Popul popul;
    };

    extern const ec::Array<const char*, Type> typeKeys;
    extern const ec::Array<const char*, Location> locKeys;
    extern const ec::Array<const char*, Popul> popKeys;

    const Country* find(TwoLetters key);

}

namespace match {

    ///  Matcher that extracts “main” font
    class MainFont : public uc::FontMatcher {
    public:
        bool check(char32_t, const uc::Font& font) const override;
        static const MainFont INST;
    };

    ///  Normal matcher that extracts the 1st font that supports cp,
    ///  or last if no one supports
    class Normal : public uc::FontMatcher {
    public:
        bool check(char32_t cp, const uc::Font& font) const override;
        static const Normal INST;
    };

    ///  Normal matcher that extracts font that ACTUALLY supports cp
    class NullForTofu : public uc::FontMatcher {
    public:
        bool check(char32_t cp, const uc::Font& font) const override;
        static const NullForTofu INST;
    };

    ///  Matcher that extracts exactly the last font
    class FallToLast : public uc::FontMatcher {
    public:
        bool check(char32_t cp, const uc::Font& font) const override;
        static const FallToLast INST;
    };

}


// Inline implementations
// …Numeric
inline const uc::NumType& uc::Numeric::type() const { return numTypeInfo[ecType]; }

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

// LibNode
inline const uc::Version& uc::LibNode::emojiVersion() const { return versionInfo[static_cast<int>(ecEmojiVersion)]; }
