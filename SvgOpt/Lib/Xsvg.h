#pragma once

#include <vector>
#include <memory>
#include <filesystem>

#include "Xstyle.h"

namespace pugi {
    class xml_node;
}

namespace xs {

    enum class IdAction : unsigned char {
        LEAVE, CLEAN, CLEAN_SHRINK };

    enum class StyleToAttr : unsigned char {
        LEAVE,              ///< Do not change
        IF_SHORTER,         ///< If so is shorter
        IF_POSSIBLE,        ///< Everytime it's possible
        DISBAND_STYLE };    ///< Totally disband styles, pushing them to attrs

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

        /// [+] remove xmlns="....."
        /// Type: deviation from standard
        ///   As most software ceases to open, made FALSE
        bool removeXmlns = false;

        /// [+] remove version="1.1"
        /// Type: deviation from standard
        bool removeVersion = true;

        /// NOT IMPLEMENTED.
        /// coordinate precision, digits after point
        /// Type: lossy compression
        unsigned char coordPrecision = 2;
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

    class Node {
    public:
        virtual std::string_view name() const noexcept = 0;
        virtual DoesDraw doesDraw() const noexcept = 0;
        /// @return [+] was inserted to specific structures
        virtual bool trySpecificAttr(std::string_view key, std::string_view value);
        virtual void writeSpecificAttrs1(std::string& dest);
        void writeRepeatingAttrs(std::string& dest);
        virtual void writeSpecificAttrs2(std::string& dest);
        virtual void basicOptimizations(const OptSets& sets);
        /// a = stdattr, b = style
        virtual void traverseRepeats(const MultiTypeCallback& x);

        template <class U>
        void traverseRepeatsT(const U& u)
            { traverseRepeats(MultiTypeCallbackT<U>(u)); }
        void removeOverriddenAttrs();

        struct StdAttrs {
            std::string id;
            Fill fill { Inherit {} };
            MaybeFillRule fillRule { Inherit {} };
            Style style;
            std::string transform;
        } sa;
        std::vector<Attr> attrs;
        std::vector<std::unique_ptr<Node>> children;
        NodeChannel channel = NodeChannel::BOTH;

        void write(std::string& dest, Channel channel);
        void writeAttrs(std::string& dest);
        void recurseBasicOptimizations(const OptSets& sets);
        void deleteAttr(std::string_view key);
        void recurseStyleToAttrIfPossible();
    private:
    };

    class RootNode : public Node {
        using Super = Node;
    public:
        struct SaSvg {
            std::string xmlns {};
            std::string version {};
        } saSvg;

        bool trySpecificAttr(std::string_view key, std::string_view value) override;
        void writeSpecificAttrs1(std::string& dest) override;
        std::string_view name() const noexcept override
            { return "svg"; }
        DoesDraw doesDraw() const noexcept override
            { return DoesDraw::MAYBE; }
        void basicOptimizations(const OptSets& sets) override;
    };

    class StopNode : public Node {
        using Super = Node;
    public:
        struct SaStop {
            MaybeColor stopColor {};
        } saStop;
        std::string_view name() const noexcept override
            { return "stop"; }
        virtual DoesDraw doesDraw() const noexcept override
            { return DoesDraw::NO; }
        void traverseRepeats(const MultiTypeCallback& x) override;
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

    enum class Place : unsigned char { MISC, ROOT };

    struct NsInfo {
        std::string prefix {};
        std::string triggerAttr {};
        bool isNsFound = false;

        std::string_view launderAttr(
            std::string_view name, Place place) const noexcept;
        std::string_view launderObj(std::string_view name) const noexcept;
    };

    constexpr std::string_view NS_SVG = "http://www.w3.org/2000/svg";
    NsInfo detectNamespace(pugi::xml_node node, std::string_view url = NS_SVG);

}   // namespace xsin
