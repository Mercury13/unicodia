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
    constexpr MaybeInt(T y) noexcept : x(y) {}

    constexpr MaybeInt& operator = (const This& y) noexcept = default;
    constexpr MaybeInt& operator = (T y) noexcept { x = y; return *this; }

    constexpr T rawValue() const { return x; }
    constexpr T operator * ();

    constexpr bool hasValue() const { return (x != NO_VALUE); }
    constexpr T valueOr(T y) const { return hasValue() ? x : y; }
    explicit operator bool() const { return hasValue(); }

    bool operator == (const This& y) const noexcept = default;
    static This fromStr(std::string_view y);
private:
    static constexpr T NO_VALUE = IS_SIGNED ? std::numeric_limits<T>::min() : std::numeric_limits<T>::max();
    T x = NO_VALUE;
};


template <class T>
constexpr T MaybeInt<T>::operator * ()
{
    if (!hasValue())
        throw std::logic_error("[MaybeInt::op*] No value!");
    return x;
}


template <class T>
auto MaybeInt<T>::fromStr(std::string_view y) -> This
{
    This r;
    auto q = std::from_chars(
                 std::to_address(y.begin()),
                 std::to_address(y.end()),
                 r.x);
    if (q.ec != std::errc{}) {
        r.x = NO_VALUE;
    }
    return r;
}
