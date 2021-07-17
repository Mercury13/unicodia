#pragma once

// STL
#include <string>

// Qt
#include <QFont>
#include <QFontDatabase>

// Misc
#include "u_TypedFlags.h"

// Unicode data
#include "UcDefines.h"
#include "UcAutoDefines.h"

/// ALL NAMES HERE ARE IN UTF-8!

constexpr auto FONT_NOT_INSTALLED = -1000;
constexpr auto FONT_CHECKED = -999;


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

class Percent
{
public:
    constexpr explicit Percent() = default;
    constexpr explicit Percent(int x) : fValue(x) {}
    constexpr int value() const { return fValue; }
    constexpr int apply(int x) const { return x * fValue / 100; }
private:
    int fValue = 100;
};

constexpr Percent operator "" _pc (unsigned long long x) { return Percent(x); }

namespace uc {

    enum class EcContinent {
        NONE,
        EUROPE,
        ASIA_INDIAN,    ///< Asia + Indian Ocean
        PACIFIC,        ///< All Pacific Ocean, incl. Japan
        AFRICA,
        AMERICA,
        TECHNICAL
    };

    enum class EcFont
    {
        NORMAL,
        SYMBOL,
        AHOM,
        ARABIC,
        BALINESE,
        BAMUM,
        BATAK,
        BUGINESE,
        BUHID,
        CANADIAN_ABORIGINAL,
        CHEROKEE,
        DEVANAGARI,
        DEVANAGARI_SYSTEM,
        GEORGIAN,
        GLAGOLITIC,
        GURMUKHI,
        HANUNOO,
        HEBREW,
        JAVANESE,
        KANNADA,
        KAYAH_LI,
        LANNA,
        LAO,
        LEPCHA,
        LIMBU,
        LISU,
        MALAYALAM,
        MANDAIC,
        MEETEI,
        MONGOLIAN,
        MYANMAR,
        NKO,
        OGHAM,
        OLCHIKI,
        RUNIC,
        SAMARITAN,
        SINHALA,
        SUNDANESE,
        SYRIAC,
        TAGALOG,
        TAGBANWA,
        TAILE,
        TAILUE,
        TAMIL,
        TELUGU,
        THAANA,
        THAI,
        VAI,
        YI,
        NN
    };

    ///
    ///  @warning  Use several fonts → be sure to have STUB_xxx working for ALL
    ///            Better use STUB_VICEVERSA with temporary font (loaded from file, not system)
    ///
    enum class Ffg {
        BOLD = 1,               ///< Bolder
        LIGHT = 2,              ///< Lighter
        STUB_OFF = 4,           ///< Circle stub explicitly off (auto: off for Brahmic, on for the rest)
        STUB_ON = 8,            ///< Circle stub explicitly on
        STUB_VICEVERSA = 16,    ///< Circle stub before char (stuck to ZWSP, i.e. ZWSP + char + circle)
    };

    DEFINE_ENUM_OPS(Ffg)

    struct Font
    {
        static const QString qempty;

        std::string_view family, fileName;
        Flags<Ffg> flags {};
        std::string_view styleSheet {};
        Percent sizeAdjust {};

        mutable struct Q {
            QList<QString> families {};
            std::unique_ptr<QFont> table {};
            std::unique_ptr<QFont> big {};
            intptr_t installID = FONT_NOT_INSTALLED;
        } q {};
        void load() const;
        const QFont& get(std::unique_ptr<QFont>& font, int size) const;
        bool doesSupportChar(char32_t x) const;
        const QString& onlyFamily() const
            { return (q.families.size() == 1) ? q.families[0] : qempty; }
    };
    extern const Font fontInfo[static_cast<int>(EcFont::NN)];

    struct LangLife
    {
        std::u8string_view locName;
    };
    extern const LangLife langLifeInfo[static_cast<int>(EcLangLife::NN)];


    struct ScriptType
    {
        std::u8string_view locName;
    };
    extern const ScriptType scriptTypeInfo[static_cast<int>(EcScriptType::NN)];


    struct WritingDir
    {
        std::u8string_view locName;
    };
    extern const WritingDir writingDirInfo[static_cast<int>(EcWritingDir::NN)];

    struct Version
    {
        std::string_view name;
        unsigned year;

        //unsigned nNewChars = 0;
    };
    extern const Version versionInfo[static_cast<int>(EcVersion::NN)];

    constexpr int PLANE_BASE = 0;
    constexpr int PLANE_UNKNOWN = -1;

    struct Script
    {
        std::string_view id;
        QFontDatabase::WritingSystem qtCounterpart;
        EcScriptType ecType;
        EcLangLife ecLife;
        EcWritingDir ecDir;
        EcContinent ecContinent;
        std::u8string_view locName, locTime, locLangs, locDescription;
        EcFont ecFont = EcFont::NORMAL;

