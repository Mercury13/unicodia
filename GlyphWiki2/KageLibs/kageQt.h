#pragma once

#include "kage.h"

class QPaintDevice;

namespace kage {
    void draw(const GlyphSets& sets, QPaintDevice& target,
              MaybeInt<int> a1, MaybeInt<int> a2, MaybeInt<int> a3,
              MaybeInt<int> x1, MaybeInt<int> y1,
              MaybeInt<int> x2, MaybeInt<int> y2,
              MaybeInt<int> x3, MaybeInt<int> y3,
              MaybeInt<int> x4, MaybeInt<int> y4);
    void draw(const Glyph& glyph, const GlyphSets& sets,
              QPaintDevice& target);

}
