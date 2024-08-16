#pragma once

// C++
#include <cstdint>

// STL
#include <string_view>

// Libs
#include "u_Array.h"
#include "i_MemStream.h"
#include "u_Vector.h"

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

    constexpr unsigned short BAD_FORMAT = 55555;

    struct Cmap
    {
    public:
        unsigned short platformId = 0, encodingId = 0, formatId = BAD_FORMAT;
        uint32_t posInBlock = 0, posInFile = 0, length = 0;
        Buf1d<char> toBuf(Buf1d<char> data);
    };

    struct Block2
    {
        const Block* b = nullptr;
        Buf1d<char> d;

        operator bool() const { return b; }
    };

    enum class Plat {
        UCODE = 0,
        MAC = 1,
        ISO = 2,
        WIN = 3,
        CUSTOM = 4,
    };
    constexpr unsigned PLAT_N = 5;
    constexpr unsigned PLAT_N1 = 4;

    enum class EncUcode {
        U1_0 = 0,
        U1_1 = 1,
        ISO_10646 = 2,
        BMP = 3,
        FULL = 4,
        VS_14 = 5,
        FULL_13 = 6,
    };
    constexpr unsigned ENC_UNICODE_N = 7;

    enum class EncMac {
        ROMAN = 0,
        JAPN = 1,
        CHINESE_TRAD = 2,
        KORE = 3,
        ARAB = 4,
        HEBR = 5,
        GREK = 6,
        RUSSIAN = 7,
        RSYMBOL = 8,
        DEVA = 9,
        GURU = 10,
        GUJR = 11,
        ORYA = 12,
        BENG = 13,
        TAML = 14,
        TELU = 15,
        KNDA = 16,
        MLYM = 17,
        SINH = 18,
        MYMR = 19,
        KHMR = 20,
        THAI = 21,
        LAOO = 22,
        GEOR = 23,
        ARMN = 24,
        CHINESE_SIMP = 25,
        TIBT = 26,
        MONG = 27,
        ETHI = 28,
        SLAVIC = 29,
        VIETNAMESE = 30,
        SINDHI = 31,
        UNINTERPRETED = 32,
    };
    constexpr unsigned ENC_MAC_N = 33;

    enum class IsoEnc {
        ASCII = 0,
        ISO_10646 = 1,
        ISO_1 = 2
    };
    constexpr size_t ENC_ISO_N = 3;

    enum class WinEnc {
        SYMBOL = 0,
        UNICODE_BMP = 1,
        SHIFT_JIS = 2,
        PRC = 3,
        BIG5 = 4,
        WANSUNG = 5,
        JOHAB = 6,
        RESERVED_7 = 7,
        RESERVED_8 = 8,
        RESERVED_9 = 9,
        UNICODE_FULL = 10
    };
    constexpr unsigned ENC_WIN_N = 11;

    enum class TableFormat {
        SINGLE_BYTE = 0,
        HIGH_BYTE = 2,
        SEGMENT_TO_DELTA = 4,
        TRIMMED_TABLE = 6,
        MIXED = 8,
        TRIMMED_ARRAY = 10,
        SEGMENT_COVERAGE = 12,
        MANY_TO_ONE_RANGE = 13,
        UNICODE_VARIATION = 14,
    };
    constexpr unsigned TABLE_FORMAT_N = 15;

}   // namespace mf


class MemFont
{
public:
    void clear();
    bool load(const QString& fname);
    bool load(QIODevice& f);

    // High-level bhv
    mf::Block2 findBlock(mf::Char4 name) noexcept;
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
