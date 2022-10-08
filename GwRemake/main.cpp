// STL
#include <iostream>
#include <filesystem>
#include <vector>
#include <algorithm>

// XML
#include "pugixml.hpp"

// Geo2d
#include "g2svgUtils.h"

// Current
#include "FixupList.h"

using namespace std::string_view_literals;

#define PATH_WORK "../Font-Source/GlyphWiki/"
constexpr std::string_view DIR_IN = PATH_WORK "Raw";
constexpr std::string_view DIR_OUT = PATH_WORK "AutoRemade";

struct Baddy {
    std::string fname;
    std::string problem;

    Baddy(std::string aFname, std::string aProblem)
        : fname(std::move(aFname)), problem(std::move(aProblem)) {}
};

void printBaddies(const std::vector<Baddy>& x)
{
    std::cout << "GOT " << x.size() << " BADDIES" << std::endl;
    for (auto& v: x) {
        std::cout << v.fname << ": " << v.problem << std::endl;
    }
}

namespace {

    void doFixup(g2sv::Polyline& v, fix::Glyph& fixup)
    {
        // Check for single-point fixups
        for (auto& pt : v.pts) {
            auto [beg, end] = fixup.points.equal_range(pt.x);
            for (auto p = beg; p != end; ++p) {
                if (p->second.before.y == pt.y) {
                    pt = p->second.after;
                    p->second.wasUsed = true;
                    break;
                }
            }
        }
    }


    struct MyPolyPath : public g2sv::Polypath
    {
    public:
        pugi::xml_node node;

        void remakeAlone(
                const g2sv::SimplifyOpt& simopt,
                fix::Glyph* glyph);
    };

    void MyPolyPath::remakeAlone(
            [[maybe_unused]] const g2sv::SimplifyOpt& simopt,
            fix::Glyph* glyph)
    {
        removeByDiameter(simopt.minDiameter);
        for (auto& v : curves) {
            v.removeRepeating();
            v.removeBackForth(simopt.sharpCos);
            if (glyph)
                doFixup(v, *glyph);
            v.removeShortSegments(simopt);
            v.removeRepeating();    // short segments can make repeats, so do once again
            v.removeBackForth(simopt.sharpCos);
        }
        simplify(simopt);
    }

    struct Svg
    {
    public:
        std::vector<MyPolyPath> paths;

        void clear();
        void loadFrom(pugi::xml_document& document);
        void remake(fix::Glyph* glyph);
        void write();
    private:
        void loadFromRec(pugi::xml_node node);
    };

    void Svg::clear()
    {
        paths.clear();
    }

    /// @todo [urgent] 31643: There are two nearby close points, corner and smooth.
    ///        They round in such a fashion…
    ///       ____ c               c
    /// --- s     /    =>   ------+s
    ///          /               /
    ///        What to do?
    ///
    constexpr int MY_SCALE = 5;
    constinit const g2sv::SimplifyOpt simopt {
        .tolerance = 3 * MY_SCALE,
        .minDiameter = 3 * MY_SCALE,
        .tangentTolerance = 1.7,    // 1 smallest unit horz/diag
        .sharpCos = 0.997,          // <5°
        .scale = MY_SCALE,
        .corner = {
            .minCosine = -0.64,  // <130°
            .maxSide = 13 * MY_SCALE,
            .smoothCosine = -0.9   // ≈154°
        }
    };

    void Svg::loadFromRec(pugi::xml_node node) {
        switch (node.type()) {
        case pugi::node_document:
        case pugi::node_element:
            break;
        default:
            return;
        }
        if (node.name() == "path"sv) {
            auto data = node.attribute("d").as_string();
            auto& curve = paths.emplace_back();
            curve.node = node;
            curve.parse(data, simopt.scale);
        } else {
            for (auto v : node.children()) {
                loadFromRec(v);
            }
        }
    }

    void Svg::loadFrom(pugi::xml_document& document)
    {
        clear();
        auto node = document.root();
        loadFromRec(node);
    }

    void Svg::remake(fix::Glyph* glyph)
    {
        for (auto& v : paths) {
            v.remakeAlone(simopt, glyph);
        }
    }

    void Svg::write()
    {
        for (auto& v : paths) {
            v.node.attribute("d") = v.svgData(simopt.scale).c_str();
        }
    }
}

void remakeSvg(
        const std::filesystem::path& fnIn,
        const std::filesystem::path& fnOut,
        fix::Glyph* glyph)
{
    pugi::xml_document doc;
    doc.load_file(fnIn.c_str());

    Svg svg;
    svg.loadFrom(doc);
    svg.remake(glyph);
    svg.write();

    doc.save_file(fnOut.c_str());
}

int main()
{
    std::filesystem::path pathWork(PATH_WORK);
    auto fnFixup = pathWork / "fixup.xml";
    fix::List fixups;
    fixups.load(fnFixup, simopt.scale);

    std::filesystem::directory_iterator itIn(DIR_IN);
    std::filesystem::path pathOut(DIR_OUT);
    std::vector<Baddy> baddies;
    try {
        int nFiles = 0;
        for (auto& entryIn : itIn) {
            auto& fnIn = entryIn.path();
            auto fnFile = fnIn.filename();
            auto extFile = fnFile.extension();
            if (extFile == ".svg") {
                ++nFiles;
                std::cout << "#" << nFiles << ": " << fnFile.generic_string() << std::endl;
                auto fnOut = pathOut / fnFile;
                try {
                    remakeSvg(fnIn, fnOut, fixups.find(fnFile.generic_string()));
                } catch (const g2sv::ESvg& e) {
                    baddies.emplace_back(fnFile.generic_string(), e.what());
                } catch (...) {
                    throw;
                }
            }
        }
    } catch (const std::exception& e) {
        std::cout << "GOT ERROR: " << e.what() << std::endl;
    }
    printBaddies(baddies);

    return 0;
}
