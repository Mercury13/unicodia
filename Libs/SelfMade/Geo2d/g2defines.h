#pragma once

// C++
#include <cmath>

// STL
#include <type_traits>

namespace g2 {

    namespace detail {
        template <class To, class From>
        constexpr inline To cast_int(From x) {
            static_assert(std::is_integral_v<From>);
            if constexpr (std::is_integral_v<To>) {
                if constexpr (std::numeric_limits<From>::min() < std::numeric_limits<To>::min()) {
                    if (x < std::numeric_limits<To>::min())
                        return std::numeric_limits<To>::min();
                }
                if constexpr (std::numeric_limits<From>::max() > std::numeric_limits<To>::max()) {
                    if (x > std::numeric_limits<To>::max())
                        return std::numeric_limits<To>::max();
                }
            }
            return static_cast<To>(x);
        }
    }   // namespace detail

    template <class To> constexpr inline To cast(char x)              { return detail::cast_int<To, decltype(x)>(x); }
    template <class To> constexpr inline To cast(signed char x)       { return detail::cast_int<To, decltype(x)>(x); }
    template <class To> constexpr inline To cast(unsigned char x)     { return detail::cast_int<To, decltype(x)>(x); }
    template <class To> constexpr inline To cast(signed short x)      { return detail::cast_int<To, decltype(x)>(x); }
    template <class To> constexpr inline To cast(unsigned short x)    { return detail::cast_int<To, decltype(x)>(x); }
    template <class To> constexpr inline To cast(signed int x)        { return detail::cast_int<To, decltype(x)>(x); }
    template <class To> constexpr inline To cast(unsigned int x)      { return detail::cast_int<To, decltype(x)>(x); }
    template <class To> constexpr inline To cast(signed long x)       { return detail::cast_int<To, decltype(x)>(x); }
    template <class To> constexpr inline To cast(unsigned long x)     { return detail::cast_int<To, decltype(x)>(x); }
    template <class To> constexpr inline To cast(signed long long x)  { return detail::cast_int<To, decltype(x)>(x); }
    template <class To> constexpr inline To cast(unsigned long long x){ return detail::cast_int<To, decltype(x)>(x); }

    template <class To>
    constexpr To cast(float x) {
        if constexpr (std::is_integral_v<To>) {
            if (x > std::numeric_limits<To>::max())
                return std::numeric_limits<To>::max();
            if (x < std::numeric_limits<To>::min())
                return std::numeric_limits<To>::min();
            if constexpr (sizeof(To) > sizeof(long)) {
                return std::llroundf(x);
            } else {
                return std::lroundf(x);
            }
        } else {
            return static_cast<To>(x);
        }
    }

    template <class To>
    constexpr inline To cast(double x) {
        if constexpr (std::is_integral_v<To>) {
            if (x > std::numeric_limits<To>::max())
                return std::numeric_limits<To>::max();
            if (x < std::numeric_limits<To>::min())
                return std::numeric_limits<To>::min();
            if constexpr (sizeof(To) > sizeof(long)) {
                return std::llround(x);
            } else {
                return std::lround(x);
            }
        } else {
            return static_cast<To>(x);
        }
    }

    template <class To>
    constexpr inline To cast(long double x) {
        if constexpr (std::is_integral_v<To>) {
            if (x > std::numeric_limits<To>::max())
                return std::numeric_limits<To>::max();
            if (x < std::numeric_limits<To>::min())
                return std::numeric_limits<To>::min();
            if constexpr (sizeof(To) > sizeof(long)) {
                return std::llroundl(x);
            } else {
                return std::lroundl(x);
            }
        } else {
            return static_cast<To>(x);
        }
    }

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
            { return std::sqrt(cast<U>(radius2())); }
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
            { return std::sqrt(g2::cast<U>(dist2from(b))); }
        constexpr float distFromF(const Point<T>& b) const noexcept { return distFrom<float>(b); }
        constexpr double distFromD(const Point<T>& b) const noexcept { return distFrom<double>(b); }

        // Point == point
        constexpr bool operator == (const Point<T>& b) const { return (x == b.x && y == b.y); }

        // Point != point
        constexpr bool operator != (const Point<T>& b) const { return (x != b.x || y != b.y); }

