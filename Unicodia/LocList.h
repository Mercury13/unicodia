#pragma once

// C++
#include <set>
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
                international;  ///< 简体中文
            std::u32string sortKey;  /// CHINESE (SIMPLIFIED)
        } name;
        SafeVector<std::string> triggerLangs; ///< ISO codes, e.g. zh
        std::filesystem::path fnLang;   ///< c:\full\path\to\lang.ini
        bool showEnglishTerms = true;
        std::unique_ptr<QTranslator> translator;
        std::map<std::string, std::string, std::less<>> wikiTemplates;
        std::unordered_map<char32_t, int> sortOrder;

        void load();
        void forceLoad();
        void unload();
        bool hasTriggerLang(std::string_view iso) const;
        bool hasMainLang(std::string_view iso) const;
    };

    using VLang = SafeVector<std::unique_ptr<Lang>>;

    class LangList : public VLang
    {
    public:
        void collect(const std::filesystem::path& programPath);
        Lang* findStarting();
        Lang* byIso(std::string_view x);
        void loadStarting();
        int byPtr(const Lang* x);
    };

    extern LangList allLangs;
    extern loc::Lang* currLang;

}   // namespace loc
