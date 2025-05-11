#pragma once

namespace qa {

    enum class TestFonts : unsigned char {
        OK, CANNOT_CREATE };

    TestFonts testFonts(const char* fname);

}   // namespace qa
