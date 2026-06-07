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

    struct Node {
        std::string name;
        std::vector<Attr> attrs;
        std::vector<std::unique_ptr<Node>> children;
        NodeChannel channel = NodeChannel::BOTH;

        void write(std::string& dest, Channel channel);
        static void encodeAttr(std::string& dest, std::string_view x);
    private:
    };

    struct SaveSets {
        Channel channel = Channel::GENERAL;
        bool writeDocType = true;
    };

    struct Svg
    {
        Node root;

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
