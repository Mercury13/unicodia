#pragma once

#include <vector>
#include <memory>
#include <filesystem>

namespace pugi {
    class xml_node;
}

namespace xs {

    enum class Channel : unsigned char {
        ALWAYS, ONE, TWO };
    enum class NodeChannel : unsigned char {
        ALWAYS, ONE, TWO, ONE_TRANSP, TWO_TRANSP
    };

    struct Attr {
        std::string key, value;
        Channel channel = Channel::ALWAYS;
    };

    struct Node {
        std::string name;
        std::vector<Attr> attrs;
        std::vector<std::unique_ptr<Node>> children;
        NodeChannel channel = NodeChannel::ALWAYS;
    };

    struct Svg
    {
        Node root;

        void clear();
        void loadFile(const std::filesystem::path& s);
        std::string saveString(Channel channel);
        void saveFile(const std::filesystem::path& s, Channel channel = Channel::ONE);
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
