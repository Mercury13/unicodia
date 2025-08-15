#include "linebreak.h"

#include "loader.h"

// STL
#include <fstream>

// Lib
#include "u_Strings.h"

// Project-local
#include "ucdcom.h"

lb::Base lb::load()
{
    Base r;

    std::ifstream is(UCD_LINEBREAK);
    if (!is.is_open()) {
        throw std::logic_error("Cannot open file");
    }

    std::string line;
    while (std::getline(is, line)) {
        std::string_view s = str::trimSv(line);
        if (s.empty())
            continue;
        auto pComment = s.find('#');
        if (pComment != std::string_view::npos) {
            if (pComment == 0)
                continue;
            s = s.substr(0, pComment);
            s = str::trimRightSv(s);
        }
        auto cols = str::splitSv(s, ';', true);
        if (cols.size() != 2) {
            throw std::logic_error("Not 2 cols");
        }
        const auto sRange = cols[0];
        char32_t cpStart, cpEnd;
        auto pEllip = sRange.find("..");
        if (pEllip == std::string_view::npos) {
            cpStart = cpEnd = fromHex(sRange);
        } else {
            auto sStart = sRange.substr(0, pEllip);
            cpStart = fromHex(sStart);
            auto sEnd = sRange.substr(pEllip + 2);
            cpEnd = fromHex(sEnd);
            if (sEnd < sStart) {
                throw std::logic_error("sEnd < sStart");
            }
        }
        auto& v = r[cpStart];
        v.rangeEnd = cpEnd;
        v.value = std::string{cols[1]};
    }

    return r;
}
