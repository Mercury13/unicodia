#pragma once

// C++20 here
#include <bit>
#include <cstdint>


static_assert (sizeof(uint8_t) == 1, "Strange machine: uint8_t != 1");
union Word1 {
    uint16_t asWord;
    uint8_t asBytes[2];
    char asChars[2];
};
static_assert (sizeof(Word1) == 2, "Strange machine: Word1 != 2");

union Dword1 {
    uint32_t asDword;
    float asFloat;
    uint8_t asBytes[4];
    char asChars[4];
};
static_assert (sizeof(Dword1) == 4, "Strange machine: Dword1 != 4");

static_assert(std::endian::native == std::endian::big
            || std::endian::native == std::endian::little,
                "Strange machine: unknown endianness");

///  Swaps bytes of word ALWAYS
///
inline void doSwapW(Word1& x) {
    std::swap(x.asBytes[0], x.asBytes[1]);
}

///  Swaps bytes of word FROM/TO MOTOROLA ENDIANNESS
///
inline void swapMW(Word1& x) {
    if constexpr(std::endian::native == std::endian::little) {
        doSwapW(x);
    }
}

///  Swaps bytes of word FROM/TO INTEL ENDIANNESS
///
inline void swapIW(Word1& x) {
    if constexpr(std::endian::native == std::endian::big) {
        doSwapW(x);
    }
}

///  Swaps bytes of dword ALWAYS
///
inline void doSwapD(Dword1& x) {
    std::swap(x.asBytes[0], x.asBytes[3]);
    std::swap(x.asBytes[1], x.asBytes[2]);
}

///  Swaps bytes of dword FROM/TO MOTOROLA ENDIANNESS
///
inline void swapMD(Dword1& x) {
    if constexpr(std::endian::native == std::endian::little) {
        doSwapD(x);
    }
}

///  Swaps bytes of dword FROM/TO INTEL ENDIANNESS
///
inline void swapID(Dword1& x) {
    if constexpr(std::endian::native == std::endian::big) {
        doSwapD(x);
    }
}
