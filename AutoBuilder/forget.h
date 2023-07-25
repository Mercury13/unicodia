#pragma once

#include <string>
#include <map>

namespace forget {

    struct Cp {
        /// Data extracted from library
        struct Lib {
            /// Possible capital counterpart, unused right now
            char32_t possibleCapital = 0;
            /// [+] the CP was found in library N times
            unsigned count = 0;
        } lib;

        /// Data extracted from UC database
        struct Database {
            /// Actual capital counterpart, unused right now
            char32_t capital = 0;
            /// Character name
            std::string name;
        } database;

        /// Data computed from smth found here in CPP file
        struct Computed {
            /// [+] the CP is computed to be in library
            bool isIn = false;
        } computed;
    };

    // Need sorting 0→max
    using Map = std::map<char32_t, Cp>;

    ///  @return [+] the codepoint should be in library
    bool isIn(char32_t cp, std::string_view name);

    void processCp(Map& map, char32_t cp, std::string_view name, char32_t capital);

    struct Stats {
        unsigned nRepeat = 0;
        unsigned nMissing = 0;
        unsigned nExtra = 0;
    };

    Stats postprocess(const Map& map, const char* fname);

}   // namespace forget
