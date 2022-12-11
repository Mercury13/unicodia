#include "g2bezier.h"

#include <algorithm>

///// Seg //////////////////////////////////////////////////////////////////////

double g2bz::Seg::dist2from(const g2::Dpoint& p)
{
    auto myVec = b - a;
    auto l2 = myVec.len2();
    if (l2 == 0)
        return p.dist2from(b);
    auto t = ((p.x - a.x) * myVec.x + (p.y - a.y) * myVec.y) / l2;
    if (t <= 0)
        return p.dist2from(a);
    if (t >= 1)
        return p.dist2from(b);
    auto q = a + myVec * t;
    return p.dist2from(q);
}

///// Quad /////////////////////////////////////////////////////////////////////

g2::Dpoint g2bz::Quad::posOf(double t) const noexcept
{
    double ct = 1.0 - t;
    return g2::Dpoint::linearCombi(ct*ct, a, 2*t*ct, m, t*t, b);
}

double g2bz::Quad::fastDist2from(const g2::Dpoint& p) const
{
    g2::Dvec b0 = a - p;
    g2::Dvec b1 = m - p;
    g2::Dvec b2 = b - p;
    double a = b0.cross(b2),
           b = 2 * b1.cross(b0),
           d = 2 * b2.cross(b1);            // 𝛼,𝛽,𝛿(𝑝)
    double f = b*d - a*a;                   // 𝑓(𝑝)
    g2::Dvec d21 = b2 - b1, d10 = b1-b0, d20 = b2-b0;
    g2::Dvec gf = 2.0*(b*d21 + d*d10 + a*d20);
    gf = g2::Dvec(gf.y,-gf.x);              // ∇𝑓(𝑝)
    g2::Dvec pp = -f*gf / gf.len2();        // 𝑝′
    g2::Dvec d0p = b0-pp;                   // 𝑝′ to origin
    double ap = d0p.cross(d20),
           bp = 2*d10.cross(d0p);           // 𝛼,𝛽(𝑝′)
    // (note that 2*ap+bp+dp=2*a+b+d=4*area(b0,b1,b2))
    double tt = (ap+bp)/(2*a+b+d);
    double t = std::clamp(tt, 0.0, 1.0); // 𝑡̅
    g2::Dvec v = g2::lerp(g2::lerp(b0,b1,t), g2::lerp(b1,b2,t), t); // 𝑣𝑖 = 𝑏(𝑡̅ )}
    return v.len2D();
}

namespace {

    double calcMiddleTime(
            const g2::Dpoint& start, const g2::Dpoint& mid,
            const g2::Dpoint& end)
    {
        auto a = mid.distFromD(start);
        auto b = mid.distFromD(end);
        return a / (a + b);
    }

    double calcMiddleTimeQ(
            const g2::Dpoint& start, const g2::Dpoint& mid,
            const g2::Dpoint& end)
    {
        auto a = mid.distFromD(start);
        auto b = mid.distFromD(end);
        auto aa = sqrt(a);
        auto bb = sqrt(b);
        return aa / (aa + bb);
    }

}   // anon namespace

g2bz::Quad g2bz::Quad::by3time(
        const g2::Dpoint& start, const g2::Dpoint& mid,
        const g2::Dpoint& end, double t)
{
    double rt = 1.0 - t;
    return {
        .a = start,
        .m = (mid - start.mul(rt*rt) - end.mul(t*t)).mul(0.5/(t*rt)),
        .b = end
    };
}

g2bz::Quad g2bz::Quad::by3(
        const g2::Dpoint& start, const g2::Dpoint& mid,
        const g2::Dpoint& end)
{
    auto time = calcMiddleTime(start, mid, end);
    return by3time(start, mid, end, time);
}

g2bz::Quad g2bz::Quad::by3q(
        const g2::Dpoint& start, const g2::Dpoint& mid,
        const g2::Dpoint& end)
{
    auto time = calcMiddleTimeQ(start, mid, end);
    return by3time(start, mid, end, time);
}

std::optional<g2bz::Quad> g2bz::Quad::by2tan(
        const g2::Dpoint& a, const g2::Dvec& ah,
        const g2::Dvec& bh, const g2::Dpoint& b)
{
    // Check for side: ah and bh should be on the same side of AB
    // +simple sanity: A≠B, ah≠0, bh≠0
    auto mainLine = b - a;
    auto crossA = mainLine.cross(ah);
    if (crossA == 0)
        return std::nullopt;
    auto crossB = mainLine.cross(bh);
    if (crossB == 0 || ((crossA < 0) ^ (crossB < 0)))
        return std::nullopt;

    // a and b should point inwards
    auto crossAB = ah.cross(bh);
    if ((crossA < 0) ^ (crossAB < 0))
        return std::nullopt;

    auto tA = crossB / crossAB;
    if (!std::isfinite(tA))
        return std::nullopt;

    auto mA = a + ah * tA;

    // Debug: mA and mB should be somewhere near
//    auto tB = crossA / crossAB;
//    if (!std::isfinite(tB))
//        return std::nullopt;

//    auto mB = b + bh * tB;

    return Quad { .a = a, .m = mA, .b = b };
}
