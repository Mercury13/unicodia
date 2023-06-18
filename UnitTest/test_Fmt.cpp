// What we are testing
#include "LocFmt.h"

// Google test
#include "gtest/gtest.h"

TEST (FmtCtor, Simple)
{
    loc::Fmt fmt("alpha bravo");

    EXPECT_EQ("alpha bravo", fmt.str());
    EXPECT_EQ(0u, fmt.nAllSubsts());
    EXPECT_EQ(loc::Zsubst::NO_LINK, fmt.iFirstSubst());
}


TEST (FmtCtor, AllEscaped)
{
    loc::Fmt fmt("alpha {{ bravo } charlie");

    EXPECT_EQ("alpha { bravo } charlie", fmt.str());
    EXPECT_EQ(0u, fmt.nAllSubsts());
    EXPECT_EQ(loc::Zsubst::NO_LINK, fmt.iFirstSubst());
}


TEST (FmtCtor, SimpleSubsts)
{
    loc::Fmt fmt("alpha {4} bravobravo {2|test} charlie");
    EXPECT_EQ("alpha {4} bravobravo {2|test} charlie", fmt.str());
    EXPECT_EQ(2u, fmt.nAllSubsts());

    auto& s0 = fmt.allSubsts(0);
    EXPECT_EQ(3u, s0.key);
    EXPECT_EQ(3u, s0.length);
    EXPECT_EQ(6u, s0.advance);
    EXPECT_EQ(1u, s0.lnkNext);

    auto& s1 = fmt.allSubsts(1);
    EXPECT_EQ(1u, s1.key);
    EXPECT_EQ(8u, s1.length);
    EXPECT_EQ(12u, s1.advance);
    EXPECT_EQ(loc::Zsubst::NO_LINK, s1.lnkNext);
}
