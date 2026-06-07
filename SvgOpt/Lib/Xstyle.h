#pragma once

#include <string>
#include <variant>
#include <vector>

#include "Xcolor.h"

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

        void encodeAttr(std::string& text) const;
        void writeAttrIf(std::string& dest, std::string_view key) const;
        void parse(std::string_view x);
        bool hasSmth() const noexcept { return (index() != I_INHERIT); }
        operator bool() const noexcept { return hasSmth(); }
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
        operator bool() const noexcept { return hasSmth(); }
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
        operator bool() const noexcept { return hasSmth(); }
    };

    struct Style {
        Fill fill;
        MaybeFillRule fillRule;
        MaybeColor stopColor;
        std::vector<Attr> attrs;

        bool hasSmth() const noexcept;
        operator bool() const noexcept { return hasSmth(); }
        void encodeAttr(std::string& dest) const;
        /// The key is fixed: style
        void writeAttrIf(std::string& dest) const;
        void clear();
        void add(std::string_view key, std::string_view value);
        void parse(std::string_view x);
    };

    class MultiTypeCallback {   // interface
    public:
        virtual void onColor(MaybeColor& a, MaybeColor& b) const = 0;
        virtual void onFill(Fill& a, Fill& b) const = 0;
        virtual void onFillRule(MaybeFillRule& a, MaybeFillRule& b) const = 0;
        virtual ~MultiTypeCallback() = default;
    };

    template <class U> class MultiTypeCallbackT final
            : public MultiTypeCallback {
    public:
        MultiTypeCallbackT(const U& uu) : u(uu) {}
        void onColor(MaybeColor& a, MaybeColor& b) const override { u(a, b); }
        void onFill(Fill& a, Fill& b) const override { u(a, b); }
        void onFillRule(MaybeFillRule& a, MaybeFillRule& b) const override  { u(a, b); };
    private:
        const U& u;
    };

}   // namespace xs

namespace xsin {
    // xs, inner

    void startAttr(std::string& dest, std::string_view key);
    void encodeAttr(std::string& dest, std::string_view value);
    void writeAttrIf(std::string& dest, std::string_view key, std::string_view value);
    void writeAttr(std::string& dest, std::string_view key, std::string_view value);

}   // namespace xsin