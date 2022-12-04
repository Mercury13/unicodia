#include "g2bezier.h"

///// Quad /////////////////////////////////////////////////////////////////////

double g2bz::Quad::distFrom(const g2::Dpoint& p) const
{
    /// @todo [quad, urgent] g2bz::distFrom
    return 0;
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
    // Simple sanity-check
    if (!ah || !bh || (a == b))
        return std::nullopt;
    /// @todo [urgent] by two tangents?
    return std::nullopt;
}
