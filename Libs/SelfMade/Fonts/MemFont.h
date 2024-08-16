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

    struct Cmap
    {
    public:
        unsigned short platformId = 0, encodingId = 0;
        uint32_t posInBlock = 0, posInFile = 0, length = 0;
        Buf1d<char> toBuf(Buf1d<char> data);
    };

    struct Block2
    {
        const Block* b = nullptr;
        Buf1d<char> d;

        operator bool() const { return b; }
    };

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
