#pragma once

// Colors, sizes etc that are NOT derived from system

// STL
#include "string_view"

// Color names
#define CNAME_BG_POPUP "LightYellow"
#define CNAME_LINK_POPUP "ForestGreen"

#ifdef _WIN32
    #define FAMILY_DEFAULT "Cambria"
    #define FAMILY_BACKUP "Noto Serif"
    #define FAMILY_CONDENSED "Arial"
#else
    #error Unknown OS
#endif
