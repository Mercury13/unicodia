#pragma once

#include "2d.h"
#include "2dstroke.h"

#include <cmath>

namespace kage {

    inline Float norm2(Float x, kage::Float y) { return std::hypot(x, y); }

    Float widfun(Float t, Point<Float> a, Point<Float> b, Float wid);
    Float widfun_d(Float t, Point<Float> a, Point<Float> b, Float wid);
    Float widfun_stop(Float t, Point<Float> a, Point<Float> b, Float wid);
    Float widfun_stop_d(Float t, Point<Float> a, Point<Float> b, Float wid);
    Float widfun_stop2(Float t, Point<Float> a, Point<Float> b, Float wid);
    Float widfun_stop2_d(Float t, Point<Float> a, Point<Float> b, Float wid);
    Float widfun_fat(Float t, Point<Float> a, Point<Float> b, Float wid);
    Float widfun_fat_d(Float t, Point<Float> a, Point<Float> b, Float wid);

    struct Dir {
        Float cos, sin;
        Float rad() const noexcept { return std::atan2(sin, cos); }
        static Dir ofRad(Float rad) noexcept { return { std::cos(rad), std::sin(rad) }; }
    };

    constexpr Dir DIR_POSX { .cos =  1, .sin =  0 };
    constexpr Dir DIR_POSY { .cos =  0, .sin =  1 };
    constexpr Dir DIR_NEGX { .cos = -1, .sin =  0 };
    constexpr Dir DIR_NEGY { .cos =  0, .sin = -1 };

    Point<Float> movedPoint(Point<Float> p, Dir dir, Float delta);
    Point<Float> getExtendedDest(Point<Float> dest, Point<Float> src, Float delta);
    Point<Float> getExtendedDestWrong(Point<Float> dest, Point<Float> src, Float delta);

    struct Vec {
        Float x, y;

        Float len() const noexcept { return std::hypot(x, y); }
        Vec toLen(Float wanted) const noexcept;
        Dir dir() const noexcept;
        Vec unitNormal() const noexcept;
        Float rad() const noexcept { return std::atan2(y, x); }

        static Vec ofRad(Float rad) noexcept { return { std::cos(rad), std::sin(rad) }; }
    };

    struct Ivec {
        int x, y;
        Dir dir() const noexcept;
        Float rad() const noexcept { return std::atan2(Float(y), Float(x)); }
    };

    Float calcHosomi(Point<Float> x, Point<Float> y);

    struct Bez {
        Point<Float> a, ha, hb, b;
        const Point<Float>& v0() const noexcept { return a; }
        const Point<Float>& v1() const noexcept { return ha; }
        const Point<Float>& v2() const noexcept { return hb; }
        const Point<Float>& v3() const noexcept { return b; }

        //kage::Vec toLine(Point<Float> p0, Float rad) const;
        template <class What>
        void doTransform(What& w) {
            w(a); w(ha); w(b); w(hb);
        }
    };

    template <Numeric T>
    struct Box {
        T minX, minY, maxX, maxY;

        T x1() const noexcept { return minX; }
        T x2() const noexcept { return maxX; }
        T y1() const noexcept { return minY; }
        T y2() const noexcept { return maxY; }

        template <Lesser<T> U>
        void intersectWith(Point<U> p);
    };

    Box<int> getBoundingBox(std::span<const Stroke> strokes);

}   // namespace kage


template <kage::Numeric T> template<kage::Lesser<T> U>
void kage::Box<T>::intersectWith(kage::Point<U> p)
{
    minX = std::min<T>(minX, p.x);
    maxX = std::max<T>(maxX, p.x);
    minY = std::min<T>(minY, p.y);
    maxY = std::max<T>(maxY, p.y);
}


inline kage::Vec operator - (kage::Point<kage::Float> a, kage::Point<kage::Float> b)
    { return { a.x - b.x, a.y - b.y }; }

inline kage::Ivec operator - (kage::Point<int> a, kage::Point<int> b)
    { return { a.x - b.x, a.y - b.y }; }
