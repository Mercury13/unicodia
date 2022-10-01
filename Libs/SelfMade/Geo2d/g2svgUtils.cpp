// My header
#include "g2svgUtils.h"

// C++
#include <cmath>
#include <charconv>

// STL
#include <algorithm>
#include <optional>


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
    auto newSize = itEnd - pts.begin();
    // Closed line → check for first/last point
    if (isClosed && newSize > 1 && pts.front() == pts.back()) {
        --newSize;
    }
    if (static_cast<size_t>(newSize) != pts.size()) {
        pts.resize(newSize);
        return true;
    } else {
        return false;
    }
}


bool g2sv::Polyline::removeBackForth()
{
    static constexpr int NO_COORD = std::numeric_limits<int>::min();
    static constinit const g2::Ipoint BAD_VERTEX { NO_COORD, NO_COORD };
    auto n = pts.size();
    if (n < 4)
        return false;

    auto pPrev = &pts[0];
    auto pCurr = &pts[1];
    bool foundSmth = false;
    for (size_t i = 2; i < n; ++i) {
        auto pNext = &pts[i];
        auto vPrev = *pPrev - *pCurr;
        auto vNext = *pNext - *pCurr;
        // Erase collinear — both unneeded vertices in the middle of line,
        // and those horrible back-forth
        auto crossValue = vNext.cross(vPrev);
        if (crossValue == 0) {
            // Found!
            *pCurr = BAD_VERTEX;
            foundSmth = true;
        } else {    // Not found
            pPrev = pCurr;
        }
        pCurr = pNext;
    }

    if (foundSmth) {
        auto itEnd = std::remove(pts.begin(), pts.end(), BAD_VERTEX);
        pts.erase(itEnd, pts.end());
        // We do not check 1st and last, probably OK
        return true;
    } else {
        return false;
    }
}

namespace {
    constexpr double EPSILON = 1e-12;
    constexpr double MACHINE_EPSILON = 1.12e-16;

    /*
    const generateBezier = (points: readonly Point[], first: number, last: number, uPrime: readonly number[], tan1: Point, tan2: Point) => {
      const epsilon = EPSILON,
        abs = Math.abs,
        pt1 = points[first],
        pt2 = points[last],
        // Create the C and X matrices
        C = [
          [0, 0],
          [0, 0],
        ],
        X = [0, 0]

      for (let i = 0, l = last - first + 1; i < l; i++) {
        const u = uPrime[i],
          t = 1 - u,
          b = 3 * u * t,
          b0 = t * t * t,
          b1 = b * t,
          b2 = b * u,
          b3 = u * u * u,
          a1 = tan1._normalize(b1),
          a2 = tan2._normalize(b2),
          tmp = points[first + i]._subtract(pt1._multiply(b0 + b1))._subtract(pt2._multiply(b2 + b3))
        C[0][0] += a1._dot(a1)
        C[0][1] += a1._dot(a2)
        // C[1][0] += a1.dot(a2);
        C[1][0] = C[0][1]
        C[1][1] += a2._dot(a2)
        X[0] += a1._dot(tmp)
        X[1] += a2._dot(tmp)
      }

      // Compute the determinants of C and X
      const detC0C1 = C[0][0] * C[1][1] - C[1][0] * C[0][1]
      let alpha1
      let alpha2
      if (abs(detC0C1) > epsilon) {
        // Kramer's rule
        const detC0X = C[0][0] * X[1] - C[1][0] * X[0],
          detXC1 = X[0] * C[1][1] - X[1] * C[0][1]
        // Derive alpha values
        alpha1 = detXC1 / detC0C1
        alpha2 = detC0X / detC0C1
      } else {
        // Matrix is under-determined, try assuming alpha1 == alpha2
        const c0 = C[0][0] + C[0][1],
          c1 = C[1][0] + C[1][1]
        alpha1 = alpha2 = abs(c0) > epsilon ? X[0] / c0 : abs(c1) > epsilon ? X[1] / c1 : 0
      }

      // If alpha negative, use the Wu/Barsky heuristic (see text)
      // (if alpha is 0, you get coincident control points that lead to
      // divide by zero in any subsequent NewtonRaphsonRootFind() call.
      const segLength = pt2._getDistance(pt1),
        eps = epsilon * segLength
      let handle1, handle2
      if (alpha1 < eps || alpha2 < eps) {
        // fall back on standard (probably inaccurate) formula,
        // and subdivide further if needed.
        alpha1 = alpha2 = segLength / 3
      } else {
        // Check if the found control points are in the right order when
        // projected onto the line through pt1 and pt2.
        const line = pt2._subtract(pt1)
        // Control points 1 and 2 are positioned an alpha distance out
        // on the tangent vectors, left and right, respectively
        handle1 = tan1._normalize(alpha1)
        handle2 = tan2._normalize(alpha2)
        if (handle1._dot(line) - handle2._dot(line) > segLength * segLength) {
          // Fall back to the Wu/Barsky heuristic above.
          alpha1 = alpha2 = segLength / 3
          handle1 = handle2 = null // Force recalculation
        }
      }

      // First and last control points of the Bezier curve are
      // positioned exactly at the first and last data points
      return [pt1, pt1._add(handle1 || tan1._normalize(alpha1)), pt2._add(handle2 || tan2._normalize(alpha2)), pt2]
    }
    */

    struct Curve {
        g2::Ipoint a;
        g2::Dpoint ah;
        g2::Dpoint bh;
        g2::Ipoint b;
    };

