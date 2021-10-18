#pragma once

#include <QString>

namespace uc {

    struct Cp;

    enum class SingleError {
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
        MIN_ERROR = MULTIPLE + 1
    };
    constexpr auto SingleError_N = static_cast<int>(SingleError::NN);
    extern std::u8string_view errorStrings[SingleError_N];

    struct SingleSearchResult {
        SingleError err = SingleError::CONVERT_ERROR;
        const uc::Cp* one = nullptr;
    };

    struct SearchResult : public SingleSearchResult {
        std::vector<const uc::Cp*> multiple {};
        SearchResult() = default;
        SearchResult(const SingleSearchResult& x) : SingleSearchResult(x) {}
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
}
