#pragma once

// STL
#include <string_view>

#include <QColor>

// Colors, sizes etc that are NOT derived from system

// Color names
#define CNAME_BG_POPUP "LightYellow"
#define CNAME_LINK_POPUP "ForestGreen"
#define CNAME_LINK_DEPRECATED "#CC0000"

constexpr QColor TX_DEPRECATED { 0xDD, 0x00, 0x00 };
constexpr QColor BG_CJK { 0xFF, 0xF0, 0xF5 };   // lavenderblush
constexpr QColor TX_CJK { 0x75, 0x50, 0x7b };   // Tango medium violet
//constexpr QColor TX_CJK { 0x5C, 0x35, 0x66 };   // Tango dark violet

#define LINK_SUBTAG(color, place) \
            " style='color:" color "; "   \
            "text-decoration:none; "      \
            "background:qlineargradient(x1:0, y1:1, x2:0, y2:0, " \
                    "stop:0 " color ", stop:" place " #00000000, stop:1 #00000000);'"

#define SUBTAG_POPUP \
    LINK_SUBTAG(CNAME_LINK_POPUP, "0.05")

#define SUBTAG_DEPRECATED \
    LINK_SUBTAG(CNAME_LINK_DEPRECATED, "0.04")

#define SUBTAG_MISSING \
            " style='color:" CNAME_LINK_DEPRECATED ";'"

#ifdef _WIN32
    #define FAM_DEFAULT "Cambria"
    #define FAM_CONDENSED "Arial"
    #define FAM_EMOJI "Segoe UI Emoji"
#else
    #error Unknown OS
#endif

constexpr std::string_view FNAME_NOTO = "NotoSerif-Regular.ttf";
constexpr std::string_view FNAME_NOTOMATH = "NotoSansMath-Regular.ttf";
constexpr std::string_view FNAME_NOTOSYM1 = "NotoSansSymbols-Regular.ttf";
constexpr std::string_view FNAME_NOTOSYM2 = "NotoSansSymbols2-Regular.ttf";
constexpr std::string_view FNAME_NOTOMUSIC = "NotoMusic-Regular.ttf";
constexpr std::string_view FNAME_DEJAVU = "DejaVuSerif.ttf";

/// Alpha for space characters
constexpr int ALPHA_SPACE = 60;
