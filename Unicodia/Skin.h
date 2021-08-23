#pragma once

#include <QColor>

// Colors, sizes etc that are NOT derived from system

// Color names
#define CNAME_BG_POPUP "LightYellow"
#define CNAME_LINK_POPUP "ForestGreen"
#define CNAME_LINK_DEPRECATED "#CC0000"
constexpr QColor CL_DEPRECATED { 0xDD, 0x00, 0x00 };

#define LINK_SUBTAG(color, place) \
            " style='color:" color "; "   \
            "text-decoration:none; "      \
            "background:qlineargradient(x1:0, y1:1, x2:0, y2:0, " \
                    "stop:0 " color ", stop:" place " #00000000, stop:1 #00000000);'"

#define SUBTAG_POPUP \
    LINK_SUBTAG(CNAME_LINK_POPUP, "0.05")

#define SUBTAG_DEPRECATED \
    LINK_SUBTAG(CNAME_LINK_DEPRECATED, "0.05")

#ifdef _WIN32
    #define FAM_DEFAULT "Cambria"
    #define FAM_BACKUP "Noto Serif"
    #define FAM_CONDENSED "Arial"
    #define FAM_EMOJI "Segoe UI Emoji"
#else
    #error Unknown OS
#endif
