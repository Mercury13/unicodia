// My header
#include "g2svgUtils.h"

// C++
#include <cmath>
#include <charconv>

// STL
#include <algorithm>
//#include <iostream>


///// PathParser ///////////////////////////////////////////////////////////////


g2sv::PathParser::PathParser(std::string_view data) noexcept
    : p(data.data()), end(data.data() + data.length()) {}


bool g2sv::PathParser::skipSpaces() noexcept
{
    while (p != end) {
        if (static_cast<unsigned char>(*p) > ' ')
            return true;
        ++p;
    }
    return false;
}

namespace {

    bool latIsLetter(char v) {
        return ((v >= 'A' && v <= 'Z') || (v >= 'a' && v <= 'z'));
    }

    bool isStartingNumChar(char v)
    {
        switch (v) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '-':
        case '.':
            return true;
        default:
            return false;
        }
    }

    bool isNumChar(char v)
    {
        switch (v) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '-':
        case '.':
        case '+':
        case 'e':
        case 'E':
            return true;
        default:
            return false;
        }
    }
}


char g2sv::PathParser::getCommand()
{
    char buf[64];
    if (!skipSpaces())
        return 0;
    auto v = *p;
    if (!latIsLetter(v)) {
        snprintf(buf, std::size(buf),
                 "[SVG] Command expected (%c)", v);
        throw ESvg(buf);
    }
    ++p;
    return v;
}


double g2sv::PathParser::getNum(char command)
{
    char buf[64];
    if (!skipSpaces()) {
        snprintf(buf, std::size(buf),
                 "[SVG cmd %c] Number expected, end found", command);
        throw ESvg(buf);
    }

    auto start = p;
    if (!isStartingNumChar(*start)) {
        snprintf(buf, std::size(buf),
                 "[SVG cmd %c] Number expected, smth else (%c) found",
                 command, *start);
        throw ESvg(buf);
    }
    ++p;

    while (p != end && isNumChar(*p)) {
        ++p;
    }
    double x;
    auto res = std::from_chars(start, p, x);
    if (res.ec != std::errc()) {
        snprintf(buf, std::size(buf),
                 "[SVG cmd %c] Cannot parse number %.*s",
                 command, static_cast<int>(p - start), start);
        throw ESvg("");
    }
    return x;
}


int g2sv::PathParser::getInum(char command, int scale)
{
    return std::round(getNum(command) * scale);
}


///// Polyline /////////////////////////////////////////////////////////////////


bool g2sv::Polyline::removeRepeating()
{
    if (pts.size() < 2)
        return false;

    auto itEnd = std::unique(pts.begin(), pts.end());
    if (itEnd != pts.begin()) {
        pts.erase(itEnd, pts.end());
        return true;
    } else {
        return false;
    }
}


///// Polypath /////////////////////////////////////////////////////////////////


void g2sv::Polypath::parse(std::string_view text, int scale)
{
    dataOverride.clear();
    curves.clear();

    g2::Ipoint turtle;
    Polyline* path = nullptr;

    PathParser parser(text);
    auto moveTurtleAbs = [&turtle,&parser, scale](char command) {
        turtle.x = parser.getInum(command, scale);
        turtle.y = parser.getInum(command, scale);
    };
    auto moveTurtleRel = [&turtle,&parser, scale](char command) {
        turtle.x += parser.getInum(command, scale);
        turtle.y += parser.getInum(command, scale);
    };
    auto rqPath = [&path](const char* x) {
        if (!path)
            throw ESvg(x);
    };

    while (true) {
        auto command = parser.getCommand();
        if (command == 0)
            break;
        switch (command) {
        case 'A':
        case 'a':
            throw ESvg("Arc commands are unsupported");
        case 'C':
        case 'c':
            throw ESvg("Curve commands are unsupported");
        case 'Q':
        case 'q':
            throw ESvg("Quadratic curve commands are unsupported");
        case 'S':
        case 's':
            throw ESvg("Symmetrical curve commands are unsupported");
        case 'T':
        case 't':
            throw ESvg("Tied curve commands are unsupported");
        case 'M':
            moveTurtleAbs(command);
            path = &curves.emplace_back();
            path->pts.push_back(turtle);
            break;
        case 'm':
            moveTurtleRel(command);
            path = &curves.emplace_back();
            path->pts.push_back(turtle);
            break;
        case 'L':
            rqPath("L (abs line) command: no path to move turtle");
            moveTurtleAbs(command);
            path->pts.push_back(turtle);
            break;
        case 'l':
            rqPath("l (rel line) command: no path to move turtle");
            moveTurtleRel(command);
            path->pts.push_back(turtle);
            break;
        case 'H':
            rqPath("L (abs horz) command: no path to move turtle");
            turtle.x = parser.getInum(command, scale);
            path->pts.push_back(turtle);
            break;
        case 'h':
            rqPath("h (rel horz) command: no path to move turtle");
            turtle.x += parser.getInum(command, scale);
            path->pts.push_back(turtle);
            break;
        case 'V':
            rqPath("V (abs vert) command: no path to move turtle");
            turtle.y = parser.getInum(command, scale);
            path->pts.push_back(turtle);
            break;
        case 'v':
            rqPath("h (rel vert) command: no path to move turtle");
            turtle.y += parser.getInum(command, scale);
            path->pts.push_back(turtle);
            break;
        case 'Z':
        case 'z':
            rqPath("Z (close curve) command: no path to close curve");
            turtle = path->pts[0];  // where does turtle go?
            path->isClosed = true;
            path = nullptr;
            break;
        default:
            throw std::logic_error("Unknown turtle graphics command");
        }
    }
}


namespace {

    void appendCommand(std::string& s, char cmd)
    {
        if (!s.empty())
            s += ' ';
        s += cmd;
    }

    void appendNumber(std::string& s, int num, int scale)
    {
        auto raw = static_cast<double>(num) / scale;
        char buf[30];
        snprintf(buf, std::size(buf), " %.5g", raw);
        s += buf;
    }

}   // anon namespace


std::string g2sv::Polypath::svgData(int scale) const
{
    if (!dataOverride.empty())
        return dataOverride;

    std::string r;
    for (auto& v : curves) {
        if (!v.pts.empty()) {
            bool isFirst = true;
            for (auto& pt : v.pts) {
                appendCommand(r, isFirst ? 'M' : 'L');
                appendNumber(r, pt.x, scale);
                appendNumber(r, pt.y, scale);
                isFirst = false;
            }
            if (v.isClosed)
                appendCommand(r, 'Z');
        }
    }
    return r;
}
