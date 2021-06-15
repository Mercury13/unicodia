// My header
#include "Ttf.h"

namespace {
    namespace HEADER {
        constexpr auto OFS_NTABLES = 4;
        constexpr auto SIZE = 12;
    }
    namespace OS2 {
        constexpr auto OFS_PANOSE = 32;
        constexpr auto LEN_PANOSE = 26;
        constexpr auto MIN_LEN_PANOSE = OFS_PANOSE + LEN_PANOSE;
    }
}


Ttf::Ttf(QFile& aF) : f(aF)
{
    readDir();
}


Ttf::Block Ttf::readBlockEntry()
{
    Ttf::Block r;
    r.posInDir = f.pos();
    f.readStruc<4>(r.name);
    f.skipD();  // skip checksum;
    r.posInFile = f.readMD();
    r.length = f.readMD();
    return r;
}


void Ttf::readDir()
{
    // All data is Motorola here
    f.seek(HEADER::OFS_NTABLES);
    unsigned nTables = f.readMW();
    blocks.reserve(nTables);

    f.seek(HEADER::SIZE);
    for (unsigned iTable = 0; iTable < nTables; ++iTable) {
        Block table = readBlockEntry();
        blocks.push_back(table);
    }
}


const Ttf::Block* Ttf::findBlock(Char4 name) const
{
    for (auto& v : blocks)
        if (v.name == name)
            return &v;
    return nullptr;
}


const Ttf::Block* Ttf::rqBlock(Char4 name, uint32_t len) const
{
    auto r = findBlock(name);
    if (!r) {
        char buf[100];
        snprintf(buf, sizeof(buf), "Block <%.4s> not found", name.d.asChars);
        throw std::logic_error(buf);
    }
    if (r->length <= len) {
        char buf[100];
        snprintf(buf, sizeof(buf), "Block <%.4s> wanted %lu, found %lu",
                 name.d.asChars, long(len), long(r->length));
        throw std::logic_error(buf);
    }
    return r;
}

void Ttf::seek(const Block* block, uint32_t offset)
{
    if (!block)
        throw std::logic_error("[Ttf.seek] Block is null");
    f.seek(block->posInFile + offset);
}

PanoseData Ttf::readPanoseData()
{
    PanoseData r;
    auto block = rqBlock("OS/2", OS2::MIN_LEN_PANOSE);
    seek(block, OS2::OFS_PANOSE);
    f.readStruc<26>(r);
    return r;
}

void Ttf::writePanoseData(const PanoseData& x)
{
    auto block = rqBlock("OS/2", OS2::MIN_LEN_PANOSE);
    seek(block, OS2::OFS_PANOSE);
    f.writeStruc<26>(x);
}
