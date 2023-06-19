// What we are testing
#include "LocFmt.h"

// Google test
#include "gtest/gtest.h"

///
///  Simple creation w/o substitution
///
TEST (FmtCtor, Simple)
{
    loc::Fmt<char> fmt("alpha bravo");

    EXPECT_EQ("alpha bravo", fmt.str());
    EXPECT_EQ(0u, fmt.nAllSubsts());
    EXPECT_EQ(loc::Zsubst::NO_LINK, fmt.iFirstSubst());
}


///
///  Deduction guide const char*
///
TEST (FmtCtor, DeductionGuideConstChar)
{
    loc::Fmt<char> fmt("alpha bravo");

    EXPECT_EQ("alpha bravo", fmt.str());
    EXPECT_EQ(0u, fmt.nAllSubsts());
    EXPECT_EQ(loc::Zsubst::NO_LINK, fmt.iFirstSubst());
}


///
///  Test FmtL’s deduction guide
///
TEST (FmtCtor, LDeductionGuideConstChar)
{
    loc::FmtL fmt("alpha bravo");

    EXPECT_EQ("alpha bravo", fmt.str());
    EXPECT_EQ(0u, fmt.nAllSubsts());
    EXPECT_EQ(loc::Zsubst::NO_LINK, fmt.iFirstSubst());
}


///
///  Deduction guide std::string
///
TEST (FmtCtor, DeductionGuideString)
{
    std::string s{"alpha bravo"};
    loc::Fmt fmt(s);

    EXPECT_EQ("alpha bravo", s);
    EXPECT_EQ("alpha bravo", fmt.str());
    EXPECT_EQ(0u, fmt.nAllSubsts());
    EXPECT_EQ(loc::Zsubst::NO_LINK, fmt.iFirstSubst());
}


///
///  Deduction guide std::string
///
TEST (FmtCtor, LDeductionGuideString)
{
    std::string s{"alpha bravo"};
    loc::FmtL fmt(s);

    EXPECT_EQ("alpha bravo", s);
    EXPECT_EQ("alpha bravo", fmt.str());
    EXPECT_EQ(0u, fmt.nAllSubsts());
    EXPECT_EQ(loc::Zsubst::NO_LINK, fmt.iFirstSubst());
}


///
///  Deduction guide std::string&&
///
TEST (FmtCtor, DeductionGuideTmpString)
{
    std::string s{"alpha bravo"};
    loc::Fmt fmt(std::move(s));

    EXPECT_TRUE(s.empty());
    EXPECT_EQ("alpha bravo", fmt.str());
    EXPECT_EQ(0u, fmt.nAllSubsts());
    EXPECT_EQ(loc::Zsubst::NO_LINK, fmt.iFirstSubst());
}


///
///  Deduction guide std::string&&
///
TEST (FmtCtor, LDeductionGuideTmpString)
{
    std::string s{"alpha bravo"};
    loc::FmtL fmt(std::move(s));

    EXPECT_TRUE(s.empty());
    EXPECT_EQ("alpha bravo", fmt.str());
    EXPECT_EQ(0u, fmt.nAllSubsts());
    EXPECT_EQ(loc::Zsubst::NO_LINK, fmt.iFirstSubst());
}


///
///  Deduction guide std::string&&
///
TEST (FmtCtor, DeductionGuideSv)
{
    std::string_view s{"alpha bravo"};
    loc::Fmt fmt(s);

    EXPECT_EQ("alpha bravo", fmt.str());
    EXPECT_EQ(0u, fmt.nAllSubsts());
    EXPECT_EQ(loc::Zsubst::NO_LINK, fmt.iFirstSubst());
}


///
///  Deduction guide std::string&&
///
TEST (FmtCtor, LDeductionGuideSv)
{
    std::string_view s{"alpha bravo"};
    loc::FmtL fmt(s);

    EXPECT_EQ("alpha bravo", fmt.str());
    EXPECT_EQ(0u, fmt.nAllSubsts());
    EXPECT_EQ(loc::Zsubst::NO_LINK, fmt.iFirstSubst());
}


///
///  Simple creation, everything’s escaped
///
TEST (FmtCtor, AllEscaped)
{
    loc::Fmt fmt("alpha {{ bravo }} charlie");

    EXPECT_EQ("alpha { bravo }} charlie", fmt.str());
    EXPECT_EQ(0u, fmt.nAllSubsts());
    EXPECT_EQ(loc::Zsubst::NO_LINK, fmt.iFirstSubst());
}


///
///  Simplest substitutions with numbers
///
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


