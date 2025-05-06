#pragma once

#include "polygon.h"
#include "util.h"

namespace kage {

    ///
    ///  former Polygons
    ///
    class Drawing : public List<Polygon>
    {
    public:
        Polygon& push(const Polygon& x);
        Polygon& push(Polygon&& x);
        Polygon& push() { return data.emplace_back(); }

        std::string generateSvg() const;

        void flipLeftRight(const Box<Float>& box);
        void drawLine(Point<int> p1, Point<int> p2, Float halfwidth);
    };

}
