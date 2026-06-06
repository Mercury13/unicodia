#include "Xsvg.h"

#include "pugixml.hpp"
#include "u_Strings.h"

using namespace std::string_view_literals;

constexpr std::string_view NS_PREF = "xmlns:";
constexpr auto NS_PREF_LEN = NS_PREF.length();

namespace {

    inline bool noMoreColon(std::string_view x, size_t p)
    {
        return (x.find(':', p) == std::string_view::npos);
    }

}   // anon namespace

std::string_view xsin::NsInfo::launderAttr(std::string_view name) const noexcept
{
    if (name.empty() || name == triggerAttr)
        return name;
    auto p = name.find(':');
    if (p == std::string_view::npos)
        return name;
    if ((p + 1 == prefix.size())
            && name.starts_with(prefix)
            && noMoreColon(name, prefix.size())) {
        return name.substr(prefix.size());
    }
    return {};
}

std::string_view xsin::NsInfo::launderObj(std::string_view name) const noexcept
{
    if (prefix.empty()) {  // No prefix
        if (name.find(':') == std::string_view::npos) {
            return name;
        } else {
            return {};
        }
    } else {  // Have prefix
        if (name.starts_with(prefix)
                && noMoreColon(name, prefix.size())) {
            return name.substr(prefix.size());
        } else {
            return {};
        }
    }
}

xsin::NsInfo xsin::detectNamespace(pugi::xml_node node, std::string_view url)
{
    for (const auto& q : node.attributes()) {
        std::string_view name = q.name();
        if (name == "xmlns"sv) {  // xmlns=
            if (q.value() == url)
                return {
                    .triggerAttr = "xmlns",
                    .isNsFound = true,
                };
        } else if (name.length() > NS_PREF_LEN
                   && name.starts_with(NS_PREF)) {  // xmlns:q=
            if (q.value() == url) {
                return {
                    .prefix = str::cat(name.substr(NS_PREF_LEN), ':' ),
                    .triggerAttr { name },
                    .isNsFound = true
                };
            }
        }
    }
    return {};
}

namespace {

    struct LoadContext {
        xsin::NsInfo ns;
    };

    void loadRecurse(
            xs::Node& dest,
            pugi::xml_node src,
            const LoadContext& context)
    {
        // Attributes
        for (auto& v : src.attributes()) {
            std::string_view name = v.name();
            if (auto lau = context.ns.launderAttr(name);
                    !lau.empty()) {
                dest.attrs.emplace_back(std::string{lau}, v.value());
            }
        }
        // Objects
        for (auto& v : src.children()) {
            std::string_view name = v.name();
            if (auto lau = context.ns.launderObj(name);
                    !lau.empty()) {
                auto& bk = dest.children.emplace_back(std::make_unique<xs::Node>());
                bk->name = lau;
                loadRecurse(*bk, v, context);
            }
        }
    }

}   // anon namespace

void xs::Svg::loadFile(const std::filesystem::path& s)
{
    pugi::xml_document doc;
    auto res = doc.load_file(s.native().c_str());
    if (res.status != pugi::status_ok) {
        throw std::logic_error(res.description());
    }
    auto xroot = doc.root().first_child();
    LoadContext context;
    context.ns = xsin::detectNamespace(xroot);
    if (xroot.name() != context.ns.prefix + "svg") {
        throw std::logic_error("SVG's root must be <svg>");
    }
    loadRecurse(root, xroot, context);
}
