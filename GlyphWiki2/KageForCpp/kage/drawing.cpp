#include "drawing.h"

kage::Polygon& kage::Drawing::push(const Polygon& x)
{
    auto& back = data.emplace_back(x);
    back.fixupAndCheck();
    return back;
}


kage::Polygon& kage::Drawing::push(Polygon&& x)
{
    auto& back = data.emplace_back(std::move(x));
    back.fixupAndCheck();
    return back;
}


std::string kage::Drawing::generateSvg() const
{
    std::string r = "<svg xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" version=\"1.1\" baseProfile=\"full\" viewBox=\"0 0 200 200\" width=\"200\" height=\"200\">\n";
    for (auto& v : data) {
        r += "<path d=\"";
        r += v.svgPath();
        r += "\" fill=\"black\" />\n";
    }
    r += "</svg>\n";
    return r;
}

namespace {

    kage::Float round10 (kage::Float x)
    {
        return std::round(x * 10) * 0.1;
    }

}

void kage::Drawing::flipLeftRight(const Box<Float>& box)
{
    for (auto& vi : data) {
        bool isInside = true;
        for (const auto& vj : vi) {
            if (box.x1() > vj.x || vj.x > box.x2() || box.y1() > vj.y || vj.y > box.y2()) {
                isInside = false;
                break;
            }
        }
        if (isInside) {
            for (auto& vj : vi) {
                vj.x = round10(box.x2() - (vj.x - box.x1()));
            }
            vi.reverse();
        }
    }
}


namespace {

    class PointMaker
    {
    public:
        kage::Point<kage::Float> p;
        kage::Dir dir;
        kage::Float scale = 1;

        template <kage::Lesser<kage::Float> T>
        constexpr PointMaker(const kage::Point<T>& aP,
                kage::Dir aDir = kage::DIR_POSX, kage::Float aScale = 1)
            : p(aP), dir(aDir), scale(aScale) {}

        template <kage::Lesser<kage::Float> T>
        void moveTo(const kage::Point<T>& aP)
            { p = aP; }

        kage::Point<kage::Float> vec(kage::Float x, kage::Float y)
        {
            return { p.x + scale*dir.cos*x - scale*dir.sin*y,
                     p.y + scale*dir.sin*x + scale*dir.cos*y };
        }
          // setdir(dir){
          //   this.dir = dir;
          // }
          // setscale(scale){
          //   this.scale = scale;
          // }
    };
}


void kage::Drawing::drawLine(Point<int> p1, Point<int> p2, Float halfWidth)
{
    Polygon& poly = push();
    auto dir = (p2 - p1).dir();
    PointMaker p(p1, dir);
    poly.push(p.vec(0, halfWidth));
    poly.push(p.vec(0, -halfWidth));
    p.moveTo(p2);
    poly.push(p.vec(0, -halfWidth));
    poly.push(p.vec(0, halfWidth));
}
