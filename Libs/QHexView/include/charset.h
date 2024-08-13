#pragma once

// STL
#include <array>

namespace ch {

    constexpr unsigned short controlPrintables[] {
        0x00B7, 0x263A, 0x263B, 0x2665,   0x2666, 0x2663, 0x2660, 0x25CF,   0x25D8, 0x25CB, 0x25D9, 0x2642,   0x2640, 0x266A, 0x266B, 0x263C,
        0x25BA, 0x25C4, 0x2195, 0x203C,   0x00B6, 0x00A7, 0x25AC, 0x21A8,   0x2191, 0x2193, 0x2192, 0x2190,   0x221F, 0x2194, 0x25B2, 0x25BC,
    };
    static_assert(std::size(controlPrintables) == 32);

    using Table = unsigned short[256];

    struct SinglePatch {
        const unsigned char from;
        const unsigned short to;
        constexpr void doPatch(Table& t) const noexcept { t[from] = to; }
    };

    template <unsigned N>
    struct SpanPatch {
        const unsigned char from;
        const unsigned short to[N];
        constexpr void doPatch(Table& t) const noexcept;
    };

    template <unsigned N>
    SpanPatch(unsigned char from, const unsigned short (&to)[N]) -> SpanPatch<N>;

    constexpr SpanPatch ibmControls { 0u,
        { 0x00B7, 0x263A, 0x263B, 0x2665,   0x2666, 0x2663, 0x2660, 0x25CF,   0x25D8, 0x25CB, 0x25D9, 0x2642,   0x2640, 0x266A, 0x266B, 0x263C,
          0x25BA, 0x25C4, 0x2195, 0x203C,   0x00B6, 0x00A7, 0x25AC, 0x21A8,   0x2191, 0x2193, 0x2192, 0x2190,   0x221F, 0x2194, 0x25B2, 0x25BC,
        }};

    struct IbmAsciiPatch {      // Patch for IBM-ASCII
        constexpr IbmAsciiPatch() noexcept = default;

        constexpr void doPatch(Table& t) const noexcept
        {
            ibmControls.doPatch(t);
            t[127] = 0x2302;
        }
    };

    class Set {
    public:
        template <class... Patches>
            constexpr Set(const Patches&... patches) noexcept;
        unsigned short operator [] (unsigned char x) const noexcept { return data[x]; }
    public:
        Table data;
        constexpr void init() noexcept;
    };

}   // namespace ch

template <unsigned N>
constexpr void ch::SpanPatch<N>::doPatch(Table& t) const noexcept
{
    for (unsigned i = 0; i < N; ++i) {
        t[from + i] = to[i];
    }
}

constexpr void ch::Set::init() noexcept
{
  for (unsigned i = 0; i < 256; ++i)
      data[i] = i;
}

template <class... Patches>
constexpr ch::Set::Set(const Patches&... patches) noexcept
{
    init();
    (patches.doPatch(data), ...);
}
