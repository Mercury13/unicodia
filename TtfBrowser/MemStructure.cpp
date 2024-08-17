#include "MemStructure.h"

#include "u_Strings.h"

///// Misc functions ///////////////////////////////////////////////////////////


namespace {
    constinit const char* platNames[] {
        "Unicode", "Mac", "ISO", "Win", "Custom"
    };
    static_assert(std::size(platNames) == mf::PLAT_N);

    constinit const char* unicodeEncNames[] {
        "U1.0", "U1.1", "ISO 10646", "BMP", "Full", "VarSeq", "Full13",
    };
    static_assert(std::size(unicodeEncNames) == mf::ENC_UNICODE_N);

    constinit const char* macEncNames[] {
        "Roman", "Japanese", "Chinese (trad)", "Korean", "Arabic",
        "Hebrew", "Greek", "Russian", "RSymbol", "Devanagari",
        "Gurmukhi", "Gujarati", "Odia", "Bangla", "Tamil", "Telugu",
        "Kannada", "Malayalam", "Sinhala", "Myanmar", "Khmer", "Thai",
        "Lao", "Georgian", "Armenian", "Chinese (simp)", "Tibetan",
        "Mongol", "Ethiopic", "Slavic", "Vietnamese", "Sindhi",
        "Uninterpreted"
    };
    static_assert(std::size(macEncNames) == mf::ENC_MAC_N);

    constinit const char* isoEncNames[] {
        "ASCII", "ISO 10646", "Latin-1"
    };
    static_assert(std::size(isoEncNames) == mf::ENC_ISO_N);

    constinit const char* winEncNames[] {
        "Symbol", "Uni BMP", "ShiftJIS", "PRC", "Big5", "Wansung",
        "Johab", nullptr, nullptr, nullptr, "Uni full"
    };
    static_assert(std::size(winEncNames) == mf::ENC_WIN_N);

    constinit const Buf1d<const char* const> encNames2d[] {
        { mf::ENC_UNICODE_N, unicodeEncNames },
        { mf::ENC_MAC_N, macEncNames },
        { mf::ENC_ISO_N, isoEncNames },
        { mf::ENC_WIN_N, winEncNames },
    };
    static_assert(std::size(encNames2d) == mf::PLAT_N1);

    constinit const char* tableFormatNames[] {
        "Single byte", nullptr, "High byte", nullptr,
        "Seg to delta", nullptr, "Trimmed table", nullptr,
        "Mixed", nullptr, "Trimmed array", nullptr,
        "Seg coverage", "Many to one range", "Uni var",
    };
    static_assert(std::size(tableFormatNames) == mf::TABLE_FORMAT_N);

}   // anon namespace

mfs::MiniString mfs::toPlatformMiniString(mf::Plat plat) noexcept
{
    if (plat < mf::Plat::N) {
        return { .value = platNames[(unsigned)plat], .isStandard = false };
    }
    return { .value = "Unknown", .isStandard = true };
}

const char* mfs::toPlatformShortString(mf::Plat plat, mfs::Cbuf& buf) noexcept
{
    auto q = toPlatformMiniString(plat);
    if (q.isStandard) {
        snprintf(buf, std::size(buf), "%u", (unsigned)plat);
        return buf;
    } else {
        return q.value;
    }
}

const char* mfs::toPlatformLongString(mf::Plat plat, mfs::Cbuf& buf) noexcept
{
    auto q = toPlatformMiniString(plat);
    snprintf(buf, std::size(buf), "%s (%u)",
             q.value, (unsigned)plat);
    return buf;
}

mfs::MiniString mfs::toEncodingMiniString(mf::Plat plat, mf::Enc enc) noexcept
{
    if (plat < mf::Plat::N) {
        auto arr = encNames2d[(unsigned)plat];
        if ((size_t)enc < arr.size()) {
            if (const char* z = arr[(size_t)enc]) {
                return { .value = z, .isStandard = false };
            } else {
                return { .value = "Reserved", .isStandard = true };
            }
        }
    }
    return { .value = "Unknown", .isStandard = true };
}

