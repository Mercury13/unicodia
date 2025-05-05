#pragma once

// Kage
#include <2d.h>
#include <vec.h>

// STL
#include <string>

namespace kage {

    constexpr unsigned correctOff(unsigned x) noexcept { return (x >= 2) ? 0 : x; }

    class Off {
    public:
        constexpr Off() = default;
        constexpr Off(unsigned x) noexcept : vv(correctOff(x)) {}
        unsigned v() const noexcept { return vv; }
        operator unsigned() const noexcept { return vv; }
    private:
        unsigned vv = 0;
    };

    struct PolyPoint {
    public:
        Float x = 0, y = 0;
        Off off;

        Point<Float> pt() const noexcept { return {x, y}; }
    };

    class PtTransformer {
    public:
        virtual void go(PolyPoint& x) = 0;
        virtual ~PtTransformer() = default;
    };

    class PtIdentity final : public PtTransformer {
    public:
        virtual void go(PolyPoint&) override {}
        static PtIdentity INST;
    };

    class PtFixupAndCheck : public PtTransformer {
    public:
        virtual void go(PolyPoint&) override;
        static PtFixupAndCheck INST;
    };

    class Polygon : public Vec<PolyPoint>
    {
    public:
        using Vec<PolyPoint>::Vec;

        void push(Float x, Float y, unsigned off) { data.emplace_back(x, y, off); }

        template <Lesser<Float> T>
        void push(Point<T> p, unsigned off)   { data.emplace_back(p.x, p.y, off); }

        void set(size_t i, Float x, Float y, unsigned off);

        void convertToFont1000();
        void fixupAndCheck();

        void doTransform(PtTransformer& tr);
        PolyPoint transformed(size_t i, PtTransformer& tr) const;
        std::string svgPath(PtTransformer& tr = PtIdentity::INST) const;
    };

}   // namespace kage
