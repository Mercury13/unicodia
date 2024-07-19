#pragma once

#include <cstddef>

namespace srh {

    template <class T>
    struct Decoded {
        size_t index;
        T result;
    };

}   // namespace srh
