#pragma once

#include <optional>
#include <concepts>
#include <cmath>

namespace kage {

    template <class T>
    concept Numeric = std::integral<T> || std::floating_point<T>;

    ///  U = Lesser<T> → U is T or a lesser numeric type
    ///  For float → float and all int’s
    ///  For double → double, float, all int’s
    template <class T, class U>
    concept Lesser = Numeric<T> && Numeric<U>
                     && std::is_same_v<std::common_type_t<T, U>, T>;

    using Float = float;

    template <Numeric T>
    using TypeF = std::common_type_t<T, Float>;

    template <Numeric T>
    struct Point {
        using LocalFlt = TypeF<T>;
        T x, y;
        constexpr Point() noexcept : x(0), y(0) {}
        constexpr Point(T aX, T aY) noexcept : x(aX), y(aY) {}
        LocalFlt distFrom(const Point& a) const noexcept
            { return std::hypot(LocalFlt(a.x - x), LocalFlt(a.y - y)); }
    };

    template <Numeric T>
    using PointF = Point<TypeF<T>>;

    ///
    /// @return  Cross point of INFINITE lines
    ///
    template <Numeric T>
    std::optional<PointF<T>> getCrossPoint(
            Point<T> p11, Point<T> p12, Point<T> p21, Point<T> p22) noexcept;

    ///
    /// @return  Cross point of INFINITE lines
    ///
    template <Numeric T>
    bool isCross(Point<T> p11, Point<T> p12, Point<T> p21, Point<T> p22) noexcept;

    template <Numeric T>
    bool isCrossBox(Point<T> p1, Point<T> p2, Point<T> bp1, Point<T> bp2) noexcept;

}   // namespace kage


extern template std::optional<kage::PointF<int>> kage::getCrossPoint(Point<int> p11, Point<int> p12, Point<int> p21, Point<int> p22);
extern template std::optional<kage::PointF<float>> kage::getCrossPoint(Point<float> p11, Point<float> p12, Point<float> p21, Point<float> p22);

extern template bool kage::isCross<int>(Point<int> p11, Point<int> p12, Point<int> p21, Point<int> p22);
extern template bool kage::isCross<float>(Point<float> p11, Point<float> p12, Point<float> p21, Point<float> p22);

extern template bool kage::isCrossBox(Point<int> p1, Point<int> p2, Point<int> bp1, Point<int> bp2);
extern template bool kage::isCrossBox(Point<float> p1, Point<float> p2, Point<float> bp1, Point<float> bp2);
