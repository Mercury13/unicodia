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
        /// Tangent tolerance: close point → make tangent from next
        double tangentTolerance = 1;
        /// Scaling double → int (we mostly work in fixed point)
        int scale = 1;
        struct Corner {
            /// Always (-1…0), min cosine of vertices considered corner
            double minCosine = -0.7;
            /// Side longer than this are considered straight
            double maxSide = 100000;
            /// This cosine is considered corner regardless of length,
            /// always < minCosine
            double smoothCosine = -0.9;
        } corner;
    };

    enum class CornerType {
        REAL_CORNER,    // real corner — make tangents as you want
        SMOOTH_START,   // start of smooth conjugation — tangent is PREV point
        SMOOTH_END,     // end of smooth conjugation — tangent is NEXT point
        HORZ_EXTREMITY, // Horizontal extremity of smooth sequence
        VERT_EXTREMITY  // Vertical extremity of smooth sequence
    };

    struct Corner {
        size_t index;
        CornerType type;
    };

    struct Polyline {
        std::vector<g2::Ipoint> pts;
        bool isClosed = false;

        /// Moves point i to position 0
        /// @warning  It is NOT physical rotation, it’s just reenumeration
        ///           of closed path, and has no sense when the path is not closed.
        void rotateIndexes(size_t i);

        /// Removes repeating points
        /// @return [+] smth happened
        bool removeRepeating();

        /// Removes collinear segments that go back and forth
        /// @return [+] smth happened
        bool removeBackForth();

        /// @return [-] smth bad [0] really no corners in closed polyline
        ///             (non-closed polyline gives at least its ends)
        std::optional<std::vector<Corner>> detectCorners(
                const SimplifyOpt& opt) const;

        void appendSvgData(std::string& r, int scale) const;
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
    };

    class ESvg : public std::logic_error
    {
    public:
        using logic_error::logic_error;
    };

}   // namespace g2sv
