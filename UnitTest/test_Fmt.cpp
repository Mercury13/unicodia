// What we are testing
#include "LocFmt.h"

// Google test
#include "gtest/gtest.h"


///// Prerequisites ////////////////////////////////////////////////////////////

///
///  Vector’s prerequisites for Format: it enlarges only
///
TEST (FmtPrereq, VectorEnlargesOnly)
{
    std::vector<int> v;
    v.reserve(500);
    for (int i = 0; i < 500; ++i)
        v.push_back(i);
    auto oldCap = v.capacity();
    v.clear();
    EXPECT_EQ(oldCap, v.capacity());
}


///// Constructor //////////////////////////////////////////////////////////////

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

    EXPECT_TRUE(s.empty());     // OK, check that it was moved
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

    EXPECT_TRUE(s.empty());     // OK, check that it was moved
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


///// Simple numbers ///////////////////////////////////////////////////////////


TEST (FmtSimpleNum, BasicShorter)
{
    loc::Fmt fmt("alpha {1} bravo");
    fmt(7);
    EXPECT_EQ("alpha 7 bravo", fmt.str());
    EXPECT_EQ(loc::Zsubst::NO_LINK, fmt.iFirstSubst());
}


TEST (FmtSimpleNum, BasicEqual)
{
    loc::Fmt fmt("alpha {1} bravo");
    fmt(345);
    EXPECT_EQ("alpha 345 bravo", fmt.str());
    EXPECT_EQ(loc::Zsubst::NO_LINK, fmt.iFirstSubst());
}


TEST (FmtSimpleNum, BasicLonger)
{
    loc::Fmt fmt("alpha {1} bravo");
    fmt(-24680);
    EXPECT_EQ("alpha -24680 bravo", fmt.str());
    EXPECT_EQ(loc::Zsubst::NO_LINK, fmt.iFirstSubst());
}


TEST (FmtSimpleNum, Identical)
{
    loc::Fmt fmt("alpha {1} bravo {1}{1} charlie");
    fmt(-6);
    EXPECT_EQ("alpha -6 bravo -6-6 charlie", fmt.str());
    EXPECT_EQ(loc::Zsubst::NO_LINK, fmt.iFirstSubst());
}


TEST (FmtSimpleNum, TwoDifferent)
{
    loc::Fmt fmt("{2}alpha {1} bravo {2}{2} charlie{1}");
    fmt(-7)(438);
    EXPECT_EQ("438alpha -7 bravo 438438 charlie-7", fmt.str());
    EXPECT_EQ(loc::Zsubst::NO_LINK, fmt.iFirstSubst());
}


TEST (FmtSimpleNum, MultiEat)
{
    loc::Fmt fmt("{2}alpha {1} bravo {2}{2} charlie{1}");
    fmt(-7, 438);
    EXPECT_EQ("438alpha -7 bravo 438438 charlie-7", fmt.str());
    EXPECT_EQ(loc::Zsubst::NO_LINK, fmt.iFirstSubst());
}


///// Simple strings ///////////////////////////////////////////////////////////


TEST (FmtSimpleStr, BasicShorter)
{
    loc::Fmt fmt("alpha {1|qqq} bravo");
    fmt("ab");
    EXPECT_EQ("alpha ab bravo", fmt.str());
    EXPECT_EQ(loc::Zsubst::NO_LINK, fmt.iFirstSubst());
}


TEST (FmtSimpleStr, BasicEqual)
{
    loc::Fmt fmt("alpha {1} bravo");
    std::string_view sv("cde");
    fmt(sv);
    EXPECT_EQ("alpha cde bravo", fmt.str());
    EXPECT_EQ(loc::Zsubst::NO_LINK, fmt.iFirstSubst());
}


TEST (FmtSimpleStr, BasicLonger)
{
    loc::Fmt fmt("alpha {1} bravo");
    std::string s("qwertyuiop");
    fmt(s);
    EXPECT_EQ("alpha qwertyuiop bravo", fmt.str());
    EXPECT_EQ(loc::Zsubst::NO_LINK, fmt.iFirstSubst());
}


TEST (FmtSimpleStr, Identical)
{
    loc::Fmt fmt("alpha {1} bravo {1}{1} charlie");
    fmt("ghgh");
    EXPECT_EQ("alpha ghgh bravo ghghghgh charlie", fmt.str());
    EXPECT_EQ(loc::Zsubst::NO_LINK, fmt.iFirstSubst());
}


TEST (FmtSimpleStr, TwoDifferent)
{
    loc::Fmt fmt("{2}alpha {1} bravo {2}{2} charlie{1}");
    fmt("ab")("cdef");
    EXPECT_EQ("cdefalpha ab bravo cdefcdef charlieab", fmt.str());
    EXPECT_EQ(loc::Zsubst::NO_LINK, fmt.iFirstSubst());
}