    Curve generateBezier(
            const std::vector<g2::Ipoint>& points,
            size_t first, size_t last,
            const std::vector<double>& uPrime,
            const g2::Ivec& tan1, const g2::Ivec& tan2)
    {
        static constexpr double epsilon = EPSILON;
        auto& pt1I = points[first];
        auto& pt2I = points[last];
        auto pt1 = g2::Dvec::fromPt(pt1I);
        auto pt2 = g2::Dvec::fromPt(pt2I);
        double C[2][2] = { { 0, 0 }, { 0, 0 } };
        double X[2] = { 0, 0 };

        for (size_t i = 0, l = last - first + 1; i < l; i++) {
            const double u = uPrime[i],
                t = 1 - u,
                b = 3 * u * t,
                b0 = t * t * t,
                b1 = b * t,
                b2 = b * u,
                b3 = u * u * u;
            g2::Dvec a1 = tan1.normalized<double>(b1);
            g2::Dvec a2 = tan2.normalized<double>(b2);
            auto tmp = g2::Dvec::fromPt(points[first + i]) - (pt1*(b0 + b1)) - (pt2*(b2 + b3));
            C[0][0] += a1.dot(a1);
            C[0][1] += a1.dot(a2);
            // C[1][0] += a1.dot(a2);
            C[1][0] = C[0][1];
            C[1][1] += a2.dot(a2);
            X[0] += a1.dot(tmp);
            X[1] += a2.dot(tmp);
        }

        // Compute the determinants of C and X
        const auto detC0C1 = C[0][0] * C[1][1] - C[1][0] * C[0][1];
        double alpha1, alpha2;
        if (std::abs(detC0C1) > epsilon) {
            // Kramer's rule
            const auto detC0X = C[0][0] * X[1] - C[1][0] * X[0],
                       detXC1 = X[0] * C[1][1] - X[1] * C[0][1];
          // Derive alpha values
            alpha1 = detXC1 / detC0C1;
            alpha2 = detC0X / detC0C1;
        } else {
            // Matrix is under-determined, try assuming alpha1 == alpha2
            const auto c0 = C[0][0] + C[0][1],
                       c1 = C[1][0] + C[1][1];
            alpha1 = alpha2 = std::abs(c0) > epsilon ? X[0] / c0 : std::abs(c1) > epsilon ? X[1] / c1 : 0;
        }

        // If alpha negative, use the Wu/Barsky heuristic (see text)
        // (if alpha is 0, you get coincident control points that lead to
        // divide by zero in any subsequent NewtonRaphsonRootFind() call.
        const auto segLength = (pt2 - pt1).lenD(),
                   eps = epsilon * segLength;

        std::optional<g2::Dvec> handle1 = std::nullopt, handle2 = std::nullopt;
        if (alpha1 < eps || alpha2 < eps) {
            // fall back on standard (probably inaccurate) formula,
            // and subdivide further if needed.
            alpha1 = alpha2 = segLength / 3;
        } else {
            // Check if the found control points are in the right order when
            // projected onto the line through pt1 and pt2.
            const auto line = pt2 - pt1;
            // Control points 1 and 2 are positioned an alpha distance out
            // on the tangent vectors, left and right, respectively
            handle1 = tan1.normalized(alpha1);
            handle2 = tan2.normalized(alpha2);
            if (handle1->dot(line) - handle2->dot(line) > segLength * segLength) {
                // Fall back to the Wu/Barsky heuristic above.
                alpha1 = alpha2 = segLength / 3;
                handle1.reset();
                handle2.reset();
            }
        }

        // First and last control points of the Bezier curve are
        // positioned exactly at the first and last data points
        if (!handle1)
            handle1 = tan1.normalized(alpha1);
        if (!handle2)
            handle2 = tan2.normalized(alpha2);

        return { pt1I, (pt1 + *handle1).toPt<double>(),
                       (pt2 + *handle2).toPt<double>(), pt2I };
    }

}   // anon namespace


/*
const fit = (points: Point[], closed: unknown, error: number) => {
  // We need to duplicate the first and last segment when simplifying a
  // closed path.
  if (closed) {
    points.unshift(points[points.length - 1])
    points.push(points[1]) // The point previously at index 0 is now 1.
  }
  const length = points.length
  if (length === 0) {
    return []
  }
  // To support reducing paths with multiple points in the same place
  // to one segment:
  const segments = [new Segment(points[0])]
  fitCubic(
    points,
    segments,
    error,
    0,
    length - 1,
    // Left Tangent
    points[1]._subtract(points[0]),
    // Right Tangent
    points[length - 2]._subtract(points[length - 1]),
  )
  // Remove the duplicated segments for closed paths again.
  if (closed) {
    segments.shift()
    segments.pop()
  }
  return segments
}
*/

void g2sv::Polyline::fit(double error) const
{
    if (pts.size() < 3)
        return;

    std::vector<g2::Ipoint> newPoints;
    const std::vector<g2::Ipoint>* wk = &this->pts;     // working set
    if (isClosed) {
        // We need to duplicate the first and last segment when simplifying a
        // closed path.
        newPoints.reserve(pts.size() + 2);
        newPoints.push_back(pts.back());
        newPoints.insert(newPoints.end(), pts.begin(), pts.end());
        newPoints.push_back(pts.front());
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
            char command = 'M';
            for (auto& pt : v.pts) {
                if (&pt - v.pts.data() == 45) {
                    command = 'L';
                }
                appendCommand(r, command);
                appendNumber(r, pt.x, scale);
                appendNumber(r, pt.y, scale);
                command = 'L';
            }
            if (v.isClosed)
                appendCommand(r, 'Z');
        }
    }
    return r;
}


///// SVG simplify /////////////////////////////////////////////////////////////
