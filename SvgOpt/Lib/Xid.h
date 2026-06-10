#ifndef XACT
    #error "XACT(id, string, bits) must be defined"
#endif

#include "Xbit.h"

// C
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
XACT( FILL,         "fill",         xs::IdBit::FILL )
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
XACT( IMAGE_RENDERING, "image-rendering", xs::IdBit::COMBO_ID_IMAGE_RENDER )
// M
XACT( METADATA,     "metadata",     xs::IdBit::COMBO_META )
// S
XACT( STOP_COLOR,   "stop-color",   xs::IdBit::STOP )
XACT( STOP_OPACITY, "stop-opacity", xs::IdBit::STOP )
XACT( STYLE,        "style",        xs::IdBit::SPECIAL )
// T
XACT( TITLE,        "title",        xs::IdBit::COMBO_META )

#undef XACT
