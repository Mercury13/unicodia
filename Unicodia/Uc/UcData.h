#pragma once

// STL
#include <string>

// Qt
#include <QFont>

// Misc
#include "u_TypedFlags.h"

// Unicode data
#include "UcDefines.h"

/// ALL NAMES HERE ARE IN UTF-8!

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
        AHOM,
        ARABIC,
        BALINESE,
        BAMUM,
        BUGINESE,
        BUHID,
        CANADIAN_ABORIGINAL,
        CHEROKEE,
        GLAGOLITIC,
        HANUNOO,
        HEBREW,
        LANNA,
        LAO,
        LEPCHA,
        LIMBU,
        LISU,
        MALAYALAM,
        MANDAIC,
        MEETEI,
        MONGOLIAN,
        OLCHIKI,
        RUNIC,
        SAMARITAN,
        SUNDANESE,
        SYRIAC,
        TAGALOG,
        TAGBANWA,
        TAILE,
        TAILUE,
        TAMIL,
        THAANA,
        YI,
        NN
    };

    enum class Ffg {
        NEED_STUB = 1
    };

    struct Font
    {
        std::string_view family, fileName;
        Flags<Ffg> flags {};
        std::string_view styleSheet {};
        int sizeAdjust = 100;

        mutable struct Q {
            std::unique_ptr<QFont> table {};
            std::unique_ptr<QFont> big {};
            bool isLoaded = false;
        } q {};
        void load() const;
        const QFont& get(std::unique_ptr<QFont>& font, int size) const;
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
        mutable unsigned nChars = 0;
    };
    extern const BidiClass bidiClassInfo[static_cast<int>(EcBidiClass::NN)];
    inline const BidiClass* findBidiClass(std::string_view x) { return findInArray(x, bidiClassInfo); }

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

    extern const char8_t allStrings[];

    constexpr int N_BLOCKS = 302;
    extern const Block blocks[N_BLOCKS];
    constexpr int DEFAULT_BLOCK_HINT = N_BLOCKS / 2;

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
            const char8_t* tech() const { return allStrings + iTech.val(); }
        } name;
        EcCategory ecCategory;          // +1 = 10
        EcVersion ecVersion;            // +1 = 11
        EcBidiClass ecBidiClass;        // +1 = 12
        EcScript ecScript;              // +1 = 13
        struct Numeric {
            CompressedFraction val;
            EcNumType ecType = EcNumType::NONE;
            const NumType& type() const { return numTypeInfo[static_cast<int>(ecType)]; }
            bool isPresent() const { return (ecType != EcNumType::NONE); }
            consteval Numeric() = default;
            consteval Numeric(long long num, unsigned long long denom, EcNumType aType)
                : val(num, denom), ecType(aType) {}
        } numeric;                      // +4 = 17
        const Version& version() const { return versionInfo[static_cast<int>(ecVersion)]; }
        const Category& category() const { return categoryInfo[static_cast<int>(ecCategory)]; }
        const BidiClass& bidiClass() const { return bidiClassInfo[static_cast<int>(ecBidiClass)]; }
        const Script& script() const { return scriptInfo[static_cast<int>(ecScript)]; }
        EcScript ecScriptEx(const Block*& hint) const;
        const Font& font(const Block*& hint) const
            { return scriptInfo[static_cast<int>(ecScriptEx(hint))].font(); }
        SampleProxy sampleProxy(const Block*& hint) const;
        QString osProxy() const;

        ///  @return [+] it is a true space, really white
        bool isTrueSpace() const
                { return (ecCategory == EcCategory::SEPARATOR_SPACE &&
                          ecScript != EcScript::Ogam); }    // Ogham space is a continuing line (edge of stick)
        constexpr int plane() const { return subj.val() >> 16; }
    };

    extern Cp cpInfo[N_CPS];

    constexpr int N_CHARS = 65536 * 17;
    extern Cp* cps[N_CHARS];

    void completeData();
    const Block* blockOf(char32_t subj, const Block* hint);
    inline const Block* blockOf(char32_t subj, int iHint)
        { return blockOf(subj, std::begin(blocks) + std::max(iHint, 0)); }

}   // namespace uc
