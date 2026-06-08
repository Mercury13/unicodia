// What we are testing
#include "Xstyle.h"
#include <charconv>

// Google test
#include "gtest/gtest.h"


///// Opacity //////////////////////////////////////////////////////////////////


///
///  SHOULD understand .25 etc
///
TEST (Xopacity, Prereq)
{
    std::string_view in = ".25";
    double val;
    auto r = std::from_chars(std::begin(in), std::end(in), val);
    EXPECT_EQ(std::errc{}, r.ec);
    EXPECT_EQ(0.25, val);
}

TEST (Xopacity, CtorUnit)
{
    xs::Opacity op(0.1237, xs::OpacityUnit::UNIT);
    EXPECT_EQ(124, op.raw());
}

TEST (Xopacity, CtorPercent)
{
    xs::Opacity op(12.37, xs::OpacityUnit::PERCENT);
    EXPECT_EQ(124, op.raw());
}

TEST (Xopacity, CtorPermille)
{
    xs::Opacity op(123.7, xs::OpacityUnit::PERMILLE);
    EXPECT_EQ(124, op.raw());
}

TEST (Xopacity, CtorNeg)
{
    xs::Opacity op(-1, xs::OpacityUnit::PERMILLE);
    EXPECT_EQ(0, op.raw());
}

TEST (Xopacity, CtorBig)
{
    xs::Opacity op(1001, xs::OpacityUnit::PERMILLE);
    EXPECT_EQ(1000, op.raw());
}
