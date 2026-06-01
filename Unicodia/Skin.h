#pragma once

// Qt
#include <QColor>

// Colors, sizes etc that are NOT derived from system

enum class Fsz : unsigned char {    // font size
    TABLE = 15,
    LIST = 14,
    BIG = 50,
};
constexpr int ISZ_LIST = 36;  // icon size: 15→39, 14→?

// alpha of search/emoji border
/// @todo [hidpi] When DPI is really high, make it more opaque
constexpr int ALPHA_BORDER = 30;
// Alpha of international char’s BG
constexpr int ALPHA_INTERNATIONAL = 15;


// Light = light PALETTE rather than light colour
// Color names
#define CNAME_BG_POPUP "LightYellow"
#define CNAME_LINK_POPUP_LIGHT "ForestGreen"
#define CNAME_LINK_POPUP_DARK "MediumSpringGreen"
#define CNAME_LINK_POPUP_LO "DarkGreen"
/// @todo [future, #528] style unused, but let it be for now
//#define CNAME_LINK_CODE "#0000CC"
#define CNAME_LINK_DEPRECATED_LIGHT "#CC0000"
#define CNAME_LINK_DEPRECATED_DARK  "#EF2929"
#define CNAME_LINK_OUTSIDE "#1565C0"        // Google blue 800
#define CNAME_U_COPY "#808080"
#define CNAME_U_BIGCOPY "rgba(128,128,128,45%)"
#define CNAME_U_BIGOUTSIDE "rgba(21,101,192,35%)"
#define CNAME_TRANSCRIPTION "Goldenrod"
#define CNAME_ALTNAME_LIGHT "#808080"
#define CNAME_ALTNAME_DARK  "#A0A0A0"
#define CNAME_TEXT_DEPRECATED_LIGHT CNAME_ALTNAME_LIGHT
#define CNAME_TEXT_DEPRECATED_DARK  CNAME_ALTNAME_DARK

constexpr QColor FG_DEPRECATED { 0xDD, 0x00, 0x00 };

#define STYLE_LINK2(color1,color2,place) \
            "color:" color1 "; "   \
            "text-decoration:none; "      \
            "background:qlineargradient(x1:0, y1:1, x2:0, y2:0, " \
                    "stop:0 " color2 ", stop:" place " #00000000, stop:1 #00000000);"
#define STYLE_LINK(color,place)    STYLE_LINK2(color,color,place)
// Camouflaged link
#define STYLE_LINK_CAMO(color,place)    STYLE_LINK2("palette(window-text)",color,place)

#define STYLE_POPUP_LIGHT  STYLE_LINK(CNAME_LINK_POPUP_LIGHT, "0.05")
#define STYLE_POPUP_DARK   STYLE_LINK(CNAME_LINK_POPUP_DARK,  "0.05")
#define STYLE_POPUP_LO     STYLE_LINK(CNAME_LINK_POPUP_LO, "0.04")
#define STYLE_DEPREC_LIGHT STYLE_LINK(CNAME_LINK_DEPRECATED_LIGHT, "0.04")
#define STYLE_DEPREC_DARK  STYLE_LINK(CNAME_LINK_DEPRECATED_DARK, "0.04")
#define STYLE_INET         STYLE_LINK(CNAME_LINK_OUTSIDE, "0.05")
#define STYLE_BIGINET      STYLE_LINK2(CNAME_LINK_OUTSIDE, CNAME_U_BIGOUTSIDE, "0.05")
#define STYLE_COPY         STYLE_LINK_CAMO(CNAME_U_COPY, "0.05")
#define STYLE_ALTNAME_OF(color, ratio) STYLE_LINK2(color, CNAME_U_COPY, ratio) " font-weight:bold;"
#define STYLE_ALTNAME_LIGHT STYLE_ALTNAME_OF(CNAME_ALTNAME_LIGHT, "0.04")
#define STYLE_ALTNAME_DARK  STYLE_ALTNAME_OF(CNAME_ALTNAME_DARK, "0.04")
#define STYLE_BIGCOPY      STYLE_LINK_CAMO(CNAME_U_BIGCOPY, "0.05")
//#define STYLE_CODE         STYLE_LINK(CNAME_LINK_CODE, "0.04")
#define STYLE_MISRENDER_LIGHT "color:" CNAME_LINK_DEPRECATED_LIGHT
#define STYLE_MISRENDER_DARK  "color:" CNAME_LINK_DEPRECATED_DARK
#define STYLE_QUERY        "color:" CNAME_LINK_OUTSIDE"; text-decoration:none;"

const char* stylesWiki();
const char* styleMisrender();
const char* stylePopup();

#ifdef _WIN32
    #define FACE_DEFAULT "Cambria"
    #define FACE_TOFU "Times New Roman"      ///< Need just tofu glyph
    #define FACE_CONDENSED "Arial"
    #define FACE_EMOJI "Segoe UI Emoji"
#else
    #error Unknown OS
#endif

/// Alpha for space characters
constexpr int ALPHA_SPACE = 70;
