#include "Xcolor.h"

// STL
#include <span>
#include <limits>

// Lib
#include "u_Strings.h"

// Machine checks
    // A-Z lookup
static_assert('Z' - 'A' == 25, "Non-ASCII machine");
static_assert('z' - 'a' == 25, "Non-ASCII machine");
    // Bytes
static_assert(std::numeric_limits<unsigned char>::max() == 255,
              "Strange machine");

namespace {

    struct TableEntry {
        std::string_view name;
        xs::Color value;
    };
    consteval xs::Color by(unsigned x) {
        return {
            .r = (unsigned char)(x >> 16),
            .g = (unsigned char)(x >> 8),
            .b = (unsigned char)(x),
        };
    }

    constexpr TableEntry colorsA[] {
        { "AliceBlue",      by(0xF0F8FF) },
        { "AntiqueWhite",   by(0xFAEBD7) },
        { "Aqua",           by(0x00FFFF) }, // #0ff
        { "Aquamarine",     by(0x7FFFD4) },
        { "Azure",          by(0xF0FFFF) } }; // SHORT
    constexpr TableEntry colorsB[] {
        { "Beige",          by(0xF5F5DC) }, // SHORT
        { "Bisque",         by(0xFFE4C4) }, // SHORT
        { "Black",          by(0x000000) }, // #000
        { "BlanchedAlmond", by(0xFFEBCD) },
        { "Blue",           by(0x0000FF) }, // #00f
        { "BlueViolet",     by(0x8A2BE2) },
        { "Brown",          by(0xA52A2A) }, // SHORT
        { "BurlyWood",      by(0xDEB887) } };
    constexpr TableEntry colorsC[] {
        { "CadetBlue",      by(0x5F9EA0) },
        { "Chartreuse",     by(0x7FFF00) },
        { "Chocolate",      by(0xD2691E) },
        { "Coral",          by(0xFF7F50) }, // SHORT
        { "CornflowerBlue", by(0x6495ED) },
        { "Cornsilk",       by(0xFFF8DC) },
        { "Crimson",        by(0xDC143C) },
        { "Cyan",           by(0x00FFFF) } }; // #0ff
    constexpr TableEntry colorsD[] {
        { "DarkBlue",       by(0x00008B) },
        { "DarkCyan",       by(0x008B8B) },
        { "DarkGoldenRod",  by(0xB8860B) },
        { "DarkGray",       by(0xA9A9A9) },
        { "DarkGrey",       by(0xA9A9A9) },
        { "DarkGreen",      by(0x006400) },
        { "DarkKhaki",      by(0xBDB76B) },
        { "DarkMagenta",    by(0x8B008B) },
        { "DarkOliveGreen", by(0x556B2F) },
        { "DarkOrange",     by(0xFF8C00) },
        { "DarkOrchid",     by(0x9932CC) },
        { "DarkRed",        by(0x8B0000) },
        { "DarkSalmon",     by(0xE9967A) },
        { "DarkSeaGreen",   by(0x8FBC8F) },
        { "DarkSlateBlue",  by(0x483D8B) },
        { "DarkSlateGray",  by(0x2F4F4F) },
        { "DarkSlateGrey",  by(0x2F4F4F) },
        { "DarkTurquoise",  by(0x00CED1) },
        { "DarkViolet",     by(0x9400D3) },
        { "DeepPink",       by(0xFF1493) },
        { "DeepSkyBlue",    by(0x00BFFF) },
        { "DimGray",        by(0x696969) },
        { "DimGrey",        by(0x696969) },
        { "DodgerBlue",     by(0x1E90FF) } };
    constexpr std::span<const TableEntry> colorsE {};
    constexpr TableEntry colorsF[] {
        { "FireBrick",      by(0xB22222) },
        { "FloralWhite",    by(0xFFFAF0) },
        { "ForestGreen",    by(0x228B22) },
        { "Fuchsia",        by(0xFF00FF) } };
    constexpr TableEntry colorsG[] {
        { "Gainsboro",      by(0xDCDCDC) },
        { "GhostWhite",     by(0xF8F8FF) },
        { "Gold",           by(0xFFD700) }, // SHORT
        { "GoldenRod",      by(0xDAA520) },
        { "Gray",           by(0x808080) }, // SHORT
        { "Grey",           by(0x808080) },
        { "Green",          by(0x008000) },
        { "GreenYellow",    by(0xADFF2F) } };
    constexpr TableEntry colorsH[] {
        { "HoneyDew",       by(0xF0FFF0) },
        { "HotPink",        by(0xFF69B4) } };
    constexpr TableEntry colorsI[] {
        { "IndianRed",      by(0xCD5C5C) },
        { "Indigo",         by(0x4B0082) },
        { "Ivory",          by(0xFFFFF0) } }; // SHORT
    constexpr std::span<const TableEntry> colorsJ {};
    constexpr TableEntry colorsK[] {
        { "Khaki",          by(0xF0E68C) } }; // SHORT
    constexpr TableEntry colorsL[] {
        { "Lavender",       by(0xE6E6FA) },
        { "LavenderBlush",  by(0xFFF0F5) },
        { "LawnGreen",      by(0x7CFC00) },
        { "LemonChiffon",   by(0xFFFACD) },
        { "LightBlue",      by(0xADD8E6) },
        { "LightCoral",     by(0xF08080) },
        { "LightCyan",      by(0xE0FFFF) },
        { "LightGoldenRodYellow", by(0xFAFAD2) },
        { "LightGray",      by(0xD3D3D3) },
        { "LightGrey",      by(0xD3D3D3) },
        { "LightGreen",     by(0x90EE90) },
        { "LightPink",      by(0xFFB6C1) },
        { "LightSalmon",    by(0xFFA07A) },
        { "LightSeaGreen",  by(0x20B2AA) },
        { "LightSkyBlue",   by(0x87CEFA) },
        { "LightSlateGray", by(0x778899) },
        { "LightSlateGrey", by(0x778899) },
        { "LightSteelBlue", by(0xB0C4DE) },
        { "LightYellow",    by(0xFFFFE0) },
        { "Lime",           by(0x00FF00) }, // #0f0
        { "LimeGreen",      by(0x32CD32) },
        { "Linen",          by(0xFAF0E6) } }; // SHORT
    constexpr TableEntry colorsM[] {
        { "Magenta",        by(0xFF00FF) },
        { "Maroon",         by(0x800000) }, // SHORT
        { "MediumAquaMarine", by(0x66CDAA) },
        { "MediumBlue",     by(0x0000CD) },
        { "MediumOrchid",   by(0xBA55D3) },
        { "MediumPurple",   by(0x9370DB) },
        { "MediumSeaGreen", by(0x3CB371) },
        { "MediumSlateBlue", by(0x7B68EE) },
        { "MediumSpringGreen", by(0x00FA9A) },
        { "MediumTurquoise", by(0x48D1CC) },
        { "MediumVioletRed", by(0xC71585) },
        { "MidnightBlue",   by(0x191970) },
        { "MintCream",      by(0xF5FFFA) },
        { "MistyRose",      by(0xFFE4E1) },
        { "Moccasin",       by(0xFFE4B5) } };
    constexpr TableEntry colorsN[] {
        { "NavajoWhite",    by(0xFFDEAD) },
        { "Navy",           by(0x000080) } }; // SHORT
    constexpr TableEntry colorsO[] {
        { "OldLace",        by(0xFDF5E6) },
        { "Olive",          by(0x808000) }, // SHORT
        { "OliveDrab",      by(0x6B8E23) },
        { "Orange",         by(0xFFA500) }, // SHORT
        { "OrangeRed",      by(0xFF4500) },
        { "Orchid",         by(0xDA70D6) } }; // SHORT
    constexpr TableEntry colorsP[] {
        { "PaleGoldenRod",  by(0xEEE8AA) },
        { "PaleGreen",      by(0x98FB98) },
        { "PaleTurquoise",  by(0xAFEEEE) },
        { "PaleVioletRed",  by(0xDB7093) },
        { "PapayaWhip",     by(0xFFEFD5) },
        { "PeachPuff",      by(0xFFDAB9) },
        { "Peru",           by(0xCD853F) }, // SHORT
        { "Pink",           by(0xFFC0CB) }, // SHORT
        { "Plum",           by(0xDDA0DD) }, // SHORT
        { "PowderBlue",     by(0xB0E0E6) },
        { "Purple",         by(0x800080) } }; // SHORT
    constexpr std::span<const TableEntry> colorsQ {};
    constexpr TableEntry colorsR[] {
        { "RebeccaPurple",  by(0x663399) },
        { "Red",            by(0xFF0000) }, // REALLY SHORT
        { "RosyBrown",      by(0xBC8F8F) },
        { "RoyalBlue",      by(0x4169E1) } };
    constexpr TableEntry colorsS[] {
        { "SaddleBrown",    by(0x8B4513) },
        { "Salmon",         by(0xFA8072) }, // SHORT
        { "SandyBrown",     by(0xF4A460) },
        { "SeaGreen",       by(0x2E8B57) },
        { "SeaShell",       by(0xFFF5EE) },
        { "Sienna",         by(0xA0522D) }, // SHORT
        { "Silver",         by(0xC0C0C0) }, // SHORT
        { "SkyBlue",        by(0x87CEEB) },
        { "SlateBlue",      by(0x6A5ACD) },
        { "SlateGray",      by(0x708090) },
        { "SlateGrey",      by(0x708090) },
        { "Snow",           by(0xFFFAFA) }, // SHORT
        { "SpringGreen",    by(0x00FF7F) },
        { "SteelBlue",      by(0x4682B4) } };
    constexpr TableEntry colorsT[] {
        { "Tan",            by(0xD2B48C) }, // SHORT
        { "Teal",           by(0x008080) }, // SHORT
        { "Thistle",        by(0xD8BFD8) },
        { "Tomato",         by(0xFF6347) }, // SHORT
        { "Turquoise",      by(0x40E0D0) } };
    constexpr std::span<const TableEntry> colorsU {};
    constexpr TableEntry colorsV[] {
        { "Violet",         by(0xEE82EE) } }; // SHORT
    constexpr TableEntry colorsW[] {
        { "Wheat",          by(0xF5DEB3) }, // SHORT
        { "White",          by(0xFFFFFF) }, // #fff
        { "WhiteSmoke",     by(0xF5F5F5) } };
    constexpr std::span<const TableEntry> colorsX {};
    constexpr TableEntry colorsY[] {
        { "Yellow",         by(0xFFFF00) }, // #ff0
        { "YellowGreen",    by(0x9ACD32) } };
    constinit const std::span<const TableEntry> colorLookup[] {
        colorsA, colorsB, colorsC, colorsD, colorsE, colorsF, colorsG, colorsH,
        colorsI, colorsJ, colorsK, colorsL, colorsM, colorsN, colorsO, colorsP,
        colorsQ, colorsR, colorsS, colorsT, colorsU, colorsV, colorsW, colorsX,
        colorsY };

