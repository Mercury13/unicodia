#pragma once

// Qt
#include <QString>

// Libs
#include "u_Vector.h"
#include "u_SearchEngine.h"


namespace uc {

    struct Cp;

    enum class SingleError {
        // Not errors
        ONE,
        MULTIPLE,
        NO_SEARCH,          ///< Search did not occur at all
        NOT_FOUND,
        CONVERT_ERROR,
        TOO_BIG,
        NONCHARACTER,
        PRIVATE_USE,
        SURROGATE,
        UNALLOCATED,
        RESERVED,           ///< Allocated but still reserved
        NN,
    };
    constexpr auto SingleError_N = static_cast<int>(SingleError::NN);
    enum class IsCp { NO, YES };
    struct ErrorInfo {
        IsCp ecIsCp;
        std::u8string_view message;
        bool isCp() const { return static_cast<bool>(ecIsCp); }
    };
    extern const ErrorInfo errorInfo[SingleError_N];

    struct SingleSearchResult {
        char32_t singleCode = 0;
        SingleError err = SingleError::CONVERT_ERROR;        
        const uc::Cp* one = nullptr;
    };

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

    struct SearchLine {
        char32_t code = 0;                  ///< char code
        SingleError type = SingleError::ONE;  ///< what found
        const uc::Cp* cp = nullptr;         ///< code point
        std::u8string_view triggerName;     ///< name that triggered inclusion to search results
        srh::Prio prio;                     ///< its priority
        /// @warning in reverse order!!
        std::partial_ordering operator <=>(const SearchLine& x) const
            { return x.prio <=> prio; }
        static const SearchLine STUB;
    };

    struct SearchResult : public SingleSearchResult {
        SafeVector<SearchLine> multiple {};
        SearchResult() = default;
        SearchResult(const SingleSearchResult& x) : SingleSearchResult(x) {}
        SearchResult(SafeVector<uc::SearchLine>&& v)
            : SingleSearchResult { 0, SingleError::MULTIPLE },  multiple(std::move(v)) {}
    };

    /// @return [+] cp is noncharacter
    constexpr bool isNonChar(char32_t cp)
    {
        return ((cp & 0xFFFF) >= 0xFFFE)
            || (cp >= 0xFDD0 && cp <= 0xFDEF);
    }

    SingleSearchResult findCode(char32_t code);
    SingleSearchResult findStrCode(QStringView what, int base);
    SearchResult doSearch(QString what);
    bool isNameChar(char32_t cp);
    bool isNameChar(QStringView x);
    bool isMnemoChar(char32_t cp);
    bool isMnemoChar(QStringView x);
    std::u8string toMnemo(QString x);
}
