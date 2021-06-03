// What we are testing
#include "u_Iterator.h"

// Google test
#include "gtest/gtest.h"

///// hintedUpperBound — op< ///////////////////////////////////////////////////

constexpr int CANARY = 10001;

class SafeInt
{
public:
    constexpr SafeInt(int aV) : v(aV) {}
    constexpr int value() const { return v; }
private:
    int v;
};


constexpr bool operator < (SafeInt x, SafeInt y)
{
    if (x.value() == CANARY || y.value() == CANARY)
        throw std::logic_error("[op< (SI, SI)] Canary is dead!");
    return x.value() < y.value();
}


bool operator < (SafeInt, int) = delete;
bool operator < (int, SafeInt) = delete;


namespace twoCanaries {
    SafeInt a[] { CANARY, CANARY };
    constexpr auto beg = std::begin(a) + 1;
    constexpr auto end = beg;

    constexpr auto unsafeBeg = std::begin(a);
    constexpr auto unsafeEnd = std::end(a);
}

TEST(HubLess, TestCanary)
{
    using namespace twoCanaries;
    using namespace detail;
    EXPECT_THROW(upperBoundHint(unsafeBeg, unsafeEnd, SafeInt{42}, beg), std::logic_error);
}


TEST(HubLess, EmptyArray)
{
    using namespace twoCanaries;
    using namespace detail;
    EXPECT_EQ(Place::FOUND, upperBoundHint(beg, end, SafeInt{42}, beg));
}


namespace oneItem {
    SafeInt a[] { CANARY, 10, CANARY };
    constexpr auto beg = std::begin(a) + 1;
    constexpr auto end = std::end(a) - 1;
}

TEST(HubLess, OneLessRight)
{
    using namespace oneItem;
    using namespace detail;
    EXPECT_EQ(Place::FOUND, upperBoundHint(beg, end, SafeInt{8}, beg));
}


TEST(HubLess, OneLessWrong)
{
    using namespace oneItem;
    using namespace detail;
    EXPECT_EQ(Place::BEFORE, detail::upperBoundHint(beg, end, SafeInt{8}, end));
}


TEST(HubLess, OneEqRight)
{
    using namespace oneItem;
    using namespace detail;
    EXPECT_EQ(Place::FOUND, upperBoundHint(beg, end, SafeInt{10}, end));
}


TEST(HubLess, OneEqWrong)
{
    using namespace oneItem;
    using namespace detail;
    EXPECT_EQ(Place::AFTER, upperBoundHint(beg, end, SafeInt{10}, beg));
}


TEST(HubLess, OneGreaterRight)
{
    using namespace oneItem;
    using namespace detail;
    EXPECT_EQ(Place::FOUND, upperBoundHint(beg, end, SafeInt{12}, end));
}


TEST(HubLess, OneGreaterWrong)
{
    using namespace oneItem;
    using namespace detail;
    EXPECT_EQ(Place::AFTER, upperBoundHint(beg, end, SafeInt{12}, beg));
}


namespace someItems {
    SafeInt a[] { CANARY, 10, 20, 30, CANARY };
    constexpr auto beg = std::begin(a) + 1;
    constexpr auto end = std::end(a) - 1;
    constexpr auto twenty = beg + 1;
    constexpr auto thirty = beg + 2;
}

TEST(HubLess, Many20Beg)
{
    using namespace someItems;
    using namespace detail;
    EXPECT_EQ(Place::AFTER, upperBoundHint(beg, end, SafeInt{20}, beg));
}

TEST(HubLess, Many20Less)
{
    using namespace someItems;
    using namespace detail;
    EXPECT_EQ(Place::AFTER, upperBoundHint(beg, end, SafeInt{20}, twenty));
}

TEST(HubLess, Many20Ok)
{
    using namespace someItems;
    using namespace detail;

    // Check for actual upper_bound
    auto v = std::upper_bound(beg, end, SafeInt{20});
    EXPECT_EQ(30, v->value());

    EXPECT_EQ(Place::FOUND, upperBoundHint(beg, end, SafeInt{20}, thirty));
}

TEST(HubLess, Many20End)
{
    using namespace someItems;
    using namespace detail;
    EXPECT_EQ(Place::BEFORE, upperBoundHint(beg, end, SafeInt{20}, end));
}


TEST(HubLess, Many15Beg)
{
    using namespace someItems;
    using namespace detail;
    EXPECT_EQ(Place::AFTER, upperBoundHint(beg, end, SafeInt{15}, beg));
}

TEST(HubLess, Many15Ok)
{
    using namespace someItems;
    using namespace detail;
    EXPECT_EQ(Place::FOUND, upperBoundHint(beg, end, SafeInt{15}, twenty));
}

TEST(HubLess, Many15More)
{
    using namespace someItems;
    using namespace detail;
    EXPECT_EQ(Place::BEFORE, upperBoundHint(beg, end, SafeInt{15}, thirty));
}

TEST(HubLess, Many15End)
{
    using namespace someItems;
    using namespace detail;
    EXPECT_EQ(Place::BEFORE, detail::upperBoundHint(beg, end, SafeInt{15}, end));
}

TEST(HubLess, Many10Less)
{
    using namespace someItems;
    using namespace detail;
    EXPECT_EQ(Place::FOUND, upperBoundHint(beg, end, SafeInt{10}, twenty));
}

TEST(HubLess, Many10Ok)
{
    using namespace someItems;
    using namespace detail;
    EXPECT_EQ(Place::BEFORE, upperBoundHint(beg, end, SafeInt{10}, thirty));
}


namespace manyItems {
    SafeInt a[] { CANARY, 10, 20, 30, 40, 50, 60, CANARY };
    constexpr auto beg = std::begin(a) + 1;
    constexpr auto end = std::end(a) - 1;
    constexpr auto forty = beg + 3;
    constexpr auto ten = beg;
    constexpr auto sixty = end - 1;
}

///
///  Check for correctness of directions
///
TEST(HubLess, HintOk)
{
    using namespace manyItems;
    auto v = hintedUpperBound(beg, end, SafeInt{30}, forty);
    EXPECT_EQ(40, v->value());
}

TEST(HubLess, HintAfter)
{
    using namespace manyItems;
    auto v = hintedUpperBound(beg, end, SafeInt{40}, ten);
    EXPECT_EQ(50, v->value());
}


TEST(HubLess, HintBefore)
{
    using namespace manyItems;
    auto v = hintedUpperBound(beg, end, SafeInt{20}, sixty);
    EXPECT_EQ(30, v->value());
}

TEST(HubLess, HintMidOk)
{
    using namespace manyItems;
    auto v = hintedUpperBound(beg, end, SafeInt{35}, forty);
    EXPECT_EQ(40, v->value());
}

TEST(HubLess, HintMidAfter)
{
    using namespace manyItems;
    auto v = hintedUpperBound(beg, end, SafeInt{45}, ten);
    EXPECT_EQ(50, v->value());
}


TEST(HubLess, HintMidBefore)
{
    using namespace manyItems;
    auto v = hintedUpperBound(beg, end, SafeInt{25}, sixty);
    EXPECT_EQ(30, v->value());
}
