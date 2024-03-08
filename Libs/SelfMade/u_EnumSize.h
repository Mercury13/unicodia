#pragma once

#include <string_view>

namespace detail {
    template <class Enum> struct EnumSize;

    ///
    ///  @return  "a, b, c, d" → 4, for DEFINE_ENUM_N
    ///
    consteval int countValues(std::string_view s)
    {
        int r = 1;
        // Last comma does not count!!
        size_t n = s.size();
        if (n > 0)
            --n;
        for (size_t i = 0; i < s.size() - 1; ++i) {
            char c = s[i];
            switch (c) {
            case ',' : ++r; break;
            case '=' : --r; break;
            default: ;
            }
        }
        return r;
    }
}

namespace ec {

    /// Reimplement if you want
    template <class Enum>
    constexpr unsigned size() {
        return ::detail::EnumSize<Enum>::size();
    }

}   // namespace ec


#define DEFINE_ENUM(Name, ...) \
    enum class Name { __VA_ARGS__ }; \
    template <> struct detail::EnumSize<Name> {  \
        static constexpr unsigned size() { return detail::countValues(#__VA_ARGS__); } \
    };

#define DEFINE_ENUM_IN_NS(Ns, Name, ...) \
    enum class Name { __VA_ARGS__ };  } \
    template <> struct detail::EnumSize<:: Ns :: Name > {  \
        static constexpr unsigned size() { return detail::countValues(#__VA_ARGS__); } \
    };  \
    namespace Ns {
