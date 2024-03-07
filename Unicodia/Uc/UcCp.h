#pragma once

#include <string_view>

namespace cp {

    // Specials
    constexpr char32_t ZWJ = 0x200D;

    // Planes
    constexpr char32_t PLANE_MASK = 0xFFFF;
    constexpr char32_t PLANE_2_START = 0xF'0000;
    constexpr char32_t PLANE_F_START = 0xF'0000;

    // Non-characters
    constexpr char32_t PRIV_BMP_FIRST = 0xE000;
    constexpr char32_t PRIV_BMP_LAST = 0xF8FF;

    // Mathematical etc
    constexpr char32_t ZERO_THIRDS = 0x2189;
    constexpr char32_t MATH_GREEK_FIRST = 0x1D6A8;
    constexpr char32_t MATH_GREEK_LAST = 0x1D7CB;

    // Variation selectors
    constexpr char32_t VS15 = 0xFE0E;
    constexpr char32_t VS16 = 0xFE0F;

    // Emoji special
    constexpr char32_t SKIN1 = 0x1F3FB;
    constexpr char32_t SKIN2 = 0x1F3FC;
    constexpr char32_t SKIN3 = 0x1F3FD;
    constexpr char32_t SKIN4 = 0x1F3FE;
    constexpr char32_t SKIN5 = 0x1F3FF;

    // Misc emoji
    constexpr char32_t RIGHT_ARROW = 0x27A1;
    constexpr char32_t MALE = 0x2642;
    constexpr char32_t FEMALE = 0x2640;
    constexpr char32_t MAN = 0x1F468;
    constexpr char32_t WOMAN = 0x1F469;
    constexpr char32_t BOY = 0x1F466;
    constexpr char32_t GIRL = 0x1F467;
    constexpr char32_t ADULT = 0x1F9D1;
    constexpr char32_t CHILD = 0x1F9D2;
    constexpr char32_t MAN_AND_WOMAN = 0x1F46B;
    constexpr char32_t TWO_MEN = 0x1F46C;
    constexpr char32_t TWO_WOMEN = 0x1F46D;
    constexpr char32_t SANTA_CLAUS = 0x1F385;
    constexpr char32_t MRS_CLAUS = 0x1F936;
    constexpr char32_t FLAG_A = 0x1F1E6;
    constexpr char32_t FLAG_Z = 0x1F1FF;

    // Tags
    constexpr char32_t TAG_MIN = 0xE0000;
    constexpr char32_t TAG_MAX = 0xE007F;

    // NONCHAR
    constexpr char32_t NONCHAR_ARAB_FIRST = 0xFDD0;
    constexpr char32_t NONCHAR_ARAB_LAST  = 0xFDEF;
    constexpr char32_t NONCHAR_BMP_FIRST = 0xFFFE;

    // Maximum non-private codepoint
    constexpr char32_t MAX_NONPRIVATE = 0x0EFFFD;
}   // namespace cp

constexpr std::u32string_view U32_ZWJ_RIGHT_ARROW_VS16 = U"\u200D" "\u27A1" "\uFE0F";

namespace uc {

    /// @return [+] cp is noncharacter
    constexpr bool isNonChar(char32_t cp)
    {
        return ((cp & cp::PLANE_MASK) >= cp::NONCHAR_BMP_FIRST)
            || (cp >= cp::NONCHAR_ARAB_FIRST && cp <= cp::NONCHAR_ARAB_LAST);
    }

}   // namespace uc
