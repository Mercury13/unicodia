#pragma once

namespace xs {

    enum class IdBit : unsigned int {
        FILL = 1 << 8,          ///< Useful where there is fill
        STROKE = 1 << 9,        ///< Useful where there is stroke
        TEXT = 1 << 10,         ///< Useful where there is text
        META = 1 << 11,         ///< Useless, just metadata
        STOP = 1 << 12,         ///< Related to gradient stop
        ATTR_ONLY = 1 << 28,    ///< Only attribute, forbidden in style
        SPECIAL = 1 << 29,      ///< Special cases
        FREE_STRING = 1 << 30,  ///< The type is free unparsed string
        COMBO_META = META | ATTR_ONLY | FREE_STRING,
        COMBO_DRAW = FILL | STROKE | TEXT,
    };

}   // namespace xs

#define XACT(x, y, z)
