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
