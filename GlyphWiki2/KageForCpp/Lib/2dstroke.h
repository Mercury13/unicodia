#pragma once

#include "2d.h"

// STL
#include <span>
#include <string>

namespace kage {

    struct Ends {
        Point<int> start, end;
    };

    struct Stroke {
        int v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10;

        Point<int> p3_4()  const noexcept { return { v3, v4  }; }
        Point<int> p5_6()  const noexcept { return { v5, v6  }; }
        Point<int> p7_8()  const noexcept { return { v7, v8  }; }
        Point<int> p9_10() const noexcept { return { v9, v10 }; }
        std::optional<Ends> ends() const noexcept;
    };

    bool isCrossBoxWithOthers(
            std::span<Stroke> strokesArray,
            unsigned i, Point<int> b1, Point<int> b2);

    bool isCrossWithOthers(
            std::span<Stroke> strokesArray,
            unsigned i, Point<int> b1, Point<int> b2);

} // namespace kage
