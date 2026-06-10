#pragma once

namespace xs {

    enum class IdBit : unsigned int {
        FILL = 1 << 8,          ///< Useful where there is fill
        STROKE = 1 << 9,        ///< Useful where there is stroke
        TEXT = 1 << 10,         ///< Useful where there is text
        META = 1 << 11,         ///< Useless, just metadata
        STOP = 1 << 12,         ///< Related to gradient stop
        GRADIENT = 1 << 13,     ///< Related to gradient
        CURRENT_COLOR = 1 << 14, ///< Related to currentColor
        INTERACTION = 1 << 15,  ///< Related to interaction
        IMAGE = 1 << 16,        ///< Related to images
        RENDERING_HINT = 1<< 26, ///< Rendering hint
        NOT_INHERITED = 1<< 27, ///< Is not inherited
        ATTR_ONLY = 1 << 28,    ///< Only attribute, forbidden in style
        SPECIAL = 1 << 29,      ///< Special cases
        FREE_STRING = 1 << 30,  ///< The type is free unparsed string
        COMBO_META = META | ATTR_ONLY | FREE_STRING,
        COMBO_DRAW = FILL | STROKE | TEXT,
        COMBO_ID_CLIP_PATH = COMBO_DRAW | NOT_INHERITED,
        COMBO_ALL = COMBO_DRAW | GRADIENT | STOP,
        COMBO_ID_CURSOR = COMBO_ALL | INTERACTION,
        COMBO_ID_IMAGE_RENDER = IMAGE | RENDERING_HINT,
    };

}   // namespace xs

#define XACT(x, y, z)
