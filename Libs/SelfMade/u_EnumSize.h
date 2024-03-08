#pragma once

namespace detail {
    template <class Enum> struct EnumSize;
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
        enum QQQ { __VA_ARGS__, NNNNNN };  \
        static constexpr unsigned size() { return static_cast<unsigned>(NNNNNN); } \
    };

#define DEFINE_ENUM_IN_NS(Ns, Name, ...) \
    enum class Name { __VA_ARGS__ };  } \
    template <> struct detail::EnumSize<:: Ns :: Name > {  \
        enum QQQ { __VA_ARGS__, NNNNNN };  \
        static constexpr unsigned size() { return static_cast<unsigned>(NNNNNN); } \
    };  \
    namespace Ns {