TEST (FmtSimpleStr, NumAndStr)
{
    loc::Fmt fmt("{2}alpha {1} bravo {2}{2} charlie{1}");
    fmt(-123456789012LL)("cdef");
    EXPECT_EQ("cdefalpha -123456789012 bravo cdefcdef charlie-123456789012", fmt.str());
    EXPECT_EQ(loc::Zsubst::NO_LINK, fmt.iFirstSubst());
}


TEST (FmtSimpleStr, TwoDifferentSingleBracket)
{
    loc::Fmt fmt("{2}alpha {1} bravo {2}{2} charlie{1}");
    fmt("ab", "cdef");
    EXPECT_EQ("cdefalpha ab bravo cdefcdef charlieab", fmt.str());
    EXPECT_EQ(loc::Zsubst::NO_LINK, fmt.iFirstSubst());
}


TEST (FmtSimpleStr, NumAndStrSingleBracket)
{
    loc::Fmt fmt("{2}alpha {1} bravo {2}{2} charlie{1}");
    fmt(-123456789012LL, "cdef");
    EXPECT_EQ("cdefalpha -123456789012 bravo cdefcdef charlie-123456789012", fmt.str());
    EXPECT_EQ(loc::Zsubst::NO_LINK, fmt.iFirstSubst());
}


///// Simple empty /////////////////////////////////////////////////////////////


TEST (FmtSimpleEmpty, NumAndStr)
{
    loc::Fmt fmt("{2}alpha {1} bravo {2}{2} charlie{1}");
    fmt(-123456789012LL).emptyStr();
    EXPECT_EQ("alpha -123456789012 bravo  charlie-123456789012", fmt.str());
    EXPECT_EQ(loc::Zsubst::NO_LINK, fmt.iFirstSubst());
}


TEST (FmtSimpleEmpty, EmptyBracketsDoNothing)
{
    loc::Fmt fmt("{2}alpha {1} bravo {2}{2} charlie{1}");
    fmt()("A", "B");
    EXPECT_EQ("Balpha A bravo BB charlieA", fmt.str());
    EXPECT_EQ(loc::Zsubst::NO_LINK, fmt.iFirstSubst());
}


///// Advanced numbers /////////////////////////////////////////////////////////


///
///  Simple work: should use ONE
///
TEST (FmtAdvNum, Simple)
{
    loc::Fmt fmt("{1|one=? lap|many=? laps} completed{2}");
    fmt(1)("!");
    EXPECT_EQ("1 lap completed!", fmt.str());
}


///
///  Simple work: should use MANY
///
TEST (FmtAdvNum, Simple2)
{
    loc::Fmt fmt("{1|one=? lap|many=? laps} completed{2}");
    fmt(6, "!");
    EXPECT_EQ("6 laps completed!", fmt.str());
}


///
///  Repeating question mark → should work
///
TEST (FmtAdvNum, RepeatingQun)
{
    loc::Fmt fmt("{1|one=?? lap?|many=? laps} completed{2}");
    fmt(1)("!");
    EXPECT_EQ("11 lap1 completed!", fmt.str());
}


///
///  Plural form missing → do dumb replace
///
TEST (FmtAdvNum, Missing)
{
    loc::Fmt fmt("{1|one=? lap|few=? laps} completed{2}");
    fmt(6, "!");
    EXPECT_EQ("6 completed!", fmt.str());
}


TEST (FmtAdvNum, NoQun)
{
    loc::Fmt fmt("{1|one=One lap|many=? laps} completed{2}");
    fmt(1)("!");
    EXPECT_EQ("One lap completed!", fmt.str());
}


///
///  Escape some characters with {
///
TEST (FmtAdvNum, Escape)
{
    loc::Fmt fmt("{1|one={?? l{ap{{{|{}!?!|many=? laps} completed{2}");
    fmt(1)("!");
    EXPECT_EQ("?1 lap{|}!1! completed!", fmt.str());
}


/////  Give string /////////////////////////////////////////////////////////////


///
///  giveStr should stop substituting, all other calls do nothing
///
TEST (FmtGive, ShouldStop1)
{
    loc::Fmt fmt("{1} - {2}");
    fmt(12);
    fmt.giveStr();
    fmt(34);
    EXPECT_EQ("12 - {2}", fmt.str());
}


///
///  The same
///
TEST (FmtGive, ShouldStop2)
{
    loc::Fmt fmt("{2} - {1}");
    fmt(12);
    fmt.giveStr();
    fmt(34);
    EXPECT_EQ("{2} - 12", fmt.str());
}


///
///  giveStr should give string, fmt owns nothing
///
TEST (FmtGive, ShouldGive)
{
    loc::Fmt fmt("{2} - {1}");
    fmt(12);
    auto s = fmt.giveStr();
    EXPECT_EQ("{2} - 12", s);
    EXPECT_EQ("", fmt.str());
}
