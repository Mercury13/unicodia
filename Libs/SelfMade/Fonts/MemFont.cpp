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

Buf1d<char> mf::Block::toBuf(Buf1d<char> data) const
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
        ms.borrow(blk.d);
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
    auto v = rqBlock("name", 32);
    Mems blk(v.d);

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


void MemFont::traverseCps(mf::CbCpGlyph cb) const
{
    /// @todo [urgent] traverseCps
}
