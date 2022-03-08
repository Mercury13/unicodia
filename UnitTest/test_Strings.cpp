// What we are testing
#include "u_Strings.h"

// Google test
#include "gtest/gtest.h"


///
///  Any string does not contain empty word
///
TEST (ContainsWord, Empty)
{
    EXPECT_FALSE(str::containsWord("a aa aaa", ""));
    EXPECT_FALSE(str::containsWord("", ""));
}


///
///  Equal strings contain
///
TEST (ContainsWord, Eq)
{
    EXPECT_TRUE(str::containsWord("alpha", "alpha"));
}


TEST (ContainsWord, GoodWords)
{
    EXPECT_TRUE(str::containsWord("alpha bravo charlie", "alpha"));
    EXPECT_TRUE(str::containsWord("alpha bravo charlie", "bravo"));
    EXPECT_TRUE(str::containsWord("alpha bravo charlie", "charlie"));
}


TEST (ContainsWord, BadWords)
{
    EXPECT_FALSE(str::containsWord("alpha bravo charlie", "al"));
    EXPECT_FALSE(str::containsWord("alpha bravo charlie", "ha"));
    EXPECT_FALSE(str::containsWord("alpha bravo charlie", "ch"));
    EXPECT_FALSE(str::containsWord("alpha bravo charlie", "ie"));
}


///
///  isIndex
///
TEST (isIndex, Simple)
{
    EXPECT_FALSE(str::isIndex(""));
    EXPECT_FALSE(str::isIndex("A"));
    EXPECT_FALSE(str::isIndex("1"));
    EXPECT_FALSE(str::isIndex("SDKLJFHSDKFJH"));
    EXPECT_FALSE(str::isIndex("46238476239"));
    EXPECT_TRUE (str::isIndex("A1"));
    EXPECT_TRUE (str::isIndex("Z9"));
    EXPECT_TRUE (str::isIndex("G1221498723948728945623846823746238764"));
    EXPECT_TRUE (str::isIndex("A1J"));
    EXPECT_TRUE (str::isIndex("G1221498723948728945623846823746238764J"));
    EXPECT_FALSE(str::isIndex("GH1212"));
    EXPECT_TRUE (str::isIndex("G1212"));
    EXPECT_TRUE (str::isIndex("G1212L"));
    EXPECT_FALSE(str::isIndex("G1212LL"));
    EXPECT_FALSE(str::isIndex("G1212L9"));
}
