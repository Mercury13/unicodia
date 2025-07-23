#pragma once

#include <string_view>

namespace cp {

    // Spaces
    constexpr char32_t NBSP = 0xA0;
    constexpr char32_t NNBSP = 0x202F;  // narrow no-break space

    // Marks
    constexpr char32_t MARK_KEYCAP = 0x20E3;

    // Specials
    constexpr char32_t ZWJ = 0x200D;

    // Arrows
    constexpr char32_t ARROW_UP_DOWN = 0x2195;

    // Shapes
    constexpr char32_t DOTTED_CIRCLE = 0x25CC;

    // Coloured squares
    constexpr char32_t SQUARE_BLACK  = 0x2B1B;
    constexpr char32_t SQUARE_WHITE  = 0x2B1C;
    constexpr char32_t SQUARE_RED    = 0x1F7E5;
    constexpr char32_t SQUARE_ORANGE = 0x1F7E7;
    constexpr char32_t SQUARE_YELLOW = 0x1F7E8;
    constexpr char32_t SQUARE_GREEN  = 0x1F7E9;
    constexpr char32_t SQUARE_BLUE   = 0x1F7E6;
    constexpr char32_t SQUARE_PURPLE = 0x1F7EA;
    constexpr char32_t SQUARE_BROWN  = 0x1F7EB;

    // Kangxi radicals
    constexpr int KANGXI_N = 214;
    constexpr char32_t KANGXI_FIRST = 0x2F00;
    constexpr char32_t KANGXI_DELTA = KANGXI_FIRST - 1;
    constexpr char32_t KANGXI_LAST = KANGXI_DELTA + KANGXI_N;

    // Han ideographs
    constexpr char32_t HANI_A_FIRST = 0x3400;
    constexpr char32_t HANI_A_LAST = 0x4DBF;

    constexpr char32_t HANI_0_FIRST = 0x4E00;
    constexpr char32_t HANI_0_LAST = 0x9FFF;

    constexpr char32_t HANI_COMPAT_FIRST = 0xF900;
    constexpr char32_t HANI_COMPAT_LAST = 0xFAFF;

    // Planes
    constexpr char32_t PLANE_MASK = 0xFFFF;
    constexpr char32_t PLANE_2_START = 0xF'0000;
    constexpr char32_t PLANE_F_START = 0xF'0000;

    // Non-characters, private, wtc
    constexpr char32_t PRIV_BMP_FIRST = 0xE000;
    constexpr char32_t PRIV_BMP_LAST = 0xF8FF;

    // Mathematical etc
    constexpr char32_t ZERO_THIRDS = 0x2189;
    constexpr char32_t MATH_GREEK_FIRST = 0x1D6A8;
    constexpr char32_t MATH_GREEK_LAST = 0x1D7CB;

    // Dingbats
    constexpr char32_t DING_BLACK_HEART = 0x2764;
    constexpr char32_t EMOJI_RED_HEART = DING_BLACK_HEART;
    constexpr char32_t DING_RIGHT_ARROW = 0x27A1;
    constexpr char32_t EMOJI_RIGHT_ARROW = DING_RIGHT_ARROW;

    // Hangul syllables
    constexpr char32_t HANGUL_SYLL_FIRST = 0xAC00;
    constexpr char32_t HANGUL_SYLL_LAST = 0xD7AF;

    // Variation selectors
    constexpr char32_t VS01 = 0xFE00;
    constexpr char32_t VS02 = 0xFE01;
    constexpr char32_t VS03 = 0xFE02;
    constexpr char32_t VS04 = 0xFE03;
    constexpr char32_t VS15 = 0xFE0E;
    constexpr char32_t VS16 = 0xFE0F;

    // Emoji special
    constexpr char32_t SKIN1 = 0x1F3FB;
    constexpr char32_t SKIN2 = 0x1F3FC;
    constexpr char32_t SKIN3 = 0x1F3FD;
    constexpr char32_t SKIN4 = 0x1F3FE;
    constexpr char32_t SKIN5 = 0x1F3FF;

