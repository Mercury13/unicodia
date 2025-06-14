#pragma once

#include <cstddef>

namespace uc {
    struct LibNode;
}

namespace srh {

    enum class EmojiType : unsigned char {
        FULL,   ///< fully-qualified
        PART,   ///< minimally qualified
    };

    struct DecodedLine {
        const uc::LibNode* result;
        size_t index;
        unsigned length;
        EmojiType type;
    };

}   // namespace srh
