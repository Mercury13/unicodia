#include "kageQt.h"

// C++
#include <cmath>

// Qt
#include <QPaintEngine>


void kage::drawSerif(QPaintEngine& target, const GlyphSets& sets,
                int a1, int a2, int a3,
                MaybePoint p1, MaybePoint p2, MaybePoint p3, MaybePoint p4)
{
}


namespace {

    g2::Fpoint applyMage(g2::Ipoint p1, g2::Ipoint p2, float rate, const kage::GlyphSets& sets)
    {
        if (p1.x == p2.x) {
            float v = (p1.y < p2.y) ? rate : -rate;
            return {
                float(p2.x),
                p2.y - sets.kMage * v };
        } else if (p1.y == p2.y) {
            float v = (p1.x < p2.x) ? 1 : -1;
            return {
                p2.x - sets.kMage * v,
                float(p2.y) };
        } else {
            auto norm = (p2 - p1).normalized(sets.kMage * rate);
            return {
                p2.x - norm.x,
                p2.y - norm.y };
        }
    }

    g2::Fpoint applyLiteMage(g2::Ipoint p2, g2::Ipoint p3, const kage::GlyphSets& sets)
    {
        if(p2.x == p3.x) {
            return {
                float(p3.x),
                p3.y - sets.kMage };
        } else if (p2.y == p3.y) {
            return {
                p3.x - sets.kMage,
                float(p3.y) };
        } else {
            auto norm = (p3 - p2).normalized(sets.kMage);
            return {
                p3.x - norm.x,
                p3.y - norm.y };
        }
    }

    std::pair<g2::Fpoint, g2::Fpoint> applyMagePair(g2::Ipoint p, const kage::GlyphSets& sets)
    {
        g2::Fpoint t1 { p.x + sets.kMage, float(p.y) };
        return { t1,
            { t1.x + sets.kMage * 0.5f,
              p.y - sets.kMage * 2.0f } };
    };

    float calcRate(g2::Ipoint p2, g2::Ipoint p3)
    {
        auto sqh = (p3 - p2).len2();
        if (sqh < 14400) { // smaller than 120 x 120
            return sqrtf(sqh) * (6.0f / 120.0f);
        }
        return 6.0f;
    }

}   // anon namespace


void kage::drawSansLine(QPaintEngine& target,
                  const kage::GlyphSets& sets,
                  g2::Fpoint tt1, const g2::Fpoint tt2,
                  int ta1, int ta2)
{
    if (tt1.x == tt2.x){ //if TATE stroke, use y-axis
        g2::Fpoint p1, p2;
        int a1, a2;

        if (tt1.y  > tt2.y) {
            p1 = tt2;
            p2 = tt1;
            a1 = ta2;
            a2 = ta1;
        } else  {
            p1 = tt1;
            p2 = tt2;
            a1 = ta1;
            a2 = ta2;
        }

        if (a1 % 10 == 2) { p1.y -= sets.kWidth; }
        if (a2 % 10 == 2) { p2.y += sets.kWidth; }
        if (a1 % 10 == 3) { p1.y -= sets.kWidth * sets.kKakato; }
        if (a2 % 10 == 3) { p2.y += sets.kWidth * sets.kKakato; }

        QPointF poly[4] {
            { p1.x - sets.kWidth, p1.y },
            { p2.x - sets.kWidth, p2.y },
            { p2.x + sets.kWidth, p2.y },
            { p1.x + sets.kWidth, p1.y },
        };
        target.drawPolygon(poly, 4, QPaintEngine::ConvexMode);
    } else if(tt1.y == tt2.y) { //if YOKO stroke, use x-axis
        g2::Fpoint p1, p2;
        int a1, a2;

        if(tt1.x > tt2.x) {
            p1 = tt2;
            p2 = tt1;
            a1 = ta2;
            a2 = ta1;
        } else {
            p1 = tt1;
            p2 = tt2;
            a1 = ta1;
            a2 = ta2;
        }
        if (a1 % 10 == 2) { p1.x -= sets.kWidth; }
        if (a2 % 10 == 2) { p2.x += sets.kWidth; }
        if (a1 % 10 == 3) { p1.x -= sets.kWidth * sets.kKakato; }
        if (a2 % 10 == 3) { p2.x += sets.kWidth * sets.kKakato; }

        QPointF poly[4] {
            { p1.x, p1.y - sets.kWidth },
            { p2.x, p2.y - sets.kWidth },
            { p2.x, p2.y + sets.kWidth },
            { p1.x, p1.y + sets.kWidth },
        };
        target.drawPolygon(poly, 4, QPaintEngine::ConvexMode);
    } else { //for others, use x-axis
        g2::Fpoint p1, p2;
        int a1, a2;

        if(tt1.x > tt2.x) {
            p1 = tt2;
            p2 = tt1;
            a1 = ta2;
            a2 = ta1;
        } else {
            p1 = tt1;
            p2 = tt2;
            a1 = ta1;
            a2 = ta2;
        }
        auto normW = (p2 - p1).normalized(sets.kWidth);
        if(a1 % 10 == 2) { p1 -= normW; }
        if(a2 % 10 == 2) { p2 += normW; }
        if(a1 % 10 == 3) { p1 -= normW * sets.kKakato; }
        if(a2 % 10 == 3) { p2 += normW * sets.kKakato; }

        //SUICHOKU NO ICHI ZURASHI HA Math.sin TO Math.cos NO IREKAE + x-axis MAINASU KA
        g2::Fvec normWrot { normW.y, -normW.x };
        g2::Fpoint v1 = p1 + normWrot;
        g2::Fpoint v2 = p2 + normWrot;
        g2::Fpoint v3 = p2 - normWrot;
        g2::Fpoint v4 = p1 - normWrot;
        QPointF poly[4] {
            { v1.x, v1.y },
            { v2.x, v2.y },
            { v3.x, v3.y },
            { v4.x, v4.y },
        };
        target.drawPolygon(poly, 4, QPaintEngine::ConvexMode);
    }
}


void kage::drawSans(QPaintEngine& target, const GlyphSets& sets,
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
            drawSansLine(target, sets, p1, p2, a2, a3);
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
            drawSansLine(target, sets, p1, t1, a2, 1);
            //cdDrawCurve(kage, polygons, tx1, ty1, x2, y2, tx2, ty2, 1, 1);
            drawSansLine(target, sets, t2, t3, 1, 1);
            //cdDrawCurve(kage, polygons, tx3, ty3, x3, y3, tx4, ty4, 1, 0);
        } else {
            auto t1 = applyMage(p1, p2, 1.0f, sets);
            auto t2 = applyMage(p3, p2, 1.0f, sets);
            drawSansLine(target, sets, p1, t1, a2, 1);
            //cdDrawCurve(kage, polygons, tx1, ty1, x2, y2, tx2, ty2, 1, 1);
            drawSansLine(target, sets, t2, p3, 1, a3);
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


void kage::draw(QPaintEngine& target, const GlyphSets& sets, const Glyph& glyph)
{
    auto drawFunc = drawSans;
    switch (sets.style) {
        case Style::SERIF:
        drawFunc = drawSerif;
    case Style::SANS:
        break;
    }

    for (auto v : glyph.lines) {
        drawFunc(target, sets,
             *v[0], *v[1], *v[2],
             { v[3], v[4] }, { v[5], v[6] },
             { v[7], v[8] }, { v[9], v[10] });
    }
}