    // Misc emoji
    constexpr char32_t MARS = 0x2642;
    constexpr char32_t VENUS = 0x2640;
    constexpr char32_t SANTA_CLAUS = 0x1F385;
    constexpr char32_t BLACK_FLAG = 0x1F3F4;
    constexpr char32_t RABBIT_FACE = 0x1F430;
    constexpr char32_t BOY = 0x1F466;
    constexpr char32_t GIRL = 0x1F467;
    constexpr char32_t MAN  = 0x1F468;
    constexpr char32_t WOMAN = 0x1F469;
    constexpr char32_t MAN_AND_WOMAN = 0x1F46B;
    constexpr char32_t TWO_MEN = 0x1F46C;
    constexpr char32_t PLAYBOY_BUNNIES = 0x1F46F;
    constexpr char32_t TWO_WOMEN = 0x1F46D;
    constexpr char32_t KISS_MARK = 0x1F48B;
    constexpr char32_t KISS = 0x1F48F;
    constexpr char32_t COUPLE_HEART = 0x1F491;
    constexpr char32_t HANDSHAKE = 0x1F91D;
    constexpr char32_t MRS_CLAUS = 0x1F936;
    constexpr char32_t WRESTLERS = 0x1F93C;
    constexpr char32_t ADULT = 0x1F9D1;
    constexpr char32_t CHILD = 0x1F9D2;
    constexpr char32_t FIGHT_CLOUD = 0x1FAEF;

    // Smiley
    constexpr char32_t SMILE_SLIGHTLY_SMILING = 0x1F642;

    // Appearance
    constexpr char32_t PERSON_BLOND = 0x1F471;
    constexpr char32_t HAIR_RED = 0x1F9B0;
    constexpr char32_t HAIR_CURLY = 0x1F9B1;
    constexpr char32_t HAIR_BALD = 0x1F9B2;
    constexpr char32_t HAIR_WHITE = 0x1F9B3;

    // Regional
    constexpr char32_t FLAG_A = 0x1F1E6;
    constexpr char32_t FLAG_B = 0x1F1E7;
    // E8
    constexpr char32_t FLAG_C = 0x1F1E8;
    constexpr char32_t FLAG_D = 0x1F1E9;
    constexpr char32_t FLAG_E = 0x1F1EA;
    constexpr char32_t FLAG_F = 0x1F1EB;
    constexpr char32_t FLAG_G = 0x1F1EC;
    constexpr char32_t FLAG_H = 0x1F1ED;
    constexpr char32_t FLAG_I = 0x1F1EE;
    constexpr char32_t FLAG_J = 0x1F1EF;
    // F0
    constexpr char32_t FLAG_K = 0x1F1F0;
    constexpr char32_t FLAG_L = 0x1F1F1;
    constexpr char32_t FLAG_M = 0x1F1F2;
    constexpr char32_t FLAG_N = 0x1F1F3;
    constexpr char32_t FLAG_O = 0x1F1F4;
    constexpr char32_t FLAG_P = 0x1F1F5;
    constexpr char32_t FLAG_Q = 0x1F1F6;
    constexpr char32_t FLAG_R = 0x1F1F7;
    // F8
    constexpr char32_t FLAG_S = 0x1F1F8;
    constexpr char32_t FLAG_T = 0x1F1F9;
    constexpr char32_t FLAG_U = 0x1F1FA;
    constexpr char32_t FLAG_V = 0x1F1FB;
    constexpr char32_t FLAG_W = 0x1F1FC;
    constexpr char32_t FLAG_X = 0x1F1FD;
    constexpr char32_t FLAG_Y = 0x1F1FE;
    constexpr char32_t FLAG_Z = 0x1F1FF;

    // Tags
    constexpr char32_t TAG_MIN = 0xE0000;
    constexpr char32_t TAG_CANCEL = 0xE007F;
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
