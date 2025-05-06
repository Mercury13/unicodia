#pragma once

#include "drawing.h"
#include "2dstroke.h"

namespace kage {

    class Style {
    public:
        virtual Drawing draw(std::span<const Stroke> strokes) const = 0;
        virtual ~Style() = default;
    };

}
