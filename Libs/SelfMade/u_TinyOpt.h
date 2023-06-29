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
    TinyOpt() noexcept = default;
    TinyOpt(tinynull_t) noexcept {}
    TinyOpt(const T& aTarget) noexcept : target(&aTarget) {}
    TinyOpt(const T* aTarget) noexcept : target(&aTarget) {}
    TinyOpt& operator = (const T& x) noexcept { target = &x; return *this; }
    TinyOpt& operator = (const T* x) noexcept { target = x; return *this; }
    explicit operator bool () const noexcept { return target; }
    const T& operator *  () const noexcept { return *target; }
    const T* operator -> () const noexcept { return target; }
};
