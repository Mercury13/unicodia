#pragma once

#include <string>
#include <variant>
#include <vector>
#include <cmath>
#include <algorithm>

#include "Xcolor.h"
#include "Xdic.h"

#include "u_TypedFlags.h"
#include "u_Strings.h"

namespace xsin {

    constexpr inline int toPermille(double x, int unit) noexcept
    {
        int r = std::round(x * unit);
        return std::clamp(r, 0, 1000);
    }

    void encodeAttr(std::string& dest, std::string_view value);

}   // namespace xsi

namespace xs {

#define DEFINE_DEFAULT_5(T) \
T() = default;   \
    T(const T&) = default;  \
    T(T&&) = default;  \
    T& operator = (const T&) = default;  \
    T& operator = (T&&) = default;

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

    struct None {
        bool operator == (const None&) const noexcept { return true; }
    };
    struct Special {
        std::string text;
        DEFINE_DEFAULT_5(Special)
        template <class T> explicit Special(T&& x) : text(std::forward<T>(x)) {}
        bool operator == (const Special&) const noexcept = default;
        void encodeAttr(std::string& dest) const
            { xsin::encodeAttr(dest, text); }
    };

    template <class T>
    concept SimpleStyleType = requires (
        T x, std::optional<T> opt, std::string_view sv, std::string dest) {
        opt = T::parse(sv);
        x.encodeAttr(dest);
    };

    template <SimpleStyleType Payload>
    using TripleMaybeFather = std::variant<Inherit, Payload, Special>;

    template <SimpleStyleType Payload>
    class TripleMaybe : public TripleMaybeFather<Payload> {
        using Super = TripleMaybeFather<Payload>;
    public:
        static constexpr int I_INHERIT = 0;
        static constexpr int I_PAYLOAD = 1;
        static constexpr int I_SPECIAL = 2;
        static constexpr int I_N = 3;
        static_assert(I_N == std::variant_size_v<Super>);

        bool operator == (const TripleMaybe&) const noexcept = default;
        // Inherited ctor and op= (e.g. for *this = Inherit{};)
        using Super::Super;

        DEFINE_DEFAULT_5(TripleMaybe)

        // Functions borrowed from templated father
        using Super::index;

        void clear() { *this = Inherit{}; }
        bool hasSmth() const noexcept { return (index() != I_INHERIT); }
        explicit operator bool() const noexcept { return hasSmth(); }

        void encodeAttr(std::string& dest) const;
        /// @return  index
        int parse(std::string_view x);
    };

    using MaybeColor = TripleMaybe<Color>;

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
        DEFINE_DEFAULT_5(Fill)

