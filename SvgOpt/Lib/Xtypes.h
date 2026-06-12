#pragma once

#include <string>
#include <optional>
#include <variant>

//
//  Basic types
//

#define DEFINE_DEFAULT_5(T) \
T() = default;   \
    T(const T&) = default;  \
    T(T&&) = default;  \
    T& operator = (const T&) = default;  \
    T& operator = (T&&) = default;

namespace xs {

    template <class T>
    concept Stylish = requires(
        T x, T y, std::string s, std::string_view sv) {
        x.operator = (y);
        x.clear();
        static_cast<bool>(x);
        x.encodeAttr(s);
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
    struct Inherit {
        bool operator == (const Inherit&) const noexcept { return true; }
    };

    using ValueVar = std::variant<Inherit, IdLink>;

}   // xs

namespace xsin {
    // xs, inner

    void encodeAttr(std::string& dest, std::string_view value);
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
