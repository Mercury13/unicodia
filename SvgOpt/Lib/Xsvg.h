#pragma once

#include <vector>
#include <memory>
#include <filesystem>

namespace pugi {
    class xml_node;
}

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

    enum class DoesDraw : unsigned char {
        NO,         ///< Does not draw, never: <linearGradient>
        MAYBE,      ///< Maybe draws, check children: <g>
        YES,        ///< Yep, draws: <path>
        SPECIAL,    ///< Special rules: <defs>
    };

    class Node {
    public:
        virtual std::string_view name() const noexcept = 0;
        virtual DoesDraw doesDraw() const noexcept = 0;

        struct StdAttrs {
            std::string id;
            std::string fill;
            std::string style;
            std::string transform;
        } sa;
        std::vector<Attr> attrs;
        std::vector<std::unique_ptr<Node>> children;
        NodeChannel channel = NodeChannel::BOTH;

        void write(std::string& dest, Channel channel);
        static void writeAttrIf(std::string& dest, std::string_view key, std::string_view value);
        static void writeAttr(std::string& dest, std::string_view key, std::string_view value);
        static void encodeAttr(std::string& dest, std::string_view x);
    private:
    };

    class RootNode : public Node {
    public:
        virtual std::string_view name() const noexcept override
            { return "svg"; }
        virtual DoesDraw doesDraw() const noexcept override
            { return DoesDraw::MAYBE; }
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

}   // namespace xsin