///
///  Escaped both substitution and text
///
TEST (FmtCtor, EscapedSubsts)
{
    loc::Fmt fmt("alpha {4} bravobravo {3|te{st{}te{st} char{{}lie {8}");
    EXPECT_EQ("alpha {4} bravobravo {3|te{st{}te{st} char{}lie {8}", fmt.str());
    EXPECT_EQ(3u, fmt.nAllSubsts());

    auto& s0 = fmt.allSubsts(0);
    EXPECT_EQ(3u, s0.key);
    EXPECT_EQ(3u, s0.length);
    EXPECT_EQ(6u, s0.advance);
    EXPECT_EQ(1u, s0.lnkNext);

    auto& s1 = fmt.allSubsts(1);
    EXPECT_EQ(2u, s1.key);
    EXPECT_EQ(16u, s1.length);
    EXPECT_EQ(12u, s1.advance);
    EXPECT_EQ(2u, s1.lnkNext);

    auto& s2 = fmt.allSubsts(2);
    EXPECT_EQ(7u, s2.key);
    EXPECT_EQ(3u, s2.length);
    EXPECT_EQ(11u, s2.advance);
    EXPECT_EQ(loc::Zsubst::NO_LINK, s2.lnkNext);
}


///
///  Default enumeration of substitutions
///
TEST (FmtCtor, DefaultSubsts)
{
    loc::Fmt fmt("alpha {} bravobravo {te{st{}te{st} char{{}lie {}");
    EXPECT_EQ("alpha {} bravobravo {te{st{}te{st} char{}lie {}", fmt.str());
    EXPECT_EQ(3u, fmt.nAllSubsts());

    auto& s0 = fmt.allSubsts(0);
    EXPECT_EQ(0u, s0.key);
    EXPECT_EQ(2u, s0.length);
    EXPECT_EQ(6u, s0.advance);
    EXPECT_EQ(1u, s0.lnkNext);

    auto& s1 = fmt.allSubsts(1);
    EXPECT_EQ(1u, s1.key);
    EXPECT_EQ(14u, s1.length);
    EXPECT_EQ(12u, s1.advance);
    EXPECT_EQ(2u, s1.lnkNext);

    auto& s2 = fmt.allSubsts(2);
    EXPECT_EQ(2u, s2.key);
    EXPECT_EQ(2u, s2.length);
    EXPECT_EQ(11u, s2.advance);
    EXPECT_EQ(loc::Zsubst::NO_LINK, s2.lnkNext);
}


///
///  Combination of explicit and default substitutions
///
TEST (FmtCtor, ExplicitDefaultCombo)
{
    loc::Fmt fmt("alpha {5} bravobravo {3|te{st{}te{st} char{{}lie {}");
    EXPECT_EQ("alpha {5} bravobravo {3|te{st{}te{st} char{}lie {}", fmt.str());
    EXPECT_EQ(3u, fmt.nAllSubsts());

    auto& s0 = fmt.allSubsts(0);
    EXPECT_EQ(4u, s0.key);
    EXPECT_EQ(3u, s0.length);
    EXPECT_EQ(6u, s0.advance);
    EXPECT_EQ(1u, s0.lnkNext);

    auto& s1 = fmt.allSubsts(1);
    EXPECT_EQ(2u, s1.key);
    EXPECT_EQ(16u, s1.length);
    EXPECT_EQ(12u, s1.advance);
    EXPECT_EQ(2u, s1.lnkNext);

    auto& s2 = fmt.allSubsts(2);
    EXPECT_EQ(3u, s2.key);
    EXPECT_EQ(2u, s2.length);
    EXPECT_EQ(11u, s2.advance);
    EXPECT_EQ(loc::Zsubst::NO_LINK, s2.lnkNext);
}


///
///  Combination of explicit and default substitutions
///
TEST (FmtCtor, NumParse)
{
    loc::Fmt fmt("alpha {1234} bravo {12345678901} charlie");
    EXPECT_EQ("alpha {1234} bravo {12345678901} charlie", fmt.str());
    EXPECT_EQ(2u, fmt.nAllSubsts());

    auto& s0 = fmt.allSubsts(0);
    EXPECT_EQ(1233u, s0.key);
    EXPECT_EQ(6u, s0.length);
    EXPECT_EQ(6u, s0.advance);
    EXPECT_EQ(1u, s0.lnkNext);

    auto& s1 = fmt.allSubsts(1);
    EXPECT_EQ(1234u, s1.key);
    EXPECT_EQ(13u, s1.length);
    EXPECT_EQ(7u, s1.advance);
    EXPECT_EQ(loc::Zsubst::NO_LINK, s1.lnkNext);
}
