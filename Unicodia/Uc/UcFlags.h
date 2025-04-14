#pragma once

//
//  Flags are common for AutoBuilder and Unicodia
//

// What else used
#include "u_TypedFlags.h"

// C++
#include <cassert>
#include <cstdint>

namespace uc {

    ///
    ///   There are some chars that dub as typographic and emoji.
    ///   E.g. arrows, ‼
    ///   How to write them?
    ///
    enum class EmojiDraw : unsigned char {
        FORCE_TEXT,     ///< Do not draw graphics AT ALL
        MOSTLY_TEXT,    ///< Mostly library: draw emoji as text
        CONSERVATIVE,   ///< Table: those chars are drawn as text
        GRAPHIC         ///< Library: all emoji are drawn as graphic
    };

    enum class Cfg : unsigned short {
        M_BIT_0          = 1<<0, ///< Method: bit 0
        M_BIT_1          = 1<<1, ///< Method: bit 1
        M_BIT_2          = 1<<2, ///< Method: bit 2
            /// @todo [future] All three bits are used up and cannot save a bit,
            ///                but let it be bit 3?
        M_NO_SHAPING     = 1<<3, ///< Method: turn off shaping
        U_DEPRECATED     = 1<<4, ///< [+] UC feature: char is deprecated
        U_DEF_IGNORABLE  = 1<<5, ///< [+] UC feature: default-ignorable
        U_VS16_EMOJI     = 1<<6, ///< [+] UC feature: to surely make this char graphic, use VS16
        G_RENDER_BUG     = 1<<7, ///< [+] Glyph: use font BUG_PREFER, or drop BUG_AVOID
        G_STYLE_0        = 1<<8, ///< [+] Glyph: has variable style 0
        G_STYLE_1        = 1<<9, ///< [+] Glyph: has variable style 1
        G_STYLE_2       = 1<<10, ///< [+] Glyph: has variable style 2
        G_STYLE_3       = 1<<11, ///< [+] Glyph: has variable style 3 (some day will be needed for isolated/initial/medial/final Arabic)
        G_MISRENDER     = 1<<12, ///< [+] emoji misrenders are possible
        S_COOL_2        = 1<<13, ///< [+] Search: these words are meaningful: ON
        DYN_SYSTEM_TOFU = 1<<15, ///< cached in runtime; [+] the char is tofu in system fonts
    };
    DEFINE_ENUM_OPS(Cfg)
    DEFINE_ENUM_SHIFTS(Cfg)
    using Cfgs = Flags<Cfg>;

    constexpr Cfgs STYLE_ALL = Cfg::G_STYLE_0 | Cfg::G_STYLE_1 | Cfg::G_STYLE_2 | Cfg::G_STYLE_3;
    // Methods
    namespace m {
        constexpr Cfgs ALL = Cfg::M_BIT_0 | Cfg::M_BIT_1 | Cfg::M_BIT_2;
        constexpr Cfgs SAMPLE = NO_FLAGS;
        constexpr Cfgs NO_AA = Cfg::M_BIT_0;
        constexpr Cfgs SPACE = Cfg::M_BIT_1;
        constexpr Cfgs ABBREVIATION = Cfg::M_BIT_1 | Cfg::M_BIT_0;
        constexpr Cfgs CUSTOM_CONTROL = Cfg::M_BIT_2;
        constexpr Cfgs SVG_EMOJI = Cfg::M_BIT_2 | Cfg::M_BIT_0;
        constexpr Cfgs SAMPLED_CONTROL = Cfg::M_BIT_2 | Cfg::M_BIT_1;
        constexpr Cfgs VIRTUAL_VIRAMA = Cfg::M_BIT_2 | Cfg::M_BIT_1 | Cfg::M_BIT_0;
        // Warning: all bits full!
    }

