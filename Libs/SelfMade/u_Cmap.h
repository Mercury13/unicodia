#pragma once

#include <algorithm>
#include <array>
#include <concepts>
#include <utility>

///
/// \brief  Constant map implemented with sorted array
///
template <std::unsigned_integral K, class V, size_t N>
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
    template <std::integral K1>
    bool query(K1 k, V& v) const;
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

// Deduction guide, for Birman
template <std::integral K, class V, size_t N1>
Cmap(const std::pair<K,V> (&x)[N1]) -> Cmap<K, V, N1>;


template <std::unsigned_integral K, class V, size_t N>
constexpr typename Cmap<K, V, N>::iterator Cmap<K, V, N>::lower_bound(K k) const
    { return std::lower_bound(begin(), end(), k, isLess2); }


template <std::unsigned_integral K, class V, size_t N>
constexpr typename Cmap<K, V, N>::iterator Cmap<K, V, N>::upper_bound(K k) const
    { return std::upper_bound(begin(), end(), k, isLess1); }


template <std::unsigned_integral K, class V, size_t N>
typename Cmap<K, V, N>::iterator Cmap<K, V, N>::findNull(K k) const
{
    auto b = begin();
    auto e = end();
    auto v = std::lower_bound(b, e, k, isLess2);
    return (v != e && v->first == k) ? v : nullptr;
}

namespace detail {

    template <std::integral TT>
    struct SafeComper {
        using T = TT;
    };

    template <>
    struct SafeComper<bool> {
        using T = unsigned char;
    };

    template <>
    struct SafeComper<char16_t> {
        using T = unsigned short;
    };

    template <>
    struct SafeComper<char32_t> {
        using T = unsigned int;
    };

    template <std::integral Wanted, std::integral Actual>
    constexpr inline bool isWithinLo(Actual x)
    {
        using ScA = SafeComper<Actual>::T;
        using ScW = SafeComper<Wanted>::T;
        constexpr ScW Wmin = std::numeric_limits<Wanted>::min();
        constexpr ScA Amin = std::numeric_limits<Actual>::min();
        if constexpr (Wmin <= Amin) {
            return true;
        } else {
            return std::cmp_greater_equal(static_cast<ScA>(x), Wmin);
        }
    }

    template <std::integral Wanted, std::integral Actual>
    constexpr inline bool isWithinHi(Actual x)
    {
        using ScA = SafeComper<Actual>::T;
        using ScW = SafeComper<Wanted>::T;
        constexpr ScA Amax = std::numeric_limits<Actual>::max();
        constexpr ScW Wmax = std::numeric_limits<Wanted>::max();
        if constexpr (Amax <= Wmax) {
            return true;
        } else {
            return std::cmp_less_equal(static_cast<ScA>(x), Wmax);
        }
    }

}


template <std::integral Wanted, std::integral Actual>
constexpr bool isWithin(Actual x)
{
    return detail::isWithinLo<Wanted, Actual>(x)
        && detail::isWithinHi<Wanted, Actual>(x);
}

// Unit test
static_assert(!isWithin<signed char>(-129));
static_assert( isWithin<signed char>(-128));
static_assert( isWithin<signed char>( 127));
static_assert(!isWithin<signed char>( 128));

template <std::unsigned_integral K, class V, size_t N>
    template <std::integral K1>
bool Cmap<K, V, N>::query(K1 k, V& v) const
{
    if (!isWithin<K>(k))
        return false;
    if (auto vFound = findNull(k)) {
        v = vFound->second;
        return true;
    } else {
        return false;
    }
}


constexpr unsigned char CMAP_NO_COMMON = 1;
constexpr unsigned char CMAP_LAST_TECH = 1;


template <std::unsigned_integral K, size_t N>
class DoubleCmap : public Cmap<K, unsigned char, N>
{
    using Super = Cmap<K, unsigned char, N>;
public:
    using typename Super::value_type;
    consteval DoubleCmap() {}

    template <size_t N1>
    consteval DoubleCmap(const value_type (&x)[N1]) : Super(x)
    {
        std::fill(std::begin(revMap), std::end(revMap), K{});
        for (auto& [k, v] : x) {
            assert(v >= BASE);
            revMap[v - BASE] = k;
        }
    }
    ///  If k is found: changes v, returns true
    ///  If k is not found: v intact, returns false
    template <std::integral K1>
    bool query(K1 k, unsigned char commonValue, unsigned char& v) const;
private:
    static_assert(N <= 256);
    static constexpr unsigned BASE = 256 - N;
    K revMap [N];
};


template <std::unsigned_integral K, size_t N>
    template <std::integral K1>
bool DoubleCmap<K, N>::query(K1 k, unsigned char commonValue, unsigned char& v) const
{
    bool r = Super::query(k, v);
    if (!r) {
        // Not found
        if (commonValue >= BASE) {
            K revKey = revMap[commonValue - BASE];
            if (revKey != K{} && revKey != k) {
                v = CMAP_NO_COMMON;
                return true;
            }
        }
    }
    return r;
}
