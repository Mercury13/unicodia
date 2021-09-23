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
