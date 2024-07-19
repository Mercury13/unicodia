// What we are testing
#include "forget.h"

// Google test
#include "gtest/gtest.h"


///
///  Now Cyrillic is also forget-tested
///
TEST (Forget, Cyrillic)
{
    EXPECT_TRUE(forget::isIn(0x1E048, "modifier letter Cyrillic small E", ""));
}


///
///  IDK what we tested before, but that concept (avoid Cyrillic) fell
///
TEST (Forget, Bug1)
{
    EXPECT_FALSE(forget::isIn(0x1E048, "Klingon letter small E", ""));
}
