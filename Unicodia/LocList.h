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
        void forceLoad();
        void unload();
    };

    using VLang = SafeVector<std::unique_ptr<Lang>>;

    class LangList : public VLang
    {
    public:
        void collect(const std::filesystem::path& programPath);
        Lang* findFirst();
        Lang* byIso(std::u8string_view x);
        void loadFirst();
    };

    extern LangList allLangs;
    extern loc::Lang* currLang;

}   // namespace loc
