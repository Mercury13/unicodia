#pragma once

#include <vector>
#include <memory>
#include <filesystem>

namespace pugi {
    class xml_node;
}

namespace xs {

    enum class AttrChannel : unsigned char {
        BOTH = 0, ONE = 1, TWO = 2 };
    enum class NodeChannel : unsigned char {
        BOTH = 0, ONE = 1, TWO = 2, ONE_TRANSP = 3, TWO_TRANSP = 4
    };
    enum class SaveChannel : unsigned char {
        ONE = 1, TWO = 2  };

    struct Attr {
        std::string key, value;
        AttrChannel channel = AttrChannel::BOTH;
    };

    struct Node {
        std::string name;
        std::vector<Attr> attrs;
        std::vector<std::unique_ptr<Node>> children;
        NodeChannel channel = NodeChannel::BOTH;
    };

    struct Svg
    {
        Node root;

        void clear();
        void loadFile(const std::filesystem::path& s);
        std::string saveString(SaveChannel channel);
        void saveFile(const std::filesystem::path& s, SaveChannel channel = SaveChannel::ONE);
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
