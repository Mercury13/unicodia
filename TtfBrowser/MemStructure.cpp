#include "MemStructure.h"


///// Block ////////////////////////////////////////////////////////////////////

const mfs::Obj& mfs::Block::childAt(size_t) const
{
    throw std::out_of_range("[Block.childAt] Block has no children");
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
}


void mfs::Font::loadFrom(const MemFont& font)
{
    slave = &font;
    fileSize = font.dataSize();
    blocks.clear();
    blocks.reserve(slave->nBlocks());
    for (auto& blk : slave->blocks()) {
        blocks.push_back(std::make_unique<Block>(blk));
    }
}
