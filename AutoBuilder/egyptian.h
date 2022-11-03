#pragma once

// STL
#include <unordered_map>
#include <string>

namespace egyp {

    using Base = std::unordered_map<char32_t, std::string>;

    Base loadBase();

}   // ns egyp
