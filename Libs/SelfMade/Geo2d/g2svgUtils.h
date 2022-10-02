#pragma once

///
/// SVG utils: some transformations with polyline SVG
///

// STL
#include <vector>
#include <string_view>
#include <stdexcept>

// Geo2d
#include "g2defines.h"

namespace g2sv {

    struct SimplifyOpt {
        double tolerance = 2.5;
        /// Always (-1…0), max. cosine of vertices considered smooth
        double smoothCosine = -0.7;
        /// Scaling double → int (we mostly work in fixed point)
        int scale = 1;
    };

    struct Polyline {
        std::vector<g2::Ipoint> pts;
        bool isClosed = false;

        /// Removes repeating points
        /// @return [+] smth happened
        bool removeRepeating();

        /// Removes collinear segments that go back and forth
        /// @return [+] smth happened
        bool removeBackForth();

        std::vector<size_t> detectCorners(double maxCosine) const;
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