const char* mfs::toEncodingShortString(mf::Plat plat, mf::Enc enc, Cbuf& buf) noexcept
{
    auto q = toEncodingMiniString(plat, enc);
    if (q.isStandard) {
        snprintf(buf, std::size(buf), "%u", (int)enc);
        return buf;
    } else {
        return q.value;
    }
}

const char* mfs::toEncodingLongString(mf::Plat plat, mf::Enc enc, Cbuf& buf) noexcept
{
    auto q = toEncodingMiniString(plat, enc);
    snprintf(buf, std::size(buf), "%s (%u)",
             q.value, (unsigned)enc);
    return buf;
}


mfs::MiniString mfs::toTableFormatMiniString(mf::TableFormat fm) noexcept
{
    if (fm < mf::TableFormat::N) {
        if (auto q = tableFormatNames[(unsigned)fm]) {
            return { .value = q, .isStandard = false };
        }
    }
    return { .value = "Unknown", .isStandard = true };
}


const char* mfs::toTableFormatLongString(mf::TableFormat fm, mfs::Cbuf& buf) noexcept
{
    auto q = toTableFormatMiniString(fm);
    snprintf(buf, std::size(buf), "%s (%u)",
             q.value, (unsigned)fm);
    return buf;
}


///// Obj //////////////////////////////////////////////////////////////////////


Buf1d<const char> mfs::Obj::body(Buf1d<const char> entireFile) const noexcept
{
    auto ds = dataSpan();
    if (!ds)
        return {};
    return entireFile.sliceMid(ds->fileOffset, ds->size);
}


///// Block ////////////////////////////////////////////////////////////////////

const mfs::Obj& mfs::Block::childAt(size_t) const
{
    throw std::out_of_range("[Block.childAt] Block has no children");
}


///// Cmap /////////////////////////////////////////////////////////////////////


const mfs::Obj& mfs::Cmap::childAt(size_t) const
{
    throw std::out_of_range("[Cmap.childAt] Cmap has no children");
}


std::optional<mfs::CmapInfo> mfs::Cmap::cmapInfo() const noexcept
{
    return CmapInfo{
        .platformId = slave.platformId,
        .encodingId = slave.encodingId,
        .formatId = slave.formatId,
    };
}

mfs::Text mfs::Cmap::text() const noexcept
{
    Cbuf buf1, buf2;
    auto text1 = toPlatformShortString(slave.platformId, buf1);
    auto text2 = toEncodingShortString(slave.platformId, slave.encodingId, buf2);
    return {
        .value = str::cat(text1, "/", text2),
        .isConstantLength = false
    };
}


///// CmapBlock ////////////////////////////////////////////////////////////////


mfs::CmapBlock::CmapBlock(const mf::Block& aBlock, Buf1d<const mf::Cmap> aCmaps)
    : Super(aBlock)
{
    fChildren.reserve(aCmaps.size());
    for (auto& v : aCmaps) {
        fChildren.emplace_back(std::make_shared<mfs::Cmap>(v));
    }
}


///// Font /////////////////////////////////////////////////////////////////////


size_t mfs::Font::nChildren() const noexcept
{
    return blocks.size();
}

const mfs::Block& mfs::Font::childAt(size_t i) const
{
    if (i >= nChildren())
        throw std::logic_error("[MemFont.childAt] Index out of range");
    return *blocks[i];
}


void mfs::Font::clear()
{
    slave = nullptr;
    fileSize = 0;
    blocks.clear();
    cmap.block.reset();
    cmap.index = 0;
}


void mfs::Font::loadFrom(const MemFont& font)
{
    slave = &font;
    fileSize = font.dataSize();
    blocks.clear();
    blocks.reserve(slave->nBlocks());
    unsigned index = 0;
    for (auto& blk : slave->blocks()) {
        if (blk.name == "cmap") {
            cmap.block = std::make_shared<CmapBlock>(blk, font.cmaps());
            cmap.index = index;
            blocks.push_back(cmap.block);
        } else {
            blocks.push_back(std::make_unique<Block>(blk));
        }
        ++index;
    }
}
