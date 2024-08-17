// My header
#include "MemFont.h"

// STL
#include <iostream>

// Qt
#include <QFile>

constexpr bool debugMemFont = false;

template <class... Args>
inline void msg0([[maybe_unused]] Args&&... x)
{
    if constexpr (debugMemFont) {
        ((std::cout << x), ...);
    }
}

template <class... Args>
inline void msg([[maybe_unused]] Args&&... x)
{
    if constexpr (debugMemFont) {
        ((std::cout << x), ...);
        // OK, this is debug and flushing is really needed
        std::cout << std::endl;
    }
}

inline void msgln()
{
    if constexpr (debugMemFont) {
        // OK, this is debug and flushing is really needed
        std::cout << std::endl;
    }
}

///// Block ////////////////////////////////////////////////////////////////////

namespace HEADER {
    /// Offset: # of tables
    constexpr auto OFS_NTABLES = 4;
    /// Size of one table
    constexpr auto SIZE = 12;
}

Buf1d<const char> mf::Block::toBuf(Buf1d<const char> data) const
{
    auto end = posInFile + length;
    if (end > data.size() || end < posInFile) {
        char buf[100];
        snprintf(buf, std::size(buf), "[Block.toBuf] File overrun in %.4s: pos %lld+%lld, file size %lld",
                 name.d.asChars,
                 static_cast<unsigned long long>(posInFile),
                 static_cast<unsigned long long>(length),
                 static_cast<unsigned long long>(data.size()));
        throw std::logic_error(buf);
    }
    return { length, data.buffer() + posInFile };
}


Buf1d<char> mf::Block::toBuf(Buf1d<char> data) const
{
    Buf1d<const char> d1(data);
    auto r = toBuf(d1);
    return { r.size(), const_cast<char*>(r.buffer()) };
}


///// Cmap /////////////////////////////////////////////////////////////////////


Buf1d<const char> mf::Cmap::toBuf(Buf1d<const char> data) const noexcept
{
    return data.sliceMid(posInFile, length);
}


bool mf::Cmap::isSupported() const noexcept
{
    switch (formatId) {
    case mf::TableFormat::SEGMENT_TO_DELTA:
    case mf::TableFormat::SEGMENT_COVERAGE:
        return true;
    default:
        return false;
    }
}


///// MemFont //////////////////////////////////////////////////////////////////


bool MemFont::load(const QString& fname)
{
    QFile f(fname);
    if (!f.open(QIODevice::ReadOnly))
        return false;
    return load(f);
}


void MemFont::clear()
{
    fBlocks.clear();
    fCmaps.clear();
}

bool MemFont::load(QIODevice& f)
{
    clear();
    auto sz = f.size();
    slave.alloc(sz);
    f.read(slave.beg(), sz);
    bool b = readDir();
    if (b) {
        loadCmaps();
    }
    return b;
}

mf::Block MemFont::readBlockEntry()
{
    mf::Block r;
    r.posInDir = slave.pos();
    slave.readStruc<4>(r.name);
    slave.skipD();  // skip checksum;
    r.posInFile = slave.readMD();
    r.length = slave.readMD();
    return r;
}

bool MemFont::readDir()
{
    // All data is Motorola here
    slave.seek(HEADER::OFS_NTABLES);
    unsigned nTables = slave.readMW();
    fBlocks.reserve(nTables);

    slave.seek(HEADER::SIZE);
    for (unsigned iTable = 0; iTable < nTables; ++iTable) {
        mf::Block table = readBlockEntry();
        fBlocks.push_back(table);
    }

    if constexpr (debugMemFont) {
        msg0("Read blocks:");
        for (auto& v : fBlocks) {
            msg0(' ', v.name.toSv());
        }
        msgln();
    }

    return true;
}


