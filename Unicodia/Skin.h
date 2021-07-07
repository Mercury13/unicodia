#pragma once

// Colors, sizes etc that are NOT derived from system

// STL
#include "string_view"

// Color names
#define CNAME_BG_POPUP "LightYellow"
#define CNAME_LINK_POPUP "ForestGreen"

#ifdef _WIN32
    constexpr auto FAMILY_DEFAULT = std::string_view("Cambria");
#else
    #error Unknown OS
#endif
