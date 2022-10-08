#include "FixupList.h"

// XML
#include "pugixml.hpp"


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
                if (x != NO_VALUE && y != NO_VALUE && vec != g2::ZEROVEC) {
                    auto xscaled = lround(x * scale);
                    auto yscaled = lround(y * scale);
                    auto it2 = glyph.points.emplace(xscaled, std::in_place);
                    auto& point = it2->second;
                    point.before.x = xscaled;
                    point.before.y = yscaled;
                    point.after = point.before + vec;
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
