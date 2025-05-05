#pragma once

// Kage
#include <2d.h>

// STL
#include <vector>
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

    class Polygon
    {
    private:
        using Vec = std::vector<PolyPoint>;
    public:
        Polygon() = default;
        Polygon(size_t n) : data(n) {};

        PolyPoint& operator[](size_t i) { return data.at(i); }
        const PolyPoint& operator[](size_t i) const { return data.at(i); }
        PolyPoint& at(size_t i) { return data.at(i); }
        const PolyPoint& at(size_t i) const { return data.at(i); }

        using iterator = Vec::iterator;
        using const_iterator = Vec::const_iterator;

        [[nodiscard]] auto begin()  noexcept { return data.begin(); }
        [[nodiscard]] auto end()    noexcept { return data.end(); }
        [[nodiscard]] auto begin() const noexcept { return data.begin(); }
        [[nodiscard]] auto end()   const noexcept { return data.end(); }
        [[nodiscard]] auto cbegin() const noexcept { return data.cbegin(); }
        [[nodiscard]] auto cend()   const noexcept { return data.cend(); }
        [[nodiscard]] bool empty() const noexcept { return data.empty(); }
        [[nodiscard]] bool isEmpty() const noexcept { return data.empty(); }
        [[nodiscard]] size_t size() const noexcept { return data.size(); }

        void push(Float x, Float y, unsigned off) { data.emplace_back(x, y, off); }

        template <Lesser<Float> T>
        void push(Point<T> p, unsigned off)   { data.emplace_back(p.x, p.y, off); }

        void set(size_t i, Float x, Float y, unsigned off);


        void convertToFont1000();
        void reverse();

        void append(const Polygon& poly);
        [[deprecated("Use append")]] void concat(const Polygon& poly) { return append(poly); }

        PolyPoint pop_front();
        [[deprecated("Use pop_front")]] auto shift() { return pop_front(); }

        /// @return new length
        size_t push_front(const PolyPoint& x);
        [[deprecated("Use pop_front")]] auto unshift(const PolyPoint& x) { return push_front(x); }

        PolyPoint transformed(size_t i, PtTransformer& tr) const;
        std::string svgPath(PtTransformer& tr = PtIdentity::INST) const;
        //std::string subPathSvgFont() const;
    private:
        Vec data;
    };

}   // namespace kage
