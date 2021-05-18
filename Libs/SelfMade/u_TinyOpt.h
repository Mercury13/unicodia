#pragma once

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
    explicit operator bool () const { return target; }
    const T& operator *  () const { return *target; }
    const T* operator -> () const { return target; }
};
