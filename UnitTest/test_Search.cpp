// What we are testing
#include "u_SearchEngine.h"

// Google test
#include "gtest/gtest.h"

TEST (Classify, Letters)
{
    EXPECT_EQ (srh::Class::LETTER, srh::classify('A'));
    EXPECT_EQ (srh::Class::LETTER, srh::classify('Q'));
    EXPECT_EQ (srh::Class::LETTER, srh::classify('Z'));
    EXPECT_EQ (srh::Class::LETTER, srh::classify('a'));
    EXPECT_EQ (srh::Class::LETTER, srh::classify('q'));
    EXPECT_EQ (srh::Class::LETTER, srh::classify('z'));
}


TEST (Classify, Digits)
{
    EXPECT_EQ (srh::Class::DIGIT, srh::classify('0'));
    EXPECT_EQ (srh::Class::DIGIT, srh::classify('4'));
    EXPECT_EQ (srh::Class::DIGIT, srh::classify('9'));
}


TEST (Classify, Others)
{
    EXPECT_EQ (srh::Class::OTHER, srh::classify('-'));
    EXPECT_EQ (srh::Class::OTHER, srh::classify('/'));
    EXPECT_EQ (srh::Class::OTHER, srh::classify(':'));
    EXPECT_EQ (srh::Class::OTHER, srh::classify('@'));
    EXPECT_EQ (srh::Class::OTHER, srh::classify('['));
    EXPECT_EQ (srh::Class::OTHER, srh::classify('`'));
    EXPECT_EQ (srh::Class::OTHER, srh::classify('{'));
}
