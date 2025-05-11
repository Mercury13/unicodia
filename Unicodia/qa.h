#pragma once

#include <filesystem>

namespace qa {

    enum class TestFonts : unsigned char {
        OK, CANNOT_CREATE, EMPTY_FNAME };

    TestFonts testFonts(const std::filesystem::path& fname);

}   // namespace qa
