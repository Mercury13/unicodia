#include "Xsvg.h"

#include "pugixml.hpp"
#include "u_Strings.h"

using namespace std::string_view_literals;

constexpr std::string_view NS_PREF = "xmlns:";
constexpr auto NS_PREF_LEN = NS_PREF.length();

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

}   // anon namespace

void xs::Svg::loadFile(const std::filesystem::path& s)
{
    pugi::xml_document doc;
    auto res = doc.load_file(s.native().c_str());
    if (res.status != pugi::status_ok) {
        throw std::logic_error(res.description());
    }
    auto root = doc.root().first_child();
    LoadContext context;
    context.ns = xsin::detectNamespace(root);
}
