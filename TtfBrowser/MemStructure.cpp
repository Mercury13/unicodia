#include "MemStructure.h"


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
