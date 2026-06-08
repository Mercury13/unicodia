#pragma once

#include <string>
#include <variant>
#include <vector>
#include <limits>

#include "Xcolor.h"

#include "u_TypedFlags.h"

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
        void writeAttrIf(std::string& dest, std::string_view key) const;
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
        void writeAttrIf(std::string& dest, std::string_view key) const;
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
        void writeAttrIf(std::string& dest, std::string_view key) const;
        void parse(std::string_view x);
        bool hasSmth() const noexcept { return (index() != I_INHERIT); }
        explicit operator bool() const noexcept { return hasSmth(); }
    };

    template <class T>
    concept Stylish = requires(
            T x, T y, std::string s, std::string_view sv) {
        x.operator = (y);
        x.clear();
        x.writeAttrIf(s, sv);
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
    private:
        const U& u;
    };

    struct Style {
        StyleObj<Fill> fill;
        StyleObj<MaybeFillRule> fillRule;
        StyleObj<MaybeColor> stopColor;
        std::vector<Attr> attrs;

        bool hasSmth() const noexcept;
        operator bool() const noexcept { return hasSmth(); }
        void encodeAttr(std::string& dest) const;
        /// The key is fixed: style
        void writeAttrIf(std::string& dest) const;
        void traverse(Flags<AllowAttr> allowed, const MultiTypeCallback& x);
        /// @return [+] was called
        bool find(std::string_view key, Flags<AllowAttr> allowed,
                const MultiTypeCallback& cb);
        void clear();
        void add(std::string_view key, std::string_view value);
        void parse(std::string_view x);

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

}   // namespace xsin