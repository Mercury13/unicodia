#pragma once

#include <map>
#include <string>

namespace lb {

    struct Entry {
        char32_t rangeEnd;
        std::string value;
    };

    using Base = std::map<char32_t, Entry>;

    Base load();

}   // namespace lb
