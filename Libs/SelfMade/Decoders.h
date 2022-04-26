#pragma once

#include <string>

#include "u_Vector.h"
#include "u_Strings.h"

template<class Ec>
constexpr Ec ecIf(bool x) {
    if constexpr(static_cast<int>(Ec::NO) == 0
              && static_cast<int>(Ec::YES) == 1) {
        return static_cast<Ec>(x);
    } else {
        return x ? Ec::YES : Ec::NO;
    }
}

enum class Enquote { NO, YES };

namespace escape {

    enum class Spaces { NO, YES };

    std::u8string_view cppSv(
            std::u8string_view x,
            std::u8string& cache,
            char8_t lf,
            escape::Spaces escapeSpaces = escape::Spaces::NO,
            Enquote enquote = Enquote::NO);

}

namespace decode {

    /// @return 0..15 if x = 0..F
    ///         999 otherwise
    unsigned int hexDigitValue(char32_t x);

    /// @return 0..7 if x = 0..7
    ///         999 otherwise
    inline unsigned int octDigitValue(char32_t x) {
        if (x >= '0' && x <= '7')
            return x - '0';
        return 999;
    }

    std::u32string_view normalizeEolSv(
            std::u32string_view x,
            std::u32string &cache);
    std::wstring_view normalizeEolSv(
            std::wstring_view x,
            std::wstring &cache);

    namespace dcpp {
        ///  @return [+] x is digit 0..9, Latin letter a..z A..Z, underscore _
        bool isAlnum(char32_t x);

        ///  @return [+] x is Latin letter a..z A..Z, or underscore _
        bool isAlpha(char32_t x);
    }

    /// Decodes C++
    /// "alpha\nbravo" → alpha<LF>bravo
    /// Why U32: handle \U00012345
    ///
    /// Additional things:
    /// • starting and ending IDs: u8"text"sv
    /// • trailing chars: )},;
    ///
    /// NOT checked right now
    /// • Raw strings   R"(abc\ndef)"
    /// • Escape seqs w/o quotes   abc\ndef
    /// • Space policy when input has both quoted and unquoted text  abc "def" ghi
    ///
    std::u32string cpp(std::u32string_view x);

    /// Converts <br> → <br><LF>
    /// Why system-dependent wide…
    ///   • Unicode, better not UTF-8: QPlainTextEdit’s selection uses raw data, U+2029 inside
    ///   • Not C++20 u8string: Qt supports it in 6.2+ that dropped W7
    ///   • Locales: standard C++ regex
    ///
    std::wstring htmlBr(std::wstring_view x);

    /// Converts <p> → <LF><LF>
    std::wstring htmlP(std::wstring_view x);

    // Both p and br
    std::wstring htmlPBr(std::wstring_view x);

}   // namespace decode
