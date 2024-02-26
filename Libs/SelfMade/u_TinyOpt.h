#pragma once

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
inline bool operator == (TinyOpt<T> x, TinyOpt<T> y)
{
    if (x) {
        return y && ((*x) == (*y));
    } else {
        return !y;
    }
}

template <class T>
inline bool operator == (TinyOpt<T> x, const T& y)
{
    return x && (*x == y);
}
