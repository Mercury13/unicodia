#pragma once

// C++
#include <filesystem>
#include <map>
#include <unordered_map>

// Libs
#include "u_Vector.h"

class QTranslator;

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
        struct Icons {
            std::string sortAZ;
        } icons;
        struct Numfmt {
            char16_t decimalPoint = '.';
        } numfmt;
        struct Ellipsis {
            std::u8string text;
            std::u32string blocks;  // just string, sorted asc
        } ellipsis;
        int stamp = 0;
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
        const std::string& mainLang() const { return triggerLangs.front(); }
    };

    namespace active {
        extern Lang::Icons icons;
        extern Lang::Numfmt numfmt;
        extern bool showEnglishTerms;
    }

    using VLang = SafeVector<std::unique_ptr<Lang>>;

    class LangList : public VLang
    {
    public:
        void collect(const std::filesystem::path& programPath);
        Lang* findStarting(
                std::string_view lastLang, int lastStamp);
        Lang* byIso(std::string_view x,
            int lastStamp = std::numeric_limits<int>::min());
        void loadStarting(
                std::string_view lastLang, int lastStamp);
        int byPtr(const Lang* x);
        int lastStamp() const;
    };

    extern LangList allLangs;
    extern loc::Lang* currLang;

}   // namespace loc
