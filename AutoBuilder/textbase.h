#pragma once

#include <unordered_map>
#include <set>
#include <string>

namespace tx {

    struct Cmp {
        using is_transparent = void;
        inline bool operator() (std::string_view x, std::string_view y) const noexcept
            { return (x < y); }
    };

    struct Cp
    {
        std::set<std::string, Cmp> names;
        /// @todo [future] similar chars?

        void eraseName(std::string_view x);
    };

    using Base = std::unordered_map<char32_t, Cp>;

    Base loadBase();

}   // ns tx
