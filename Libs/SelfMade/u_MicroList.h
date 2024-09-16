#pragma once

#include <utility>

template <class T, unsigned N>
class MicroList
{
public:
    constexpr MicroList() noexcept : sz(0) {}

    template <class... Args> constexpr MicroList(Args&&... x)
        : sz(sizeof...(Args)), d{std::forward<Args>(x)...}
    {
        static_assert(sizeof...(Args) <= N, "Too many initializers");
    }

    unsigned size() const noexcept { return sz; }
    using iterator = T*;
    using const_iterator = const T*;
    iterator begin() noexcept { return d; }
    iterator end() noexcept { return d + sz; }
    const_iterator begin() const noexcept { return d; }
    const_iterator end() const noexcept { return d + sz; }
    const_iterator cbegin() const noexcept { return d; }
    const_iterator cend() const noexcept { return d + sz; }
    bool empty() const noexcept { return (sz == 0); }
    bool isEmpty() const noexcept { return (sz == 0); }
    bool areEmpty() const noexcept { return (sz == 0); }
    bool hasSmth() const noexcept { return (sz != 0); }
    // Oh, need C++23’s this params
    T&  operator [] (unsigned i) &  { return d[i]; }
    T&& operator [] (unsigned i) && { return std::move(d[i]); }
    const T& operator  [] (unsigned i) const & { return d[i]; }
    const T&& operator [] (unsigned i) const && { return std::move(d[i]); }
private:
    unsigned sz;
    T d[N];
};
