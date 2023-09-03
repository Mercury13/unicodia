#include "kageQt.h"

#include <cmath>


void kage::drawSerif(const GlyphSets& sets, QPaintDevice& target,
                int a1, int a2, int a3,
                MaybePoint p1, MaybePoint p2, MaybePoint p3, MaybePoint p4)
{
}


namespace {

    kage::Fpoint applyMage(kage::Point p1, kage::Point p2, float rate, const kage::GlyphSets& sets)
    {
        if (p1.x == p2.x) {
            float v = (p1.y < p2.y) ? rate : -rate;
            return {
                .x = float(p2.x),
                .y = p2.y - sets.kMage * v };
        } else if (p1.y == p2.y) {
            float v = (p1.x < p2.x) ? 1 : -1;
            return {
                .x = p2.x - sets.kMage * v,
                .y = float(p2.y) };
        } else {
            /// @todo [urgent] Unneeded trigonometry here, use vectors
            float rad = atan2f(p2.y - p1.y, p2.x - p1.x);
            return {
                .x = p2.x - sets.kMage * cosf(rad) * rate,
                .y = p2.y - sets.kMage * sinf(rad) * rate };
        }
    }

    kage::Fpoint applyLiteMage(kage::Point p2, kage::Point p3, const kage::GlyphSets& sets)
    {
        if(p2.x == p3.x) {
            return {
                .x = float(p3.x),
                .y = p3.y - sets.kMage };
        } else if (p2.y == p3.y) {
            return {
                .x = p3.x - sets.kMage,
                .y = float(p3.y) };
        } else {
            /// @todo [urgent] Unneeded trigonometry here, use vectors
            float rad = atan2f(p3.y - p2.y, p3.x - p2.x);
            return {
                .x = p3.x - sets.kMage * cosf(rad),
                .y = p3.y - sets.kMage * sinf(rad) };
        }
    }

    std::pair<kage::Fpoint, kage::Fpoint> applyMagePair(kage::Point p, const kage::GlyphSets& sets)
    {
        kage::Fpoint t1 {
            .x = p.x + sets.kMage,
            .y = float(p.y) };
        return { t1,
            { .x = t1.x + sets.kMage * 0.5f,
              .y = p.y - sets.kMage * 2.0f } };
    };

    float calcRate(kage::Point p2, kage::Point p3)
    {
        auto dx = p3.x - p2.x;
        auto dy = p3.y - p2.y;
        auto sqh = dx * dx - dy * dy;
        if (sqh < 14400) { // smaller than 120 x 120
            return sqrtf(sqh) * (6.0f / 120.0f);
        }
        return 6.0f;
    }

}


