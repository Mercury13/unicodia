#include "2dstroke.h"

std::optional<kage::Ends> kage::Stroke::ends() const noexcept
{
    switch(type) {
    case 0:
    case 8:
    case 9:
        return std::nullopt;
    case 6:
    case 7:
        return Ends { p7_8(), p9_10() };
    case 2:
    case 12:
    case 3:
    case 4:
        return Ends { p5_6(), p7_8() };
    default:
        return Ends { p3_4(), p5_6() };
    }
}


bool kage::isCrossBoxWithOthers(
        std::span<Stroke> strokesArray,
        unsigned i, Point<int> b1, Point<int> b2)
{
    for (unsigned j = 0; j < strokesArray.size(); ++j) {
        if (i == j) { continue; }
        auto ends = strokesArray[j].ends();
        if (!ends)
            continue;
        if (isCrossBox(ends->start, ends->end, b1, b2)) { return true; }
    }
    return false;
}


bool kage::isCrossWithOthers(
        std::span<Stroke> strokesArray,
        unsigned i, Point<int> b1, Point<int> b2)
{
    for (unsigned j = 0; j < strokesArray.size(); ++j) {
        if (i == j) { continue; }
        auto ends = strokesArray[j].ends();
        if (!ends)
            continue;
        if (isCross(ends->start, ends->end, b1, b2)) { return true; }
    }
    return false;
}
