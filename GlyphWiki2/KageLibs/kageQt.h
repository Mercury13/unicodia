#pragma once

#include "kage.h"

class QPaintDevice;

namespace kage {
    struct Point {
        int x, y;
    };

    struct Fpoint {
        float x, y;
    };

    struct MaybePoint {
        MaybeInt<int> x, y;

        operator Point () const { return Point { *x, *y }; }
    };

    void drawSerif(const GlyphSets& sets, QPaintDevice& target,
              int a1, int a2, int a3,
              MaybePoint p1, MaybePoint p2, MaybePoint p3, MaybePoint p4);
    void drawSans(const GlyphSets& sets, QPaintDevice& target,
              int a1, int a2, int a3,
              MaybePoint p1, MaybePoint p2, MaybePoint p3, MaybePoint p4);
    void draw(const Glyph& glyph, const GlyphSets& sets,
              QPaintDevice& target);

}
