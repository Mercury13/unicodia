#pragma once

#include <string> // won’t be included actually

namespace mojibake::detail {

    template <typename ToC, typename FromC, bool isEqual>
    class ConvStringProto;

    template <typename ToC, typename FromC>
    class ConvStringProto<ToC, FromC, false>
    {
    protected:
        using ToStr = std::basic_string<ToC>;
        using FromSv = std::basic_string_view<FromC>;
        ToStr tmp;

        ConvStringProto(FromSv x) : tmp(toQ<ToStr>(x)) {}
        size_t length() const { return tmp.length(); }
        const void* data() const { return tmp.data(); }
        void* nonConstData() { return const_cast<void*>(data()); }
        static constexpr bool isConverted() { return true; }
    };

    template <typename ToC, typename FromC>
    class ConvStringProto<ToC, FromC, true>
    {
    protected:
        using FromSv = std::basic_string_view<FromC>;
        FromSv tmp;
        ConvStringProto(FromSv x) : tmp(x) {}
        size_t length() const { return tmp.length(); }
        const void* data() const { return tmp.data(); }
        void* nonConstData() { return const_cast<void*>(data()); }
        static constexpr bool isConverted() { return false; }
    };

    /// @return  [+] it’s possible to alias-cast buffer of FromC’s to buffer of ToC’s
    template <typename ToC, typename FromC>
    constexpr bool isAliasable()
    {
        return (sizeof(ToC) == sizeof(FromC)) && (alignof(ToC) <= alignof(FromC));
    }

}
