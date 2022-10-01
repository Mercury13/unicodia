#pragma once

// C++
#include <cmath>

// STL
#include <type_traits>

namespace g2 {

    enum class Origin { INST };
    constexpr Origin ORIGIN = Origin::INST;

    ///
    ///  Point in 2D space
    ///
    template <class T>
    struct Point {
        static_assert(std::is_arithmetic_v<T>);
        T x = 0, y = 0;
        constexpr Point() noexcept = default;
        constexpr Point(Origin) noexcept {}
        constexpr Point(T aX, T aY) noexcept : x(aX), y(aY) {}

        // squared radius
        constexpr T radius2() const noexcept
            { return x*x + y*y; }
        // |a|, length
        template <class U>
        constexpr float radius() const noexcept
            { return std::sqrt(static_cast<U>(radius2())); }
        constexpr float radiusF() const noexcept { return radius<float>(); }
        constexpr float radiusD() const noexcept { return radius<double>(); }

        // Point == point
        constexpr bool operator == (const Point<T>& b) const { return (x == b.x && y == b.y); }

        // Point != point
        constexpr bool operator != (const Point<T>& b) const { return (x != b.x || y != b.y); }
    };

    ///
    ///  Vector in 2D space
    ///
    template <class T>
    struct Vec {
        static_assert(std::is_arithmetic_v<T>);
        T x = 0, y = 0;

        constexpr Vec() noexcept = default;
        constexpr Vec(T aX, T aY) noexcept : x(aX), y(aY) {}

        // 2D cross product = oriented parallelogram area = |a|·|b|·sin α
        constexpr T cross(const Vec& b) const noexcept
            { return (x * b.y) - (y * b.x); }
        // Dot product = |a|·|b|·cos α
        constexpr T dot(const Vec& b) const noexcept
            { return (x * b.x) + (y * b.y); }
        // |a|², squared length
        constexpr T len2() const noexcept
            { return x*x + y*y; }
        // |a|, length
        template <class U>
        constexpr float len() const noexcept
            { return std::sqrt(static_cast<U>(len2())); }
        constexpr float lenF() const noexcept { return len<float>(); }
        constexpr float lenD() const noexcept { return len<double>(); }

        // Vec == vec
        constexpr bool operator == (const Vec<T>& b) const { return (x == b.x && y == b.y); }

        // Vec != vec
        constexpr bool operator != (const Vec<T>& b) const { return (x != b.x || y != b.y); }
    };

    using Ipoint = Point<int>;
    using Ivec = Vec<int>;
}

// Point + point → cannot
template <class T>
constexpr g2::Vec<T> operator + (const g2::Point<T>& a, const g2::Point<T>& b) = delete;

// Point − point → vec
template <class T>
constexpr g2::Vec<T> operator - (const g2::Point<T>& a, const g2::Point<T>& b)
    { return { a.x - b.x, a.y - b.y }; }

// Point − point0 → vec
template <class T>
constexpr g2::Vec<T> operator - (const g2::Point<T>& a, g2::Origin)
    { return { a.x, a.y }; }

// Point ± vec → point
template <class T>
constexpr g2::Point<T> operator + (const g2::Point<T>& a, const g2::Vec<T>& b)
    { return { a.x + b.x, a.y + b.y }; }

template <class T>
constexpr g2::Point<T> operator + (const g2::Vec<T>& a, const g2::Point<T>& b)
    { return { a.x + b.x, a.y + b.y }; }

template <class T>
constexpr g2::Point<T> operator - (const g2::Point<T>& a, const g2::Vec<T>& b)
    { return { a.x - b.x, a.y - b.y }; }

template <class T>
constexpr g2::Point<T> operator - (const g2::Vec<T>& a, const g2::Point<T>& b)
    { return { a.x - b.x, a.y - b.y }; }

// Point0 ± vec → point
template <class T>
constexpr g2::Point<T> operator + (g2::Origin, const g2::Vec<T>& b)
    { return { b.x, b.y }; }

template <class T>
constexpr g2::Point<T> operator + (const g2::Vec<T>& a, g2::Origin)
    { return { a.x, a.y }; }

template <class T>
constexpr g2::Point<T> operator - (g2::Origin, const g2::Vec<T>& b)
    { return { - b.x, - b.y }; }

template <class T>
constexpr g2::Point<T> operator - (const g2::Vec<T>& a, g2::Origin)
    { return { a.x, a.y }; }

// Vec ± vec → vec
template <class T>
constexpr g2::Vec<T> operator + (const g2::Vec<T>& a, const g2::Vec<T>& b)
    { return { a.x + b.x, a.y + b.y }; }

template <class T>
constexpr g2::Vec<T> operator - (const g2::Vec<T>& a, const g2::Vec<T>& b)
    { return { a.x - b.x, a.y - b.y }; }

// ±vec → vec
template <class T>
constexpr g2::Vec<T> operator + (const g2::Vec<T>& a)
    { return a; }

template <class T>
constexpr g2::Vec<T> operator - (const g2::Vec<T>& a)
    { return { -a.x, -a.y }; }

// Vec * num → vec
template <class T>
constexpr g2::Vec<T> operator * (const g2::Vec<T>& a, T b)
    { return { a.x * b, a.y * b }; }

template <class T>
constexpr g2::Vec<T> operator - (T a, const g2::Vec<T>& b)
    { return { b.x * a, b.y * a }; }

// Vec / num → vec
template <class T>
constexpr g2::Vec<T> operator / (const g2::Vec<T>& a, T b)
    { return { a.x / b, a.y / b }; }
