// What we are testing
#include "u_Version.h"

// Google test
#include "gtest/gtest.h"

using namespace std::string_view_literals;

///
/// Ctor, 0 params
///
TEST (Version, Ctor0)
{
    Version v;
    EXPECT_EQ(0u, v.part1);
    EXPECT_EQ(0u, v.part2);
    EXPECT_EQ(0u, v.part3);
}


///
/// Ctor, 1 param
///
TEST (Version, Ctor1)
{
    Version v{ 5 };
    EXPECT_EQ(5u, v.part1);
    EXPECT_EQ(0u, v.part2);
    EXPECT_EQ(0u, v.part3);
}


///
/// Ctor, 2 params
///
TEST (Version, Ctor2)
{
    Version v{ 2, 3 };
    EXPECT_EQ(2u, v.part1);
    EXPECT_EQ(3u, v.part2);
    EXPECT_EQ(0u, v.part3);
}


///
/// Ctor, 3 params
///
TEST (Version, Ctor3)
{
    Version v{ 1, 3, 5 };
    EXPECT_EQ(1u, v.part1);
    EXPECT_EQ(3u, v.part2);
    EXPECT_EQ(5u, v.part3);
}


///
/// Compare, equal
///
TEST (Version, Cmp1)
{
    Version v1{ 1, 3, 5 };
    Version v2{ 1, 3, 5 };
    EXPECT_TRUE (v1 == v2);
}


///
/// Compare, the 3rd digit
///
TEST (Version, Cmp2)
{
    Version v1{ 1, 3, 4 };
    Version v2{ 1, 3, 5 };
    EXPECT_TRUE (v1 < v2);
    EXPECT_TRUE (v2 > v1);
}


///
/// Compare, the 2nd digit
///
TEST (Version, Cmp3)
{
    Version v1{ 1, 2, 100 };
    Version v2{ 1, 3, 5 };
    EXPECT_TRUE (v1 < v2);
    EXPECT_TRUE (v2 > v1);
}


///
/// Compare, the 1st digit
///
TEST (Version, Cmp4)
{
    Version v1{ 1, 99, 100 };
    Version v2{ 2, 3, 5 };
    EXPECT_TRUE (v1 < v2);
    EXPECT_TRUE (v2 > v1);
}


///
/// HasSmth: empty/false
///
TEST (Version, HasSmth1)
{
    Version v;
    EXPECT_FALSE(v);
}


///
/// HasSmth: the 1st digit
///
TEST (Version, HasSmth2)
{
    Version v{ 1 };
    EXPECT_TRUE(v);
}


///
/// HasSmth: the 2nd digit
///
TEST (Version, HasSmth3)
{
    Version v{ 0, 2 };
    EXPECT_TRUE(v);
}


///
/// HasSmth: the 3rd digit
///
TEST (Version, HasSmth4)
{
    Version v{ 0, 0, 3 };
    EXPECT_TRUE(v);
}


///
/// Sprintf: empty
///
TEST (Version, Sprintf1)
{
    char c[30];
    Version v;
    v.sprintf(c);
    EXPECT_STREQ("0.0", c);
}


///
/// Sprintf: the 1st digit
///
TEST (Version, Sprintf2)
{
    char c[30];
    Version v { 12 };
    v.sprintf(c);
    EXPECT_STREQ("12.0", c);
}


///
/// Sprintf: the 2nd digit
///
TEST (Version, Sprintf3)
{
    char c[30];
    Version v { 12, 34 };
    v.sprintf(c);
    EXPECT_STREQ("12.34", c);
}


///
/// Sprintf: the 3rd digit
///
TEST (Version, Sprintf4)
{
    char c[30];
    Version v { 12, 34, 56 };
    v.sprintf(c);
    EXPECT_STREQ("12.34.56", c);
}


