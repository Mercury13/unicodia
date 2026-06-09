#pragma once

#include <string>
#include <variant>
#include <vector>
#include <cmath>
#include <algorithm>

#include "Xcolor.h"

#include "u_TypedFlags.h"

namespace xsi {

    constexpr inline int toPermille(double x, int unit) noexcept
    {
        int r = std::round(x * unit);
        return std::clamp(r, 0, 1000);
    }

}   // namespace xsi

namespace xs {

    enum class Channel : unsigned char {
        BOTH = 0, ONE = 1, TWO = 2, GENERAL = BOTH };
    enum class NodeChannel : unsigned char {
        BOTH = 0, ONE = 1, TWO = 2,
        ONE_TRANSP = 3,     // 1 → written, 2 → transparent
        TWO_TRANSP = 4
    };

    struct Attr {
        std::string key, value;
        Channel channel = Channel::BOTH;
    };

    struct IdLink {
        std::string wantedId;
        bool operator == (const IdLink&) const noexcept = default;
    };
    struct Inherit {
        bool operator == (const Inherit&) const noexcept { return true; }
    };
    struct None {
        bool operator == (const None&) const noexcept { return true; }
    };
    struct Special {
        std::string text;
        bool operator == (const Special&) const noexcept = default;
    };

    using MaybeColorFather = std::variant<Inherit, Color, Special>;
    class MaybeColor : public MaybeColorFather {
    public:
        static constexpr int I_INHERIT = 0;
        static constexpr int I_COLOR = 1;
        static constexpr int I_SPECIAL = 2;
        static constexpr int I_N = 3;
        static_assert(I_N == std::variant_size_v<MaybeColorFather>);

        bool operator == (const MaybeColor&) const noexcept = default;
        using MaybeColorFather::MaybeColorFather;
        using MaybeColorFather::operator =;

        void clear() { *this = Inherit(); }
        void encodeAttr(std::string& text) const;
        void parse(std::string_view x);
        bool hasSmth() const noexcept { return (index() != I_INHERIT); }
        explicit operator bool() const noexcept { return hasSmth(); }
    };

    using FillFather = std::variant<Inherit, None, Color, IdLink, Special>;
    class Fill : public FillFather {
    public:
        static constexpr int I_INHERIT = 0;
        static constexpr int I_NONE = 1;
        static constexpr int I_COLOR = 2;
        static constexpr int I_IDLINK = 3;
        static constexpr int I_SPECIAL = 4;
        static constexpr int I_N = 5;
        static_assert(I_N == std::variant_size_v<FillFather>);

        bool operator == (const Fill&) const noexcept = default;
        using FillFather::FillFather;
        using FillFather::operator =;

        void clear() { *this = Inherit{}; }
        void encodeAttr(std::string& dest) const;
        void parse(std::string_view x);
        bool hasSmth() const noexcept { return (index() != I_INHERIT); }
        explicit operator bool() const noexcept { return hasSmth(); }
    };

    enum class FillRule : unsigned char { NONZERO, EVENODD };
    using MaybeFillRuleFather = std::variant<Inherit, FillRule, Special>;
    class MaybeFillRule : public MaybeFillRuleFather {
    public:
        static constexpr int I_INHERIT = 0;
        static constexpr int I_FILLRULE = 1;
        static constexpr int I_SPECIAL = 2;
        static constexpr int I_N = 3;
        static_assert(I_N == std::variant_size_v<MaybeFillRuleFather>);

        bool operator == (const MaybeFillRule&) const noexcept = default;
        using MaybeFillRuleFather::MaybeFillRuleFather;
        using MaybeFillRuleFather::operator =;

        void clear() { *this = Inherit{}; }
        void encodeAttr(std::string& text) const;
        void parse(std::string_view x);
        bool hasSmth() const noexcept { return (index() != I_INHERIT); }
        explicit operator bool() const noexcept { return hasSmth(); }
    };

    // Three digits are always enough!
    constexpr int OPACITY_UNIT = 1000;
    constexpr int OPACITY_DIGITS = 3;
    enum class OpacityUnit : unsigned short {
        UNIT = OPACITY_UNIT, PERCENT = OPACITY_UNIT / 100, PERMILLE = OPACITY_UNIT / 1000 };
    class Opacity {
    public:
        constexpr Opacity() noexcept = default;
        constexpr Opacity(double x) noexcept
            : permille(xsi::toPermille(x, OPACITY_UNIT)) {}
        constexpr Opacity(double x, OpacityUnit unit) noexcept
            : permille(xsi::toPermille(x, static_cast<int>(unit))) {}
        bool operator == (const Opacity&) const noexcept = default;
        void encode(std::string& dest) const;
        int raw() const noexcept { return permille; }
    private:
        int permille = OPACITY_UNIT;
    };
    constexpr Opacity OPAQUE {};

