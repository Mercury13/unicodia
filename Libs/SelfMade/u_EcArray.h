#pragma once

#include "magic_enum.hpp"
#include <stdexcept>

namespace ec {

#ifdef AT_RANGE_CHECK
    constexpr bool RANGE_CHECK = true;
#else
    constexpr bool RANGE_CHECK = false;
#endif
    constexpr bool AT_NOEXCEPT = !RANGE_CHECK;

    namespace detail {
        enum class ArrayInit { INST };
    }
    constexpr detail::ArrayInit ARRAY_INIT = detail::ArrayInit::INST;

    ///
    ///  Size of ec::Array
    ///  Reimplement if you want
    ///
    template <class Ec> requires std::is_enum_v<Ec>
    consteval size_t size() {
        auto values = magic_enum::enum_values<Ec>();
        size_t r = 0;
        for (auto v : values)
            r = std::max(r, static_cast<size_t>(v) + 1);
        return r;
    }

    template <class V, class Ec> requires std::is_enum_v<Ec>
    class Array
    {
    public:
        using value_type = V;
        using iterator = V*;
        using const_iterator = const V*;
        static constexpr size_t Size = ec::size<Ec>();
        static constexpr size_t InitCount = Size;
        using CArray = V[Size];
        using CppArray = std::array<V, Size>;

        constexpr Array() noexcept = default;

        template <class... U>
        constexpr Array(U&&... args) : a{std::forward<U>(args)...}
        {
            static_assert(sizeof...(args) == InitCount, "[ec::Array()] Wrong size");
        }

        template <class U, size_t N>
        constexpr Array(detail::ArrayInit, std::array<U, N>&& x)
        {
            static_assert(N == InitCount, "[ec::Array(arr&&)] Wrong size!");
            std::move(x.begin(), x.end(), std::begin(a));
        }

        template <class U, size_t N>
        constexpr Array(detail::ArrayInit, const std::array<U, N>& x)
        {
            static_assert(N == InitCount, "[ec::Array(arr)] Wrong size!");
            std::copy(x.begin(), x.end(), std::begin(a));
        }

        constexpr size_t size() const noexcept { return Size; }
        constexpr       iterator begin()        noexcept { return std::begin(a); }
        constexpr       iterator end()          noexcept { return std::end(a); }
        constexpr const_iterator begin()  const noexcept { return std::begin(a); }
        constexpr const_iterator end()    const noexcept { return std::end(a); }
        constexpr const_iterator cbegin() const noexcept { return std::begin(a); }
        constexpr const_iterator cend()   const noexcept { return std::end(a); }
        constexpr       V& front()       noexcept { return a[0]; }
        constexpr const V& front() const noexcept { return a[0]; }
        constexpr       V& back()        noexcept { return a[Size - 1]; }
        constexpr const V& back()  const noexcept { return a[Size - 1]; }

        /// @return  C array
        constexpr       CArray& cArray()       noexcept { return a; }
        constexpr const CArray& cArray() const noexcept { return a; }

        constexpr V& operator [] (Ec i) noexcept(AT_NOEXCEPT)
        {
            if constexpr (RANGE_CHECK) {
                if (static_cast<size_t>(i) >= Size)
                    throw std::range_error("ec::Array range check!");
            }
            return a[static_cast<size_t>(i)];
        }

        constexpr const V& operator [] (Ec i) const noexcept(AT_NOEXCEPT)
        {
            if constexpr (RANGE_CHECK) {
                if (static_cast<size_t>(i) >= Size)
                    throw std::range_error("ec::Array const range check!");
            }
            return a[static_cast<size_t>(i)];
        }

        template <class U>
        Ec findDef(U&& x, Ec def) const;

        template <class Pred>
        Ec findIfDef(Ec def, Pred&& pred) const;
    private:
        CArray a {};
    };

}   // namespace ec


template <class V, class Ec> requires std::is_enum_v<Ec>
template <class U>
Ec ec::Array<V, Ec>::findDef(U&& x, Ec def) const
{
    for (size_t i = 0; i < Size; ++i) {
        if (a[i] == x)
            return static_cast<Ec>(i);
    }
    return def;
}


template <class V, class Ec> requires std::is_enum_v<Ec>
template <class Pred>
Ec ec::Array<V, Ec>::findIfDef(Ec def, Pred&& pred) const
{
    for (size_t i = 0; i < Size; ++i) {
        if (pred(a[i]))
            return static_cast<Ec>(i);
    }
    return def;
}
