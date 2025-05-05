#include "polygon.h"

// C++
#include <stdexcept>
#include <cmath>

// STL
#include <charconv>

// Kage
#include <defs.h>

kage::PtIdentity kage::PtIdentity::INST;
kage::PtFixupAndCheck kage::PtFixupAndCheck::INST;

// Check once again compiler’s features
static_assert(std::is_nothrow_default_constructible_v<kage::Off>);


void kage::Polygon::set(size_t i, Float x, Float y, unsigned off)
{
    auto& q = at(i);
    q.x = x;  q.y = y; q.off = off;
}

void kage::Polygon::convertToFont1000()
{
    for (auto& p : data) {
        p.x = p.x * 5;
        p.y = p.y * (-5) - 200;
    }
}

kage::PolyPoint kage::Polygon::transformed(size_t i, PtTransformer& tr) const
{
    auto r = at(i);
    tr.go(r);
    return r;
}

void kage::Polygon::doTransform(PtTransformer& tr)
{
    for (auto& v : data)
        tr.go(v);
}

namespace {

    kage::Float clamp(kage::Float x)
    {
        return std::min<kage::Float>(
                std::max<kage::Float>(x, kage::dim::MIN), kage::dim::MAX);
    }

}

void kage::PtFixupAndCheck::go(PolyPoint& p)
{
    if (!std::isfinite(p.x) || !std::isfinite(p.y))
        throw std::logic_error("[fixupAndCheck] Bad point’s X/Y");
    p.x = clamp(p.x);
    p.y = clamp(p.y);
}


void kage::Polygon::fixupAndCheck()
{
    if (size() < 3)
        throw std::logic_error("[fixupAndCheck] Too few points in polygon");
    doTransform(PtFixupAndCheck::INST);
}


namespace {

    void appendNum(char*& ptr, char* bigEnd, kage::Float num)
    {
        auto r = std::to_chars(ptr, bigEnd, num);
        ptr = r.ptr;
        if (ptr >= bigEnd)
            throw std::logic_error("[kage SVG] exceeded buffer, really bug");
    }

    void appendPoint(std::string& x, const kage::PolyPoint& point, char command)
    {
        char buf[100];
        // Build data in buffer, then append in one shot
        char* ptr = buf;
        char* bigEnd = std::end(buf);
        if (command != 0) {
            *(ptr++) = command;
        }
        appendNum(ptr, bigEnd, point.x);
        *(ptr++) = ',';
        appendNum(ptr, bigEnd, point.y);
        *(ptr++) = ' ';

        x.append(buf, ptr);
    }

}

std::string kage::Polygon::svgPath(PtTransformer& tr) const
{
    if (empty())
        return {};
    std::string r;
    auto aj = transformed(0, tr);
    appendPoint(r, aj, 'M');
    char mode = 0;
    for (size_t j = 1; j < size(); ++j) {
        auto aprev = aj;
        aj = transformed(j, tr);
        char command = 0;  // mode will stay, command will be reset
        switch (aj.off) {
        // Here we rely on the fact: too many points → repeat command
        case 0:
            if (aprev.off == 0 && mode != 'L') {
                command = mode = 'L';
            } break;
        case 1:
            if (mode != 'Q') {
                command = mode = 'Q';
            } break;
        case 2:
            if (aprev.off == 0 && mode != 'C') {
                command = mode = 'C';
            } break;
        default:;
        }
        appendPoint(r, aj, command);
    }
    r += 'Z';
    return r;
}


/*
  get_sub_path_svg_font(){
    let buffer = "";
    buffer += "M";
    buffer += (this.array[0].x*5.250 - 25) + "," + (916 - this.array[0].y*5.250) + " ";
    let mode = "";
    for(var j = 1; j < this.array.length; j++){
      if(this.array[j].off == 1 && mode != "Q"){
        buffer += "Q"; mode = "Q";
      } else if(this.array[j-1].off == 0 && this.array[j].off == 2 && mode != "C"){
        buffer += "C"; mode = "C";
      } else if(this.array[j-1].off == 0 && this.array[j].off == 0 && mode != "L"){
        buffer += "L"; mode = "L";
      }
      buffer += (this.array[j].x*5.250 - 25) + "," + (916 - this.array[j].y*5.250) + " ";
    }
    buffer += "Z";
    return buffer;
  }
*/
