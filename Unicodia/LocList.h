#pragma once

// C++
#include <filesystem>

// Qt
#include <QTranslator>

// Libs
#include "u_Vector.h"

namespace loc
{

    struct Lang
    {
        struct Name {
            std::u8string
                tech,           ///< CHINESE
                native,         ///< Chinese (simplified)
                international,  ///< 简体中文
                isoSmall;       ///< zh
        } name;
        std::filesystem::path fnLang;   ///< c:\full\path\to\lang.ini
        bool showEnglishTerms = true;
        std::unique_ptr<QTranslator> translator;

        void load();
        void unload();
    };

    using LangList = SafeVector<std::unique_ptr<Lang>>;
    void collectLangs(const std::filesystem::path& programPath);
    void loadFirstLang();
    void loadFirstLang();

    extern LangList allLangs;
    extern loc::Lang* currLang;

}   // namespace loc