    /// The default opacity is ONE!! → so no Inherit
    using MaybeOpacityFather = std::variant<Opacity, Special>;
    class MaybeOpacity : public MaybeOpacityFather {
    public:
        static constexpr int I_OPACITY = 0;
        static constexpr int I_SPECIAL = 1;
        static constexpr int I_N = 2;
        static_assert(I_N == std::variant_size_v<MaybeOpacityFather>);

        bool operator == (const MaybeOpacity&) const noexcept = default;
        using MaybeOpacityFather::MaybeOpacityFather;
        using MaybeOpacityFather::operator =;

        void clear() { *this = OPAQUE; }
        void encodeAttr(std::string& dest) const;
        void parse(std::string_view x);
        bool hasSmth() const noexcept { return (*this != OPAQUE); }
        explicit operator bool() const noexcept { return hasSmth(); }
    };

    template <class T>
    concept Stylish = requires(
            T x, T y, std::string s, std::string_view sv) {
        x.operator = (y);
        x.clear();
        static_cast<bool>(x);
        x.encodeAttr(s);
    };

    enum class AllowAttr : unsigned int {
        DRAW = 1<<0,    // draw-related: fill, fillRule
        STOP = 1<<1,    // stop-related: stopColor
    };
    DEFINE_ENUM_OPS(AllowAttr)

    template <Stylish T>
    struct StyleObj {
        T attr, style;
        T& active() noexcept { return style ? style : attr; }
        const T& active() const noexcept { return style ? style : attr; }
        bool hasSmth() const noexcept { return attr || style; }
        explicit operator bool() const noexcept { return hasSmth(); }
        void clear() { attr.clear(); style.clear(); }
    };

    class MultiTypeCallback {   // interface
    public:
        virtual void onColor(std::string_view key,
                StyleObj<MaybeColor>& x, bool mayHaveAttr) const = 0;
        virtual void onFill(std::string_view key,
                StyleObj<Fill>& x, bool mayHaveAttr) const = 0;
        virtual void onFillRule(std::string_view key,
                StyleObj<MaybeFillRule>& x, bool mayHaveAttr) const = 0;
        virtual void onOpacity(std::string_view key,
                StyleObj<MaybeOpacity>& x, bool mayHaveAttr) const = 0;
        virtual ~MultiTypeCallback() = default;
    };

    template <class U> class MultiTypeCallbackT final
        : public MultiTypeCallback {
    public:
        MultiTypeCallbackT(const U& uu) : u(uu) {}
        void onColor(std::string_view key,
                StyleObj<MaybeColor>& x, bool mayHaveAttr) const override
            { u(key, x, mayHaveAttr); }
        void onFill(std::string_view key,
                StyleObj<Fill>& x, bool mayHaveAttr) const override
            { u(key, x, mayHaveAttr); }
        void onFillRule(std::string_view key,
                StyleObj<MaybeFillRule>& x, bool mayHaveAttr) const override
            { u(key, x, mayHaveAttr); }
        void onOpacity(std::string_view key,
                StyleObj<MaybeOpacity>& x, bool mayHaveAttr) const override
            { u(key, x, mayHaveAttr); }
    private:
        const U& u;
    };

    struct Style {
        StyleObj<Fill> fill;
        StyleObj<MaybeFillRule> fillRule;

        // Stop-related
        StyleObj<MaybeColor> stopColor;
        StyleObj<MaybeOpacity> stopOpacity;

        std::vector<Attr> attrs;

        bool hasSmth() noexcept;
        bool hasStyle() noexcept;
        void encodeAttr(std::string& dest);
        /// The key is fixed: style
        void writeAttrIf(std::string& dest);
        void traverse(Flags<AllowAttr> allowed, const MultiTypeCallback& x);
        /// @return [+] was called
        bool find(std::string_view key, Flags<AllowAttr> allowed,
                const MultiTypeCallback& cb);
        void clear();
        void add(std::string_view key, std::string_view value);
        void parse(std::string_view x);
        bool trySpecificAttr(std::string_view key, std::string_view value);

        template <class T>
        void traverseT(Flags<AllowAttr> allowed, const T& cb)
            { traverse(allowed, MultiTypeCallbackT<T>(cb)); }
        template <class T>
        bool findT(std::string_view key, Flags<AllowAttr> allowed, const T& cb)
            { return find(key, allowed, MultiTypeCallbackT<T>(cb)); }
    };

}   // namespace xs

namespace xsin {
    // xs, inner

    void startAttr(std::string& dest, std::string_view key);
    void encodeAttr(std::string& dest, std::string_view value);
    void writeAttrIf(std::string& dest, std::string_view key, std::string_view value);
    void writeAttr(std::string& dest, std::string_view key, std::string_view value);

    /// Probably distinction string_view vs Stylish is enough
    ///   to resolve overload reliably
    template <xs::Stylish T>
    void writeAttr(std::string& dest, std::string_view key, T& value)
    {
        startAttr(dest, key);
        value.encodeAttr(dest);
        dest += '"';
    }

    template <xs::Stylish T>
    void writeAttrIf(std::string& dest, std::string_view key, T& value)
    {
        if (value)
            writeAttr<T>(dest, key, value);
    }

}   // namespace xsin