void MemFont::loadCmaps()
{
    Mems ms;
    int blockOffset;
    uint32_t blockSize;
    try {
        auto blk = findBlock("cmap");
        if (!blk)
            return;
        blockOffset = blk.b->posInFile;
        blockSize = blk.b->length;
        ms.borrowR(blk.d);
    } catch (...) {
        // Really bad, clear CMAPs
        fCmaps.clear();
        return;
    }

    try {
        auto version = ms.readMW();
        if (version != 0)
            return;
        unsigned nMaps = ms.readMW();
        fCmaps.reserve(nMaps);
        for (unsigned i = 0; i < nMaps; ++i) {
            mf::Cmap newCmap;
            newCmap.platformId = (mf::Plat)ms.readMW();
            newCmap.encodingId = (mf::Enc)ms.readMW();
            newCmap.formatId = mf::TableFormat::REALLY_BAD;
            newCmap.posInBlock = ms.readMD();
            if (newCmap.posInBlock > blockSize)
                continue;
            newCmap.posInFile = newCmap.posInBlock + blockOffset;
            newCmap.length = blockSize - newCmap.posInBlock;
            fCmaps.push_back(newCmap);
        }
    } catch (...) {
        // Leave as is!
    }

    for (auto& cm : fCmaps) {
        if (cm.length >= 8) {
            ms.seek(cm.posInBlock);
            cm.formatId = (mf::TableFormat)ms.readMW();
            switch (static_cast<mf::TableFormat>(cm.formatId)) {
            case mf::TableFormat::SINGLE_BYTE:
            case mf::TableFormat::HIGH_BYTE:
            case mf::TableFormat::SEGMENT_TO_DELTA:
            case mf::TableFormat::TRIMMED_TABLE:
                // Length mword, right after
                cm.length = ms.readMW();
                break;
            case mf::TableFormat::MIXED:
            case mf::TableFormat::TRIMMED_ARRAY:
            case mf::TableFormat::SEGMENT_COVERAGE:
            case mf::TableFormat::MANY_TO_ONE_RANGE:
                ms.skip(2);
                cm.length = ms.readMD();
                break;
            case mf::TableFormat::UNICODE_VARIATION:
                cm.length = ms.readMD();
                break;
            default: ;
            }
        }
    }
}


mf::Block2 MemFont::findBlock(mf::Char4 name)
{
    for (auto& v : fBlocks)
        if (v.name == name)
            return { &v, v.toBuf(slave.data()) };
    return {};
}


mf::Block2 MemFont::rqBlock(mf::Char4 name, uint32_t len)
{
    char buf[100];
    auto r = findBlock(name);
    if (!r) {
        snprintf(buf, sizeof(buf), "Block <%.4s> not found", name.d.asChars);
        throw std::logic_error(buf);
    }
    if (r.b->length <= len) {
        snprintf(buf, sizeof(buf), "Block <%.4s> wanted %lu, found %lu",
                 name.d.asChars, long(len), long(r.b->length));
        throw std::logic_error(buf);
    }
    return r;
}

void MemFont::mangle(std::string_view bytes)
{
    if (bytes.empty())
        return;
    auto v = rqBlock("name", 32);
    Mems blk(v.toWriteable());

    blk.skipW();    // should be 0
    unsigned nRecs = blk.readMW();
    unsigned stringOffset = blk.readMW();
    msg(nRecs, " records found");
    msg0("Records found:");
    for (unsigned i = 0; i < nRecs; ++i) {
        blk.skip(6);    // platform, platformSpecific, language
        unsigned nameId = blk.readMW();
        unsigned length = blk.readMW();
        unsigned offset = blk.readMW();
        msg0(' ', nameId);
        if (nameId == 1 || nameId == 4 || nameId == 6) {
            if (length < bytes.length()) {
                throw std::logic_error("Font name is too short");
            }
            auto oldPos = blk.pos();
            blk.seek(stringOffset + offset);
            for (auto v : bytes)
                blk.writeMW(v);
            blk.seek(oldPos);
        }
    }
    msgln();
    recomputeChecksum(*v.b);
}

