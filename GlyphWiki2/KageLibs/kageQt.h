#pragma once

#include "kage.h"

// Libs
#include "g2defines.h"

class QPaintEngine;

namespace kage {
    struct MaybePoint {
        MaybeInt<int> x, y;

        operator g2::Ipoint () const { return { *x, *y }; }
        operator g2::Fpoint () const { return { float(*x), float(*y) }; }
        g2::Ipoint p() const { return { *x, *y }; }
    };

    void drawSerif(
                QPaintEngine& target, const GlyphSets& sets,
                int a1, int a2, int a3,
                MaybePoint p1, MaybePoint p2, MaybePoint p3, MaybePoint p4);
    void drawSansLine(
                QPaintEngine& target, const kage::GlyphSets& sets,
                g2::Fpoint tt1, const g2::Fpoint tt2,
                int ta1, int ta2);
    void drawSansBezier(
                QPaintEngine& target, const kage::GlyphSets& sets,
                g2::Fpoint p1, g2::Fpoint s1, g2::Fpoint s2, g2::Fpoint p2,
                int ta1, int ta2);
    void drawSans(
                QPaintEngine& target, const GlyphSets& sets,
                int a1, int a2, int a3,
                MaybePoint p1, MaybePoint p2, MaybePoint p3, MaybePoint p4);
    void draw(QPaintEngine& target, const GlyphSets& sets, const Glyph& glyph);

}
