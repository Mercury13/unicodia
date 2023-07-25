// What we are testing
#include "forget.h"

// Google test
#include "gtest/gtest.h"


TEST (Forget, Bug1)
{
    EXPECT_FALSE(forget::isIn(0x1E048, "modifier letter Cyrillic small E"));
}
