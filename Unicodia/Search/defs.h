#pragma once

#include <cstddef>

namespace srh {

    /// @warning From best to worst;
    ///     unrelated (PART and UNKNOWN_FLAG) in any order
    enum class EmojiLevel : unsigned char {
        FULL,   ///< fully-qualified
        PART,   ///< minimally qualified/unqualified
        UNKNOWN_FLAG,  ///< unknown flag        
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
