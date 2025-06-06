#pragma once

// STL
#include <unordered_map>
#include <string>

#include "UcFlags.h"

namespace egyp {

    struct Entry {
        std::string descEwp;        ///< description from English Wikipedia
        std::string descUnicode;    ///< description from Unicode
        std::string meaning;        ///< meaning from UniKemet
        std::u32string equivSequence;  ///< equivalent sequence for legacy hiero
        uc::EgypReliability reliability = uc::EgypReliability::EXTENDED;
    };

    using Base = std::unordered_map<char32_t, Entry>;

    Base loadBase();

}   // ns egyp
