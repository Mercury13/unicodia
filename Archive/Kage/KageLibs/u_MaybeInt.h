#pragma once

// STL
#include <type_traits>
#include <limits>
#include <stdexcept>
#include <charconv>


template <class T>
struct MaybeInt
{
public:
    static_assert(std::is_integral_v<T>, "MaybeInt should use integral types only");

    using This = MaybeInt<T>;
    static constexpr bool IS_SIGNED = std::is_signed_v<T>;
    static constexpr bool IS_UNSIGNED = !IS_SIGNED;
    static constexpr T MIN_VALUE = IS_SIGNED ? std::numeric_limits<T>::min() + 1 : 0;
    static constexpr T MAX_VALUE = IS_SIGNED ? std::numeric_limits<T>::max() : std::numeric_limits<T>::max() - 1;

    constexpr MaybeInt() noexcept = default;
    constexpr MaybeInt(const MaybeInt&) noexcept = default;
    constexpr MaybeInt(T y) noexcept : v(y) {}

    constexpr MaybeInt& operator = (const This& y) noexcept = default;
    constexpr MaybeInt& operator = (T y) noexcept { v = y; return *this; }

    constexpr T rawValue() const { return v; }
    constexpr T operator * () const;

    constexpr bool hasValue() const { return (v != NO_VALUE); }
    constexpr T valueOr(T y) const { return hasValue() ? v : y; }
    explicit operator bool() const { return hasValue(); }

    bool operator == (const This& y) const noexcept = default;
    bool operator == (T y) const noexcept { return (hasValue() && (v == y)); }
    std::partial_ordering operator <=> (const This& y) const noexcept;
    std::partial_ordering operator <=> (T y) const noexcept;

    This& operator += (T y);
    This& operator += (const This& y);

    static This fromStr(std::string_view y);
private:
    static constexpr T NO_VALUE = IS_SIGNED ? std::numeric_limits<T>::min() : std::numeric_limits<T>::max();
    T v = NO_VALUE;
};


template <class T>
constexpr T MaybeInt<T>::operator * () const
{
    if (!hasValue())
        throw std::logic_error("[MaybeInt::op*] No value!");
    return v;
}


template <class T>
auto MaybeInt<T>::fromStr(std::string_view y) -> This
{
    This r;
    auto q = std::from_chars(
                 std::to_address(y.begin()),
                 std::to_address(y.end()),
                 r.v);
    if (q.ec != std::errc{}) {
        r.v = NO_VALUE;
    }
    return r;
}


template <class T>
std::partial_ordering MaybeInt<T>::operator <=> (const This& y) const noexcept
{
    if (hasValue() && y.hasValue()) {
        return (v <=> y.v);
    } else {
        return std::partial_ordering::unordered;
    }
}


template <class T>
std::partial_ordering MaybeInt<T>::operator <=> (T y) const noexcept
{
    if (hasValue()) {
        return (v <=> y);
    } else {
        return std::partial_ordering::unordered;
    }
}


template <class T>
auto MaybeInt<T>::operator += (T y) -> This&
{
    if (hasValue())
        v += y;
    return *this;
}


template <class T>
auto MaybeInt<T>::operator += (const This& y) -> This&
{
    if (hasValue() && y.hasValue())
        v += y.v;
    return *this;
}