        // Point == origin
        constexpr bool operator == (Origin) const { return (x == 0 && y == 0); }

        // Point != origin
        constexpr bool operator != (const Origin) const { return (x != 0 || y != 0); }

        template <class U>
        constexpr Point<U> cast() const noexcept
            { return { cast<U>(x), cast<U>(y) }; }
    };

    template <class T>
    constexpr T sqr(T x) noexcept { return x * x; }

    ///
    ///  Vector in 2D space
    ///
    template <class T>
    struct Vec {
        static_assert(std::is_arithmetic_v<T>);
        T x = 0, y = 0;
        using Type = T;
        template <class U>
        using CommonType = std::common_type_t<T, U>;

        constexpr Vec() noexcept = default;
        constexpr Vec(ZeroVec) noexcept {}
        constexpr Vec(T aX, T aY) noexcept : x(aX), y(aY) {}

        // 2D cross product = oriented parallelogram area = |a|·|b|·sin α
        constexpr T cross(const Vec<T>& b) const noexcept
            { return (x * b.y) - (y * b.x); }
        template <class U> constexpr U crossT(const Vec<T>& b) const noexcept
            { return (CommonType<U>(x) * b.y) - (CommonType<U>(y) * b.x); }
        constexpr long long crossL(const Vec<T>& b) const noexcept { return crossT<long long>(b); }
        constexpr float crossF(const Vec<T>& b) const noexcept { return crossT<float>(b); }
        constexpr double crossD(const Vec<T>& b) const noexcept { return crossT<double>(b); }

        // Dot product = |a|·|b|·cos α
        constexpr T dot(const Vec<T>& b) const noexcept
            { return (x * b.x) + (y * b.y); }
        template <class U> constexpr U dotT(const Vec<T>& b) const noexcept
            { return (CommonType<U>(x) * b.x) + (CommonType<U>(y) * b.y); }
        constexpr long long dotL(const Vec<T>& b) const noexcept { return dotT<long long>(b); }
        constexpr float dotF(const Vec<T>& b) const noexcept { return dotT<float>(b); }
        constexpr double dotD(const Vec<T>& b) const noexcept { return dotT<double>(b); }

        // |a|², squared length
        constexpr T len2() const noexcept { return x*x + y*y; }
        template <class U>
        constexpr U len2T() const noexcept
            { return sqr<CommonType<U>>(x) + sqr<CommonType<U>>(y); }
        constexpr long long len2L() const noexcept { return len2T<long long>(); }
        constexpr float len2F() const noexcept { return len2T<float>(); }
        constexpr double len2D() const noexcept { return len2T<double>(); }
        // |a|, length
        template <class U>
        constexpr U len() const noexcept { return std::sqrt(len2T<U>()); }
        constexpr float lenF() const noexcept { return len<float>(); }
        constexpr double lenD() const noexcept { return len<double>(); }

        // Vec == vec
        constexpr bool operator == (const Vec<T>& b) const noexcept { return (x == b.x && y == b.y); }

        // Vec != vec
        constexpr bool operator != (const Vec<T>& b) const noexcept { return (x != b.x || y != b.y); }

        // Vec == vec
        constexpr bool operator == (ZeroVec) const noexcept { return (x == 0 && y == 0); }

        // Vec != vec
        constexpr bool operator != (ZeroVec) const noexcept { return (x != 0 || y != 0); }

        template <class U>
        constexpr Vec<U> normalized(U wantedLength = 1) const noexcept;

        template <class U>
        constexpr static Vec<T> fromPt(const Point<U>& a) noexcept
            { return { g2::cast<T>(a.x), g2::cast<T>(a.y) }; }

        template <class U>
        constexpr Point<U> toPt() const noexcept
            { return { g2::cast<U>(x), g2::cast<U>(y) }; }

        template <class U>
        constexpr Vec<U> cast() const noexcept
            { return { g2::cast<U>(x), g2::cast<U>(y) }; }

        constexpr explicit operator bool() const noexcept
            { return x != 0 || y != 0; }
    };

    using Ipoint = Point<int>;
    using Ivec = Vec<int>;
    using Dpoint = Point<double>;
    using Dvec = Vec<double>;

    /// @return cosine of angle A-B-C, or NaN if cannot compute
    template <class T>
    constexpr double cosABC(
            const Point<T>& a, const Point<T>& b, const Point<T>& c) noexcept;

    /// @return cross product of oa×ob
    template <class U, class T>
    constexpr U crossOAB(const Point<T>& o, const Point<T>& a, const Point<T>& b) noexcept;

    /// @return cross product of oa×ob
    template <class T>
    constexpr inline T crossOAB(const Point<T>& o, const Point<T>& a, const Point<T>& b) noexcept;

    /// @return [+] seg AB intersects seg CD
    template <class T>
    bool doSegsIntersect(const Point<T>& a, const Point<T>& b, const Point<T>& c, const Point<T>& d) noexcept;

    /// @return [+] seg AB strictly intersects seg CD (does not touch)
    template <class T>
    bool doSegsStrictlyIntersect(const Point<T>& a, const Point<T>& b, const Point<T>& c, const Point<T>& d) noexcept;
}   // namespace g2

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
        return Vec<U>{ g2::cast<U>(x * q), g2::cast<U>(y * q) };
    }
}

