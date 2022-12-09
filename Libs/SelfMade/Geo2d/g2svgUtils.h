#pragma once

///
/// SVG utils: some transformations with polyline SVG
///

// STL
#include <vector>
#include <string_view>
#include <stdexcept>
#include <optional>

// Geo2d
#include "g2defines.h"

namespace g2sv {

    struct SimplifyOpt {
        /// Fitting tolerance
        double tolerance = 2.5;
        /// Quotient: fitting tolerance for straight lines
        double qLine = 0.5;
        /// Min. diameter — delete smaller curves
        double minDiameter = 2.5;
        /// Cosine of an angle that’s considered glitch, always (0..1)
        double sharpCos = 0.98;
        /// Scaling double → int (we mostly work in fixed point)
        int scale = 1;
        struct Corner {
            /// Always (-1..0), min cosine of vertices considered corner
            double minCosine = -0.7;
            /// Side longer than this are considered straight
            double maxSide = 100000;
            /// This cosine is considered smooth regardless of length,
            /// always < minCosine
            double smoothCosine = -0.9;
        } corner;
        struct Tangent {
            /// Tangent tolerance: close point → make tangent from next
            double tolerance = 1;
            /// Angle close to right → snap to right!
            double snapAngle = 0.05;
            /// Made tangent collinear with nearby line → nudge out!
            double nudgeAngle = 0.03;
        } tangent;
        struct Guide {
            /// Long horz/vert segments are considered “guidelines”
            int triggerLength = 40;
            /// distance guideline → horz node
            /// and short nearby segments are also straight.
            int nearbyDist = 10;
        } guide;
    };

    /// Order matters: the bigger, the heavier
    enum class CornerType {
        AVOID_SMOOTH,   // Somehow avoiding smooth
        HORZ_EXTREMITY, // Horizontal extremity of smooth sequence
        VERT_EXTREMITY, // Vertical extremity of smooth sequence
        SMOOTH_START,   // start of smooth conjugation — tangent is PREV point
        SMOOTH_END,     // end of smooth conjugation — tangent is NEXT point
        REAL_CORNER,    // real corner — make tangents as you want
    };

    struct Corner {
        size_t index = std::numeric_limits<size_t>::max() / 2;
        CornerType type = CornerType::AVOID_SMOOTH;
    };

    using Type = int;
    using Point = g2::Point<Type>;
    using Vec = g2::Vec<Type>;

    constexpr Type NO_COORD = std::numeric_limits<int>::min();
    extern const Point BAD_VERTEX;

    struct Intersection {
        const g2::Ipoint *a = nullptr, *b = nullptr, *c = nullptr, *d = nullptr;
        Intersection() noexcept = default;
        Intersection(const g2::Ipoint& aa, const g2::Ipoint& bb, const g2::Ipoint& cc, const g2::Ipoint& dd)
            : a(&aa), b(&bb), c(&cc), d(&dd) {}
        explicit operator bool() const noexcept { return a; }
    };

    struct RemoveBackForth {
        size_t n0 = 0, nNear0Short = 0, nNear0Long = 0, n180 = 0;
    };

    struct Polyline {
        // Fields
        std::vector<Point> pts;
        bool isClosed = false;

        /// Moves point i to position 0
        /// @warning  It is NOT physical rotation, it’s just reenumeration
        ///           of closed path, and has no sense when the path is not closed.
        void rotateIndexes(size_t i);

        /// Removes repeating points
        /// @return  # of removed points
        size_t removeRepeating();

        /// Removes collinear segments that go back and forth
        /// @return  # of removed points
        RemoveBackForth removeBackForth(double sharpCos);

        /// Removes collinear segments that go back and forth
        /// @return  # of removed points
        size_t removeShortSegments(const SimplifyOpt& opt);

        /// @return [-] smth bad [0] really no corners in closed polyline
        ///             (non-closed polyline gives at least its ends)
        std::optional<std::vector<Corner>> detectCorners(
                const SimplifyOpt& opt) const;

        void appendSvgData(std::string& r, int scale) const;

        /// @return [+] one of points adjacent to lines involved
        ///         [-] no intersections found
        Intersection doesSelfIntersect() const;

        /// @throw  if some polyline self-intersects
        void checkForSelfIntersection(int scale) const;

        /// @return [+] is smaller
        bool checkDiameter(double r) const;

        inline size_t wrapIndexFwd(size_t i) const { return (i >= pts.size() ? 0 : i); }
        inline size_t wrapIndexBack(size_t i) const { return (i >= pts.size() ? pts.size() - 1 : i); }
        inline size_t nextIndex(size_t i) const { return wrapIndexFwd(i + 1); }
        inline size_t prevIndex(size_t i) const { return wrapIndexBack(i - 1); }
    };

    enum class AllowComma { NO, YES };
    enum class TurtleMode { WAIT, ABS, REL };

    class PathParser
    {
    public:
        PathParser(const char* aP, const char* aEnd) noexcept : p(aP), end(aEnd) {}
        PathParser(std::string_view data) noexcept;
        char getCommand(TurtleMode turtleMode);
        double getNum(char command, AllowComma allowComma);
        int getInum(char command, AllowComma allowComma, int scale);
    private:
        const char* p;
        const char* end;
        /// @return [+] OK [-] end        
        bool skipSpaces(AllowComma allowComma) noexcept;
    };

    struct Polypath {
        std::vector<Polyline> curves;
        std::string dataOverride;

        void parse(std::string_view text, int scale);
        void simplify(const SimplifyOpt& opt);
        std::string svgData(int scale) const;

        /// @throw  if some polyline self-intersects
        void checkForIntersection(int scale) const;

        size_t removeByDiameter(double diam);
    };

    class ESvg : public std::logic_error
    {
    public:
        using logic_error::logic_error;
    };

}   // namespace g2sv
