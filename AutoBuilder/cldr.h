#pragma once

#include <vector>
#include <string>
#include <unordered_map>

namespace cl {

    struct EmojiEntry {
        std::vector<std::string> names;
    };

    struct Base {
        std::unordered_map<char32_t, EmojiEntry> emoji;
    };

    Base load();

}