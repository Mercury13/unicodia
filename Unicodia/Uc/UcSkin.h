#pragma once

#include <QColor>

// CJK
constexpr QColor BG_CJK { 0xFF, 0xF0, 0xF5 };     // Icon/cell BG: lavenderblush
constexpr QColor TX_CJK { 0x75, 0x50, 0x7b };     // Cell FG: Tango medium violet
constexpr QColor FG_CJK { 0x5c, 0x35, 0x66 };     // Icon FG: Tango dark violet
constexpr QColor FRAME_CJK = TX_CJK;              // Format frame — same medium violet

// Africa
constexpr QColor BG_AFRICA { 0xf5, 0xde, 0xb3 };  // Icon/cell BG: wheat
constexpr QColor FG_AFRICA {  0x8f, 0x59, 0x02 }; // Icon FG: IDK where took, very chocolate-y
constexpr QColor TX_AFRICA { 0x8f, 0x59, 0x02 };  // Cell FG: Tango dark brown

// Europe (no cells for now, but lots of synth icons)
constexpr QColor BG_EUROPE { 0x7F, 0xFF, 0xFF };  // Icon BG: some shade of aqua
