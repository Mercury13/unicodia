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
        MULTIMODAL = 1 << 17,   ///< Related to multimodal objects (fill/stroke/markers)
        SHAPE = 1 << 18,        ///< Shapes
        SHAPE_STROKE = 1 << 19, ///< Stroke + shape (for markers)
        OPEN_STROKE = 1 << 20,  ///< Open strokes (for butt)
        SHARP_STROKE = STROKE,  ///< Strokes with sharp corners
        ANIMATION = 1 << 21,    ///< Animation-related
        EFFECT = 1 << 22,       ///< Effect-related
        JUST_LEAVE = 1 << 25,   ///< Just leave
        RENDERING_HINT = 1<< 26, ///< Rendering hint
        NOT_INHERITED = 1<< 27, ///< Is not inherited
        ATTR_ONLY = 1 << 28,    ///< Only attribute, forbidden in style
        SPECIAL = 1 << 29,      ///< Special cases
        FREE_STRING = 1 << 30,  ///< The type is free unparsed string
        COMBO_META = META | ATTR_ONLY | FREE_STRING,
        COMBO_DRAW = FILL | STROKE | TEXT,
        COMBO_VECTOR = SHAPE | TEXT,
        COMBO_ID_CLIP_PATH = COMBO_DRAW | NOT_INHERITED,
        COMBO_ALL = COMBO_DRAW | GRADIENT | STOP,
        COMBO_ID_CURSOR = COMBO_ALL | INTERACTION,
        COMBO_ID_IMAGE_RENDER = IMAGE | RENDERING_HINT,
        COMBO_ID_OPACITY = COMBO_DRAW | NOT_INHERITED,
        COMBO_ID_POINTER_EVENTS = COMBO_DRAW | INTERACTION,
        COMBO_ID_SHAPE_RENDER = FILL | STROKE | RENDERING_HINT,
        COMBO_ID_TEXT_RENDER = TEXT | RENDERING_HINT,
        COMBO_ANIMATION = ANIMATION | ATTR_ONLY,
        COMBO_EFFECT = EFFECT | ATTR_ONLY,
    };

}   // namespace xs

#define XACT(x, y, z)
