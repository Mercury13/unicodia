#pragma once

#include <string>
#include <optional>
#include <variant>
#include <limits>

#include "Xcolor.h"

//
//  Basic types
//

#define DEFINE_DEFAULT_5(T) \
T() = default;   \
    T(const T&) = default;  \
    T(T&&) = default;  \
    T& operator = (const T&) = default;  \
    T& operator = (T&&) = default;

namespace xsin {
    void encodeAttr(std::string& dest, std::string_view value);
}   // nsmespace xsin

namespace xs {

    template <class T>
    concept Stylish = requires(
        T x, T y, std::string s, std::string_view sv) {
        x.operator = (y);
        x.clear();
        static_cast<bool>(x);
        x.encodeAttr(s);
    };

    ///
    ///  @warning  The objects must check default values for themselves!
    ///
    struct Inherit {
        bool operator == (const Inherit&) const noexcept { return true; }
        void encodeAttr(std::string&) const {}  // do nothing
    };

    struct Number {
        double value = std::numeric_limits<double>::quiet_NaN();

        static void encodeAttr(double value, std::string& dest, int precision);
        void encodeAttr(std::string& dest) const { encodeAttr(value, dest, 15); }
        bool operator == (const Number&) const noexcept = default;
    };

    enum class CharType : unsigned char { BAN, NEXT, START };
    struct IdLink {
        std::string refId;

        bool operator == (const IdLink&) const noexcept = default;
        DEFINE_DEFAULT_5(IdLink)
        template <class T> explicit IdLink(T&& x)
            : refId(std::forward<T>(x)) {}
        static CharType charType(unsigned char x) noexcept;
        static CharType charType(char x) noexcept { return charType(static_cast<unsigned char>(x)); }
        static bool isId(std::string_view x) noexcept;
        static std::optional<IdLink> parse(std::string_view x);
        void encodeAttr(std::string& dest) const;
    };

    struct Special {
        std::string text;
        DEFINE_DEFAULT_5(Special)
        template <class T> explicit Special(T&& x) : text(std::forward<T>(x)) {}
        bool operator == (const Special&) const noexcept = default;
        void encodeAttr(std::string& dest) const { xsin::encodeAttr(dest, text); }
    };

    using ValueVarFather = std::variant<Inherit, Number, Color, IdLink, Special>;
    class ValueVar : public ValueVarFather {
        using Super = ValueVarFather;
    public:
        static constexpr int I_INHERIT = 0;
        static constexpr int I_NUMBER = 1;
        static constexpr int I_COLOR = 2;
        static constexpr int I_IDLINK = 3;
        static constexpr int I_SPECIAL = 4;
        static constexpr int I_N = 5;
        static_assert(I_N == std::variant_size_v<Super>);

        bool operator == (const ValueVar&) const noexcept = default;
        using Super::Super;

        DEFINE_DEFAULT_5(ValueVar)

        void encodeAttr(std::string& dest) const;
    };
}   // xs

namespace xsin {
    // xs, inner

    void startAttr(std::string& dest, std::string_view key);
    void writeAttrIf(std::string& dest, std::string_view key, std::string_view value);
    void writeAttr(std::string& dest, std::string_view key, std::string_view value);

    /// Probably distinction string_view vs Stylish is enough
    ///   to resolve overload reliably
    /// So no need to writeAttrT
    template <xs::Stylish T>
    void writeAttr(std::string& dest, std::string_view key, T& value);

    template <xs::Stylish T>
    void writeAttrIf(std::string& dest, std::string_view key, T& value);

    constexpr std::string_view V_INHERIT = "inherit";

}   // namespace xsin

template <xs::Stylish T>
void xsin::writeAttr(std::string& dest, std::string_view key, T& value)
{
    startAttr(dest, key);
    value.encodeAttr(dest);
    dest += '"';
}

template <xs::Stylish T>
void xsin::writeAttrIf(std::string& dest, std::string_view key, T& value)
{
    if (value)
        writeAttr<T>(dest, key, value);
}