    int parseHexDig(char c)
    {
        if ((c >= '0') && (c <= '9'))
            return c - '0';
        if ((c >= 'A') && (c <= 'F')) {
            static constexpr int BASE1 = 'A' - 10;
            return c - BASE1;
        }
        if ((c >= 'a') && (c <= 'f')) {
            static constexpr int BASE2 = 'a' - 10;
            return c - BASE2;
        }
        return -1;
    }

    int parseTwoHexDig(const char* c)
    {
        auto bigger = parseHexDig(c[0]);
        if (bigger < 0)
            return -1;
        auto smaller = parseHexDig(c[1]);
        if (smaller < 0)
            return -1;
        return (bigger << 4) | smaller;
    }

}   // anon namespace

std::optional<xs::Color> xs::Color::parse(std::string_view x)
{
    if (x.empty())
        return std::nullopt;
    auto c0 = x[0];
    if (c0 == '#') {
        switch (x.length()) {
        case 4: {
            auto r = parseHexDig(x[1]);
            auto g = parseHexDig(x[2]);
            auto b = parseHexDig(x[3]);
            if (r >= 0 && g >= 0 && b >= 0) {
                return Color{
                        .r = (unsigned char)(r * 0x11),
                        .g = (unsigned char)(g * 0x11),
                        .b = (unsigned char)(b * 0x11) };
            }
            return std::nullopt;
        }
        case 7: {
            auto r = parseTwoHexDig(x.data() + 1);
            auto g = parseTwoHexDig(x.data() + 3);
            auto b = parseTwoHexDig(x.data() + 5);
            if (r >= 0 && g >= 0 && b >= 0) {
                return Color{
                        .r = (unsigned char)r,
                        .g = (unsigned char)g,
                        .b = (unsigned char)b };
            }
            return std::nullopt;
        }
        default:
            return std::nullopt;
        }
    } else if (c0 >= 'A' && c0 <= 'Y') {
        auto index = c0 - 'A';
        for (auto& c : colorLookup[index]) {
            if (lat::areCaseEqual(x, c.name))
                return c.value;
        }
    } else if (c0 >= 'a' && c0 <= 'y') {
        auto index = c0 - 'a';
        for (auto& c : colorLookup[index]) {
            if (lat::areCaseEqual(x, c.name))
                return c.value;
        }
    }
    return std::nullopt;
}

bool xs::Color::isShort() const noexcept
{
    return (r % 0x11 == 0)
    && (g % 0x11 == 0)
        && (b % 0x11 == 0);
}

void xs::Color::encodeAttr(std::string& dest) const
{
    dest.reserve(dest.size() + 7);
    char buf[16];
    if (isShort()) {
        snprintf(buf, std::size(buf), "#%x%x%x",
                 r / 0x11, g / 0x11, b / 0x11);
    } else {
        snprintf(buf, std::size(buf), "#" "%02x" "%02x" "%02x",
                 r, g, b);
    }
    dest += buf;
}
