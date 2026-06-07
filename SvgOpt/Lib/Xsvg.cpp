#include "Xsvg.h"

#include "pugixml.hpp"
#include "u_Strings.h"

// STL
#include <fstream>

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
            switch (v.type()) {
            case pugi::node_null:
            case pugi::node_comment:
                break;
            case pugi::node_document:
                throw std::logic_error("PugiXml error, document in a strange place");
            case pugi::node_declaration:
                throw std::logic_error("PugiXml error, declaration in a strange place");
            case pugi::node_doctype:
                throw std::logic_error("SVG cannot have a doctype");
            case pugi::node_pi:
                throw std::logic_error("Processing instructions are unsupported");
            case pugi::node_element: {
                    std::string_view name = v.name();
                    if (auto lau = context.ns.launderObj(name);
                        !lau.empty()) {
                        auto& bk = dest.children.emplace_back(std::make_unique<xs::Node>());
                        bk->name = lau;
                        loadRecurse(*bk, v, context);
                    }
                } break;
            case pugi::node_pcdata:
            case pugi::node_cdata:
                throw std::logic_error("IDK what to do with text");
            }
        }
    }

}   // anon namespace

void xs::Svg::loadFile(const std::filesystem::path& s)
{
    clear();
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
    root.name = "svg";
    loadRecurse(root, xroot, context);
}

void xs::Svg::clear()
{
    root.name.clear();
    root.attrs.clear();
    root.children.clear();
    root.channel = NodeChannel::BOTH;
}


void xs::Node::encodeAttr(std::string& dest, std::string_view x)
{
    dest.reserve(dest.length() + x.length());
    for (auto c : x) {
        switch (c) {
        case '"':
            dest += "&quot;";
            break;
        default:
            dest += c;
            break;
        }
    }
}


void xs::Node::write(std::string& dest, Channel channel)
{
    dest += "<";
    dest += name;
    for (auto& v : attrs) {
        dest += ' ';
        dest += v.key;
        dest += R"(=")";
        encodeAttr(dest, v.value);
        dest += '"';
    }
    bool isTagOpen = true;
    for (auto& v : children) {
        if (!v)
            continue;
        if (isTagOpen) {
            dest += '>';
            isTagOpen = false;
        }
        v->write(dest, channel);
    }
    if (isTagOpen) {
        dest += "/>";
    } else {
        dest += "</";
        dest += name;
        dest += '>';
    }
}


std::string xs::Svg::saveString(const SaveSets& sets)
{
    std::string s;
    if (sets.writeDocType) {
        s += R"(<?xml version="1.0" encoding="UTF-8"?>)";
    }
    root.write(s, sets.channel);
    return s;
}

void xs::Svg::saveFile(
            const std::filesystem::path& s,
            const SaveSets& sets)
{
    auto str = saveString(sets);
    std::ofstream os(s, std::ios::binary);
    if (!os.is_open()) {
        throw std::logic_error("Cannot open file " + s.string());
    }
    os.write(str.data(), str.length());
}
