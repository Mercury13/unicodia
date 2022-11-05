#pragma once

#include <unordered_map>
#include <vector>
#include <string>

namespace tx {

    class Cp {
        std::vector<std::string> names;
        /// @todo [future] similar chars?
    };

    using Base = std::unordered_map<char32_t, Cp>;

    Base loadBase();

}   // ns tx
