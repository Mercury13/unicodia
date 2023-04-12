#pragma once

// STL
#include <map>
#include <string>
#include <filesystem>

// Geo2d
#include "g2defines.h"

namespace fix {

    struct Point
    {
        g2::Ipoint before, after;
        bool wasUsed = false;

        Point() noexcept = default;
        Point(std::in_place_t) noexcept {}
    };

    struct Glyph
    {
        /// by X
        std::multimap<int, Point> points;
        Glyph() noexcept = default;
        Glyph(std::in_place_t) noexcept {}
        void checkUsage(const std::string& name, int scale);
    };

    struct CmpBySv {
        using is_transparent = void;
        bool operator()(std::string_view x, std::string_view y) const { return (x < y); }
    };

    struct List
    {
        std::map<std::string, Glyph, CmpBySv> glyphs;

        void load(const std::filesystem::path& fname, int scale);
        void clear();
        Glyph* find(std::string_view x);
        void checkUsage(int scale);
    };

}
