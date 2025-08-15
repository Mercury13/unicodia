#pragma once

namespace sw {

    struct Result {
        int nLines = 0;
        char32_t firstInequal = 0;
    };

    Result process();

}   // namespace sw
