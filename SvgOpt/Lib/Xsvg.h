#pragma once

#include <vector>
#include <memory>
#include <filesystem>
#include <variant>

#include "Xcolor.h"

namespace pugi {
    class xml_node;
}

namespace xs {

    enum class IdAction : unsigned char {
        LEAVE, CLEAN, CLEAN_SHRINK };

    enum class StyleToAttr : unsigned char {
        LEAVE, IF_SHORTER, ALWAYS };

    struct OptSets {  // the default is rather strong and unsafe
        /// NOT IMPLEMENTED.
        /// Editors make lots of unused IDs, what to do?
        /// Type: garbage collection
        IdAction idAction = IdAction::CLEAN_SHRINK;

        /// NOT IMPLEMENTED.
        /// [+] transform="translate:1,5" to x="1" y="5"
        /// Type: cunning move
        bool transformToXy = true;

        /// NOT IMPLEMENTED.
        /// [+] clean unused objects
        /// (foreign namespaces are NOT EVEN LOADED and auto-cleaned)
        /// Type: garbage collection
        bool cleanUnusedObjects = true;

        /// NOT IMPLEMENTED
        /// [+] clean unused styles: e.g. delete font if there are no
        ///     texts nearby
        /// Type: garbage collection
        bool cleanUnusedStyles = true;

        /// Will remove unused groups, move styles etc
        /// Type: garbage collection / cunning move
        bool groupOptimization = true;

        /// NOT IMPLEMENTED.
        /// [+] width="10" height="10" (22) will be shrunken to
        ///     viewBox="0 0 10 10" (19)
        /// Type: cunning move
        bool optimizeViewBoxWh = true;

        /// NOT IMPLEMENTED.
        /// [+] Remove colour if it is default black #000
        /// Type: cunning move
        bool useDefaultBlack = true;

        /// NOT IMPLEMENTED.
        /// [+] style="stop-color:#D32F2F" → stop-color="#d32f2f"
        /// Type: cunning move
        StyleToAttr styleToAttr = StyleToAttr::IF_SHORTER;

        /// NOT IMPLEMENTED
        /// [+] remove xmlns="....."
        /// Type: deviation from standard
        bool removeXmlns = true;

        /// NOT IMPLEMENTED.
        /// coordinate precision, digits after point
        /// Type: lossy compression
        unsigned char coordPrecision = 2;
    };

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

    enum class DoesDraw : unsigned char {
        NO,         ///< Does not draw, never: <linearGradient>
        MAYBE,      ///< Maybe draws, check children: <g>
        YES,        ///< Yep, draws: <path>
        SPECIAL,    ///< Special rules: <defs>
    };
    // When to delete:
    // NO: no links to id
    // MAYBE: no objects that are actually drawn
    // YES: never
    // SPECIAL: really empty

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
        void encodeAttr(std::string& text) const;
        void writeAttrIf(std::string& dest, std::string_view key) const;
        void parse(std::string_view x);
        bool hasSmth() const noexcept { return (index() != I_INHERIT); }
        operator bool() const noexcept { return hasSmth(); }
    };

    struct Style {
        Fill fill;
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

    class Node {
    public:
        virtual std::string_view name() const noexcept = 0;
        virtual DoesDraw doesDraw() const noexcept = 0;
        /// @return [+] was inserted to specific structures
        virtual bool trySpecificAttr(std::string_view key, std::string_view value);
        virtual void writeSpecificAttrs(std::string& dest);
        virtual void basicOptimizations(const OptSets& sets);

        struct StdAttrs {
            std::string id;
            Fill fill { Inherit {} };
            Style style;
            std::string transform;
        } sa;
        std::vector<Attr> attrs;
        std::vector<std::unique_ptr<Node>> children;
        NodeChannel channel = NodeChannel::BOTH;

        void write(std::string& dest, Channel channel);
        void writeAttrs(std::string& dest);
        void recurseBasicOptimizations(const OptSets& sets);
    private:
    };

    class RootNode : public Node {
    public:
        virtual std::string_view name() const noexcept override
            { return "svg"; }
        virtual DoesDraw doesDraw() const noexcept override
            { return DoesDraw::MAYBE; }
        void basicOptimizations(const OptSets& sets);
    };

    class FreeNode : public Node {
    public:
        std::string myName;
        DoesDraw myDoesDraw;

        FreeNode(std::string aName, DoesDraw aDoesDraw)
            : myName(std::move(aName)), myDoesDraw(aDoesDraw) {}
        std::string_view name() const noexcept override { return myName; }
        DoesDraw doesDraw() const noexcept override { return myDoesDraw; }
    };

    struct SaveSets {
        Channel channel = Channel::GENERAL;
        bool writeDocType = true;
    };

    struct Svg
    {
        RootNode root;

        void clear();
        void loadFile(const std::filesystem::path& s);
        void optimize(const OptSets& sets);
        std::string saveString(const SaveSets& sets);
        void saveFile(const std::filesystem::path& s,
                      const SaveSets& sets);
    };

}   // namespace xs

// Inner, used for testing only
namespace xsin {

    struct NsInfo {
        std::string prefix {};
        std::string triggerAttr {};
        bool isNsFound = false;

        std::string_view launderAttr(std::string_view name) const noexcept;
        std::string_view launderObj(std::string_view name) const noexcept;
    };

    constexpr std::string_view NS_SVG = "http://www.w3.org/2000/svg";
    NsInfo detectNamespace(pugi::xml_node node, std::string_view url = NS_SVG);
    void startAttr(std::string& dest, std::string_view key);
    void encodeAttr(std::string& dest, std::string_view value);
    void writeAttrIf(std::string& dest, std::string_view key, std::string_view value);
    void writeAttr(std::string& dest, std::string_view key, std::string_view value);

}   // namespace xsin
