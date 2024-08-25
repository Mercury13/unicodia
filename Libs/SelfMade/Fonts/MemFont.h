#pragma once

// MemFont
#include "MemFontDefs.h"

// C++
#include <cstdint>
#include <filesystem>

// STL
#include <string_view>

// Libs
#include "u_Array.h"
#include "i_MemStream.h"
#include "u_Vector.h"
#include "function_ref.hpp"

class QIODevice;
class QString;

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

        Buf1d<const char> toBuf(Buf1d<const char> data) const;
        Buf1d<char> toBuf(Buf1d<char> data) const;
    };

    struct Cmap
    {
    public:
        Plat platformId = Plat::REALLY_BAD;
        Enc encodingId = Enc::REALLY_BAD;
        TableFormat formatId = TableFormat::REALLY_BAD;
        uint32_t posInBlock = 0, posInFile = 0, length = 0;

        Buf1d<const char> toBuf(Buf1d<const char> data) const noexcept;
        /// @return [+] My library supports this CMAP
        bool isSupported() const noexcept;
    };

    struct Block2
    {
        const Block* b = nullptr;
        Buf1d<const char> d;

        operator bool() const noexcept { return b; }
        Buf1d<char> toWriteable() const noexcept
            { return { d.size(), const_cast<char*>(d.buffer()) }; }
    };

    /// Char32_t = CP
    /// unsigned = glyph
    using CbCpGlyph = tl::function_ref<void(char32_t, unsigned)>;

}   // namespace mf


class MemFont
{
public:
    void clear();
    bool load(const std::filesystem::path& fname);
    bool load(std::istream& f);
#ifdef QT_CORE_LIB
    bool load(const QString& fname);
    bool load(QIODevice& f);
#endif

    // High-level bhv
    mf::Block2 findBlock(mf::Char4 name);
    mf::Block2 rqBlock(mf::Char4 name, uint32_t len = 0);
    /// @param  bytes   ASCII only!!
    /// @warning  Does nothing if bytes are empty
    void mangle(std::string_view bytes);
    /// @param  iGlyph   index of glyph
    /// @warning   NOT CODE POINT
    /// @return  [+] dehinting actually happened
    ///          (errors are indicated with exceptions)
    bool dehintGlyph(unsigned iGlyph);

    // Misc info
    /// prefer over nChildren
    size_t nBlocks() const noexcept { return fBlocks.size(); }
    Buf1d<const char> data() const noexcept { return slave.data(); }
#ifdef QT_CORE_LIB
    auto qdata() const { return slave.qdata(); }
#endif
    Mems& stream() & noexcept { return slave; }
    Mems&& stream() && noexcept { return std::move(slave); }
    Mems&& giveStream() noexcept { return std::move(slave); }
    size_t dataSize() const noexcept { return slave.size(); }
    Buf1d<const mf::Block> blocks() const noexcept
        { return { fBlocks.size(), fBlocks.data() }; }
    Buf1d<const mf::Cmap> cmaps() const noexcept
        { return { fCmaps.size(), fCmaps.data() }; }
    /// @return [+] OK
    bool traverseCps(mf::CbCpGlyph cb) const;
    bool traverseCps(const mf::Cmap* cmap, mf::CbCpGlyph cb) const;
    bool traverseCps(const mf::Cmap& cmap, mf::CbCpGlyph cb) const;
    /// @return  [+] The best CMAP possible, always isSupported()
    ///          [0] none found
    /// @warning  Priority is Unicode BMP < Unicode full
    ///           regardless of format (of course, if supported)
    ///           The first if several found
    const mf::Cmap* getBestCmap() const;
private:
    Mems slave;
    SafeVector<mf::Block> fBlocks;
    SafeVector<mf::Cmap> fCmaps;

    bool readDir();
    mf::Block readBlockEntry();
    void recomputeChecksum(const mf::Block& b);
    void loadCmaps();
    /// Function common for Qt and Filesystem
    bool finishLoading();
    /// @return [+] cmap is good
    bool traverseSegmentToDelta(const mf::Cmap& cmap, mf::CbCpGlyph cb) const;
    bool traverseSegmentCoverage(const mf::Cmap& cmap, mf::CbCpGlyph cb) const;
};
