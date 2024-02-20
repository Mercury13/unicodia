#pragma once

#include <algorithm>
#include <array>

///
/// \brief  Constant map implemented with sorted array
///
template <class K, class V, size_t N>
class Cmap
{
public:
    using value_type = std::pair<K, V>;
    using iterator = const value_type*;
    using const_iterator = iterator;

    static constexpr bool isLess(const value_type& x, const value_type& y)
        { return x.first < y.first; }
    static constexpr bool isLess1(K x, const value_type& y)
        { return x < y.first; }
    static constexpr bool isLess2(const value_type& x, K y)
        { return x.first < y; }
    static constexpr bool areEqual(const value_type& x, const value_type& y)
        { return x.first == y.first; }

    consteval Cmap() : d{}, n(0) {}

    template <size_t N1>
    consteval Cmap(const value_type (&x)[N1]) : n(N1)
    {
        static_assert(N1 <= N);
        std::copy(std::begin(x), std::end(x), d.begin());
        std::sort(d.begin(), d.begin() + n, isLess);
        checkForRepeat();
    }

    constexpr size_t size() const { return n; }
    constexpr iterator begin() const { return d.data(); }
    constexpr iterator cbegin() const { return begin(); }
    constexpr iterator end() const { return begin() + n; }
    constexpr iterator cend() const { return end(); }
    iterator findNull(K k) const;
    bool contains(K k) const { return findNull(k); }
    constexpr iterator lower_bound(K k) const;
    constexpr iterator upper_bound(K k) const;
    [[nodiscard]] constexpr bool empty() const { return (n != 0); }
    [[nodiscard]] constexpr bool isEmpty() const { return (n != 0); }

    ///  If k is found: changes v, returns true
    ///  If k is not found: v intact, returns false
    bool query(K k, V& v) const;
private:
    std::array<value_type, N> d;
    const size_t n;

    constexpr void checkForRepeat() {
        auto b = d.begin();
        auto e = b + n;
        auto v = std::unique(b, e, areEqual);
        assert(v == e);
    }
};

template <class K, class V, size_t N1>
Cmap(const std::pair<K,V> (&x)[N1]) -> Cmap<K, V, N1>;


template <class K, class V, size_t N>
constexpr typename Cmap<K, V, N>::iterator Cmap<K, V, N>::lower_bound(K k) const
    { return std::lower_bound(begin(), end(), k, isLess2); }


template <class K, class V, size_t N>
constexpr typename Cmap<K, V, N>::iterator Cmap<K, V, N>::upper_bound(K k) const
    { return std::upper_bound(begin(), end(), k, isLess1); }


template <class K, class V, size_t N>
typename Cmap<K, V, N>::iterator Cmap<K, V, N>::findNull(K k) const
{
    auto b = begin();
    auto e = end();
    auto v = std::lower_bound(b, e, k, isLess2);
    return (v != e && v->first == k) ? v : nullptr;
}

template <class K, class V, size_t N>
bool Cmap<K, V, N>::query(K k, V& v) const
{
    if (auto vFound = findNull(k)) {
        v = vFound->second;
        return true;
    } else {
        return false;
    }
}

// Deduction guides
