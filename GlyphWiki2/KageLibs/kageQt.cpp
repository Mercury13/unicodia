#include "kageQt.h"


void kage::draw(const GlyphSets& sets, QPaintDevice& target,
                MaybeInt<int> a1, MaybeInt<int> a2, MaybeInt<int> a3,
                MaybeInt<int> x1, MaybeInt<int> y1,
                MaybeInt<int> x2, MaybeInt<int> y2,
                MaybeInt<int> x3, MaybeInt<int> y3,
                MaybeInt<int> x4, MaybeInt<int> y4)
{

}


void kage::draw(const Glyph& glyph, const GlyphSets& sets, QPaintDevice& target)
{
    for (auto v : glyph.lines) {
        draw(sets, target,
             v[0], v[1], v[2], v[3], v[4], v[5], v[6],
             v[7], v[8], v[9], v[10]);
    }
}
