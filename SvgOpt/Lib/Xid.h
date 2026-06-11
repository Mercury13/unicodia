#ifndef XACT
    #error "XACT(id, string, bits) must be defined"
#endif

#include "Xbit.h"

// A
XACT( ACCUMULATE,     "accumulate",    xs::IdBit::COMBO_ANIMATION )
XACT( ADDITIVE,       "additive",      xs::IdBit::COMBO_ANIMATION )
XACT( AMPLITUDE,      "amplitude",     xs::IdBit::COMBO_EFFECT )
        // We will check ARIA (assistive technilogies) by name!
XACT( ATTRIBUTE_NAME, "attributeName", xs::IdBit::COMBO_ANIMATION )
XACT( AZIMUTH,        "azimuth",       xs::IdBit::COMBO_EFFECT )
// B
XACT( BASE_FREQ, "baseFrequency", xs::IdBit::COMBO_EFFECT )
XACT( BEGIN,     "begin",         xs::IdBit::COMBO_ANIMATION )
XACT( BIAS,      "bias",          xs::IdBit::COMBO_EFFECT )
XACT( BY,        "by",            xs::IdBit::COMBO_ANIMATION )
// C
XACT( CALC_MODE, "calcMode",  xs::IdBit::COMBO_ANIMATION )
XACT( CLASS,     "class",     xs::IdBit::JUST_LEAVE )
XACT( CLIP_PATH, "clip-path", xs::IdBit::COMBO_ID_CLIP_PATH )
XACT( CLIP_RULE, "clip-rule", xs::IdBit::COMBO_DRAW )  // clip-rule is inherited!
XACT( COLOR,     "color",     xs::IdBit::CURRENT_COLOR )
XACT( COLOR_INTERP, "color-interpolation", xs::IdBit::COMBO_DRAW )
XACT( COLOR_RENDER, "color-rendering", xs::IdBit::COMBO_DRAW )
XACT( CURSOR,    "cursor",    xs::IdBit::COMBO_ID_CURSOR )
// D
XACT( DESC,    "desc",    xs::IdBit::COMBO_META )
XACT( DISPLAY, "display", xs::IdBit::COMBO_DRAW )
// F
XACT( FILL,         "fill",         xs::IdBit::COMBO_VECTOR )
XACT( FILL_OPACITY, "fill-opacity", xs::IdBit::FILL )
XACT( FILL_RULE,    "fill-rule",    xs::IdBit::FILL )
XACT( FILTER,       "filter",       xs::IdBit::COMBO_DRAW )
XACT( FONT_FAMILY,  "font-family",  xs::IdBit::TEXT )
XACT( FONT_SIZE,    "font-size",    xs::IdBit::TEXT )
XACT( FONT_SIZE_ADJUST, "font-size-adjust", xs::IdBit::TEXT )
XACT( FONT_STRETCH, "font-stretch", xs::IdBit::TEXT )
XACT( FONT_STYLE,   "font-style",   xs::IdBit::TEXT )
XACT( FONT_VARIANT, "font-variant", xs::IdBit::TEXT )
XACT( FONT_WEIGHT,  "font-weight",  xs::IdBit::TEXT )
// I
XACT( IMAGE_RENDER, "image-rendering", xs::IdBit::COMBO_ID_IMAGE_RENDER )
// L
XACT( LETTER_SPACING, "letter-spacing", xs::IdBit::TEXT )
// M
XACT( MARKER_END,   "marker-end",   xs::IdBit::SHAPE_STROKE )
XACT( MARKER_MID,   "marker-mid",   xs::IdBit::SHAPE_STROKE )
XACT( MARKER_START, "marker-start", xs::IdBit::SHAPE_STROKE )
XACT( MASK,         "mask",         xs::IdBit::JUST_LEAVE )
XACT( MEDIA,        "media",        xs::IdBit::JUST_LEAVE )
XACT( METADATA,     "metadata",     xs::IdBit::COMBO_META )
// O
XACT( OPACITY,  "opacity",  xs::IdBit::COMBO_ID_OPACITY )
XACT( OVERFLO,  "overflow", xs::IdBit::JUST_LEAVE )  // OVERFLOW is some macro
// P
XACT( PAINT_ORDER,    "paint-order",    xs::IdBit::MULTIMODAL )
XACT( POINTER_EVENTS, "pointer-events", xs::IdBit::COMBO_ID_POINTER_EVENTS )
// S
XACT( SHAPE_RENDER,      "shape-rendering",   xs::IdBit::COMBO_ID_SHAPE_RENDER )
XACT( STOP_COLOR,        "stop-color",        xs::IdBit::STOP )
XACT( STOP_OPACITY,      "stop-opacity",      xs::IdBit::STOP )
XACT( STROKE,            "stroke",            xs::IdBit::COMBO_VECTOR )
XACT( STROKE_DASHARRAY,  "stroke-dasharray",  xs::IdBit::STROKE )
XACT( STROKE_DASHOFFSET, "stroke-dashoffset", xs::IdBit::STROKE )
XACT( STROKE_LINECAP,    "stroke-linecap",    xs::IdBit::OPEN_STROKE )
XACT( STROKE_LINEJOIN,   "stroke-linejoin",   xs::IdBit::SHARP_STROKE )
XACT( STROKE_MITERLIMIT, "stroke-miterlimit", xs::IdBit::SHARP_STROKE )
XACT( STROKE_OPACITY,    "stroke-opacity",    xs::IdBit::STROKE )
XACT( STROKE_WIDTH,      "stroke-width",      xs::IdBit::STROKE )
XACT( STYLE,             "style",             xs::IdBit::SPECIAL )
// T
XACT( TEXT_ANCHOR,     "text-anchor",     xs::IdBit::TEXT )
XACT( TEXT_DECORATION, "text-decoration", xs::IdBit::TEXT )
XACT( TEXT_OVERFLOW,   "text-overflow",   xs::IdBit::TEXT )
XACT( TEXT_RENDER,     "text-rendering",  xs::IdBit::COMBO_ID_TEXT_RENDER )
XACT( TITLE,           "title",           xs::IdBit::COMBO_META )
// U
XACT( UNICODE_BIDI, "unicode-bidi", xs::IdBit::SPECIAL )
// V
XACT( VECTOR_EFFECT, "vector-effect", xs::IdBit::COMBO_DRAW )
XACT( VISIBILITY,    "visibility",    xs::IdBit::COMBO_DRAW )
// W
XACT( WHITE_SPACE,  "white-space",  xs::IdBit::TEXT )
XACT( WRITING_MODE, "writing-mode", xs::IdBit::TEXT )

#undef XACT
