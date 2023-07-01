#pragma once

#include <string_view>

constexpr char32_t ZWJ = 0x200D;
constexpr char32_t VS16 = 0xFE0F;
constexpr char32_t SKIN1 = 0x1F3FB;
constexpr char32_t SKIN2 = 0x1F3FC;
constexpr char32_t SKIN3 = 0x1F3FD;
constexpr char32_t SKIN4 = 0x1F3FE;
constexpr char32_t SKIN5 = 0x1F3FF;
constexpr char32_t RIGHT_ARROW = 0x27A1;
constexpr char32_t MALE = 0x2642;
constexpr char32_t FEMALE = 0x2640;
constexpr char32_t MAN = 0x1F468;
constexpr char32_t WOMAN = 0x1F469;
constexpr char32_t MAN_AND_WOMAN = 0x1F46B;
constexpr char32_t TWO_MEN = 0x1F46C;
constexpr char32_t TWO_WOMEN = 0x1F46D;
constexpr char32_t SANTA_CLAUS = 0x1F385;
constexpr char32_t MRS_CLAUS = 0x1F936;
constexpr char32_t FLAG_A = 0x1F1E6;
constexpr char32_t FLAG_Z = 0x1F1FF;

constexpr std::u32string_view U32_ZWJ_RIGHT_ARROW_VS16 = U"\u200D" "\u27A1" "\uFE0F";