bool MemFont::dehintGlyph(unsigned iGlyph)
{
    auto vHead = rqBlock("head", 0x34);

    // Head
    // 0:w = major version = 1
    // 2:w = minor version = 0
    // 4:fix4 = fontRevision
    // 8:d = checksumAdjustment for TTC
    // 0C:d = 0x5F0F3CF5
    // 10:w = flags
    // 12:w = unitsPerEm
    // 14:date8 = dateCreated
    // 1C:date8 = dateModified
    // 24:w = xMin
    // 26:w = yMin
    // 28:w = xMax
    // 2A:w = yMax
    // 2C:w = macStyle
    // 2E:w = lowestReadablePpem
    // 30:w = directionHint
    // 32:w = indexToLocFormat
    // 34:w = glyphDataFormat(0)
    // ===== size = $36
    Mems blk(vHead.d);
    auto majorVersion = blk.readMW();
    if (majorVersion != 1)
        throw std::logic_error("[dehintGlyph] Major version should be 1");
    auto minorVersion = blk.readMW();
    if (minorVersion != 0)
        throw std::logic_error("[dehintGlyph] Minor version should be 0");
    blk.seek(0x0C);
    auto magic = blk.readMD();
    if (magic != 0x5F0F3CF5)
        throw std::logic_error("[dehintGlyph] Magic should be 5F0F3CF5");
    blk.seek(0x32);
    auto locaFormat = blk.readMW();
    unsigned unitSize = 2;
    switch (locaFormat) {
    case 0: break;
    case 1: unitSize = 4; break;
    default:
        throw std::logic_error("[dehintGlyph] Loca format should be 0 or 1");
    }

    // Now read loca
    auto position = iGlyph * unitSize;
    auto vLoca = rqBlock("loca", position + (unitSize << 1));
    blk.borrowR(vLoca.d);
    blk.seek(position);
    unsigned glyfOffset, nextOffset;
    if (locaFormat == 0) {
        glyfOffset = (unsigned)blk.readMW() << 1;
        nextOffset = (unsigned)blk.readMW() << 1;
    } else {
        glyfOffset = blk.readMD();
        nextOffset = blk.readMD();
    }
    // Really have data for that glyph?
    if (glyfOffset >= nextOffset)
        return false;

    //
    // NOW READ glyf
    //
    auto vGlyf = rqBlock("glyf", nextOffset);
    auto data = vGlyf.toWriteable();
    blk.borrowRW(data);
    blk.seek(glyfOffset);

    int16_t nContours = blk.readMW();
    if (nContours < 0)  // composite glyph
        return false;
    blk.skip(8 + (nContours << 1));    // xmin, ymin, xmax, ymax, endPts
    auto posInstrLength = blk.pos();
    auto instrLength = blk.readMW();
    if (instrLength == 0)
        return false;

    // Now rewrite!
    blk.seek(posInstrLength);
    blk.writeMW(0);
    auto pDest = blk.ptr();
    auto pSrc = pDest + instrLength;
    auto pEnd = data.begin() + nextOffset;
    std::copy(pSrc, pEnd, pDest);

    // Version 2: fill with NOP-like instructions
    // The only instruction that does nothing (turn off rounding)
    // for (unsigned i = 0; i < instrLength; ++i) {
    //     blk.writeB(0x7A);
    // }

    recomputeChecksum(*vGlyf.b);
    return true;
}

void MemFont::recomputeChecksum(const mf::Block& b)
{
    slave.seek(b.posInFile);
    uint32_t sum = 0;
    for (auto nLongs = (b.length + 3) / 4; nLongs > 0; --nLongs) {
        sum += slave.readMD();
    }
    slave.seek(b.posInDir + 4);
    slave.writeMD(sum);
}


