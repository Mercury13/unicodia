#pragma once

#include "2d.h"
#include "2dstroke.h"

#include <cmath>

namespace kage {

    Float norm2(Float x, kage::Float y) { return std::hypot(x, y); }

    Float widfun(Float t, Point<Float> a, Point<Float> b, Float wid);
    Float widfun_d(Float t, Point<Float> a, Point<Float> b, Float wid);
    Float widfun_stop(Float t, Point<Float> a, Point<Float> b, Float wid);
    Float widfun_stop_d(Float t, Point<Float> a, Point<Float> b, Float wid);
    Float widfun_stop2(Float t, Point<Float> a, Point<Float> b, Float wid);
    Float widfun_stop2_d(Float t, Point<Float> a, Point<Float> b, Float wid);
    Float widfun_fat(Float t, Point<Float> a, Point<Float> b, Float wid);
    Float widfun_fat_d(Float t, Point<Float> a, Point<Float> b, Float wid);

    struct Vec1 {
        Float cos, sin;
    };
    Vec1 getDir(Float x, Float y);

    constexpr Vec1 DIR_POSX { .cos =  1, .sin =  0 };
    constexpr Vec1 DIR_POSY { .cos =  0, .sin =  1 };
    constexpr Vec1 DIR_NEGX { .cos = -1, .sin =  0 };
    constexpr Vec1 DIR_NEGY { .cos =  0, .sin = -1 };

    Point<Float> movedPoint(Point<Float> p, Vec1 dir, Float delta);
    Point<Float> getExtendedDest(Point<Float> dest, Point<Float> src, Float delta);
    Point<Float> getExtendedDestWrong(Point<Float> dest, Point<Float> src, Float delta);

    Point<Float> unitNormalVector(Float ix, Float iy);

}   // namespace kage
