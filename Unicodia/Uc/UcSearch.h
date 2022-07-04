#pragma once

// Qt
#include <QString>

// Libs
#include "u_Vector.h"
#include "u_SearchEngine.h"

// Unicode
#include "UcAutoDefines.h"

namespace uc {

    struct Cp;

    enum class CpType {
        EXISTING,
        NONCHARACTER,
        PRIVATE_USE,
        SURROGATE,
        UNALLOCATED,
        RESERVED,           ///< Allocated but still reserved
        NN
    };
    constexpr auto CpType_N = static_cast<int>(CpType::NN);
    extern const std::string_view cpTypeKeys[CpType_N];

    enum class SearchError {
        OK,
        NO_SEARCH,          ///< Search did not occur at all
        NOT_FOUND,
        CONVERT_ERROR,
        TOO_BIG,
        NN
    };
    constexpr auto SearchError_N = static_cast<int>(SearchError::NN);
    extern const std::string_view searchErrorKeys[SearchError_N];

    enum {
        // LOWEST
        HIPRIO_KEYWORD = 0,
        HIPRIO_MNEMONIC_CASE,
        HIPRIO_MNEMONIC_EXACT,
        HIPRIO_DEC,
        HIPRIO_HEX,
        // HIGHEST
        HIPRIO_FIRST_ONE = HIPRIO_MNEMONIC_EXACT  // [>=] can convert multiple to one
    };

    struct MiniLine {
        char32_t code = 0;                  ///< char code
        CpType type = CpType::NONCHARACTER; ///< what found
        const uc::Cp* cp = nullptr;         ///< code point

        constexpr MiniLine() = default;
        // Clazy’s −warn
        constexpr MiniLine(char32_t aCode,
                 CpType aType = CpType::NONCHARACTER,
                 const uc::Cp* aCp = nullptr)
            : code(aCode), type(aType), cp(aCp) {}
    };

    struct SearchLine : public MiniLine {
        std::u8string_view triggerName;     ///< name that triggered inclusion to search results
        srh::Prio prio;                     ///< its priority
        /// @warning in reverse order!!
        std::partial_ordering operator <=>(const SearchLine& x) const
            { return x.prio <=> prio; }
        static const SearchLine STUB;
        SearchLine() = default;
        SearchLine(const MiniLine& x)
            : MiniLine{x} {}
        SearchLine(const uc::Cp& cp)
            : MiniLine{cp.subj, CpType::EXISTING, &cp} {}
        SearchLine(const uc::Cp& cp, std::u8string_view tn)
            : MiniLine{cp.subj, CpType::EXISTING, &cp}, triggerName(tn) {}
        SearchLine(const uc::Cp& cp, std::u8string_view tn, const srh::Prio& pr)
            : MiniLine{cp.subj, CpType::EXISTING, &cp}, triggerName(tn), prio(pr) {}
    };

    struct SingleResult : public MiniLine {
        SearchError err = SearchError::OK;

        SingleResult(char32_t code, CpType type)
            : MiniLine{ code, type } {}
        SingleResult(char32_t code, CpType type, const uc::Cp* cp)
            : MiniLine{ code, type, cp } {}
        SingleResult(const uc::Cp& cp)
            : MiniLine{cp.subj, CpType::EXISTING, &cp} {}
        SingleResult(const MiniLine& line) : MiniLine(line) {}
        SingleResult(SearchError aErr) : MiniLine{ 0 }, err(aErr) {}
    };

    struct MultiResult {
        SearchError err = SearchError::OK;
        SafeVector<SearchLine> v {};
        MultiResult(const SingleResult& x);
        MultiResult(SafeVector<SearchLine>&& aV) :
            err(aV.empty() ? SearchError::NOT_FOUND : SearchError::OK),
            v(std::move(aV)) {}
        const uc::Cp* one() const;
    };

    /// @return [+] cp is noncharacter
    constexpr bool isNonChar(char32_t cp)
    {
        return ((cp & 0xFFFF) >= 0xFFFE)
            || (cp >= 0xFDD0 && cp <= 0xFDEF);
    }

    SingleResult findCode(char32_t code);
    SingleResult findStrCode(QStringView what, int base);
    MultiResult doSearch(QString what);
    bool isNameChar(char32_t cp);
    bool isNameChar(QStringView x);
    bool isMnemoChar(char32_t cp);
    bool isMnemoChar(QStringView x);
    std::u8string toMnemo(const QString& x);
}
