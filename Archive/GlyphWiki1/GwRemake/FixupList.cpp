#include "FixupList.h"

// XML
#include "pugixml.hpp"

///// Glyph ////////////////////////////////////////////////////////////////////


void fix::Glyph::checkUsage(const std::string& name, int scale)
{
    for (auto& [key, value] : points) {
        if (!value.wasUsed) {
            auto xBefore = static_cast<double>(value.before.x) / scale;
            auto yBefore = static_cast<double>(value.before.y) / scale;
            char buf[100];
            snprintf(buf, std::size(buf),
                     "Glyph '%s' point (%g %g) unused",
                     name.c_str(), xBefore, yBefore);
            throw std::logic_error(buf);
        }
    }
}


///// List /////////////////////////////////////////////////////////////////////

void fix::List::clear()
{
    glyphs.clear();
}


namespace {

    constexpr double NO_VALUE = -1000;
    g2::Ivec compassInfo(std::string_view x)
    {
        g2::Ivec r;
        for (auto c : x) {
            switch (c) {
            case 'n':
            case 'N': --r.y; break;
            case 's':
            case 'S': ++r.y; break;
            case 'w':
            case 'W': --r.x; break;
            case 'e':
            case 'E': ++r.x; break;
            default:;
            }
        }
        return r;
    }

}


void fix::List::load(const std::filesystem::path& fname, int scale)
{
    clear();
    if (!std::filesystem::exists(fname))
        return;

    pugi::xml_document doc;
    doc.load_file(fname.c_str());

    auto root = doc.root();
    auto hFixup = root.child("fixup");
    for (auto hGlyph : hFixup.children("glyph")) {
        std::string_view name = hGlyph.attribute("name").as_string();
        if (!name.empty()) {
            auto [it, wasIns] = glyphs.emplace(name, std::in_place);
            auto& glyph = it->second;
            for (auto hPoint : hGlyph.children("point")) {
                auto x = hPoint.attribute("x").as_double(NO_VALUE);
                auto y = hPoint.attribute("y").as_double(NO_VALUE);
                g2::Ivec vec = compassInfo(hPoint.attribute("dir").as_string());
                std::string_view action = (hPoint.attribute("action").as_string());
                if (x != NO_VALUE && y != NO_VALUE) {
                    auto xscaled = lround(x * scale);
                    auto yscaled = lround(y * scale);
                    if (vec != g2::ZEROVEC) {
                        auto it2 = glyph.points.emplace(xscaled, std::in_place);
                        auto& point = it2->second;
                        point.before.x = xscaled;
                        point.before.y = yscaled;
                        point.after = point.before + vec;
                        point.action = Action::FIXUP;
                    } else if (action == "del") {
                        auto it2 = glyph.points.emplace(xscaled, std::in_place);
                        auto& point = it2->second;
                        point.before.x = xscaled;
                        point.before.y = yscaled;
                        point.action = Action::DELETE;
                    }
                }
            }
        }
    }
}


fix::Glyph* fix::List::find(std::string_view x)
{
    auto it = glyphs.find(x);
    if (it == glyphs.end())
        return nullptr;
    return &it->second;
}


void fix::List::checkUsage(int scale)
{
    for (auto& [name, glyph] : glyphs) {
        glyph.checkUsage(name, scale);
    }
}
