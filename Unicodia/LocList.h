#pragma once

// C++
#include <filesystem>
#include <map>
#include <unordered_map>

// Libs
#include "u_Vector.h"
#include "LocFmt.h"
#include "u_EnumSize.h"

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

    DEFINE_ENUM_TYPE_IN_NS(loc, FracPolicy, unsigned char,
        NEVER, AVOID, ONEDIG, PREFER, EXCEPT1 )

    DEFINE_ENUM_TYPE_IN_NS(loc, MoreLessPhase, unsigned char,
        RAW, UNIT )

    struct ImpreciseInfo {
        std::string tmpl, biggerUnit;
        unsigned char shift, biggerSubshift;
        FracPolicy policy;
    };

    struct SHash : public std::hash<std::string_view> {
        using is_transparent = void;
        using std::hash<std::string_view>::operator ();
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
            SafeVector<ImpreciseInfo> imprecise;
            static constexpr char16_t DEFAULT_DECIMAL_POINT = '.';
            static constexpr char16_t DEFAULT_THOUSAND_POINT = ' ';
            char16_t decimalPoint = DEFAULT_DECIMAL_POINT;
            char16_t thousandPoint = DEFAULT_THOUSAND_POINT;
            struct Thousand {
                static constexpr unsigned DEFAULT_MIN_LENGTH = 1;
                static constexpr unsigned DEFAULT_PERIOD = 3;
                unsigned minLength = DEFAULT_MIN_LENGTH;
                // There’ll be other problems
            } thousand;
            Thousand denseThousand;  ///< thousand format in dense text
            struct MoreLess {
                std::string less, more;
                MoreLessPhase phase = MoreLessPhase::RAW;
            } moreLess;
            struct Percent {
                static constexpr bool DEFAULT_INVERSE = false;
                static constexpr bool DEFAULT_SPACE = true;
                bool isInverse = DEFAULT_INVERSE;
                bool isSpace = DEFAULT_SPACE;
            } percent;
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
                    indentEllip,   ///< ellipsis for indent
                    range,         ///< range 1…2
                    yearRange;     ///< range 2000-2010
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
        std::unordered_map<std::string, std::string, SHash, std::equal_to<>> altCodeRename;

        void load();
        void forceLoad();
        void unload();
        bool hasTriggerLang(std::string_view iso) const;
        bool hasMainLang(std::string_view iso) const;
        const std::string& mainLang() const { return triggerLangs.front(); }

        /// Renames locale of Alt codes for patriotism
        std::string_view renameAltCodeSv(std::string_view x) const noexcept;

        // Locale
        const PluralRule& cardinalRule() const override { return cardRule; }
    };

    struct NumFmtHelp {
        std::string u8DecimalPoint, u8ThousandPoint;
    };

    namespace active {
        extern Lang::Icons icons;
        extern Lang::Numfmt numfmt;
        extern NumFmtHelp numfmtHelp;
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
