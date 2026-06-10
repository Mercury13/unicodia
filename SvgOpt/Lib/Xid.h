#ifndef XACT
    #error "XACT(id, string, bits) must be defined"
#endif

#include "Xbit.h"

// C
XACT( CLIP_PATH, "clip-path", xs::IdBit::COMBO_DRAW )
// D
XACT( DESC, "desc", xs::IdBit::COMBO_META )
// F
XACT( FILL,         "fill",         xs::IdBit::FILL )
XACT( FILL_OPACITY, "fill-opacity", xs::IdBit::FILL )
XACT( FILL_RULE,    "fill-rule",    xs::IdBit::FILL )
// M
XACT( METADATA,     "metadata",     xs::IdBit::COMBO_META )
// S
XACT( STOP_COLOR,   "stop-color",   xs::IdBit::STOP )
XACT( STOP_OPACITY, "stop-opacity", xs::IdBit::STOP )
XACT( STYLE,        "style",        xs::IdBit::SPECIAL )
// T
XACT( TITLE,        "title",        xs::IdBit::COMBO_META )

#undef XACT
