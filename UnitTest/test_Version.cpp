// What we are testing
#include "u_Version.h"

// Google test
#include "gtest/gtest.h"

using namespace std::string_view_literals;

TEST (Version, Ctor0)
{
    Version v;
    EXPECT_EQ(0, v.part1);
    EXPECT_EQ(0, v.part2);
    EXPECT_EQ(0, v.part3);
}


TEST (Version, Ctor1)
{
    Version v{ 5 };
    EXPECT_EQ(5, v.part1);
    EXPECT_EQ(0, v.part2);
    EXPECT_EQ(0, v.part3);
}


TEST (Version, Ctor2)
{
    Version v{ 2, 3 };
    EXPECT_EQ(2, v.part1);
    EXPECT_EQ(3, v.part2);
    EXPECT_EQ(0, v.part3);
}



TEST (Version, Ctor3)
{
    Version v{ 1, 3, 5 };
    EXPECT_EQ(1, v.part1);
    EXPECT_EQ(3, v.part2);
    EXPECT_EQ(5, v.part3);
}



TEST (Version, Cmp1)
{
    Version v1{ 1, 3, 5 };
    Version v2{ 1, 3, 5 };
    EXPECT_TRUE (v1 == v2);
}



TEST (Version, Cmp2)
{
    Version v1{ 1, 3, 4 };
    Version v2{ 1, 3, 5 };
    EXPECT_TRUE (v1 < v2);
    EXPECT_TRUE (v2 > v1);
}



TEST (Version, Cmp3)
{
    Version v1{ 1, 2, 100 };
    Version v2{ 1, 3, 5 };
    EXPECT_TRUE (v1 < v2);
    EXPECT_TRUE (v2 > v1);
}


TEST (Version, Cmp4)
{
    Version v1{ 1, 99, 100 };
    Version v2{ 2, 3, 5 };
    EXPECT_TRUE (v1 < v2);
    EXPECT_TRUE (v2 > v1);
}


TEST (Version, HasSmth1)
{
    Version v;
    EXPECT_FALSE(v);
}


TEST (Version, HasSmth2)
{
    Version v{ 1 };
    EXPECT_TRUE(v);
}


TEST (Version, HasSmth3)
{
    Version v{ 0, 2 };
    EXPECT_TRUE(v);
}


TEST (Version, HasSmth4)
{
    Version v{ 0, 0, 3 };
    EXPECT_TRUE(v);
}


TEST (Version, Sprintf1)
{
    char c[30];
    Version v;
    v.sprintf(c);
    EXPECT_STREQ("0.0", c);
}


TEST (Version, Sprintf2)
{
    char c[30];
    Version v { 12 };
    v.sprintf(c);
    EXPECT_STREQ("12.0", c);
}


TEST (Version, Sprintf3)
{
    char c[30];
    Version v { 12, 34 };
    v.sprintf(c);
    EXPECT_STREQ("12.34", c);
}


TEST (Version, Sprintf4)
{
    char c[30];
    Version v { 12, 34, 56 };
    v.sprintf(c);
    EXPECT_STREQ("12.34.56", c);
}


TEST (Version, Sprintf5)
{
    char c[7];
    Version v { 12, 34, 56 };
    auto n = v.sprintf(c);
    EXPECT_STREQ("12.34.", c);
    EXPECT_EQ(8, n);
}


TEST (Version, SprintfU8)
{
    char8_t c[30];
    Version v { 12, 34, 56 };
    v.sprintf(c);
    EXPECT_TRUE(u8"12.34.56"sv == c);
}


TEST (Version, ToSv1)
{
    char8_t c[30];
    Version v { 12, 34, 56 };
    auto sv = v.toSv(c);
    EXPECT_TRUE(u8"12.34.56"sv == sv);
}


TEST (Version, ToSv2)
{
    char c[30];
    Version v { 12, 34, 56 };
    auto sv = v.toSv(c);
    EXPECT_TRUE("12.34.56"sv == sv);
}


TEST (Version, ToSv3)
{
    char c[7];
    Version v { 12, 34, 56 };
    auto sv = v.toSv(c);
    EXPECT_TRUE("12.34."sv == sv);
}


TEST (Version, ToS)
{
    Version v { 12, 34, 56 };
    EXPECT_EQ("12.34.56", v.toS());
}


TEST (Version, ParseP1)
{
    auto v = Version::parsePermissive("");
    EXPECT_FALSE(v);
}


TEST (Version, ParseP2)
{
    auto v = Version::parsePermissive("23");
    EXPECT_FALSE(v);
}


TEST (Version, ParseP3)
{
    auto v = Version::parsePermissive("a.a.a23.0");
    Version expec { 23 };
    EXPECT_EQ(expec, v);
}


TEST (Version, ParseP4)
{
    auto v = Version::parsePermissive("23.");
    EXPECT_FALSE(v);
}


TEST (Version, Parse5)
{
    auto v = Version::parsePermissive("23.a34");
    EXPECT_FALSE(v);
}


TEST (Version, Parse5a)
{
    auto v = Version::parsePermissive("23.0");
    Version expec { 23 };
    EXPECT_EQ(expec, v);
}


TEST (Version, Parse6)
{
    auto v = Version::parsePermissive("23.34");
    Version expec { 23, 34 };
    EXPECT_EQ(expec, v);
}


TEST (Version, Parse7)
{
    auto v = Version::parsePermissive("23.34.");
    Version expec { 23, 34 };
    EXPECT_EQ(expec, v);
}


TEST (Version, Parse8)
{
    auto v = Version::parsePermissive("23.34.a56");
    Version expec { 23, 34 };
    EXPECT_EQ(expec, v);
}


TEST (Version, Parse9)
{
    auto v = Version::parsePermissive("23.34.35");
    Version expec { 23, 34, 35 };
    EXPECT_EQ(expec, v);
}


TEST (Version, Parse10)
{
    auto v = Version::parsePermissive("23.34.35a");
    Version expec { 23, 34, 35 };
    EXPECT_EQ(expec, v);
}


TEST (Version, Parse11)
{
    auto v = Version::parsePermissive("v.3.4alpha");
    Version expec { 3, 4 };
    EXPECT_EQ(expec, v);
}
