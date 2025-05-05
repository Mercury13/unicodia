#include "2d.h"

// UT’s for kage::TypeF
static_assert(std::is_same_v<kage::TypeF<int>, float>);
static_assert(std::is_same_v<kage::TypeF<long long>, float>);
static_assert(std::is_same_v<kage::TypeF<float>, float>);
static_assert(std::is_same_v<kage::TypeF<double>, double>);


#define x11   p11.x
#define x12   p12.x
#define x21   p21.x
#define x22   p22.x
#define y11   p11.y
#define y12   p12.y
#define y21   p21.y
#define y22   p22.y

template <kage::Numeric T>
std::optional<kage::PointF<T>> kage::getCrossPoint(
        Point<T> p11, Point<T> p12, Point<T> p21, Point<T> p22) noexcept
{
    auto a1 = y12 - y11;
    auto b1 = x11 - x12;
    auto c1 = -a1 * x11 - b1 * y11;
    auto a2 = y22 - y21;
    auto b2 = x21 - x22;
    auto c2 = -a2 * x21 - b2 * y21;

    TypeF<T> temp = b1 * a2 - b2 * a1;
    if(temp == 0) {
        return std::nullopt;
    }
    return Point{ (c1 * b2 - c2 * b1) / temp, (a1 * c2 - a2 * c1) / temp };
}

template <kage::Numeric T>
bool kage::isCross(Point<T> p11, Point<T> p12, Point<T> p21, Point<T> p22) noexcept
{
    auto temp = getCrossPoint<T>(p11, p12, p12, p22);
    if (!temp) { return false; }
    if ((x11 < x12 && (temp->x < x11 || x12 < temp->x)) ||
        (x11 > x12 && (temp->x < x12 || x11 < temp->x)) ||
        (y11 < y12 && (temp->y < y11 || y12 < temp->y)) ||
        (y11 > y12 && (temp->y < y12 || y11 < temp->y))) {
        return false;
    }
    if ((x21 < x22 && (temp->x < x21 || x22 < temp->x)) ||
        (x21 > x22 && (temp->x < x22 || x21 < temp->x)) ||
        (y21 < y22 && (temp->y < y21 || y22 < temp->y)) ||
        (y21 > y22 && (temp->y < y22 || y21 < temp->y))) {
        return false;
    }
    return true;
}

#define bx1  bp1.x
#define bx2  bp2.x
#define by1  bp1.y
#define by2  bp2.y

template <kage::Numeric T>
bool kage::isCrossBox(Point<T> p1, Point<T> p2, Point<T> bp1, Point<T> bp2) noexcept
{
  #define corner1  p1
    Point corner2 (bx2, by1);
  #define corner3  p2
    Point corner4 (bx1, by2);

    return isCross(p1, p2, corner1, corner2)
        || isCross(p1, p2, corner2, corner3)
        || isCross(p1, p2, corner4, corner3)
        || isCross(p1, p2, corner1, corner4);
}


template std::optional<kage::PointF<int>> kage::getCrossPoint(Point<int> p11, Point<int> p12, Point<int> p21, Point<int> p22);
template std::optional<kage::PointF<float>> kage::getCrossPoint(Point<float> p11, Point<float> p12, Point<float> p21, Point<float> p22);

template bool kage::isCross<int>(Point<int> p11, Point<int> p12, Point<int> p21, Point<int> p22);
template bool kage::isCross<float>(Point<float> p11, Point<float> p12, Point<float> p21, Point<float> p22);

template bool kage::isCrossBox<int>(Point<int> p11, Point<int> p12, Point<int> p21, Point<int> p22);
template bool kage::isCrossBox<float>(Point<float> p11, Point<float> p12, Point<float> p21, Point<float> p22);
