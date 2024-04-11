#pragma once

#include <unordered_map>
#include <set>
#include <string>
#include <vector>

// UCD
#include "ucdcom.h"

namespace tx {

    struct Cp
    {
        std::string correction;
        std::vector<std::string> abbrs, controls;
        std::set<std::string, ucd::Cmp> names;
        /// @todo [future] similar chars?

        void eraseName(std::string_view x);
    };

    using Base = std::unordered_map<char32_t, Cp>;
    using Scripts = ucd::RangeMap<std::string_view>;

    Base loadBase();
    Scripts loadScripts(const ucd::PropBase& propBase);

}   // ns tx
