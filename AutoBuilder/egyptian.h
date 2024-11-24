#pragma once

// STL
#include <unordered_map>
#include <string>

namespace egyp {

    struct Entry {
        std::string descEwp;        ///< description from English Wikipedia
        std::string descUnicode;    ///< description from Unicode
    };

    using Base = std::unordered_map<char32_t, Entry>;

    Base loadBase();

}   // ns egyp