///
/// Sprintf: too short buffer
///
TEST (Version, Sprintf5)
{
    char c[7];
    Version v { 12, 34, 56 };
    auto n = v.sprintf(c);
    EXPECT_STREQ("12.34.", c);
    EXPECT_EQ(8u, n);
}


///
/// Sprintf: UTF-8 version
///
TEST (Version, SprintfU8)
{
    char8_t c[30];
    Version v { 12, 34, 56 };
    v.sprintf(c);
    EXPECT_TRUE(u8"12.34.56"sv == c);
}


///
/// toSv: simple workability
///
TEST (Version, ToSv1)
{
    char8_t c[30];
    Version v { 12, 34, 56 };
    auto sv = v.toSv(c);
    EXPECT_TRUE(u8"12.34.56"sv == sv);
}


///
/// toSv: same but char
///
TEST (Version, ToSv2)
{
    char c[30];
    Version v { 12, 34, 56 };
    auto sv = v.toSv(c);
    EXPECT_TRUE("12.34.56"sv == sv);
}


///
/// toSv: short buffer
///
TEST (Version, ToSv3)
{
    char c[7];
    Version v { 12, 34, 56 };
    auto sv = v.toSv(c);
    EXPECT_TRUE("12.34."sv == sv);
}


///
/// toS: simple workability
///
TEST (Version, ToS)
{
    Version v { 12, 34, 56 };
    EXPECT_EQ("12.34.56", v.toS());
}


///
/// parsePermissive: empty string, false
///
TEST (Version, ParseP1)
{
    auto v = Version::parsePermissive("");
    EXPECT_FALSE(v);
}


///
/// parsePermissive: one number, false here
///
TEST (Version, ParseP2)
{
    auto v = Version::parsePermissive("23");
    EXPECT_FALSE(v);
}


///
/// parsePermissive: let it be this way
///
TEST (Version, ParseP3)
{
    auto v = Version::parsePermissive("a.a.a23.0");
    Version expec { 23 };
    EXPECT_EQ(expec, v);
}


///
/// parsePermissive: 23., false
///
TEST (Version, ParseP4)
{
    auto v = Version::parsePermissive("23.");
    EXPECT_FALSE(v);
}


///
/// parsePermissive: let it be this way
///
TEST (Version, Parse5)
{
    auto v = Version::parsePermissive("23.a34");
    EXPECT_FALSE(v);
}


///
/// parsePermissive: 23.0, OK
///
TEST (Version, Parse5a)
{
    auto v = Version::parsePermissive("23.0");
    Version expec { 23 };
    EXPECT_EQ(expec, v);
}


///
/// parsePermissive: 23.34, OK
///
TEST (Version, Parse6)
{
    auto v = Version::parsePermissive("23.34");
    Version expec { 23, 34 };
    EXPECT_EQ(expec, v);
}


///
/// parsePermissive: 23.34., OK
///
TEST (Version, Parse7)
{
    auto v = Version::parsePermissive("23.34.");
    Version expec { 23, 34 };
    EXPECT_EQ(expec, v);
}


///
/// parsePermissive: 23.34.a56, let it be this way
///
TEST (Version, Parse8)
{
    auto v = Version::parsePermissive("23.34.a56");
    Version expec { 23, 34 };
    EXPECT_EQ(expec, v);
}


///
/// parsePermissive: 23.34.35, OK
///
TEST (Version, Parse9)
{
    auto v = Version::parsePermissive("23.34.35");
    Version expec { 23, 34, 35 };
    EXPECT_EQ(expec, v);
}


///
/// parsePermissive: 23.34.35a, OK
///
TEST (Version, Parse10)
{
    auto v = Version::parsePermissive("23.34.35a");
    Version expec { 23, 34, 35 };
    EXPECT_EQ(expec, v);
}


///
/// parsePermissive: v.3.4alpha, OK
///
TEST (Version, Parse11)
{
    auto v = Version::parsePermissive("v.3.4alpha");
    Version expec { 3, 4 };
    EXPECT_EQ(expec, v);
}
