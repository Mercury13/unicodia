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


// Color names
#define CNAME_BG_POPUP "LightYellow"
#define CNAME_LINK_POPUP "ForestGreen"
#define CNAME_LINK_POPUP_DARK "DarkGreen"
/// @todo [future, #528] style unused, but let it be for now
//#define CNAME_LINK_CODE "#0000CC"
#define CNAME_LINK_DEPRECATED "#CC0000"
#define CNAME_LINK_OUTSIDE "#1565C0"        // Google blue 800
#define CNAME_U_COPY "#808080"
#define CNAME_U_BIGCOPY "rgba(128,128,128,45%)"
#define CNAME_U_BIGOUTSIDE "rgba(21,101,192,35%)"
#define CNAME_TRANSCRIPTION "Goldenrod"
#define CNAME_ALTNAME "palette(shadow)"
#define CNAME_TEXT_DEPRECATED CNAME_ALTNAME

constexpr QColor FG_DEPRECATED { 0xDD, 0x00, 0x00 };

#define STYLE_LINK2(color1,color2,place) \
            "color:" color1 "; "   \
            "text-decoration:none; "      \
            "background:qlineargradient(x1:0, y1:1, x2:0, y2:0, " \
                    "stop:0 " color2 ", stop:" place " #00000000, stop:1 #00000000);"
#define STYLE_LINK(color,place)    STYLE_LINK2(color,color,place)
// Camouflaged link
#define STYLE_LINK_CAMO(color,place)    STYLE_LINK2("palette(window-text)",color,place)

#define STYLE_POPUP        STYLE_LINK(CNAME_LINK_POPUP, "0.05")
#define STYLE_POPUP_DARK   STYLE_LINK(CNAME_LINK_POPUP_DARK, "0.04")
#define STYLE_DEPRECATED   STYLE_LINK(CNAME_LINK_DEPRECATED, "0.04")
#define STYLE_INET         STYLE_LINK(CNAME_LINK_OUTSIDE, "0.05")
#define STYLE_BIGINET      STYLE_LINK2(CNAME_LINK_OUTSIDE, CNAME_U_BIGOUTSIDE, "0.05")
#define STYLE_COPY         STYLE_LINK_CAMO(CNAME_U_COPY, "0.05")
#define STYLE_ALTNAME      STYLE_LINK2(CNAME_ALTNAME, CNAME_U_COPY, "0.05") " font-weight:bold;"
#define STYLE_BIGCOPY      STYLE_LINK_CAMO(CNAME_U_BIGCOPY, "0.05")
//#define STYLE_CODE         STYLE_LINK(CNAME_LINK_CODE, "0.04")
#define STYLE_MISRENDER    "color:" CNAME_LINK_DEPRECATED
#define STYLE_QUERY        "color:" CNAME_LINK_OUTSIDE"; text-decoration:none;"

#define STYLES_WIKI \
        ".copy { " STYLE_COPY " } "                                             \
        ".altname { " STYLE_ALTNAME " } "                                       \
        ".bigcopy { " STYLE_BIGCOPY " } "                                       \
        ".popup { " STYLE_POPUP " } "                                           \
        ".deprecated { " STYLE_DEPRECATED " } "                                 \
        ".deph { color: " CNAME_TEXT_DEPRECATED "; } "                          \
        ".inet { " STYLE_INET " } "                                             \
        ".missing { color: " CNAME_LINK_DEPRECATED "; } "                       \
        ".tr { color: " CNAME_TRANSCRIPTION "; }"                               \
        ".query { " STYLE_QUERY " }"                                            \

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