    enum class Lfg : unsigned short {
        GRAPHIC_EMOJI = 1<<0,   ///< [+] EmojiDraw::GRAPHIC  [-] CONSERVATIVE
        NO_TILE       = 1<<1,   ///< [+] do not draw tile (Component right now)
        CODE_AS_NAME  = 1<<2,   ///< [+] use code as name (strange CJK right now)
        TRANSLATE     = 1<<3,   ///< [+] library line is localized using name as key
        HAS_TEXT      = 1<<4,   ///< [+] object has text
        SEARCHABLE    = 1<<5,   ///< [+] emoji is searchable. Unsearchable are single-character, VS16, race/direction
        DECODEABLE    = 1<<6,   ///< [+] emoji is decodeable. Undecodeable are single-character, VS16
        NO_COUNTING   = 1<<7,   ///< [+] do not count (components only)
        MISRENDER_0   = 1<<8,   ///< [+] emoji misrenders are possible
        MISRENDER_1   = 1<<9,   ///< [+] emoji misrenders are possible
        MISRENDER_2   = 1<<10,  ///< [+] emoji misrenders are possible
    };
    using Lfgs = Flags<Lfg>;
    DEFINE_ENUM_OPS(Lfg)
    constexpr Lfgs MISRENDER_MASK = Lfg::MISRENDER_0 | Lfg::MISRENDER_1 | Lfg::MISRENDER_2;
    constexpr Lfgs MISRENDER_SIMPLE { Lfg::MISRENDER_0 };       // 01
    constexpr Lfgs MISRENDER_FAMILY { Lfg::MISRENDER_1 };       // 10
    constexpr Lfgs MISRENDER_IO     { Lfg::MISRENDER_1 | Lfg::MISRENDER_0 }; // 11

    namespace detail {
        // it’ll leave >25k bytes of string
        constexpr unsigned MIN_2BYTE_STRING = 160;
    }

    enum class TextRole : unsigned char {
        CMD_END = 0,    // Command: text end
        MAIN_NAME = 1,  // Character’s name, should exist
        ALT_NAME = 2,   // Alternate names, always unique (non-template)
        ABBREV = 3,     // Abbreviation, always unique (non-template)
        HTML = 4,       // HTML mnemonic, WITH ampersand & and semicolon ;
        DEP_INSTEAD = 5, // For deprecated chars: what’s instead
        DEP_INSTEAD2 = 6, // For deprecated chars: alternative instead
        EMOJI_NAME = 7, // Emoji name from Library, if inequal
        EGYP_EWP = 8,   // Egyptian description from English Wikipedia
        EGYP_UC = 9,    // Egyptian description from Unicode
    };

    enum : unsigned char {
        INSTEAD_REVERT = '1',
        INSTEAD_MIN = INSTEAD_REVERT,
        INSTEAD_BIDI = '2',
        INSTEAD_FORMS = '3',
        INSTEAD_DIGITS = '4',
        INSTEAD_APPLEVEL = '5',
        INSTEAD_MAX = INSTEAD_APPLEVEL,
    };

    class SvgChecker {  // interface
    public:
        virtual bool canDraw(char32_t cp) = 0;
        virtual ~SvgChecker() = default;
    };

    ///
    /// \brief The OldComp enum
    ///   Old computers, now controlled by AutoBuilder
    ///
    enum class OldComp : unsigned int {
        NONE        = 0,
        AMSTRAD_CPC = 1 << 0,
        APPLE       = 1 << 1,
        AQUARIUS    = 1 << 2,
        ATARI_8BIT  = 1 << 3,
        ATARI_ST    = 1 << 4,
        MINITEL     = 1 << 5,
        MSX         = 1 << 6,
        ORIC        = 1 << 7,
        COMMODORE   = 1 << 8,
        RISC_OS     = 1 << 9,
        SINCLAIR    = 1 << 10,
        TELETEXT    = 1 << 11,
        TANDY_TRS80 = 1 << 12,
        TANDY_COCO  = 1 << 13,
        KORVET      = 1 << 14,
        SMALLTALK   = 1 << 15,
        SHARP_MZ    = 1 << 16,
        OHIO_SCIENTIFIC = 1 << 17,
        ROBOTRON    = 1 << 18,
        HP_TERMINAL = 1 << 19,
        KAYPRO      = 1 << 20,
        IBM         = 1 << 21,
        MAC         = 1 << 22,
        SPECTRUM    = 1 << 23,
        BBC_MASTER  = 1 << 24,
        // Technical
        DUMMY,
        LAST = DUMMY - 1,
        FIRST = 1
    };
    DEFINE_ENUM_OPS(OldComp)

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
