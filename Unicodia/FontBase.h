#pragma once

///
/// Base of local fonts.
/// Currently
///

// C++
#include <string_view>

// Misc libs
#include "u_TypedFlags.h"

namespace fb {

    enum class Fg : unsigned char {
        SMALL_CELL = 1,   ///< keep character cell small, for W11 font
                          ///< "Sans Serif Collection"
    };

    struct Info {
        Flags<Fg> flags = NO_FLAGS;

        static const Info EMPTY;
    };

    /// @return  Actual base entry, or nullptr
    const Info* getOrNull(std::string_view name);

    /// @return  Never nullptr; pointer just means we do not copy
    const Info* getOrEmpty(std::string_view name);

}   // namespace fb