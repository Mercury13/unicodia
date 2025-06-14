#pragma once

#include <cstddef>

namespace srh {

    enum class EmojiType : unsigned char {
        FULL,   ///< fully-qualified
        PART,   ///< minimally qualified
        UNKNOWN_FLAG,  ///< unknown flag
    };

    /// @tparam R
    ///    Needed for unit-testing: won’t pull things here
    template <class R>
    struct Decoded {
        R result;
        size_t index;
        unsigned length;
        EmojiType type;
    };

}   // namespace srh
