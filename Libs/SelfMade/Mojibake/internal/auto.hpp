#pragma once
#include <algorithm>

namespace mojibake::detail {

    // Simple case fold
    constexpr int CF_BLOCKSHIFT = 5;
    constexpr int CF_BLOCKSIZE = 1 << CF_BLOCKSHIFT;
    constexpr int CF_BLOCKMASK = CF_BLOCKSIZE - 1;

    extern const char32_t simpleCfBlocks[89][32];
    extern const signed char simpleCfLookup[3914];

    constexpr char32_t CF_MAXCP = std::size(simpleCfLookup) * CF_BLOCKSIZE;

}   // mojibake::detail
