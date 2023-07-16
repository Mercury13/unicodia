#pragma once

//
//  Flags are common for AutoBuilder and Unicodia
//

// C++
#include <cassert>

// What else used
#include "u_TypedFlags.h"

namespace uc {

    ///
    ///   There are some chars that dub as typographic and emoji.
    ///   E.g. arrows, ‼
    ///   How to write them?
    ///
    enum class EmojiDraw {
        TEXT,           ///< Mostly library: draw emoji as text
        CONSERVATIVE,   ///< Table: those chars are drawn as text
        GRAPHIC         ///< Library: all emoji are drawn as graphic
    };

    enum class Cfg : unsigned short {
        M_ABBREVIATION   = 1<<0, ///< [+] Method: 1st synonym is abbreviation
        U_DEPRECATED     = 1<<1, ///< [+] UC feature: char is deprecated
        RENDER_BUG       = 1<<2, ///< [+] use font BUG_PREFER, or drop BUG_AVOID
        M_CUSTOM_CONTROL = 1<<3, ///< [+] Method: custom-drawn control char
        NO_AA            = 1<<4, ///< [+] Submethod of SAMPLE: temporarily disable anti-aliasing
        U_DEF_IGNORABLE  = 1<<5, ///< [+] UC feature: default-ignorable
        U_VS16_EMOJI     = 1<<6, ///< [+] UC feature: to surely make this char graphic, use VS16
        M_SVG_EMOJI      = 1<<7, ///< [+] Method: SVG emoji
        M_SPACE          = 1<<8, ///< [+] Method: draw as space, even if it is not space
        STYLE_0         =  1<<9, ///< [+] Has variable style 0
        STYLE_1         = 1<<10, ///< [+] Has variable style 1
        STYLE_2         = 1<<11, ///< [+] Has variable style 2
        DYN_SYSTEM_TOFU = 1<<15, ///< cached in runtime; [+] the char is tofu in system fonts
    };
    DEFINE_ENUM_OPS(Cfg)
    DEFINE_ENUM_SHIFTS(Cfg)
    using Cfgs = Flags<Cfg>;

    constexpr Cfgs STYLE_ALL = Cfg::STYLE_0 | Cfg::STYLE_1 | Cfg::STYLE_2;
    // Methods
    /// @todo [urgent] use 3 bits here: 0 = default, 1 = abbreviation, etc
    namespace m {
        constexpr Cfgs ALL = Cfg::M_ABBREVIATION | Cfg::M_CUSTOM_CONTROL | Cfg::M_SVG_EMOJI | Cfg::M_SPACE;
    }

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
        HTML = 4,       // HTML mnemonic, WITH ampersand & and semicolon ;
        DEP_INSTEAD = 5, // For deprecated chars: what’s instead
        DEP_INSTEAD2 = 6, // For deprecated chars: alternative instead
    };

    enum {
        INSTEAD_REVERT = '1',
        INSTEAD_MIN = INSTEAD_REVERT,
        INSTEAD_BIDI = '2',
        INSTEAD_FORMS = '3',
        INSTEAD_DIGITS = '4',
        INSTEAD_APPLEVEL = '5',
        INSTEAD_MAX = INSTEAD_APPLEVEL,
    };

}   // namespace uc

namespace sw {  // Sutton SignWriting

    constexpr char32_t CMIN = 0x1D800;
    constexpr char32_t CMAX = 0x1DA8B;
    constexpr int CLEN = CMAX - CMIN + 1;
    constexpr int NGOODCPS = CLEN;
    constexpr char32_t F2  = 0x1DA9B;
    constexpr char32_t F6  = 0x1DA9F;
    constexpr char32_t R2  = 0x1DAA1;
    constexpr char32_t R16 = 0x1DAAF;
    constexpr char32_t VIRTUAL_F1  = F2 - 1;
    constexpr char32_t VIRTUAL_R1  = R2 - 1;

    constexpr int N_FILL = 6;
    constexpr int N_ROT = 16;

    struct Char {
        uint16_t rot;
        uint8_t fill;
        uint8_t minSpecialFill;

        constexpr bool hasFill(unsigned i) const {
            assert(i < sw::N_FILL);
            return fill & (1 << i);
        }

        constexpr bool hasRotation(unsigned i) const {
            assert(i < sw::N_ROT);
            return rot & (1 << i);
        }

        constexpr bool operator == (const Char& x) const noexcept = default;
    };

    extern const sw::Char data[sw::CLEN];
}   // namespace sw
