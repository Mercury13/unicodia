#pragma once

// C++
#include <cstdint>

// STL
#include <string_view>

// Libs
#include "u_Array.h"
#include "i_MemStream.h"
#include "u_Vector.h"
#include "function_ref.hpp"

class QIODevice;

namespace mf {

    struct Char4
    {
        union {
            char asChars[4] { 0, 0, 0, 0 };
            uint32_t asInt;
        } d;

        constexpr Char4() = default;
        constexpr Char4(const char (&x)[5])
            : d {{ x[0], x[1], x[2], x[3] }} {}
        std::string_view toSv() const { return { d.asChars, 4}; }
    };

    constexpr bool operator == (Char4 x, Char4 y) { return x.d.asInt == y.d.asInt; }
    constexpr bool operator != (Char4 x, Char4 y) { return x.d.asInt != y.d.asInt; }

    class Block
    {
    public:
        Char4 name;
        uint32_t posInDir = 0, posInFile = 0, length = 0;

        Buf1d<char> toBuf(Buf1d<char> data) const;
    };


    enum class Plat : unsigned short {
        UCODE = 0,
        MAC = 1,
        ISO = 2,
        WIN = 3,
        CUSTOM = 4,
        N = 5,
        REALLY_BAD = 55555,
    };
    constexpr auto PLAT_N  = (unsigned)Plat::N;
    constexpr auto PLAT_N1 = (unsigned)Plat::CUSTOM;

    enum class Enc : unsigned short {
        // Unicode
        UCODE_1_0 = 0,
        UCODE_1_1 = 1,
        UCODE_ISO_10646 = 2,
        UCODE_BMP = 3,
        UCODE_FULL = 4,
        UCODE_VS_14 = 5,
        UCODE_FULL_13 = 6,
        UCODE_N = 7,
        // Mac
        MAC_ROMAN = 0,
        MAC_JAPN = 1,
        MAC_CHINESE_TRAD = 2,
        MAC_KORE = 3,
        MAC_ARAB = 4,
        MAC_HEBR = 5,
        MAC_GREK = 6,
        MAC_RUSSIAN = 7,
        MAC_RSYMBOL = 8,
        MAC_DEVA = 9,
        MAC_GURU = 10,
        MAC_GUJR = 11,
        MAC_ORYA = 12,
        MAC_BENG = 13,
        MAC_TAML = 14,
        MAC_TELU = 15,
        MAC_KNDA = 16,
        MAC_MLYM = 17,
        MAC_SINH = 18,
        MAC_MYMR = 19,
        MAC_KHMR = 20,
        MAC_THAI = 21,
        MAC_LAOO = 22,
        MAC_GEOR = 23,
        MAC_ARMN = 24,
        MAC_CHINESE_SIMP = 25,
        MAC_TIBT = 26,
        MAC_MONG = 27,
        MAC_ETHI = 28,
        MAC_SLAVIC = 29,
        MAC_VIETNAMESE = 30,
        MAC_SINDHI = 31,
        MAC_UNINTERPRETED = 32,
        MAC_N = 33,
        // ISO
        ISO_ASCII = 0,
        ISO_10646 = 1,
        ISO_1 = 2,  ///< Encoding known as ISO-1
        ISO_N = 3,
        // Windows
        WIN_SYMBOL = 0,
        WIN_UNICODE_BMP = 1,
        WIN_SHIFT_JIS = 2,
        WIN_PRC = 3,
        WIN_BIG5 = 4,
        WIN_WANSUNG = 5,
        WIN_JOHAB = 6,
        WIN_RESERVED_7 = 7,
        WIN_RESERVED_8 = 8,
        WIN_RESERVED_9 = 9,
        WIN_UNICODE_FULL = 10,
        WIN_N = 11,
        // Really bad
        REALLY_BAD = 55555,
    };
    constexpr auto ENC_UNICODE_N = (unsigned)Enc::UCODE_N;
    constexpr auto ENC_MAC_N = (unsigned)Enc::MAC_N;
    constexpr auto ENC_ISO_N = (unsigned)Enc::ISO_N;
    constexpr auto ENC_WIN_N = (unsigned)Enc::WIN_N;

    enum class TableFormat : unsigned short {
        SINGLE_BYTE = 0,
        HIGH_BYTE = 2,
        SEGMENT_TO_DELTA = 4,
        TRIMMED_TABLE = 6,
        MIXED = 8,
        TRIMMED_ARRAY = 10,
        SEGMENT_COVERAGE = 12,
        MANY_TO_ONE_RANGE = 13,
        UNICODE_VARIATION = 14,
        N = 15,
        REALLY_BAD = 55555,
    };
    constexpr auto TABLE_FORMAT_N = (unsigned)TableFormat::N;

    struct Cmap
    {
    public:
        Plat platformId = Plat::REALLY_BAD;
        Enc encodingId = Enc::REALLY_BAD;
        TableFormat formatId = TableFormat::REALLY_BAD;
        uint32_t posInBlock = 0, posInFile = 0, length = 0;
        Buf1d<char> toBuf(Buf1d<char> data);
    };

    struct Block2
    {
        const Block* b = nullptr;
        Buf1d<char> d;

        operator bool() const { return b; }
    };

    /// Char32_t =
    using CbObjGlyph = tl::function_ref<void(char32_t, unsigned)>;

}   // namespace mf


class MemFont
{
public:
    void clear();
    bool load(const QString& fname);
    bool load(QIODevice& f);

    // High-level bhv
    mf::Block2 findBlock(mf::Char4 name);
    mf::Block2 rqBlock(mf::Char4 name, uint32_t len = 0);
    /// @param bytes  ASCII only!!
    void mangle(std::string_view bytes);

    // Misc info
    /// prefer over nChildren
    size_t nBlocks() const noexcept { return fBlocks.size(); }
    Buf1d<const char> data() const noexcept { return slave.data(); }
    auto qdata() const { return slave.qdata(); }
    Mems& stream() & noexcept { return slave; }
    Mems&& stream() && noexcept { return std::move(slave); }
    Mems&& giveStream() noexcept { return std::move(slave); }
    size_t dataSize() const noexcept { return slave.size(); }
    Buf1d<const mf::Block> blocks() const noexcept
        { return { fBlocks.size(), fBlocks.data() }; }
    Buf1d<const mf::Cmap> cmaps() const noexcept
        { return { fCmaps.size(), fCmaps.data() }; }
private:
    Mems slave;
    SafeVector<mf::Block> fBlocks;
    SafeVector<mf::Cmap> fCmaps;

    bool readDir();
    mf::Block readBlockEntry();
    void recomputeChecksum(const mf::Block& b);
    void loadCmaps();
};
