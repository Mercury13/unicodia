#pragma once

#include <filesystem>
#include <vector>

namespace uc {
    struct Block;
}

namespace qa {

    enum class TestFonts : unsigned char {
        OK, CANNOT_CREATE, EMPTY_FNAME };

    TestFonts testFonts(const std::filesystem::path& fname);

    enum class BlockFontSpecial : unsigned char {
         NORMAL, NONFONT, TOFU
    };

    struct BlockFontLine {
        BlockFontSpecial special = BlockFontSpecial::NORMAL;
        std::string fname;
        std::string sortKey;
        unsigned count = 0;
    };

    struct BlockFontStats {
        std::vector<BlockFontLine> lines;
        unsigned nTotal = 0;
    };

    BlockFontStats blockFontStats(const uc::Block& block);

}   // namespace qa
