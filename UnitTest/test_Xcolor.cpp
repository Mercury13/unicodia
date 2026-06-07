// What we are testing
#include "Xcolor.h"

// Google test
#include "gtest/gtest.h"

///
///  Simple write
///
TEST (Xcolor, Write)
{
    xs::Color cl { .r = 0x12, .g = 0x78, .b = 0xAB };
    std::string r;
    cl.encodeAttr(r);
    EXPECT_EQ("#1278ab", r);
}

///
///  Short write
///
TEST (Xcolor, WriteShort)
{
    xs::Color cl { .r = 0xBB, .g = 0x99, .b = 0x44 };
    std::string r;
    cl.encodeAttr(r);
    EXPECT_EQ("#b94", r);
}

///
///  Parse: empty string
///
TEST (Xcolor, ParseEmpty)
{
    auto q = xs::Color::parse("");
    EXPECT_FALSE(q);
}

///
///  Parse: bad string
///
TEST (Xcolor, ParseBad)
{
    auto q = xs::Color::parse("abrakadabra");
    EXPECT_FALSE(q);
}

///
///  Parse: simple colour
///
TEST (Xcolor, ParseSimple)
{
    auto q = xs::Color::parse("#12abCD");
    EXPECT_TRUE(q);
    EXPECT_EQ(xs::Color(0x12, 0xAB, 0xCD), *q);
}

///
///  Parse: short colour
///
TEST (Xcolor, ParseShort)
{
    auto q = xs::Color::parse("#1ab");
    EXPECT_TRUE(q);
    EXPECT_EQ(xs::Color(0x11, 0xAA, 0xBB), *q);
}

///
///  Parse: 8 digits, forbidden
///
TEST (Xcolor, Parse8)
{
    auto q = xs::Color::parse("#12abCDff");
    EXPECT_FALSE(q);
}

///
///  Parse: 4 digits, forbidden
///
TEST (Xcolor, Parse4)
{
    auto q = xs::Color::parse("#12ab");
    EXPECT_FALSE(q);
}

///
///  Parse: 5 digits, really forbidden
///
TEST (Xcolor, Parse5)
{
    auto q = xs::Color::parse("#12345");
    EXPECT_FALSE(q);
}

///
///  Parse: name capital
///
TEST (Xcolor, ParseCap)
{
    auto q = xs::Color::parse("RED");
    EXPECT_TRUE(q);
    EXPECT_EQ(xs::Color(0xFF, 0, 0), *q);
}

///
///  Parse: name small
///
TEST (Xcolor, ParseSmall)
{
    auto q = xs::Color::parse("blue");
    EXPECT_TRUE(q);
    EXPECT_EQ(xs::Color(0, 0, 0xFF), *q);
}

///
///  Parse: name mixed
///
TEST (Xcolor, ParseFence)
{
    auto q = xs::Color::parse("GrEeN");
    EXPECT_TRUE(q);
    EXPECT_EQ(xs::Color(0, 0x80, 0), *q);
}
