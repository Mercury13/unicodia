#pragma once

// STL
#include <string>

// Unicode data
#include "UcDefines.h"

/// ALL NAMES HERE ARE IN UTF-8!

namespace uc {

    struct Font
    {
        int index;
        std::string name;
    };


    struct Script
    {
        std::string_view locName;
        std::string_view locGreatestLangs;
        ScriptType type = ScriptType::UNKNOWN;
        LangLife life = LangLife::NOMATTER;
        int nEncodedChars = 0;
        const Font* font;
    };


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


    struct Version
    {
        std::string_view name;
        unsigned year;

        //unsigned nNewChars = 0;
    };

    extern Version versionInfo[static_cast<int>(EcVersion::NN)];

    struct Category
    {
        UpCategory upCat;
        std::string_view locName;
        // unsigned nChars = 0;
    };

    extern Category categoryInfo[static_cast<int>(EcCategory::NN)];

    struct Cp   // code point
    {
        char32_t subj = 0, proxy = NO_CHAR;
        struct Name {
            std::string_view tech;
            std::string_view loc;
        } name;
        struct Bidi {
            EcBidiClass ecClazz = EcBidiClass::LTR;
            bool isMirrored = false;
        } bidi = {};
        //const Range* range = nullptr;
        EcVersion ecVersion = EcVersion::UNKNOWN;
        EcCategory ecCategory = EcCategory::CONTROL;
        const Version& version() const { return versionInfo[static_cast<int>(ecVersion)]; }
        const Category& category() const { return categoryInfo[static_cast<int>(ecCategory)]; }
    };

    extern unsigned nCps();
    extern Cp cpInfo[];

}   // namespace uc
