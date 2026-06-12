// What we are testing
#include "Xtypes.h"
#include <charconv>

// Google test
#include "gtest/gtest.h"

///
///  Prerequisites for to_chars:
///   strange documentation
///
TEST (ToChars, Prereq1)
{
    char buf[40];
    auto res = std::to_chars(std::begin(buf), std::end(buf),
        0.14159265,
        std::chars_format::general, 4);
    EXPECT_EQ(std::errc{}, res.ec);
    std::string_view sv(std::begin(buf), res.ptr);
    EXPECT_EQ("0.1416", sv);
}

///
///  Prerequisites for to_chars:
///    precision + non-meaning digits
///
TEST (ToChars, Prereq2)
{
    char buf[40];
    auto res = std::to_chars(std::begin(buf), std::end(buf),
        0.10000000001,
        std::chars_format::general, 4);
    EXPECT_EQ(std::errc{}, res.ec);
    std::string_view sv(std::begin(buf), res.ptr);
    EXPECT_EQ("0.1", sv);
}


///
///  Numeric conversions: basic workability
///
TEST (NumConv, Basic)
{
    std::string r;
    xs::Number::encodeAttr(12.345678, r, 15);
    EXPECT_EQ("12.345678", r);
}

///
///  Numeric conversions: number under 0 (0.5)
///
TEST (NumConv, Under0)
{
    std::string r;
    xs::Number::encodeAttr(0.001234, r, 15);
    EXPECT_EQ(".001234", r);
}


///
///  Numeric conversions: number under 0 (0.5)
///
TEST (NumConv, MinusUnder0)
{
    std::string r;
    xs::Number::encodeAttr(-0.001234, r, 15);
    EXPECT_EQ("-.001234", r);
}


///
///  Numeric conversions: very big number
///
TEST (NumConv, VeryBig)
{
    std::string r;
    xs::Number::encodeAttr(1'000'000'000.0, r, 4);
    EXPECT_EQ("1e9", r);
}


///
///  Numeric conversions: there's a way to get rid of point
///
TEST (NumConv, VeryBig2)
{
    std::string r;
    xs::Number::encodeAttr(1'230'000'000.0, r, 6);
    EXPECT_EQ("123e7", r);
}


///
///  15 will probably make an integer, so there's another way
///  to get rid of it
///
TEST (NumConv, VeryBig3)
{
    std::string r;
    xs::Number::encodeAttr(1'230'000'000.0, r, 15);
    EXPECT_EQ("123e7", r);
}


///
///  100 → no way
///
TEST (NumConv, Hundred)
{
    std::string r;
    xs::Number::encodeAttr(100, r, 15);
    EXPECT_EQ("100", r);
}


///
///  1000 → OK
///
TEST (NumConv, Thousand)
{
    std::string r;
    xs::Number::encodeAttr(1000, r, 15);
    EXPECT_EQ("1e3", r);
}


///
///  12000 → OK
///
TEST (NumConv, MoreThousand)
{
    std::string r;
    xs::Number::encodeAttr(12'000, r, 15);
    EXPECT_EQ("12e3", r);
}
