#pragma once

//
//  Flags are common for AutoBuilder and Unicodia
//

#include "u_TypedFlags.h"

namespace uc {

    enum class Cfg : unsigned short {
        M_ABBREVIATION = 1,     ///< [+] Method: 1st synonym is abbreviation
        U_DEPRECATED = 2,       ///< [+] UC feature: char is deprecated
        RENDER_BUG = 4,         ///< [+] use font BUG_PREFER, or drop BUG_AVOID
        M_CUSTOM_CONTROL = 8,   ///< [+] Method: custom-drawn control char
        NO_AA = 16,             ///< [+] Submethod of SAMPLE: temporarily disable anti-aliasing
        U_DEF_IGNORABLE = 32,   ///< [+] UC feature: default-ignorable
        U_VS16_EMOJI = 64,      ///< [+] UC feature: to surely make this char graphic, use VS16
        M_SVG_EMOJI = 128,      ///< [+] Method: SVG emoji
        M_SPACE = 256,          ///< [+] Method: draw as space, even if it is not space
        M_EGYPTIAN_HATCH = 512, ///< [+] Method: draw Egyptian hatch (not really a hatch, but OK)
        DYN_SYSTEM_TOFU = 32768,///< cached in runtime; [+] the char is tofu in system fonts
        M_ALL = M_ABBREVIATION | M_CUSTOM_CONTROL | M_SVG_EMOJI | M_SPACE | M_EGYPTIAN_HATCH
    };
    using Cfgs = Flags<Cfg>;

    enum class Lfg : unsigned char {
        GRAPHIC_EMOJI = 1 << 0,     ///< [+] EmojiDraw::GRAPHIC  [-] CONSERVATIVE
        NO_TILE       = 1 << 1,     ///< [+] do not draw tile (Component right now)
        CODE_AS_NAME  = 1 << 2,     ///< [+] use code as name (strange CJK right now)
        TRANSLATE     = 1 << 3,     ///< [+] library line is localized using name as key
    };
    using Lfgs = Flags<Lfg>;

    DEFINE_ENUM_OPS(Lfg)

    enum class TextRole : unsigned char {
        CMD_END = 0,    // Command: text end
        MAIN_NAME = 1,  // Character’s name, should exist
        ALT_NAME = 2,   // Alternate names, always unique (non-template)
        ABBREV = 3,     // Abbreviation, always unique (non-template)
        HTML = 4        // HTML mnemonic, NO ampersand & and semicolon ;
    };

}   // namespace uc

namespace sw {  // Sutton SignWriting

    constexpr char32_t CMIN = 0x1D800;
    constexpr char32_t CMAX = 0x1DA8B;
    constexpr int CLEN = CMAX - CMIN + 1;
    constexpr char32_t F2  = 0x1DA9B;
    constexpr char32_t F6  = 0x1DA9F;
    constexpr char32_t R2  = 0x1DAA1;
    constexpr char32_t R16 = 0x1DAAF;
    constexpr char32_t VIRTUAL_F1  = F2 - 1;
    constexpr char32_t VIRTUAL_R1  = R2 - 1;

    constexpr int N_FILL = 6;
    constexpr int N_ROT = 16;

    struct Char {
        uint16_t fills[N_FILL] { 0, 0, 0, 0, 0, 0 };
    };

}   // namespace sw
