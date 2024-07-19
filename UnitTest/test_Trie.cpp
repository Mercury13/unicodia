// What we are testing
#include "Search/trie.h"

// Google test
#include "gtest/gtest.h"

#include "UcCp.h"


enum class Emoji {
    CUBA,
    PUERTO_RICO,
    SPAIN,
};


class Trie1 : public srh::TrieRoot<Emoji>
{
public:
    Trie1();
};

Trie1::Trie1()
{
    addMulti(Emoji::CUBA,        cp::FLAG_C, cp::FLAG_U);
    addMulti(Emoji::PUERTO_RICO, cp::FLAG_P, cp::FLAG_R);
    addMulti(Emoji::SPAIN,       cp::FLAG_E, cp::FLAG_S);
};

TEST (DecodeTrie, ThreeFlags)
{
    Trie1 tr;
    const char32_t data[] { cp::FLAG_P, cp::FLAG_R, cp::FLAG_C, cp::FLAG_U, cp::FLAG_E, cp::FLAG_S, 0 };
    auto res = tr.decode(data);

    EXPECT_EQ(3u, res.size());

    auto& r0 = res[0];
    EXPECT_EQ(0u, r0.index);
    EXPECT_EQ(Emoji::PUERTO_RICO, r0.result);

    auto& r1 = res[1];
    EXPECT_EQ(2u, r1.index);
    EXPECT_EQ(Emoji::CUBA, r1.result);

    auto& r2 = res[2];
    EXPECT_EQ(4u, r2.index);
    EXPECT_EQ(Emoji::SPAIN, r2.result);
}
