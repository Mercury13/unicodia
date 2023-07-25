#pragma once

#include <string>
#include <map>

namespace forget {

    struct Cp {
        std::string name;
        struct Manual {
            char32_t possibleCapital = 0;   ///< unused right now
            unsigned count = 0;
        } manual;
        struct Database {
            char32_t capital = 0;
            std::string name;
        } database;
    };

    // Need sorting 0→max
    using Map = std::map<char32_t, Cp>;

    ///  @return [+] the codepoint takes part in checking for forgotten CPs
    bool isIn(char32_t cp, std::string_view name);

}   // namespace forget
