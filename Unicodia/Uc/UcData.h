#pragma once

// STL
#include <string>

// Qt
#include <QFont>

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

    enum class EcFont
    {
        NORMAL,
        CHEROKEE,
        GLAGOLITIC,
        HANUNOO,
        MONGOLIAN,
        RUNIC,
        SAMARITAN,
        TAGALOG,
        NN
    };

    struct Font
    {
        std::string_view family, fileName;
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
        std::string_view locName;
    };
    extern const LangLife langLifeInfo[static_cast<int>(EcLangLife::NN)];


    struct ScriptType
    {
        std::string_view locName;
    };
    extern const ScriptType scriptTypeInfo[static_cast<int>(EcScriptType::NN)];


    struct WritingDir
    {
        std::string_view locName;
    };
    extern const WritingDir writingDirInfo[static_cast<int>(EcWritingDir::NN)];

    struct Version
    {
        std::string_view name;
        unsigned year;

        //unsigned nNewChars = 0;
    };
    extern const Version versionInfo[static_cast<int>(EcVersion::NN)];

    struct Script
    {
        std::string_view id;
        EcScriptType ecType;
        EcLangLife ecLife;
        EcWritingDir ecDir;
        std::string_view locName, locTime, locLangs, locDescription;
        EcFont ecFont = EcFont::NORMAL;
        mutable unsigned nChars = 0;
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


    struct Range
    {
        std::string name;
        int startingCp = 0, endingCp = 0;
        Script* script;
    };


    struct Category
    {
        UpCategory upCat;
        std::string_view id;
        std::string_view locName;
        std::string_view locDescription;
        mutable unsigned nChars = 0;
    };

    extern const Category categoryInfo[static_cast<int>(EcCategory::NN)];
    inline const Category* findCategory(std::string_view x) { return findInArray(x, categoryInfo); }

    struct NumType
    {
        std::string_view locName;
    };
    extern const NumType numTypeInfo[static_cast<int>(EcNumType::NN)];

    struct BidiClass
    {
        std::string_view id;
        EcBidiStrength strength;
        std::string_view locName;
        std::string_view locDescription;
        mutable unsigned nChars = 0;
    };
    extern const BidiClass bidiClassInfo[static_cast<int>(EcBidiClass::NN)];
    inline const BidiClass* findBidiClass(std::string_view x) { return findInArray(x, bidiClassInfo); }

    struct Cp   // code point
    {
        char32_t subj = 0, proxy = NO_CHAR;
        struct Name {
            std::string_view tech;
            std::string_view loc;
        } name;
        EcCategory ecCategory;
        EcVersion ecVersion;
        EcBidiClass ecBidiClass;
        EcScript ecScript;
        struct Numeric {
            EcNumType ecType = EcNumType::NONE;
            long long num = 0, denom = 0;
            const NumType& type() const { return numTypeInfo[static_cast<int>(ecType)]; }
            bool isPresent() const { return (ecType != EcNumType::NONE); }
        } numeric;
        //const Range* range = nullptr;
        const Version& version() const { return versionInfo[static_cast<int>(ecVersion)]; }
        const Category& category() const { return categoryInfo[static_cast<int>(ecCategory)]; }
        const BidiClass& bidiClass() const { return bidiClassInfo[static_cast<int>(ecBidiClass)]; }
        const Script& script() const { return scriptInfo[static_cast<int>(ecScript)]; }
    };

    extern unsigned nCps();
    extern Cp cpInfo[];

    constexpr int N_CHARS = 65536 * 17;
    extern Cp* cps[N_CHARS];

    void completeData();

}   // namespace uc
