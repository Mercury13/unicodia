#pragma once

// STL
#include <string>
#include <unordered_map>
#include <vector>

struct Entities {
    std::unordered_map<char32_t, std::vector<std::string>> data;
    struct Stats {
        struct Entities {
            size_t nGood = 0;
            size_t nTooLong = 0;
            size_t nWrong = 0;
        } entities;
    } stats;
};

Entities readEntities(const char* fname);
