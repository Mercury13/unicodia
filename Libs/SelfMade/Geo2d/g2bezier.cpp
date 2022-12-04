#include "g2bezier.h"

#include <algorithm>

///// Quad /////////////////////////////////////////////////////////////////////

double g2bz::Quad::fastDistFrom(const g2::Dpoint& p) const
{
    auto i = a - b;
    auto j = b - m;
    auto k = m - a;
    auto w = j - k;

    auto v0 = a - p;
    auto v1 = m - p;
    auto v2 = b - p;

    auto x = v0.cross(v2);
    auto y = v1.cross(v0);
    auto z = v2.cross(v1);

    auto s = 2.0*(y*j+z*k) - x*i;

    double r = (y*z-x*x*0.25)/s.len2();
    double t = std::clamp(0.5*x+y+r*(s.dot(w))/(x+y+z), 0.0, 1.0);

    return (v0+t*(k+k+t*w)).lenD();
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
