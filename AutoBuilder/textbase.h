#pragma once

// STL
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <string>
#include <vector>
#include <optional>

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
        bool contains(std::string_view x) const;
    };

    struct Empty {};

    using Base = std::unordered_map<char32_t, Cp>;
    using Scripts = ucd::RangeMap<std::string_view>;
    using Ages = ucd::RangeMap<std::string>;
    using Mirroring = std::unordered_set<char32_t>;

    struct Props {
        ucd::RangeMap<Empty> defaultIgnorable, deprecated;
    };

    Base loadBase();
    Scripts loadScripts(const ucd::PropBase& propBase);
    Ages loadAges();
    Props loadProps();

    bool isNumeric(std::string_view s);
    std::optional<unsigned long long> numericValue(std::string_view s);

}   // ns tx
