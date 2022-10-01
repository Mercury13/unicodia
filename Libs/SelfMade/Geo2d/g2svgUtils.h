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

    struct Polyline {
        std::vector<g2::Ipoint> pts;
        bool isClosed = false;

        /// Removes repeating points
        /// @return [+] smth happened
        bool removeRepeating();

        /// Removes collinear segments that go back and forth
        /// @return [+] smth happened
        bool removeBackForth();
    };

    class PathParser
    {
    public:
        PathParser(const char* aP, const char* aEnd) noexcept : p(aP), end(aEnd) {}
        PathParser(std::string_view data) noexcept;
        char getCommand();
        double getNum(char command);
        int getInum(char command, int scale);
    private:
        const char* p;
        const char* end;
        /// @return [+] OK [-] end
        bool skipSpaces() noexcept;
    };

    struct Polypath {
        std::vector<Polyline> curves;
        std::string dataOverride;

        void parse(std::string_view text, int scale);
        std::string svgData(int scale) const;
    };

    class ESvg : public std::logic_error
    {
    public:
        using logic_error::logic_error;
    };

}   // namespace g2sv
