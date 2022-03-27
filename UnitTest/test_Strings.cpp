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
    EXPECT_TRUE (str::isIndex("B105"));
}


///
///  remainder
///
TEST (RemainderSv, Pref)
{
    EXPECT_EQ("", str::remainderSv("", ""));
    EXPECT_EQ("ing", str::remainderSv("string", "str"));
    EXPECT_EQ("", str::remainderSv("string", "string"));
    EXPECT_EQ("", str::remainderSv("string", "sta"));
    EXPECT_EQ("", str::remainderSv("string", "stringify"));
    EXPECT_EQ("", str::remainderSv("string", "kawaii"));
    EXPECT_EQ("", str::remainderSv("string", "q"));
    EXPECT_EQ("string", str::remainderSv("string", {}));
}


TEST (RemainderSv, PrefSuff)
{
    EXPECT_EQ("in", str::remainderSv("string", "str", "g"));
    EXPECT_EQ("", str::remainderSv("string", "str", "ing"));
    EXPECT_EQ("", str::remainderSv("string", "sta", "g"));
    EXPECT_EQ("", str::remainderSv("string", "s", "eng"));
    EXPECT_EQ("", str::remainderSv("string", "stri", "ring"));
    EXPECT_EQ("", str::remainderSv("string", "kawaii", ""));
    EXPECT_EQ("", str::remainderSv("string", "q", {}));
    EXPECT_EQ("", str::remainderSv("string", {}, "w"));
    EXPECT_EQ("ring", str::remainderSv("string", "st", {}));
    EXPECT_EQ("stri", str::remainderSv("string", {}, "ng"));
    EXPECT_EQ("string", str::remainderSv("string", {}, {}));
}
