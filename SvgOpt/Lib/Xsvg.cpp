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


std::string_view xsin::NsInfo::launderAttr(
        std::string_view name, Place place) const noexcept
{
    if (name.empty())
        return {};
    if (name == triggerAttr && place == Place::ROOT)
        return "xmlns"sv;
    if (name == "xmlns"sv)  // When a different NS, "xmlns" is bad
        return {};
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


///// Node /////////////////////////////////////////////////////////////////////


void xs::Node::recurseBasicOptimizations(const OptSets& sets)
{
    for (auto& v : children) {
        if (v) {
            v->recurseBasicOptimizations(sets);
        }
    }
    basicOptimizations(sets);
}


bool xs::Node::trySpecificAttr(std::string_view key, std::string_view value)
{
    switch (key[0]) {
    case 'f':
        if (key == "fill"sv) {
            sa.fill.parse(value);
            return true;
        } else if (key == "fill-rule"sv) {
            sa.fillRule.parse(value);
            return true;
        }
        break;
    case 'i':
        if (key == "id"sv) {
            sa.id = value;
            return true;
        } break;
    case 's':
        if (key == "style") {
            sa.style.parse(value);
            return true;
        } break;
    case 't':
        if (key == "transform") {
            sa.transform = value;
            return true;
        } break;
    }
    return false;
}


void xs::Node::basicOptimizations(const OptSets&)
{
    // This optimization (style > inline) is automatic
    if (sa.style.fill && sa.fill)
        sa.fill.clear();
}


void xs::Node::writeSpecificAttrs(std::string& dest)
{
    xsin::writeAttrIf(dest, "id", sa.id);
    sa.fill.writeAttrIf(dest, "fill");
    sa.fill.writeAttrIf(dest, "fill-rule");
    sa.style.writeAttrIf(dest);
    xsin::writeAttrIf(dest, "transform", sa.transform);
}


void xs::Node::writeAttrs(std::string& dest)
{
    writeSpecificAttrs(dest);
    // Custom attrs
    for (auto& v : attrs) {
        xsin::writeAttr(dest, v.key, v.value);
    }
}

void xs::Node::write(std::string& dest, Channel channel)
{
    dest += "<";
    dest += name();
    writeAttrs(dest);
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
        dest += name();
        dest += '>';
    }
}


void xs::Node::deleteAttr(std::string_view key)
{
    auto q = std::ranges::find_if(attrs,
        [key](const Attr& v) -> bool {
            return (v.key == key);
        });
    if (q != attrs.end())
        attrs.erase(q);
}


void xs::Node::traverseRepeats(const MultiTypeCallback& x)
{
    x.onFill(sa.fill, sa.style.fill);
    x.onFillRule(sa.fillRule, sa.style.fillRule);
}


void xs::Node::removeOverriddenAttrs()
{
    traverseRepeatsT([](auto& a, auto& b) {
        if (b) {
            a = Inherit{};
        }
    });
}


///// RootNode /////////////////////////////////////////////////////////////////


void xs::RootNode::basicOptimizations(const OptSets& sets)
{
    Super::basicOptimizations(sets);
    if (sets.removeXmlns)
        saSvg.xmlns.clear();
    if (sets.removeVersion)
        saSvg.version.clear();
}


bool xs::RootNode::trySpecificAttr(std::string_view key, std::string_view value)
{
    if (Super::trySpecificAttr(key, value))
        return true;
    switch (key[0]) {
    case 'x':
        if (key == "xmlns") {
            saSvg.xmlns = std::string{value};
            return true;
        }
        return false;
    case 'v':
        if (key == "version") {
            saSvg.version = std::string{value};
            return true;
        }
        return false;
    }
    return false;
}


void xs::RootNode::writeSpecificAttrs(std::string& dest)
{
    xsin::writeAttrIf(dest, "xmlns", saSvg.xmlns);
    xsin::writeAttrIf(dest, "version", saSvg.version);

    Super::writeSpecificAttrs(dest);
}


///// Svg //////////////////////////////////////////////////////////////////////


namespace {

    std::unique_ptr<xs::Node> createNode(std::string_view name)
    {
        auto doesDraw = xs::DoesDraw::YES;
        switch (name[0]) {
        case 'd':
            if (name == "defs"sv) {
                doesDraw = xs::DoesDraw::SPECIAL;
            }
            break;
        case 'g':
            if (name == "g") {
                doesDraw = xs::DoesDraw::MAYBE;
            }
            break;
        case 'l':
            if (name == "linearGradient") {
                doesDraw = xs::DoesDraw::NO;
            }
            break;
        case 'r':
            if (name == "radialGradient") {
                doesDraw = xs::DoesDraw::NO;
            }
            break;
        }
        return std::make_unique<xs::FreeNode>(std::string{name}, doesDraw);
    }

    struct LoadContext {
        xsin::NsInfo ns;
    };

    void loadRecurse(
            xs::Node& dest,
            pugi::xml_node src,
            const LoadContext& context,
            xsin::Place place)
    {
        // Attributes
        for (auto& v : src.attributes()) {
            std::string_view name = v.name();
            if (auto lau = context.ns.launderAttr(name, place);
                    !lau.empty()) {
                std::string_view val = v.value();
                if (!dest.trySpecificAttr(lau, val)) {
                    dest.attrs.emplace_back(std::string{lau}, v.value());
                }
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
                        auto node = createNode(lau);
                        auto& bk = dest.children.emplace_back(std::move(node));
                        loadRecurse(*bk, v, context, xsin::Place::MISC);
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
    loadRecurse(root, xroot, context, xsin::Place::ROOT);
}

void xs::Svg::clear()
{
    root.saSvg.xmlns.clear();
    root.saSvg.version.clear();
    root.attrs.clear();
    root.children.clear();
    root.channel = NodeChannel::BOTH;
    root.sa = Node::StdAttrs{};
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

void xs::Svg::optimize(const OptSets& sets)
{
    root.recurseBasicOptimizations(sets);
}
