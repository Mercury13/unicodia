#include "g2bezier.h"

namespace {

    double calcMiddleTime(
            const g2::Dpoint& start, const g2::Dpoint& mid,
            const g2::Dpoint& end)
    {
        auto a = mid.distFromD(start);
        auto b = mid.distFromD(end);
        return a / (a + b);
    }

}   // anon namespace

g2::Dpoint g2bz::quadBy3time(
        const g2::Dpoint& start, const g2::Dpoint& mid,
        const g2::Dpoint& end, double t)
{
    double rt = 1.0 - t;
    return (mid - start.mul(rt*rt) - end.mul(t*t)).mul(0.5/(t*rt));
}

g2::Dpoint g2bz::quadBy3(
        const g2::Dpoint& start, const g2::Dpoint& mid,
        const g2::Dpoint& end)
{
    auto time = calcMiddleTime(start, mid, end);
    return quadBy3time(start, mid, end, time);
}
