#pragma once

// C++
#include <filesystem>
#include <map>
#include <unordered_map>

// Libs
#include "u_Vector.h"
#include "LocFmt.h"

// L10n
#include "LocDefs.h"

class QTranslator;

namespace loc
{
    struct CustomRule final : public PluralRule
    {
        struct Line {
            unsigned mod, min, max;
            loc::Plural outcome;
        };
        SafeVector<Line> lines;
        loc::Plural defaultOutcome = loc::Plural::OTHER;

        Plural ofUint(unsigned long long n) const override;
    };

    struct Lang final : public loc::Locale
    {
        struct Name {
            std::u8string
                tech,           ///< CHINESE
                native,         ///< Chinese (simplified)
                international;  ///< 简体中文
            std::u32string sortKey;  ///< CHINESE (SIMPLIFIED)
        } name;
        struct Icons {
            std::string sortAZ;
        } icons;
        struct Numfmt {
            char16_t decimalPoint = '.';
            struct Thousand {
                static constexpr unsigned DEFAULT_MIN_LENGTH = 1;
                static constexpr unsigned DEFAULT_PERIOD = 3;
                unsigned minLength = DEFAULT_MIN_LENGTH;
                // There’ll be other problems
            } thousand;
            Thousand denseThousand;  ///< thousand format in dense text
        } numfmt;
        struct Ellipsis {
            std::u8string text;
            std::u32string blocks;  ///< just initial codes, sorted asc
        } ellipsis;
        struct Peculiarities {
            bool stillUsesBurmese = false;  ///< uses Myanmar/Burmese
        } peculiarities;
        struct Punctuation {
            std::u8string
                    keyValueColon, ///< Key: Value
                    uniformComma,  ///< first, second
                    semicolon,     ///< first, second; third
                    leftParen,     ///< left parenthesis
                    rightParen,    ///< right parenthesis
                    indentEllip;   ///< ellipsis for indent
        } punctuation;
        int stamp = 0;
        SafeVector<std::string> triggerLangs; ///< ISO codes, e.g. zh
        std::filesystem::path fnLang;   ///< c:\full\path\to\lang.ini
        EngTerms engTerms = EngTerms::NORMAL;
        std::unique_ptr<QTranslator> translator;
        std::map<std::string, std::string, std::less<>> wikiTemplates;
        std::unordered_map<char32_t, int> sortOrder;
        CustomRule cardRule;  ///< rule for cardinal forms: 1 crow, 2 crows
        std::unordered_map<char32_t, std::u32string> alphaFixup;

        void load();
        void forceLoad();
        void unload();
        bool hasTriggerLang(std::string_view iso) const;
        bool hasMainLang(std::string_view iso) const;
        const std::string& mainLang() const { return triggerLangs.front(); }

        // Locale
        const PluralRule& cardinalRule() const override { return cardRule; }
    };

    namespace active {
        extern Lang::Icons icons;
        extern Lang::Numfmt numfmt;
        extern Lang::Punctuation punctuation;
        extern EngTerms engTerms;
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
