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

#define CNAME_U_COPY "#808080"
#define CNAME_U_BIGCOPY "rgba(128,128,128,45%)"
#define CNAME_U_BIGOUTSIDE "rgba(21,101,192,35%)"
#define CNAME_TRANSCRIPTION "Goldenrod"

constexpr QColor FG_DEPRECATED { 0xDD, 0x00, 0x00 };

#define STYLE_LINK2(color1,color2,place) \
            "color:" color1 "; "   \
            "text-decoration:none; "      \
            "background:qlineargradient(x1:0, y1:1, x2:0, y2:0, " \
                    "stop:0 " color2 ", stop:" place " #00000000, stop:1 #00000000);"
#define STYLE_LINK(color,place)    STYLE_LINK2(color,color,place)
// Camouflaged link
#define STYLE_LINK_CAMO(color,place)    STYLE_LINK2("palette(window-text)",color,place)

#define STYLE_COPY         STYLE_LINK_CAMO(CNAME_U_COPY, "0.05")
#define STYLE_BIGCOPY      STYLE_LINK_CAMO(CNAME_U_BIGCOPY, "0.05")

const char* stylesWiki();
const char* styleMisrender();
const char* stylePopup();
const char* stylePopupLo();
const char* styleInet();
const char* styleBigInet();
const char* cnameBgPopup();
const char* cnameBorderPopup();
const char* cnameInet();
/// QColor + alpha
QColor claMessage();

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