template <class T>
constexpr double g2::cosABC(const Point<T>& a, const Point<T>& b, const Point<T>& c) noexcept {
    auto v1 = a - b;
    auto v2 = c - b;
    return v1.dotD(v2) / sqrt(v1.len2D() * v2.len2D());
}

template <class U, class T>
constexpr U g2::crossOAB(const g2::Point<T>& o, const g2::Point<T>& a, const g2::Point<T>& b) noexcept
    { return (a - o).template crossT<U>(b - o); }

template <class T>
constexpr inline T g2::crossOAB(const g2::Point<T>& o, const g2::Point<T>& a, const g2::Point<T>& b) noexcept
    { return crossOAB<T, T>(o, a, b); }

// Simple unit tests
static_assert(g2::crossOAB(g2::Ipoint{0,0}, g2::Ipoint{1,0}, g2::Ipoint{1,1}) == 1);
static_assert(g2::crossOAB(g2::Ipoint{0,0}, g2::Ipoint{1,1}, g2::Ipoint{1,0}) == -1);
static_assert(g2::crossOAB<long>(g2::Ipoint{0,0}, g2::Ipoint{1,0}, g2::Ipoint{1,1}) == 1);
static_assert(g2::crossOAB<long>(g2::Ipoint{0,0}, g2::Ipoint{1,1}, g2::Ipoint{1,0}) == -1);

namespace g2::detail {
    template <class T>
    bool on_segment(const g2::Point<T>&p1, const g2::Point<T>&p2, const g2::Point<T>&p)
    {
        return std::min(p1.x, p2.x) <= p.x
            && p.x <= std::max(p1.x, p2.x)
            && std::min(p1.y, p2.y) <= p.y
            && p.y <= std::max(p1.y, p2.y);
    }
}

template <class T>
bool g2::doSegsIntersect(const g2::Point<T>& a, const g2::Point<T>& b, const g2::Point<T>& c, const g2::Point<T>& d) noexcept
{
    auto d1 = crossOAB(c, d, a);
    auto d2 = crossOAB(c, d, b);
    auto d3 = crossOAB(a, b, c);
    auto d4 = crossOAB(a, b, d);

    return (((d1 > 0 && d2 < 0) || (d1 < 0 && d2 > 0)) && ((d3 > 0 && d4 < 0) || (d3 < 0 && d4 > 0)))
        || (d1 == 0 && detail::on_segment(c, d, a))
        || (d2 == 0 && detail::on_segment(c, d, b))
        || (d3 == 0 && detail::on_segment(a, b, c))
        || (d3 == 0 && detail::on_segment(a, b, d));
}

template <class T>
bool g2::doSegsStrictlyIntersect(const g2::Point<T>& a, const g2::Point<T>& b, const g2::Point<T>& c, const g2::Point<T>& d) noexcept
{
    auto d1 = crossOAB(c, d, a);
    auto d2 = crossOAB(c, d, b);
    auto d3 = crossOAB(a, b, c);
    auto d4 = crossOAB(a, b, d);

    return (((d1 > 0 && d2 < 0) || (d1 < 0 && d2 > 0)) && ((d3 > 0 && d4 < 0) || (d3 < 0 && d4 > 0)));
}