        mutable unsigned nChars = 0;
        mutable int plane = -1;
        mutable EcVersion ecVersion = EcVersion::UNKNOWN;

        inline const ScriptType& type() const { return scriptTypeInfo[static_cast<int>(ecType)]; }
        inline const LangLife& life() const { return langLifeInfo[static_cast<int>(ecLife)]; }
        inline const WritingDir& dir() const { return writingDirInfo[static_cast<int>(ecDir)]; }
        inline const Font& font() const { return fontInfo[static_cast<int>(ecFont)]; }
        const Version& version() const { return versionInfo[static_cast<int>(ecVersion)]; }
    };
    extern const Script scriptInfo[static_cast<int>(EcScript::NN)];
    inline const Script* findScript(std::string_view x) { return findInArray(x, scriptInfo); }


//    struct Category
//    {
//        std::string name;
//    };

    struct Cp;


    struct Block
    {
        char32_t startingCp, endingCp;
        std::string_view name;
        std::u8string_view locName;

        std::u8string_view locDescription {};
        EcScript ecScript = EcScript::NONE;

        mutable const Cp* firstAllocated = nullptr;
        mutable int nChars = 0;
        mutable EcVersion ecVersion = EcVersion::NN;

        size_t index() const;
        const Version& version() const { return versionInfo[static_cast<int>(ecVersion)]; }
        const Script& script() const { return scriptInfo[static_cast<int>(ecScript)]; }
        const Font& font() const { return script().font(); }

        Block& operator = (const Block&) = delete;
        Block& operator = (Block&&) = delete;
    };


    struct Category
    {
        UpCategory upCat;
        std::string_view id;
        std::u8string_view locName;
        std::u8string_view locDescription;
        mutable unsigned nChars = 0;
    };

    extern const Category categoryInfo[static_cast<int>(EcCategory::NN)];
    inline const Category* findCategory(std::string_view x) { return findInArray(x, categoryInfo); }

    struct NumType
    {
        std::u8string_view locName;
    };
    extern const NumType numTypeInfo[static_cast<int>(EcNumType::NN)];

    struct BidiClass
    {
        std::string_view id;
        EcBidiStrength strength;
        std::u8string_view locName;
        std::u8string_view locShortName;
        std::u8string_view locId;
        std::u8string_view locDescription;
        bool isGraphical;
        mutable unsigned nChars = 0;
    };
    extern const BidiClass bidiClassInfo[static_cast<int>(EcBidiClass::NN)];
    inline const BidiClass* findBidiClass(std::string_view x) { return findInArray(x, bidiClassInfo); }

    extern const Block blocks[N_BLOCKS];
    constexpr int DEFAULT_BLOCK_HINT = N_BLOCKS / 2;

    constexpr int N_CHARS = 65536 * 17;
    extern Cp* cps[N_CHARS];

    // We’ll use this WS for Hani, we could take Japanese as well
    static constexpr auto WS_HANI = QFontDatabase::SimplifiedChinese;

    void completeData();
    const Block* blockOf(char32_t subj, const Block* hint);
    inline const Block* blockOf(char32_t subj, int iHint)
        { return blockOf(subj, std::begin(blocks) + std::max(iHint, 0)); }

    enum class TermCat {
        ENCODING, SERIALIZATION, SCRIPT_CLASS, CHAR_CLASS
    };

    struct Term
    {
        std::string_view key;
        TermCat cat;
        std::u8string_view locName;
        std::u8string_view engName;
        std::u8string_view desc;
    };

    // Implementations of Cp inlines
    inline const char8_t* Cp::Name::tech() const { return allStrings + iTech.val(); }
    inline const NumType& Cp::Numeric::type() const { return numTypeInfo[static_cast<int>(ecType)]; }
    inline const Version& Cp::version() const { return versionInfo[static_cast<int>(ecVersion)]; }
    inline const Category& Cp::category() const { return categoryInfo[static_cast<int>(ecCategory)]; }
    inline const BidiClass& Cp::bidiClass() const { return bidiClassInfo[static_cast<int>(ecBidiClass)]; }
    inline const Script& Cp::script() const { return scriptInfo[static_cast<int>(ecScript)]; }
    inline const Script& Cp::scriptEx(const Block*& hint) const
        { return scriptInfo[static_cast<int>(ecScriptEx(hint))]; }
    inline const Font& Cp::font(const Block*& hint) const
        { return scriptInfo[static_cast<int>(ecScriptEx(hint))].font(); }
    inline bool Cp::isTrueSpace() const
            { return (ecCategory == EcCategory::SEPARATOR_SPACE &&
                      ecScript != EcScript::Ogam); }    // Ogham space is a continuing line (edge of stick)

}   // namespace uc