bool MemFont::traverseSegmentToDelta(const mf::Cmap& cmap, mf::CbCpGlyph cb) const
{
    Mems ms(cmap.toBuf(slave.data()));
    ms.seek(6);  // 0:w = format  2:w = length   4:w = lang   6:w = segCount
    unsigned short segCount2 = ms.readMW();
    if ((segCount2 & 1) != 0)
        throw std::logic_error("[MemFont.traverseSegmentToDelta] Segment to delta block has odd segCountX2");
    ms.skip(6);      // 8:w = searchRange 0A:w = entrySelector 0C:w = rangeShift
    auto pEndCode = ms.pos();
    ms.skip(segCount2);
    ms.skip(2);      // +0:w = reservedPad
    auto pStartCode = ms.pos();
    ms.skip(segCount2);
    auto pDelta = ms.pos();
    ms.skip(segCount2);
    auto pRangeOffset = ms.pos();
    for (unsigned offset = 0; offset < segCount2; offset += 2) {
        ms.seek(pStartCode + offset);
        auto startCode = ms.readMW();
        ms.seek(pEndCode  + offset);
        auto endCode = ms.readMW();
        ms.seek(pDelta + offset);
        int16_t delta = ms.readMW();
        auto ro = pRangeOffset + offset;
        ms.seek(ro);
        auto rangeOffset = ms.readMW();
        if (rangeOffset == 0) {
            // Code-based mapping
            for (char32_t c = startCode; c <= endCode; ++c) {
                uint16_t glyph = c + delta;
                if (glyph != 0)
                    cb(c, glyph);
            }
        } else {
            // Array-based mapping
            ms.seek(ro + rangeOffset);
            for (char32_t c = startCode; c <= endCode; ++c) {
                auto glyph = ms.readMW();
                if (glyph != 0) {
                    cb(c, static_cast<uint16_t>(glyph + delta));
                }
            }
        }
    }
    return true;
}


bool MemFont::traverseSegmentCoverage(const mf::Cmap& cmap, mf::CbCpGlyph cb) const
{
    Mems ms(cmap.toBuf(slave.data()));
    ms.seek(12);  // 0:w = format  2:w = reserved   4:d = len   8:d = lang
    auto nGroups = ms.readMD();
    for (unsigned i = 0; i < nGroups; ++i) {
        auto startCode = ms.readMD();
        auto endCode = ms.readMD();
        auto glyphId = ms.readMD();
        for (char32_t c = startCode; c <= endCode; ++c) {
            cb(c, glyphId);
            ++glyphId;
        }
    }
    return true;
}



namespace {

    enum class Prio { NONE, BMP };
    struct Best {
        const mf::Cmap* cmap  = nullptr;
        Prio prio = Prio::NONE;

        void bump(Prio pr, const mf::Cmap& cm);
    };

    void Best::bump(Prio pr, const mf::Cmap& cm)
    {
        if (prio < pr) {
            cmap = &cm;
            prio = pr;
        }
    }

}   // anon namespace


const mf::Cmap* MemFont::getBestCmap() const
{
    Best best;
    for (auto& cmap : fCmaps) {
        if (!cmap.isSupported())
            continue;
        switch (cmap.platformId) {
        // We traverse whatever first, UCODE or WIN
        case mf::Plat::UCODE:
            switch (cmap.encodingId) {
            case mf::Enc::UCODE_BMP:
                best.bump(Prio::BMP, cmap);
                break;
            case mf::Enc::UCODE_FULL:
                // Full Unicode is automatically the best cmap
                return &cmap;
            default: ;
            }
            break;
        case mf::Plat::WIN:
            switch (cmap.encodingId) {
            case mf::Enc::WIN_UNICODE_BMP:
                best.bump(Prio::BMP, cmap);
                break;
            case mf::Enc::WIN_UNICODE_FULL:
                // Full Unicode is automatically the best cmap
                return &cmap;
            default: ;
            }
            break;
        // ISO could be a fallback platform, but neither of Unicodia’s fonts needs it
        default:
            break;
        }
    }
    return best.cmap;
}


bool MemFont::traverseCps(const mf::Cmap* cmap, mf::CbCpGlyph cb) const
{
    return cmap && traverseCps(*cmap, cb);
}

bool MemFont::traverseCps(const mf::Cmap& cmap, mf::CbCpGlyph cb) const
{
    switch (cmap.formatId) {
    case mf::TableFormat::SEGMENT_TO_DELTA:
        return traverseSegmentToDelta(cmap, cb);
    case mf::TableFormat::SEGMENT_COVERAGE:
        return traverseSegmentCoverage(cmap, cb);
    // ISO could be a fallback platform, but neither of Unicodia’s fonts needs it
    default:
        return false;
    }
}


bool MemFont::traverseCps(mf::CbCpGlyph cb) const
{
    return traverseCps(getBestCmap(), cb);
}
