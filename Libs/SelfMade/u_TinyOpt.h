#pragma once

///
/// LICENSE: PUBLIC DOMAIN
/// beacuse of extreme simplicity
///

///
/// @brief Tiny optional for passing param into function
/// @warning PASS BY VALUE
///
template <class T>
struct TinyOpt
{
    const T* target = nullptr;
    TinyOpt() = default;
    TinyOpt(const T& aTarget) : target(&aTarget) {}
    TinyOpt(const T* aTarget) : target(&aTarget) {}
    TinyOpt& operator = (const T& x) { target = &x; return *this; }
    TinyOpt& operator = (const T* x) { target = x; return *this; }
    explicit operator bool () const { return target; }
    const T& operator *  () const { return *target; }
    const T* operator -> () const { return target; }
};