        void clear() { *this = Inherit{}; }
        void encodeAttr(std::string& dest) const;
        void parse(std::string_view x);
        bool hasSmth() const noexcept { return (index() != I_INHERIT); }
        explicit operator bool() const noexcept { return hasSmth(); }
    };

    enum class FillRule : unsigned char { NONZERO, EVENODD };
    struct FillRuleWrap {
        FillRule v = FillRule::NONZERO;

        constexpr FillRuleWrap() noexcept = default;
        constexpr FillRuleWrap(FillRule x) noexcept : v(x) {}

        void encodeAttr(std::string& dest) const;
        static std::optional<FillRuleWrap> parse(std::string_view x) noexcept;
    };
    using MaybeFillRule = TripleMaybe<FillRuleWrap>;

    /// Three digits are always enough,
    ///   as most renderers work in 8 bits
    constexpr int OPACITY_UNIT = 1000;
    constexpr int OPACITY_DIGITS = 3;
    enum class OpacityUnit : unsigned short {
        UNIT = OPACITY_UNIT, PERCENT = OPACITY_UNIT / 100, PERMILLE = OPACITY_UNIT / 1000 };
    class Opacity {
    public:
        constexpr Opacity() noexcept = default;
        constexpr Opacity(double x) noexcept
            : permille(xsin::toPermille(x, OPACITY_UNIT)) {}
        constexpr Opacity(double x, OpacityUnit unit) noexcept
            : permille(xsin::toPermille(x, static_cast<int>(unit))) {}
        bool operator == (const Opacity&) const noexcept = default;
        void encodeAttr(std::string& dest) const;
        int raw() const noexcept { return permille; }
        static std::optional<Opacity> parse(std::string_view x) noexcept;
    private:
        int permille = OPACITY_UNIT;
    };
    constexpr Opacity OPAQUE {};

    using MaybePartOpacity = TripleMaybe<Opacity>;

    /// The default opacity is ONE!! → so no Inherit, no TripleMaybe
    using MaybeTotalOpacityFather = std::variant<Opacity, Special>;
    class MaybeTotalOpacity : public MaybeTotalOpacityFather {
    public:
        static constexpr int I_OPACITY = 0;
        static constexpr int I_SPECIAL = 1;
        static constexpr int I_N = 2;
        static_assert(I_N == std::variant_size_v<MaybeTotalOpacityFather>);

        bool operator == (const MaybeTotalOpacity&) const noexcept = default;
        using MaybeTotalOpacityFather::MaybeTotalOpacityFather;
        using MaybeTotalOpacityFather::operator =;

        void clear() { *this = OPAQUE; }
        void encodeAttr(std::string& dest) const;
        int parse(std::string_view x);
        bool hasSmth() const noexcept { return (*this != OPAQUE); }
        explicit operator bool() const noexcept { return hasSmth(); }
    };

    using MaybeLink = TripleMaybe<IdLink>;

    template <class T>
    concept Stylish = requires(
            T x, T y, std::string s, std::string_view sv) {
        x.operator = (y);
        x.clear();
        static_cast<bool>(x);
        x.encodeAttr(s);
    };

    enum class AllowAttr : unsigned int {
        FILL   = 1<<0,  // fill-related: fill, fillRule
        STROKE = 1<<1,  // stroke-related: stroke
        ANYDRAW = FILL | STROKE,
        STOP   = 1<<1,  // stop-related: stopColor
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
        virtual void onLink(std::string_view key,
                StyleObj<MaybeLink>& x, bool mayHaveAttr) const = 0;
        virtual void onPartOpacity(std::string_view key,
                StyleObj<MaybePartOpacity>& x, bool mayHaveAttr) const = 0;
        virtual void onTotalOpacity(std::string_view key,
            StyleObj<MaybePartOpacity>& x, bool mayHaveAttr) const = 0;
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
        void onLink(std::string_view key,
                StyleObj<MaybeLink>& x, bool mayHaveAttr) const override
            { u(key, x, mayHaveAttr); }
        void onPartOpacity(std::string_view key,
                StyleObj<MaybePartOpacity>& x, bool mayHaveAttr) const override
            { u(key, x, mayHaveAttr); }
        void onTotalOpacity(std::string_view key,
                StyleObj<MaybePartOpacity>& x, bool mayHaveAttr) const override
            { u(key, x, mayHaveAttr); }
    private:
        const U& u;
    };

    struct Style {
        StyleObj<Fill> fill;
        StyleObj<MaybePartOpacity> fillOpacity;
        StyleObj<MaybeFillRule> fillRule;
        StyleObj<MaybeLink> clipPath;

        // Stop-related
        StyleObj<MaybeColor> stopColor;
        StyleObj<MaybePartOpacity> stopOpacity;

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


///// Template implementations /////////////////////////////////////////////////

template <xs::SimpleStyleType Payload>
void xs::TripleMaybe<Payload>::encodeAttr(std::string& dest) const
{
    std::visit(
        [&dest](const auto& thing) {
            using T = std::decay_t<decltype(thing)>;
            if constexpr (std::is_same_v<T, Inherit>) {
                // do nothing
            } else {
                thing.encodeAttr(dest);
            }
        },
        *this);
}

template <xs::SimpleStyleType Payload>
int xs::TripleMaybe<Payload>::parse(std::string_view x)
{
    if (x.empty() || lat::areCaseEqual(x, xsin::V_INHERIT)) {
        *this = Inherit();
        return I_INHERIT;
    }
    if (auto q = Payload::parse(x)) {
        *this = std::move(*q);
        return I_PAYLOAD;
    }
    *this = Special(x);
    return I_SPECIAL;
}

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

