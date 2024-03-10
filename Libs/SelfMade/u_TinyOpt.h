#pragma once

#include <cstddef>
#include <limits>
#include <type_traits>
#include <utility>

///
/// LICENSE: PUBLIC DOMAIN
/// beacuse of extreme simplicity
///

enum class tinynull_t { INST };
constexpr tinynull_t TINY_NULL = tinynull_t::INST;

///
/// @brief Tiny optional for passing param into function
/// @warning PASS BY VALUE
///
template <class T>
struct TinyOpt
{
    const T* target = nullptr;
    constexpr TinyOpt() noexcept = default;
    constexpr TinyOpt(tinynull_t) noexcept {}
    constexpr TinyOpt(const T& aTarget) noexcept : target(&aTarget) {}
    constexpr TinyOpt(const T* aTarget) noexcept : target(aTarget) {}
    constexpr TinyOpt& operator = (const T& x) noexcept { target = &x; return *this; }
    constexpr TinyOpt& operator = (const T* x) noexcept { target = x; return *this; }
    constexpr explicit operator bool () const noexcept { return target; }
    constexpr const T& operator *  () const noexcept { return *target; }
    constexpr const T* operator -> () const noexcept { return target; }
};


template <class T>
inline bool operator == (TinyOpt<T> x, TinyOpt<T> y) noexcept
{
    if (x) {
        return y && ((*x) == (*y));
    } else {
        return !y;
    }
}

template <class T>
inline bool operator == (TinyOpt<T> x, const T& y) noexcept
    { return x && (*x == y); }

// Just C++17, in C++20 these four are useless
template <class T>
inline bool operator == (const T& y, TinyOpt<T> x) noexcept
    { return x && (*x == y); }

template <class T>
inline bool operator != (TinyOpt<T> x, TinyOpt<T> y) noexcept
    { return !operator == (x, y); }

template <class T>
inline bool operator != (TinyOpt<T> x, const T& y) noexcept
    { return !operator == (x, y); }

template <class T>
inline bool operator != (const T& y, TinyOpt<T> x) noexcept
    { return !operator == (x, y); }



struct TinySizet
{
    using Type = size_t;
    static constexpr auto NO_VALUE = std::numeric_limits<size_t>::max();
    Type rawValue = NO_VALUE;

    template <class Int> requires std::is_integral_v<Int>
        static constexpr size_t toStorage(Int x) noexcept
            { return std::in_range<Type >(x) ? static_cast<size_t>(x) : NO_VALUE; }

    constexpr TinySizet() noexcept = default;
    constexpr TinySizet(tinynull_t) noexcept {}

    /// Construction and assignment from bool is banned
    TinySizet(bool) = delete;
    TinySizet& operator = (bool) = delete;

    template <class Int> requires std::is_integral_v<Int>
        constexpr TinySizet(Int x) noexcept : rawValue(toStorage(x)) {}

    template <class Int>  requires std::is_integral_v<Int>
    constexpr TinySizet& operator = (Type x) noexcept { rawValue = toStorage(x); return *this; }

    constexpr explicit operator bool () const noexcept { return (rawValue != NO_VALUE); }
    constexpr size_t operator *  () const noexcept { return rawValue; }
};
