#pragma once

#include <cstddef>

#define SRH_LEVELS \
    FULL, PART, UNKNOWN_FLAG

namespace srh {

    /// @warning From best to worst;
    ///     unrelated (PART and UNKNOWN_FLAG) in any order
    enum class EmojiLevel : unsigned char {
        SRH_LEVELS
    };

    /// @tparam R
    ///    Needed for unit-testing: won’t pull Unicode bases
    ///
    template <class R>
    struct Decoded {
        R result;
        size_t index;
        unsigned length;
        EmojiLevel level;
    };

}   // namespace srh