void kage::drawSans(const GlyphSets& sets, QPaintDevice& target,
                int a1, int a2, int a3,
                MaybePoint p1, MaybePoint p2, MaybePoint p3, MaybePoint p4)
{
    switch(a1 % 100){
    case 0:
        /// @todo [urgent] What to do with dfTransform?
        //dfTransform(kage, polygons, x1, y1, x2, y2, a2, a3);
        break;
    case 1:
        if(a3 % 100 == 4) {
            auto t1 = applyMage(p1, p2, 1.0f, sets);
            /// @todo [urgent] What to draw?
            //cdDrawLine(kage, polygons, x1, y1, tx1, ty1, a2, 1);
            //cdDrawCurve(kage, polygons, tx1, ty1, x2, y2, x2 - kage.kMage * 2, y2 - kage.kMage * 0.5, 1, 0);
        }
        else{
            /// @todo [urgent] What to draw?
            //cdDrawLine(kage, polygons, x1, y1, x2, y2, a2, a3);
        } break;
    case 2:
    case 12:
        if (a3 % 100 == 4) {
            auto t1 = applyLiteMage(p2, p3, sets);
            /// @todo [urgent] What to draw?
            //cdDrawCurve(kage, polygons, x1, y1, x2, y2, tx1, ty1, a2, 1);
            //cdDrawCurve(kage, polygons, tx1, ty1, x3, y3, x3 - kage.kMage * 2, y3 - kage.kMage * 0.5, 1, 0);
        } else if (a3 == 5) {
            auto [t1, t2] = applyMagePair(p3, sets);
            //cdDrawCurve(kage, polygons, x1, y1, x2, y2, x3, y3, a2, 1);
            //cdDrawCurve(kage, polygons, x3, y3, tx1, ty1, tx2, ty2, 1, 0);
        } else {
            /// @todo [urgent] What to draw?
            //cdDrawCurve(kage, polygons, x1, y1, x2, y2, x3, y3, a2, a3);
        } break;
    case 3:
        if (a3 % 1000 == 5) {
            auto t1 = applyMage(p1, p2, 1.0f, sets);
            auto t2 = applyMage(p3, p2, 1.0f, sets);
            auto [t3, t4] = applyMagePair(p3, sets);
            //cdDrawLine(kage, polygons, x1, y1, tx1, ty1, a2, 1);
            //cdDrawCurve(kage, polygons, tx1, ty1, x2, y2, tx2, ty2, 1, 1);
            //cdDrawLine(kage, polygons, tx2, ty2, tx3, ty3, 1, 1);
            //cdDrawCurve(kage, polygons, tx3, ty3, x3, y3, tx4, ty4, 1, 0);
        } else {
            auto t1 = applyMage(p1, p2, 1.0f, sets);
            auto t2 = applyMage(p3, p2, 1.0f, sets);
            //cdDrawLine(kage, polygons, x1, y1, tx1, ty1, a2, 1);
            //cdDrawCurve(kage, polygons, tx1, ty1, x2, y2, tx2, ty2, 1, 1);
            //cdDrawLine(kage, polygons, tx2, ty2, x3, y3, 1, a3);
        }
        break;
    case 4: {
            float rate = calcRate(p2, p3);
            if (a3 == 5) {
                auto t1 = applyMage(p1, p2, rate, sets);
                auto t2 = applyMage(p3, p2, rate, sets);
                auto [t3, t4] = applyMagePair(p3, sets);
                //cdDrawLine(kage, polygons, x1, y1, tx1, ty1, a2, 1);
                //cdDrawCurve(kage, polygons, tx1, ty1, x2, y2, tx2, ty2, 1, 1);
                //cdDrawLine(kage, polygons, tx2, ty2, tx3, ty3, 1, 1);
                //cdDrawCurve(kage, polygons, tx3, ty3, x3, y3, tx4, ty4, 1, 0);
            } else {
                auto t1 = applyMage(p1, p2, rate, sets);
                auto t2 = applyMage(p3, p2, rate, sets);
                //cdDrawLine(kage, polygons, x1, y1, tx1, ty1, a2, 1);
                //cdDrawCurve(kage, polygons, tx1, ty1, x2, y2, tx2, ty2, 1, 1);
                //cdDrawLine(kage, polygons, tx2, ty2, x3, y3, 1, a3);
            }
        } break;
    case 6:
        if (a3 % 100 == 4){
            auto t1 = applyLiteMage(p3, p4, sets);
            //cdDrawBezier(kage, polygons, x1, y1, x2, y2, x3, y3, tx1, ty1, a2, 1);
            //cdDrawCurve(kage, polygons, tx1, ty1, x4, y4, x4 - kage.kMage * 2, y4 - kage.kMage * 0.5, 1, 0);
        } else if(a3 == 5){
            auto [t1, t2] = applyMagePair(p4, sets);
            //cdDrawBezier(kage, polygons, x1, y1, x2, y2, x3, y3, tx1, ty1, a2, 1);
            //cdDrawCurve(kage, polygons, tx1, ty1, x4, y4, tx2, ty2, 1, 0);
        } else {
            //cdDrawBezier(kage, polygons, x1, y1, x2, y2, x3, y3, x4, y4, a2, a3);
        } break;
    case 7:
        //cdDrawLine(kage, polygons, x1, y1, x2, y2, a2, 1);
        //cdDrawCurve(kage, polygons, x2, y2, x3, y3, x4, y4, 1, a3);
        break;
    case 9: // may not be exist
        break;
    default:
        break;
    }
}


void kage::draw(const Glyph& glyph, const GlyphSets& sets, QPaintDevice& target)
{
    auto drawFunc = drawSans;
    switch (sets.style) {
        case Style::SERIF:
        drawFunc = drawSerif;
    case Style::SANS:
        break;
    }

    for (auto v : glyph.lines) {
        drawFunc(sets, target,
             *v[0], *v[1], *v[2],
             { v[3], v[4] }, { v[5], v[6] },
             { v[7], v[8] }, { v[9], v[10] });
    }
}
