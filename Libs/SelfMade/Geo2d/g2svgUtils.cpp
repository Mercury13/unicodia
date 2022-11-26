// My header
#include "g2svgUtils.h"

// C++
#include <cmath>
#include <charconv>
#include <map>
#include <utility>
#include <unordered_map>

// STL
#include <algorithm>

extern const g2sv::Point g2sv::BAD_VERTEX { NO_COORD, NO_COORD };

///// PathParser ///////////////////////////////////////////////////////////////


g2sv::PathParser::PathParser(std::string_view data) noexcept
    : p(data.data()), end(data.data() + data.length()) {}


bool g2sv::PathParser::skipSpaces(AllowComma allowComma) noexcept
{
    while (p != end) {
        auto c = static_cast<unsigned char>(*p);
        if (static_cast<unsigned char>(*p) > ' ') {
            if (allowComma == AllowComma::NO || c != ',') {
                return true;
            }
        }
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
        case '+':
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


char g2sv::PathParser::getCommand(TurtleMode turtleMode)
{
    char buf[64];
    if (!skipSpaces(AllowComma::NO))
        return 0;
    auto v = *p;
    if (!latIsLetter(v)) {
        if (isNumChar(v)) {
            switch (turtleMode) {
            case TurtleMode::ABS:
                return 'L';
            case TurtleMode::REL:
                return 'l';
            case TurtleMode::WAIT:
                throw ESvg("[SVG] Turtle is waiting and does not allow implicit lines");
            }
        }
        snprintf(buf, std::size(buf),
                 "[SVG] Command expected (%c)", v);
        throw ESvg(buf);
    }
    ++p;
    return v;
}


double g2sv::PathParser::getNum(char command, AllowComma allowComma)
{
    char buf[64];
    if (!skipSpaces(allowComma)) {
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


int g2sv::PathParser::getInum(char command, AllowComma allowComma, int scale)
{
    return std::round(getNum(command, allowComma) * scale);
}


///// Polyline /////////////////////////////////////////////////////////////////


size_t g2sv::Polyline::removeRepeating()
{
    if (pts.size() < 2)
        return 0;

    auto itEnd = std::unique(pts.begin(), pts.end());
    auto newSize = itEnd - pts.begin();
    auto r = pts.size() - newSize;
    // Closed line → check for first/last point
    if (isClosed && newSize > 1 && pts.front() == pts.back()) {
        --newSize;
    }
    pts.resize(newSize);
    return r;
}


g2sv::RemoveBackForth g2sv::Polyline::removeBackForth(double sharpCos)
{
    g2sv::RemoveBackForth r;
    auto n = pts.size();
    if (n < 4)
        return r;

    const Point* pPrev = &pts[0];
    Point* pCurr = &pts[1];
    size_t nFound = 0;
    for (size_t i = 2; i < n; ++i) {
        auto& pNext = pts[i];
        auto vPrev = *pPrev - *pCurr;
        auto vNext = pNext - *pCurr;
        // Erase collinear — both unneeded vertices in the middle of line,
        // and those horrible back-forth
        auto crossValue = vNext.cross(vPrev);
        bool wasFound = false;
        if (crossValue == 0) {
            // Found!
            wasFound = true;
            // Check for 180° angles
            auto dotValue = vNext.dot(vPrev);
            if (dotValue <= 0)
                ++r.n180;
        } else if (crossValue > 0) {    // Not found
            auto cs = g2::cosABC(*pPrev, *pCurr, pNext);
            if (cs > sharpCos) {
                wasFound = true;
                if (vNext.len2() > 4 && vPrev.len2() > 4) {
                    ++r.nNear0Long;
                } else {
                    ++r.nNear0Short;
                }
            }
        }
        // Check
        if (wasFound) {
            *pCurr = BAD_VERTEX;
            ++nFound;
        } else {
            pPrev = pCurr;
        }
        pCurr = &pNext;
    }

    if (nFound != 0) {
        auto itEnd = std::remove(pts.begin(), pts.end(), BAD_VERTEX);
        r.n0 = pts.end() - itEnd;     // maybe BAD_VERTEX somewhere else? :)
        r.n0 -= r.n180;
        r.n0 -= r.nNear0Short;
        r.n0 -= r.nNear0Long;
        pts.erase(itEnd, pts.end());
    }
    return r;
}


bool g2sv::Polyline::checkDiameter(double r) const
{
    if (pts.empty())
        return true;
    Type xLo = std::numeric_limits<Type>::max();
    Type xHi = std::numeric_limits<Type>::min();
    Type yLo = std::numeric_limits<Type>::max();
    Type yHi = std::numeric_limits<Type>::min();
    for (auto& pt : pts) {
        xLo = std::min(xLo, pt.x);
        xHi = std::max(xHi, pt.x);
        yLo = std::min(yLo, pt.y);
        yHi = std::max(yHi, pt.y);
    }
    return (xHi - xLo < r) && (yHi - yLo < r);
}


size_t g2sv::Polyline::removeShortSegments(
        const SimplifyOpt& opt)
{
    if (pts.size() <= 3)
        return 0;

    const double tol2 = g2::sqr(opt.tangentTolerance);

    auto n = pts.size();
    size_t nFound = 0;
    auto* pt_1 = &pts.back();   // minus 1
    for (size_t i = 0; i < n; ++i) {
        auto& pt0 = pts[i];
        size_t i1 = nextIndex(i);
        auto& pt1 = pts[i1];
        if (pt0.dist2from(pt1) <= tol2) {
            // Find point that’s far enough from pt0
            size_t j = i1;
            if (i1 > i) {   // do a work only if did not wrap
                while (true) {
                    size_t j1 = j + 1;
                    if (j1 >= n)
                        break;
                    auto& ptJ1 = pts[j1];
                    if (pt0.dist2from(ptJ1) > tol2)
                        break;
                    j = j1;
                }
            }
            // <s>Duke Nukem</s> [i1..j] must die!
            if (i1 == j) {
                // 1 point: maybe kill i instead of i1?
                size_t i2 = nextIndex(i1);
                auto& pt2 = pts[i2];
                auto cos0 = g2::cosABC(*pt_1, pt0, pt1);
                auto cos1 = g2::cosABC(pt0, pt1, pt2);
                // Whose angle is bigger?
                if (cos0 < cos1) {
                    // Delete pt0
                    if (pt_1->x == pt0.x) pt1.x = pt0.x;  // keep horz/vert lines
                    if (pt_1->y == pt0.y) pt1.y = pt0.y;
                    pt0 = BAD_VERTEX;
                    pt_1 = &pt1;
                } else {
                    // Delete pt1
                    if (pt1.x == pt2.x) pt0.x = pt1.x;   // keep horz/vert lines
                    if (pt1.y == pt2.y) pt0.y = pt1.y;
                    pt1 = BAD_VERTEX;
                    pt_1 = &pt0;
                }
                ++nFound;
                i = i1;
                continue;
            } else {
                // Several consecutive — kill them all!
                for (size_t k = i1; k <= j; ++k) {
                    pts[k] = BAD_VERTEX;
                    ++nFound;
                }
                i = j;
                continue;
            }
        }
        // Step which is not done when making continue
        pt_1 = &pt0;
    }

    if (nFound != 0) {
        auto itEnd = std::remove(pts.begin(), pts.end(), BAD_VERTEX);
        nFound = pts.end() - itEnd;     // maybe BAD_VERTEX somewhere else? :)
        pts.erase(itEnd, pts.end());
    }
    // Just a debug
    //checkForSelfIntersection(opt.scale);
    return nFound;
}


namespace {

    constinit const g2sv::Corner CORNER1;

    void setCorner(std::map<size_t, g2sv::Corner>& r,
                   size_t index,
                   g2sv::CornerType type)
    {
        auto it = r.emplace_hint(r.end(), index, CORNER1);
        it->second.index = index;
        it->second.type = std::max(it->second.type, type);
    }

    struct Guide {
        int min = std::numeric_limits<int>::max();
        int max = std::numeric_limits<int>::min();

        inline void reg(int a) noexcept;
        void reg(int a, int b) noexcept;
        bool isWithin(int a, int b, int dist) const noexcept;
    };

    inline void Guide::reg(int a) noexcept
    {
        min = std::min(min, a);
        max = std::max(max, a);
    }

    void Guide::reg(int a, int b) noexcept
    {
        reg(a);
        reg(b);
    }

    bool Guide::isWithin(int a, int b, int dist) const noexcept
    {
        if (a > b)
            std::swap(a, b);
        return (b + dist >= min || a - dist <= max);
    }

    template<class Cnt, class Key>
    auto* myFind(Cnt& cnt, const Key& key)
    {
        auto it = cnt.find(key);
        if (it != cnt.end())
            return &it->second;
        return static_cast<decltype(&it->second)>(nullptr);
    }

}   // anon namespace


std::optional<std::vector<g2sv::Corner>> g2sv::Polyline::detectCorners(
        const SimplifyOpt& opt) const
{
    if (pts.size() <= 2)
        return std::nullopt;
    std::map<size_t, Corner> r;
    auto last = pts.size() - 1;

    // Detect guidelines
    if (std::isfinite(opt.guide.triggerLength) && opt.guide.triggerLength > 0) {
        std::unordered_map<int, Guide> horzGuides, vertGuides;
        auto checkGuide = [this, triggerLength = opt.guide.triggerLength, &horzGuides, &vertGuides]
                (size_t iA, size_t iB) {
            auto& a = pts[iA];
            auto& b = pts[iB];
            if (a.x == b.x) {
                // vert guide
                auto len = std::abs(a.y - b.y);
                if (len >= triggerLength) {
                    vertGuides[a.x].reg(a.y, b.y);
                }
            } else if (a.y == b.y) {
                // horz guide
                auto len = std::abs(a.x - b.x);
                if (len >= triggerLength) {
                    horzGuides[a.y].reg(a.x, b.x);
                }
            }
        };
        for (size_t i = 0; i < last; ++i) {
            checkGuide(i, i + 1);
        }
        if (isClosed) {
            checkGuide(last, 0);
        }
        if (!horzGuides.empty() || !vertGuides.empty()) {
            auto checkGuide2 = [this, &hg = std::as_const(horzGuides), &vg = std::as_const(vertGuides),
                                &r, nd = opt.guide.nearbyDist]
                    (size_t iA, size_t iB) {
                auto& a = pts[iA];
                auto& b = pts[iB];
                if (a.x == b.x) {
                    // vert guide
                    if (auto sec = myFind(vg, a.x)) {
                        if (sec->isWithin(a.y, b.y, nd)) {
                            setCorner(r, iA, g2sv::CornerType::AVOID_SMOOTH);
                            setCorner(r, iB, g2sv::CornerType::AVOID_SMOOTH);
                        }
                    }
                } else if (a.y == b.y) {
                    // horz guide
                    if (auto sec = myFind(hg, a.y)) {
                        if (sec->isWithin(a.x, b.x, nd)) {
                            setCorner(r, iA, g2sv::CornerType::AVOID_SMOOTH);
                            setCorner(r, iB, g2sv::CornerType::AVOID_SMOOTH);
                        }
                    }
                }
            };
            for (size_t i = 0; i < last; ++i) {
                checkGuide2(i, i + 1);
            }
            if (isClosed) {
                checkGuide2(last, 0);
            }
        }
    }

    auto checkCosine = [this, &r, &opt](size_t iA, size_t iB, size_t iC) {
        auto& a = pts[iA];
        auto& b = pts[iB];
        auto& c = pts[iC];

        if ((a.x == b.x && b.y == c.y) || (a.y == b.y && b.x == c.x)) {
            // Right corner
            setCorner(r, iA, g2sv::CornerType::AVOID_SMOOTH);
            setCorner(r, iB, g2sv::CornerType::REAL_CORNER);
            setCorner(r, iC, g2sv::CornerType::AVOID_SMOOTH);
        } else {
            auto v1 = a - b;
            auto v2 = c - b;

            auto len1 = v1.lenD();
            if (len1 <= 1e-6)
                throw ESvg("[SVG] Points duplicate, duplicate deletion failed");

            auto len2 = v2.lenD();
            if (len2 <= 1e-6)
                throw ESvg("[SVG] Points duplicate, duplicate deletion failed");

            auto cs = v1.dotD(v2) / (len1 * len2);
            if (!std::isfinite(cs))
                throw ESvg("[SVG] Cannot calculate angle, something really bad");

            //if (b.x == 685 && b.y == 719) {  // Debug
            //    --iB;
            //}

            if (cs >= opt.corner.minCosine) {  // Smooth angle detected — always real corner
                setCorner(r, iB, CornerType::REAL_CORNER);
            } else {
                bool notCorner = true;
                if (len1 >= opt.corner.maxSide) {  // Len1 BIG
                    if (len2 >= opt.corner.maxSide) {  // BIG-BIG — real corner unless smooth
                        if (cs < opt.corner.smoothCosine) {
                        } else {
                            setCorner(r, iB, CornerType::REAL_CORNER );
                            notCorner = false;
                        }
                    } else {    // BIG-SMALL — start smooth thing
                        setCorner(r, iB, CornerType::SMOOTH_START);
                        notCorner = false;
                    }
                } else {    // Len1 SMALL
                    if (len2 >= opt.corner.maxSide) {  // SMALL-BIG → end smooth thing
                        setCorner(r, iB, CornerType::SMOOTH_END);
                        notCorner = true;
                    }
                }
                // If not a corner → check once again for extremities
                if (notCorner) {
                    if (v1.y * v2.y > 0) {  // both >0 or <0
                        setCorner(r, iB, CornerType::HORZ_EXTREMITY);
                    } else if (v1.x * v2.x > 0) {
                        setCorner(r, iB, CornerType::VERT_EXTREMITY);
                    }
                }
            }
        }
    };

    if (isClosed) {
        checkCosine(last, 0, 1);
    } else {
        setCorner(r, 0, CornerType::REAL_CORNER);
    }

    for (size_t i = 1; i < last; ++i) {
        checkCosine(i - 1, i, i + 1);
    }

    if (isClosed) {
        checkCosine(last - 1, last, 0);
    } else {
        setCorner(r, last, CornerType::REAL_CORNER);
    }

    std::vector<g2sv::Corner> r1;
    r1.reserve(r.size());
    for (auto v : r) {
        r1.emplace_back(v.second);
    }
    return r1;
}

namespace {

    void appendCommand(std::string& s, char cmd)
    {
        if (!s.empty())
            s += ' ';
        s += cmd;
    }

    void appendCommand(std::string& s, double cmd) = delete;

    void appendNumber(std::string& s, long num, int scale)
    {
        auto raw = static_cast<double>(num) / scale;
        char buf[30];
        snprintf(buf, std::size(buf), " %.5g", raw);
        s += buf;
    }

    inline void appendNumber(std::string& s, int num, int scale)
        { appendNumber(s, static_cast<long>(num), scale); }

    [[maybe_unused]] void appendNumber(std::string& s, double num, int scale)
        { appendNumber(s, lround(num), scale); }

}   // anon namespace


void g2sv::Polyline::appendSvgData(std::string& r, int scale) const
{
    if (!pts.empty()) {
        char command = 'M';
        for (auto& pt : pts) {
            appendCommand(r, command);
            appendNumber(r, pt.x, scale);
            appendNumber(r, pt.y, scale);
            command = 'L';
        }
        if (isClosed)
            appendCommand(r, 'Z');
    }
}


void g2sv::Polyline::rotateIndexes(size_t i)
{
    if (i == 0 || i >= pts.size())
        return;
    std::rotate(pts.begin(), pts.begin() + i, pts.end());
}


g2sv::Intersection g2sv::Polyline::doesSelfIntersect() const
{
    // Performance O(n²), but OK for my tasks
    auto sz = pts.size();
    switch (sz) {
    case 0:
    case 1:
        return {};
    case 2:
        return isClosed ? Intersection{ pts[0], pts[1], pts[0], pts[1] } : Intersection{};
    case 3:
        return (g2::crossOAB(pts[0], pts[1], pts[2]) == 0)
                ? Intersection{ pts[0], pts[1], pts[0], pts[2] } : Intersection{};
    default: ;
    }

    auto sz1 = isClosed ? sz : sz - 1;
    for (size_t i = 0; i < sz1; ++i) {
        auto& a = pts[i];
        auto& b = pts[wrapIndexFwd(i + 1)];
        if (i >= 2) {
            size_t i0 = i - 2;
            for (size_t j = 0; i <= i0; ++j) {
                auto& c = pts[j];
                auto& d = pts[j + 1];
                if (g2::doSegsStrictlyIntersect(a, b, c, d))
                    return { a, b, c, d };
            }
        }
        auto nn = (i == 0) ? sz - 1 : sz1;
        for (size_t j = i + 2; j < nn; ++j) {
            auto &c = pts[j];
            auto &d = pts[wrapIndexFwd(j + 1)];
            if (g2::doSegsStrictlyIntersect(a, b, c, d))
                return { a, b, c, d };
        }
    }
    return {};
}


void g2sv::Polyline::checkForSelfIntersection(int scale) const
{
    if (auto pt = doesSelfIntersect()) {
        char buf[100];
        // Get shorter segment
        auto len1 = pt.a->dist2from(*pt.b);
        auto len2 = pt.c->dist2from(*pt.d);
        if (len2 < len1) {
            std::swap(pt.a, pt.c);
            std::swap(pt.b, pt.d);
            len1 = len2;
        }
        auto x1 = static_cast<double>(pt.a->x) / scale;
        auto y1 = static_cast<double>(pt.a->y) / scale;
        auto x2 = static_cast<double>(pt.b->x) / scale;
        auto y2 = static_cast<double>(pt.b->y) / scale;
        auto x3 = static_cast<double>(pt.c->x) / scale;
        auto y3 = static_cast<double>(pt.c->y) / scale;
        auto x4 = static_cast<double>(pt.d->x) / scale;
        auto y4 = static_cast<double>(pt.d->y) / scale;
        snprintf(buf, std::size(buf), "Curve self-intersects: (%g %g) - (%g %g), L=%.3g with (%g %g) - (%g %g)",
                 x1, y1, x2, y2, sqrt(len1) / scale,
                 x3, y3, x4, y4);
        throw ESvg(buf);
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

    /// Actually a curve point, but let it be…
    struct Segment {
        size_t index;
        g2::Ipoint a;
        g2::Dvec hIn;
        g2::Dvec hOut;

        constexpr static Segment fromPt(size_t index, const g2::Ipoint& pt)
            { return { .index = index, .a = pt, .hIn = g2::ZEROVEC, .hOut = g2::ZEROVEC }; }
    };

    struct Pt4 {
        g2::Dvec v[4];
    };

    struct Curve {
        g2::Ipoint a;
        g2::Dvec ah;
        g2::Dvec bh;
        g2::Ipoint b;

        Pt4 toPoints() const;
    };

    Pt4 Curve::toPoints() const
    {
        Pt4 r;
        r.v[0] = g2::Dvec::fromPt(a);
        r.v[1] = r.v[0] + ah;
        r.v[3] = g2::Dvec::fromPt(b);
        r.v[2] = r.v[3] + bh;
        return r;
    }


    Curve generateBezier(
            const std::vector<g2::Ipoint>& points,
            size_t first, size_t last,
            const std::vector<double>& uPrime,
            const g2::Dvec& tan1, const g2::Dvec& tan2)
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

        return { pt1I, *handle1, *handle2, pt2I };
    }

    /*
    const addCurve = (segments: Segment[], curve: readonly Point[]) => {
      const prev = segments[segments.length - 1]
      prev._handleOut = curve[1]._subtract(curve[0])
      segments.push(new Segment(curve[3], curve[2]._subtract(curve[3])))
    }
    */

    void addCurve(std::vector<Segment>& segments, size_t index, const Curve& curve)
    {
        auto& prev = segments.back();
        prev.hOut = curve.ah;
        segments.push_back(Segment{
                .index = index, .a = curve.b,
                .hIn = curve.bh, .hOut = g2::ZEROVEC });
    }

    /*
    // Assign parameter values to digitized points
    // using relative distances between points.
    const chordLengthParameterize = (points: readonly Point[], first: number, last: number) => {
      const u = [0]
      for (let i = first + 1; i <= last; i++) {
        u[i - first] = u[i - first - 1] + points[i]._getDistance(points[i - 1])
      }
      for (let i = 1, m = last - first; i <= m; i++) {
        u[i] /= u[m]
      }
      return u
    }
    */

    std::vector<double> chordLengthParameterize(
            const std::vector<g2::Ipoint>& points,
            size_t first, size_t last)
    {
        std::vector<double> u;
        u.push_back(0);
        for (auto i = first + 1; i <= last; ++i) {
            auto v = u.back() + points[i].distFromD(points[i - 1]);
            u.push_back(v);
        }
        for (size_t i = 1, m = last - first; i <= m; ++i) {
            u[i] /= u[m];
        }
        return u;
    }

    /*// Evaluate a bezier curve at a particular parameter value
    const evaluate = (degree: number, curve: readonly Point[], t: number) => {
      // Copy array
      const tmp = curve.slice()
      // Triangle computation
      for (let i = 1; i <= degree; i++) {
        for (let j = 0; j <= degree - i; j++) {
          tmp[j] = tmp[j]._multiply(1 - t)._add(tmp[j + 1]._multiply(t))
        }
      }
      return tmp[0]
    }*/
    g2::Dvec evaluate(int degree, const g2::Dvec* curve, double t)
    {
        // Copy array
        g2::Dvec tmp[5];
        for (int i = 0; i <= degree; ++i)
            tmp[i] = curve[i];
        double ttt = 1 - t;
        // Triangle computation
        for (int i = 1; i <= degree; ++i) {
            for (int j = 0; j <= degree - i; ++j) {
                tmp[j] = tmp[j] * ttt + tmp[j + 1] * t;
            }
        }
        return tmp[0];
    }

    /*
    // Find the maximum squared distance of digitized points to fitted curve.
    const findMaxError = (points: readonly Point[], first: number, last: number, curve: Point[], u: number[]) => {
      let index = Math.floor((last - first + 1) / 2),
        maxDist = 0
      for (let i = first + 1; i < last; i++) {
        const P = evaluate(3, curve, u[i - first])
        const v = P._subtract(points[i])
        const dist = v.x * v.x + v.y * v.y // squared
        if (dist >= maxDist) {
          maxDist = dist
          index = i
        }
      }
      return {
        error: maxDist,
        index: index,
      }
    }*/

    struct Error {
        double error;
        size_t index;
    };

    Error findMaxError(
            const std::vector<g2::Ipoint>& points, size_t first, size_t last,
            const Curve& curve, const std::vector<double>& u)
    {
        Error r = {
            .error = 0,
            .index = (last - first + 1) / 2
        };
        auto pts = curve.toPoints();
        for (size_t i = first + 1; i < last; ++i) {
            auto P = evaluate(3, pts.v, u[i - first]);
            auto v = P - g2::Dvec::fromPt(points[i]);
            auto dist = v.len2(); // squared
            if (dist >= r.error) {
                r.error = dist;
                r.index = i;
            }
        }
        return r;
    }

    inline bool isMachineZero(double val)
        { return (val >= -MACHINE_EPSILON && val <= MACHINE_EPSILON); }

    /*// Use Newton-Raphson iteration to find better root.
    const findRoot = (curve: readonly Point[], point: Point, u: number) => {
      const curve1 = [],
        curve2 = []
      // Generate control vertices for Q'
      for (let i = 0; i <= 2; i++) {
        curve1[i] = curve[i + 1]._subtract(curve[i])._multiply(3)
      }
      // Generate control vertices for Q''
      for (let i = 0; i <= 1; i++) {
        curve2[i] = curve1[i + 1]._subtract(curve1[i])._multiply(2)
      }
      // Compute Q(u), Q'(u) and Q''(u)
      const pt = evaluate(3, curve, u),
        pt1 = evaluate(2, curve1, u),
        pt2 = evaluate(1, curve2, u),
        diff = pt._subtract(point),
        df = pt1._dot(pt1) + diff._dot(pt2)
      // u = u - f(u) / f'(u)
      return isMachineZero(df) ? u : u - diff._dot(pt1) / df
    }*/

    double findRoot(const Pt4& curve, const g2::Ipoint& point, double u)
    {
        Pt4 curve1, curve2;
        // Generate control vertices for Q'
        for (int i = 0; i <= 2; i++) {
            curve1.v[i] = curve.v[i + 1] - curve.v[i] * 3.0;
        }
        // Generate control vertices for Q''
        for (int i = 0; i <= 1; i++) {
            curve2.v[i] = curve1.v[i + 1] - curve1.v[i] * 2.0;
        }
        // Compute Q(u), Q'(u) and Q''(u)
        auto pt = evaluate(3, curve.v, u),
            pt1 = evaluate(2, curve1.v, u),
            pt2 = evaluate(1, curve2.v, u);
        auto diff = pt - g2::Dvec::fromPt(point);
        auto df = pt1.dot(pt1) + diff.dot(pt2);
        // u = u - f(u) / f'(u)
        return isMachineZero(df) ? u : u - diff.dot(pt1) / df;
    }

    /*// Given set of points and their parameterization, try to find
    // a better parameterization.
    const reparameterize = (points: readonly Point[], first: number, last: number, u: number[], curve: Point[]) => {
      for (let i = first; i <= last; i++) {
        u[i - first] = findRoot(curve, points[i], u[i - first])
      }
      // Detect if the new parameterization has reordered the points.
      // In that case, we would fit the points of the path in the wrong order.
      for (let i = 1, l = u.length; i < l; i++) {
        if (u[i] <= u[i - 1]) return false
      }
      return true
    }*/

    bool reparameterize(
            const std::vector<g2::Ipoint>& points, size_t first, size_t last,
            std::vector<double>& u, const Curve& curve)
    {
        for (size_t i = first; i <= last; ++i) {
            u[i - first] = findRoot(curve.toPoints(), points[i], u[i - first]);
        }
        // Detect if the new parameterization has reordered the points.
        // In that case, we would fit the points of the path in the wrong order.
        for (size_t i = 1, l = u.size(); i < l; ++i) {
            if (u[i] <= u[i - 1]) return false;
        }
        return true;
    }

    /*
    // Fit a Bezier curve to a (sub)set of digitized points
    const fitCubic = (points: readonly Point[], segments: Segment[], error: number, first: number, last: number, tan1: Point, tan2: Point) => {
      //  Use heuristic if region only has two points in it
      if (last - first === 1) {
        const pt1 = points[first],
          pt2 = points[last],
          dist = pt1._getDistance(pt2) / 3
        addCurve(segments, [pt1, pt1._add(tan1._normalize(dist)), pt2._add(tan2._normalize(dist)), pt2])
        return
      }
      // Parameterize points, and attempt to fit curve
      const uPrime = chordLengthParameterize(points, first, last)
      let maxError = Math.max(error, error * error),
        split: number,
        parametersInOrder = true
      // Try not 4 but 5 iterations
      for (let i = 0; i <= 4; i++) {
        const curve = generateBezier(points, first, last, uPrime, tan1, tan2)
        //  Find max deviation of points to fitted curve
        const max = findMaxError(points, first, last, curve, uPrime)
        if (max.error < error && parametersInOrder) {
          addCurve(segments, curve)
          return
        }
        split = max.index
        // If error not too large, try reparameterization and iteration
        if (max.error >= maxError) break
        parametersInOrder = reparameterize(points, first, last, uPrime, curve)
        maxError = max.error
      }
      // Fitting failed -- split at max error point and fit recursively
      const tanCenter = points[split! - 1]._subtract(points[split! + 1])
      fitCubic(points, segments, error, first, split!, tan1, tanCenter)
      fitCubic(points, segments, error, split!, last, tanCenter._negate(), tan2)
    }
    */

    enum class Initial { NO, YES };

    template <class En>
    constexpr bool isTrue(En x) noexcept {
        static_assert(std::is_enum_v<En>);
        return (x != En::NO);
    }

    void fitCubic(
            Initial isInitial,
            const std::vector<g2::Ipoint>& points,
            std::vector<Segment>& segments,
            double error,
            size_t first, size_t last,
            const g2::Dvec& tan1, const g2::Dvec& tan2)
    {
        if (last - first == 1) {
            const auto &pt1 = points[first],
                       &pt2 = points[last];
            if (isTrue(isInitial)) {
                // Add it as a straight segment
                addCurve(segments, last,
                    Curve{ pt1, g2::ZEROVEC, g2::ZEROVEC, pt2 });
            } else {
                // Add it as smth nice
                const auto dist = pt1.distFromD(pt2) / 3;
                addCurve(segments, last,
                    Curve{ pt1, tan1.normalized(dist), tan2.normalized(dist), pt2 });
            }
            return;
        }

        // Parameterize points, and attempt to fit curve
        auto uPrime = chordLengthParameterize(points, first, last);
        auto maxError = std::max(error, error * error);
        size_t split;
        bool parametersInOrder = true;

        // Try not 4 but 5 iterations
        for (int i = 0; i <= 4; ++i) {
            auto curve = generateBezier(points, first, last, uPrime, tan1, tan2);
            //  Find max deviation of points to fitted curve
            auto max = findMaxError(points, first, last, curve, uPrime);
            if (max.error < error && parametersInOrder) {
                addCurve(segments, last, curve);
                return;
            }
            split = max.index;
            // If error not too large, try reparameterization and iteration
            if (max.error >= maxError) break;
            parametersInOrder = reparameterize(points, first, last, uPrime, curve);
            maxError = max.error;
        }
        // Fitting failed -- split at max error point and fit recursively
        auto tanCenter = (points[split - 1] - points[split + 1]).cast<double>();
        fitCubic(Initial::NO, points, segments, error, first, split, tan1, tanCenter);
        fitCubic(Initial::NO, points, segments, error, split, last, -tanCenter, tan2);
    }

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
    }*/

    g2::Dvec makeLeftTangent(
            const std::vector<g2::Ipoint>& wk,
            const g2sv::Corner& first,
            const g2sv::Corner& last,
            double tolerance)
    {
        auto& ptPrev = (first.index == 0) ? wk.back() : wk[first.index - 1];
        auto& pt0 = wk[first.index];

        // Get 1st point by tolerance
        auto i1 = first.index;
        g2::Ipoint pt1;
        do {
            ++i1;
            pt1 = wk[i1];
        } while (i1 < last.index && pt1.distFromD(pt0) < tolerance);

        switch (first.type) {
        case g2sv::CornerType::SMOOTH_START:
            return (pt0 - ptPrev).cast<double>();
        case g2sv::CornerType::SMOOTH_END:
        case g2sv::CornerType::REAL_CORNER:
        case g2sv::CornerType::AVOID_SMOOTH:
            return (pt1 - pt0).cast<double>();
        case g2sv::CornerType::HORZ_EXTREMITY:
            return { static_cast<double>(pt1.x - ptPrev.x), 0 };
        case g2sv::CornerType::VERT_EXTREMITY:
            return { 0, static_cast<double>(pt1.y - ptPrev.y) };
        }
        __builtin_unreachable();
    }

    g2::Dvec makeRightTangent(
            const std::vector<g2::Ipoint>& wk,
            const g2sv::Corner& first,
            const g2sv::Corner& last,
            double tolerance)
    {
        auto iNext = last.index + 1;
        auto& ptNext = (iNext >= wk.size()) ? wk.front() : wk[iNext];
        auto& pt10 = wk[last.index];

        // Get 9th (penultimate) point by tolerance
        auto i9 = last.index;
        g2::Ipoint pt9;
        do {
            --i9;
            pt9 = wk[i9];
        } while (i9 > first.index && pt9.distFromD(pt10) < tolerance);

        switch (last.type) {
        case g2sv::CornerType::SMOOTH_END:
            return (pt10 - ptNext).cast<double>();
        case g2sv::CornerType::SMOOTH_START:
        case g2sv::CornerType::REAL_CORNER:
        case g2sv::CornerType::AVOID_SMOOTH:
            return (pt9 - pt10).cast<double>();
        case g2sv::CornerType::HORZ_EXTREMITY:
            return { static_cast<double>(pt9.x - ptNext.x), 0 };
        case g2sv::CornerType::VERT_EXTREMITY:
            return { 0, static_cast<double>(pt9.y - ptNext.y) };
        }
        __builtin_unreachable();
    }

    std::vector<Segment> fit(
            g2sv::Polyline& pl,
            const g2sv::SimplifyOpt& opt)
    {
        if (pl.pts.size() < 3)
            return {};

        auto corners = pl.detectCorners(opt);
        if (!corners)
            return {};

        bool isCompletelySmooth = true;
        if (!corners->empty()) {
            auto corner0 = (*corners)[0].index;
            if (corner0 != 0) { // always when path closed
                pl.rotateIndexes(corner0);
                for (auto& v : *corners) {
                    v.index -= corner0;
                }
            }
            isCompletelySmooth = false;
        }

        //{   // Debug
        //    g2::Ipoint ptDebug { 733, 619 };
        //    auto it = std::find(pl.pts.begin(), pl.pts.end(), ptDebug);
        //    if (it != pl.pts.end()) {
        //        auto index = it - pl.pts.begin();
        //        ++index;
        //    }
        //}

        std::vector<g2::Ipoint> newPoints;
        const std::vector<g2::Ipoint>* wk = &pl.pts;    // work set
        if (isCompletelySmooth) {
            throw std::logic_error("As we make extrema, no more completely smooth curves!");
            // Completely smooth path: make copies of one segment
//            newPoints.reserve(pl.pts.size() + 2);
//            newPoints.push_back(pl.pts.back());
//            newPoints.insert(newPoints.end(), pl.pts.begin(), pl.pts.end());
//            newPoints.push_back(pl.pts.front());
//            wk = &newPoints;
//            corners = std::vector<g2sv::Corner>{
//                        { { .index = 0, .type = g2sv::CornerType::REAL_CORNER },
//                          { .index = newPoints.size() - 1, .type = g2sv::CornerType::REAL_CORNER } } };
        } else if (pl.isClosed) {
            // Closed path: make copy of a point
            newPoints.reserve(pl.pts.size() + 1);
            newPoints.insert(newPoints.end(), pl.pts.begin(), pl.pts.end());
            newPoints.push_back(pl.pts.front());
            wk = &newPoints;
            corners->emplace_back(g2sv::Corner{
                    .index = newPoints.size() - 1, .type = g2sv::CornerType::REAL_CORNER });
        }

        // To support reducing paths with multiple points in the same place
        // to one segment:
        std::vector<Segment> segments;
        segments.push_back(Segment::fromPt(0, (*wk)[0]));
        size_t lastCorner = corners->size() - 1;

        for (size_t i = 0; i < lastCorner; ++i) {
            auto first = (*corners)[i];
            auto last = (*corners)[i + 1];

            // Debug
            //if (i+1 == lastCorner) {
            //    ++i;
            //}
            fitCubic(
                Initial::YES,
                *wk,
                segments,
                opt.tolerance,
                first.index,
                last.index,
                makeLeftTangent(*wk, first, last, opt.tangentTolerance),
                makeRightTangent(*wk, first, last, opt.tangentTolerance)
            );
        }

        // Un-preprocess path
        if (isCompletelySmooth) {
//            if (segments.size() <= 2) {
//                segments.clear();
//            } else {
//                segments.pop_back();
//                segments.erase(segments.begin());
//            }
        } else if (pl.isClosed) {
            if (segments.size() <= 1) {
                segments.clear();
            } else {
                // 1st point has hOut only, and last has hIn only, and this is the same point → unite properly
                segments[0].hIn = segments.back().hIn;
                segments.pop_back();
            }
        }
        return segments;
    }


    /*const getSegmentsPathData = (segments: Segment[], closed: unknown, precision: number) => {
      const length = segments.length
      const precisionMultiplier = 10 ** precision
      const round = precision < 16 ? (n: number) => Math.round(n * precisionMultiplier) / precisionMultiplier : (n: number) => n
      const formatPair = (x: number, y: number) => round(x) + ',' + round(y)
      let first = true
      let prevX: number, prevY: number, outX: number, outY: number
      const parts: string[] = []

      const addSegment = (segment: Segment, skipLine?: boolean) => {
        const curX = segment._point.x
        const curY = segment._point.y
        if (first) {
          parts.push('M' + formatPair(curX, curY))
          first = false
        } else {
          const inX = curX + (segment._handleIn?.x ?? 0)
          const inY = curY + (segment._handleIn?.y ?? 0)
          if (inX === curX && inY === curY && outX === prevX && outY === prevY) {
            // l = relative lineto:
            if (!skipLine) {
              const dx = curX - prevX
              const dy = curY - prevY
              parts.push(dx === 0 ? 'v' + round(dy) : dy === 0 ? 'h' + round(dx) : 'l' + formatPair(dx, dy))
            }
          } else {
            // c = relative curveto:
            parts.push(
              'c' +
                formatPair(outX - prevX, outY - prevY) +
                ' ' +
                formatPair(inX - prevX, inY - prevY) +
                ' ' +
                formatPair(curX - prevX, curY - prevY),
            )
          }
        }
        prevX = curX
        prevY = curY
        outX = curX + (segment._handleOut?.x ?? 0)
        outY = curY + (segment._handleOut?.y ?? 0)
      }

      if (!length) return ''

      for (let i = 0; i < length; i++) addSegment(segments[i])
      // Close path by drawing first segment again
      if (closed && length > 0) {
        addSegment(segments[0], true)
        parts.push('z')
      }
      return parts.join('')
    }*/

    std::string getSegmentsPathData(
            const std::vector<Segment>& segments,
            bool closed,
            int scale)
    {
        auto length = segments.size();
        double prevX, prevY, outX, outY;
        bool first = true;
        std::string r;

        auto addSegment = [&](const Segment& segment, bool skipLine = false) {
            auto curX = segment.a.x;
            auto curY = segment.a.y;
            if (first) {
                appendCommand(r, 'M');
                appendNumber(r, curX, scale);
                appendNumber(r, curY, scale);
                first = false;
            } else {
              auto inX = curX + segment.hIn.x;
              auto inY = curY + segment.hIn.y;
              if (inX == curX && inY == curY && outX == prevX && outY == prevY) {
                // L = absolute lineto:
                    if (!skipLine) {
                        appendCommand(r, 'L');
                        appendNumber(r, curX, scale);
                        appendNumber(r, curY, scale);
                    }
                } else {
                    // c = absolute curveto:
                    appendCommand(r, 'C');
                    appendNumber(r, outX, scale);
                    appendNumber(r, outY, scale);
                    appendNumber(r, inX,  scale);
                    appendNumber(r, inY,  scale);
                    appendNumber(r, curX, scale);
                    appendNumber(r, curY, scale);
                }
            }
            prevX = curX;
            prevY = curY;
            outX = curX + segment.hOut.x;
            outY = curY + segment.hOut.y;
        };

        for (size_t i = 0; i < length; ++i)
            addSegment(segments[i]);
        // Close path by drawing first segment again
        if (closed && length > 0) {
            addSegment(segments[0], true);
            appendCommand(r, 'Z');
        }

        return r;
    }

}   // anon namespace


///// Polypath /////////////////////////////////////////////////////////////////


void g2sv::Polypath::parse(std::string_view text, int scale)
{
    dataOverride.clear();
    curves.clear();

    g2::Ipoint turtle;
    Polyline* path = nullptr;
    auto turtleMode = TurtleMode::WAIT;

    PathParser parser(text);
    auto moveTurtleAbs = [&](char command) {
        turtleMode = TurtleMode::ABS;
        turtle.x = parser.getInum(command, AllowComma::NO,  scale);
        turtle.y = parser.getInum(command, AllowComma::YES, scale);
    };
    auto moveTurtleRel = [&](char command) {
        turtleMode = TurtleMode::REL;
        turtle.x += parser.getInum(command, AllowComma::NO,  scale);
        turtle.y += parser.getInum(command, AllowComma::YES, scale);
    };
    auto rqPath = [&path](const char* x) {
        if (!path)
            throw ESvg(x);
    };

    while (true) {
        auto command = parser.getCommand(turtleMode);
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
            turtleMode = TurtleMode::ABS;
            turtle.x = parser.getInum(command, AllowComma::NO, scale);
            path->pts.push_back(turtle);
            break;
        case 'h':
            rqPath("h (rel horz) command: no path to move turtle");
            turtleMode = TurtleMode::REL;
            turtle.x += parser.getInum(command, AllowComma::NO, scale);
            path->pts.push_back(turtle);
            break;
        case 'V':
            rqPath("V (abs vert) command: no path to move turtle");
            turtleMode = TurtleMode::ABS;
            turtle.y = parser.getInum(command, AllowComma::NO, scale);
            path->pts.push_back(turtle);
            break;
        case 'v':
            rqPath("h (rel vert) command: no path to move turtle");
            turtleMode = TurtleMode::REL;
            turtle.y += parser.getInum(command, AllowComma::NO, scale);
            path->pts.push_back(turtle);
            break;
        case 'Z':
        case 'z':
            turtleMode = TurtleMode::WAIT;
            rqPath("Z (close curve) command: no path to close curve");
            if (path->pts.size() > 1 && path->pts.front() == path->pts.back())
                path->pts.pop_back();
            turtle = path->pts.front();  // where does turtle go?
            path->isClosed = true;
            path = nullptr;
            break;
        default:
            throw std::logic_error("Unknown turtle graphics command");
        }
    }
}


std::string g2sv::Polypath::svgData(int scale) const
{
    if (!dataOverride.empty())
        return dataOverride;

    std::string r;
    for (auto& v : curves) {
        v.appendSvgData(r, scale);
    }
    return r;
}


void g2sv::Polypath::simplify(const SimplifyOpt& opt)
{
    /// @todo [urgent] Remove bad nearby points, and reenable again
    checkForIntersection(opt.scale);
    dataOverride.clear();
    for (auto& curve : curves) {
        auto segs = fit(curve, opt);
        if (segs.empty()) {
            curve.appendSvgData(dataOverride, opt.scale);
        } else {
            auto pathData = getSegmentsPathData(segs, curve.isClosed, opt.scale);
            if (!pathData.empty()) {
                if (!dataOverride.empty())
                    dataOverride += ' ';
                dataOverride += pathData;
            }
        }
    }
}


void g2sv::Polypath::checkForIntersection(int scale) const
{
    for (auto& c : curves) {
        c.checkForSelfIntersection(scale);
    }
}


size_t g2sv::Polypath::removeByDiameter(double diam)
{
    for (auto& v : curves) {
        if (v.checkDiameter(diam)) {
            v.pts.clear();
        }
    }
    auto ee = std::remove_if(curves.begin(), curves.end(),
                [](const g2sv::Polyline& x) { return x.pts.size() == 0; });
    auto r = curves.end() - ee;
    curves.erase(ee, curves.end());
    return r;
}
