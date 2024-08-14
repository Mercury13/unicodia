#include "MemStructure.h"


///// Block ////////////////////////////////////////////////////////////////////

const mfs::Obj& mfs::Block::childAt(size_t) const
{
    throw std::out_of_range("[Block.childAt] Block has no children");
}


///// Font /////////////////////////////////////////////////////////////////////


size_t mfs::Font::nChildren() const noexcept
{
    if (!slave)
        return 0;
    return slave->nBlocks();
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
}
