#pragma once

#include "polygon.h"

namespace kage {

    ///
    ///  former Polygons
    ///
    class Drawing : public Vec<Polygon>
    {
    public:
        Polygon& push(const Polygon& x);
        Polygon& push(Polygon&& x);

        std::string generateSvg() const;
    };

}
