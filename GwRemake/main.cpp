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

struct Stat {
    std::string first;
    size_t count = 0;

    void reg(std::string_view charName);
    void print(std::string_view name);
};

void Stat::reg(std::string_view charName)
{
    if (count == 0)
        first = charName;
    ++count;
}

void Stat::print(std::string_view name)
{
    std::cout << name << ": ";
    if (count == 0) {
        std::cout << "none found";
    } else {
        std::cout << "found " << count << ", first " << first;
    }
    std::cout << std::endl;
}

struct Stats {
    Stat diameter, repeat, backForth, sharpShort, sharpLong, angle180;
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
                fix::Glyph* glyph,
                std::string_view charName,
                Stats* stats);
    };

    void MyPolyPath::remakeAlone(
            [[maybe_unused]] const g2sv::SimplifyOpt& simopt,
            fix::Glyph* glyph,
            std::string_view charName,
            Stats* stats)
    {
        bool wasRemovedByDiameter = removeByDiameter(simopt.error.minDiameter);
        bool wasRemovedRepeating = false;
        bool wasRemovedBackForth = false;
        bool wasRemovedSharpShort = false;
        bool wasRemovedSharpLong = false;
        bool wasRemoved180 = false;
        for (auto& v : curves) {
            wasRemovedRepeating |= v.removeRepeating();
            auto r = v.removeBackForth(simopt.sharpCos);
            wasRemovedBackForth |= (r.n0 != 0);
            wasRemovedSharpShort |= (r.nNear0Short != 0);
            wasRemovedSharpLong |= (r.nNear0Long != 0);
            wasRemoved180 |= (r.n180 != 0);
            if (glyph)
                doFixup(v, *glyph);
            v.removeShortSegments(simopt);
            v.removeRepeating();    // short segments can make repeats, so do once again
            v.removeBackForth(simopt.sharpCos);
        }
        simplify(simopt);

        if (stats) {
            if (wasRemovedByDiameter)
                stats->diameter.reg(charName);
            if (wasRemovedRepeating)
                stats->repeat.reg(charName);
            if (wasRemovedBackForth)
                stats->backForth.reg(charName);
            if (wasRemovedSharpShort)
                stats->sharpShort.reg(charName);
            if (wasRemovedSharpLong)
                stats->sharpLong.reg(charName);
            if (wasRemoved180)
                stats->angle180.reg(charName);
        }
    }

    struct Svg
    {
    public:
        std::vector<MyPolyPath> paths;

        void clear();
        void loadFrom(pugi::xml_document& document);
        void remake(fix::Glyph* glyph, std::string_view charName, Stats* stats);
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
        .sharpCos = 0.997,          // <5°
        .scale = MY_SCALE,
        .error {
            .fit = 2.5 * MY_SCALE,
            .qLine = 0.4,
            .minDiameter = 3 * MY_SCALE,
            // extremum — default
        },
        .corner {
            .minCosine = -0.64,  // <130°
            .maxSide = 13 * MY_SCALE,
            .smoothCosine = -0.9   // ≈154°
        },
        .tangent {
            .tolerance = 1.7,       // 1 smallest unit horz/diag
            .snapAngle = 0.07,      // ≈4°
            .nudgeAngle = 0.055,    // ≈3°
        },
        .guide {
            .triggerLength = 20 * MY_SCALE,
            .nearbyDist = 15 * MY_SCALE,
        },
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

    void Svg::remake(fix::Glyph* glyph, std::string_view charName, Stats* stats)
    {
        for (auto& v : paths) {
            v.remakeAlone(simopt, glyph, charName, stats);
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
        fix::Glyph* glyph,
        std::string_view charName,
        Stats* stats)
{
    pugi::xml_document doc;
    doc.load_file(fnIn.c_str());

    Svg svg;
    svg.loadFrom(doc);
    svg.remake(glyph, charName, stats);
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
    Stats stats;
    try {
        int nFiles = 0;
        for (auto& entryIn : itIn) {
            auto& fnIn = entryIn.path();
            auto fnFile = fnIn.filename();
            auto extFile = fnFile.extension();
            if (extFile == ".svg") {
                ++nFiles;
                auto charName = fnFile.generic_string();
                std::cout << "#" << nFiles << ": " << charName << std::endl;
                auto fnOut = pathOut / fnFile;
                auto pStats = (!charName.starts_with('!')) ? &stats : nullptr;
                try {
                    remakeSvg(fnIn, fnOut, fixups.find(charName), charName, pStats);
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
    stats.diameter.print("Small diameter");
    stats.repeat.print("Repeating points");
    stats.sharpShort.print("Short sharp corners");
    stats.sharpLong.print("Long sharp corners");
    stats.backForth.print("Back-forth strokes");
    stats.angle180.print("Angle 180 deg");

    return 0;
}
