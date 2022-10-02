#pragma once

// C++
#include <cmath>

// STL
#include <type_traits>

namespace g2 {

    enum class Origin { INST };
    constexpr Origin ORIGIN = Origin::INST;

    enum class ZeroVec { INST };
    constexpr ZeroVec ZEROVEC = ZeroVec::INST;

    ///
    ///  Point in 2D space
    ///
    template <class T>
    struct Point {
        static_assert(std::is_arithmetic_v<T>);
        T x = 0, y = 0;
        using Type = T;

        constexpr Point() noexcept = default;
        constexpr Point(Origin) noexcept {}
        constexpr Point(T aX, T aY) noexcept : x(aX), y(aY) {}

        // squared radius
        constexpr T radius2() const noexcept
            { return x*x + y*y; }
        // |a|, length
        template <class U>
        constexpr U radius() const noexcept
            { return std::sqrt(static_cast<U>(radius2())); }
        constexpr float radiusF() const noexcept { return radius<float>(); }
        constexpr float radiusD() const noexcept { return radius<double>(); }

        // squared distance
        constexpr T dist2from(const Point<T>& b) const noexcept
        {
            const auto dx = x - b.x;
            const auto dy = y - b.y;
            return dx*dx + dy*dy;
        }
        template <class U>
        constexpr U distFrom(const Point<T>& b) const noexcept
            { return std::sqrt(static_cast<U>(dist2from(b))); }
        constexpr float distFromF(const Point<T>& b) const noexcept { return distFrom<float>(b); }
        constexpr double distFromD(const Point<T>& b) const noexcept { return distFrom<double>(b); }

        // Point == point
        constexpr bool operator == (const Point<T>& b) const { return (x == b.x && y == b.y); }

        // Point != point
        constexpr bool operator != (const Point<T>& b) const { return (x != b.x || y != b.y); }

        template <class U>
        constexpr Point<U> cast() const noexcept
            { return { static_cast<U>(x), static_cast<U>(y) }; }
    };

    ///
    ///  Vector in 2D space
    ///
    template <class T>
    struct Vec {
        static_assert(std::is_arithmetic_v<T>);
        T x = 0, y = 0;
        using Type = T;

        constexpr Vec() noexcept = default;
        constexpr Vec(ZeroVec) noexcept {}
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
        constexpr U len() const noexcept
            { return std::sqrt(static_cast<U>(len2())); }
        constexpr float lenF() const noexcept { return len<float>(); }
        constexpr double lenD() const noexcept { return len<double>(); }

        // Vec == vec
        constexpr bool operator == (const Vec<T>& b) const noexcept { return (x == b.x && y == b.y); }

        // Vec != vec
        constexpr bool operator != (const Vec<T>& b) const noexcept { return (x != b.x || y != b.y); }

        template <class U>
        constexpr Vec<U> normalized(U wantedLength = 1) const noexcept;

        template <class U>
        constexpr static Vec<T> fromPt(const Point<U>& a) noexcept
            { return { static_cast<T>(a.x), static_cast<T>(a.y) }; }

        template <class U>
        constexpr Point<U> toPt() const noexcept
            { return { static_cast<U>(x), static_cast<U>(y) }; }

        template <class U>
        constexpr Vec<U> cast() const noexcept
            { return { static_cast<U>(x), static_cast<U>(y) }; }

        constexpr explicit operator bool() const noexcept
            { return x != 0 || y != 0; }
    };

    using Ipoint = Point<int>;
    using Ivec = Vec<int>;
    using Dpoint = Point<double>;
    using Dvec = Vec<double>;
}

// Point + point → cannot
template <class T>
constexpr g2::Vec<T> operator + (const g2::Point<T>& a, const g2::Point<T>& b) = delete;

// Point − point → vec
template <class T>
constexpr g2::Vec<T> operator - (const g2::Point<T>& a, const g2::Point<T>& b) noexcept
    { return { a.x - b.x, a.y - b.y }; }

// Point − point0 → vec
template <class T>
constexpr g2::Vec<T> operator - (const g2::Point<T>& a, g2::Origin) noexcept
    { return { a.x, a.y }; }

// Point ± vec → point
template <class T>
constexpr g2::Point<T> operator + (const g2::Point<T>& a, const g2::Vec<T>& b) noexcept
    { return { a.x + b.x, a.y + b.y }; }

template <class T>
constexpr g2::Point<T> operator + (const g2::Vec<T>& a, const g2::Point<T>& b) noexcept
    { return { a.x + b.x, a.y + b.y }; }

template <class T>
constexpr g2::Point<T> operator - (const g2::Point<T>& a, const g2::Vec<T>& b) noexcept
    { return { a.x - b.x, a.y - b.y }; }

template <class T>
constexpr g2::Point<T> operator - (const g2::Vec<T>& a, const g2::Point<T>& b) noexcept
    { return { a.x - b.x, a.y - b.y }; }

// Point0 ± vec → point
template <class T>
constexpr g2::Point<T> operator + (g2::Origin, const g2::Vec<T>& b)
    { return { b.x, b.y }; }

template <class T>
constexpr g2::Point<T> operator + (const g2::Vec<T>& a, g2::Origin) noexcept
    { return { a.x, a.y }; }

template <class T>
constexpr g2::Point<T> operator - (g2::Origin, const g2::Vec<T>& b) noexcept
    { return { - b.x, - b.y }; }

template <class T>
constexpr g2::Point<T> operator - (const g2::Vec<T>& a, g2::Origin) noexcept
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
constexpr g2::Vec<T> operator + (const g2::Vec<T>& a) noexcept
    { return a; }

template <class T>
constexpr g2::Vec<T> operator - (const g2::Vec<T>& a) noexcept
    { return { -a.x, -a.y }; }

// Vec * num → vec
template <class T>
constexpr g2::Vec<T> operator * (const g2::Vec<T>& a, T b) noexcept
    { return { a.x * b, a.y * b }; }

template <class T>
constexpr g2::Vec<T> operator - (T a, const g2::Vec<T>& b) noexcept
    { return { b.x * a, b.y * a }; }

// Vec / num → vec
template <class T>
constexpr g2::Vec<T> operator / (const g2::Vec<T>& a, T b) noexcept
    { return { a.x / b, a.y / b }; }

template <class T> template <class U>
constexpr g2::Vec<U> g2::Vec<T>::normalized(U wantedLength) const noexcept
{
    auto myLen = len<U>();
    if (myLen <= 0) {
        return { 0, 0 };
    } else {
        auto q = wantedLength / myLen;
        return Vec<U>{ static_cast<U>(x * q), static_cast<U>(y * q) };
    }
}
