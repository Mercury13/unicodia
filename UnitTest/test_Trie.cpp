// What we are testing
#include "Search/trie.h"

// Google test
#include "gtest/gtest.h"

#include "UcCp.h"


enum class Emoji : unsigned char {
    NONE,
    ARAB_EMIRATES,
    AFGHANISTAN,
    CUBA,
    PUERTO_RICO,
    SPAIN,
    WOMAN_WHITE,
    MAN_BLACK,
    HEART_RED,
    KISS_INTERRACIAL,
    HEAD_YES,
    UNKNOWN_FLAG,
};


class Trie1 : public srh::TrieRoot<Emoji>
{
public:
    Trie1();
};

Trie1::Trie1()
{
    addMulti(Emoji::AFGHANISTAN, cp::FLAG_A, cp::FLAG_F);
    addMulti(Emoji::ARAB_EMIRATES, cp::FLAG_A, cp::FLAG_E);
    addMulti(Emoji::CUBA,        cp::FLAG_C, cp::FLAG_U);
    addMulti(Emoji::PUERTO_RICO, cp::FLAG_P, cp::FLAG_R);
    addMulti(Emoji::SPAIN,       cp::FLAG_E, cp::FLAG_S);
    addMulti(Emoji::WOMAN_WHITE, cp::WOMAN,  cp::SKIN1);
    addMulti(Emoji::MAN_BLACK,   cp::MAN,    cp::SKIN5);
    addMulti(Emoji::HEART_RED,   cp::EMOJI_RED_HEART, cp::VS16);
    addMulti(Emoji::KISS_INTERRACIAL,
                cp::WOMAN, cp::SKIN1, cp::ZWJ,
                cp::EMOJI_RED_HEART, cp::VS16, cp::ZWJ,
                cp::KISS_MARK, cp::ZWJ,
                cp::MAN, cp::SKIN5);
    addMulti(Emoji::HEAD_YES,
                cp::SMILE_SLIGHTLY_SMILING, cp::ZWJ,
                cp::ARROW_UP_DOWN, cp::VS16);
    addUnknownFlags(Emoji::UNKNOWN_FLAG);
};

void expectEmoji(const srh::Decoded<Emoji>& line,
                 size_t index,
                 unsigned length,
                 srh::EmojiLevel level,
                 Emoji thing)
{
    EXPECT_EQ(index, line.index);
    EXPECT_EQ(length, line.length);
    EXPECT_EQ(thing, line.result);
    EXPECT_EQ(level, line.level);
}

///
///  This was a trouble for #371
///
TEST (DecodeTrie, ThreeFlags)
{
    Trie1 tr;
    const char32_t data[] { cp::FLAG_P, cp::FLAG_R, cp::FLAG_C, cp::FLAG_U, cp::FLAG_E, cp::FLAG_S, 0 };
    auto res = tr.decode(data);

    EXPECT_EQ(3u, res.size());

    expectEmoji(res.at(0), 0, 2, srh::EmojiLevel::FULL, Emoji::PUERTO_RICO);
    expectEmoji(res.at(1), 2, 2, srh::EmojiLevel::FULL, Emoji::CUBA);
    expectEmoji(res.at(2), 4, 2, srh::EmojiLevel::FULL, Emoji::SPAIN);
}


///
///  Prerequisite: decode interracial kiss
///
TEST (DecodeTrie, InterracialKiss)
{
    Trie1 tr;
    const char32_t data[] { 'A',
                cp::WOMAN, cp::SKIN1, cp::ZWJ,
                cp::EMOJI_RED_HEART, cp::VS16, cp::ZWJ,
                cp::KISS_MARK, cp::ZWJ,
                cp::MAN, cp::SKIN5, 0 };
    auto res = tr.decode(data);

    EXPECT_EQ(1u, res.size());

    expectEmoji(res.at(0), 1, 10, srh::EmojiLevel::FULL, Emoji::KISS_INTERRACIAL);
}


///
///  Prerequisite: decode interracial kiss, then man
///
TEST (DecodeTrie, InterracialKissMan)
{
    Trie1 tr;
    const char32_t data[] { 'A',
                cp::WOMAN, cp::SKIN1, cp::ZWJ,
                cp::EMOJI_RED_HEART, cp::VS16, cp::ZWJ,
                cp::KISS_MARK, cp::ZWJ,
                cp::MAN, cp::SKIN5,
                'B',
                cp::MAN, cp::SKIN5, 0 };
    auto res = tr.decode(data);

    EXPECT_EQ(2u, res.size());

    expectEmoji(res.at(0), 1,  10, srh::EmojiLevel::FULL, Emoji::KISS_INTERRACIAL);
    expectEmoji(res.at(1), 12, 2,  srh::EmojiLevel::FULL, Emoji::MAN_BLACK);
}


///
///  Incomplete interracial kiss with some bad char
///
TEST (DecodeTrie, KissBadChar)
{
    Trie1 tr;
    const char32_t data[] {
                cp::WOMAN, cp::SKIN1, cp::ZWJ,
                cp::EMOJI_RED_HEART, cp::VS16, cp::ZWJ,
                cp::KISS_MARK, cp::ZWJ,
                cp::MAN, 'A', 0 };
    auto res = tr.decode(data);

    EXPECT_EQ(2u, res.size());

    expectEmoji(res.at(0), 0, 2, srh::EmojiLevel::FULL, Emoji::WOMAN_WHITE);
    expectEmoji(res.at(1), 3, 2, srh::EmojiLevel::FULL, Emoji::HEART_RED);
}


///
///  Incomplete interracial kiss that ends abruptly
///
TEST (DecodeTrie, KissAbrupt)
{
    Trie1 tr;
    const char32_t data[] {
                cp::WOMAN, cp::SKIN1, cp::ZWJ,
                cp::EMOJI_RED_HEART, cp::VS16, cp::ZWJ,
                cp::KISS_MARK, cp::ZWJ,
                cp::MAN, 0 };
    auto res = tr.decode(data);

    EXPECT_EQ(2u, res.size());

    expectEmoji(res.at(0), 0, 2, srh::EmojiLevel::FULL, Emoji::WOMAN_WHITE);
    expectEmoji(res.at(1), 3, 2, srh::EmojiLevel::FULL, Emoji::HEART_RED);
}


///
///  Incomplete interracial kiss followed with more emoji
///
TEST (DecodeTrie, KissMoreEmoji)
{
    Trie1 tr;
    const char32_t data[] {
                cp::WOMAN, cp::SKIN1, cp::ZWJ,
                cp::EMOJI_RED_HEART, cp::VS16, cp::ZWJ,
                cp::KISS_MARK, cp::ZWJ,
                cp::MAN,
                cp::FLAG_E, cp::FLAG_S, 0 };
    auto res = tr.decode(data);

    EXPECT_EQ(3u, res.size());

    expectEmoji(res.at(0), 0, 2, srh::EmojiLevel::FULL, Emoji::WOMAN_WHITE);
    expectEmoji(res.at(1), 3, 2, srh::EmojiLevel::FULL, Emoji::HEART_RED);
    expectEmoji(res.at(2), 9, 2, srh::EmojiLevel::FULL, Emoji::SPAIN);
}


///
///  Current bhv for strange flags
///
TEST (DecodeTrie, StrangeFlags)
{
    Trie1 tr;
    const char32_t data[] { cp::FLAG_A, cp::FLAG_A, cp::FLAG_E, cp::FLAG_S,  0 };
    auto res = tr.decode(data);

    EXPECT_EQ(2u, res.size());

    expectEmoji(res.at(0), 0, 2, srh::EmojiLevel::UNKNOWN_FLAG, Emoji::UNKNOWN_FLAG);
    expectEmoji(res.at(1), 2, 2, srh::EmojiLevel::FULL, Emoji::SPAIN);
}


///
///  Arab Emirates in a normal position → OK
///
TEST (DecodeTrie, StrangeFlags2)
{
    Trie1 tr;
    const char32_t data[] { cp::FLAG_B, cp::FLAG_A, cp::FLAG_A, cp::FLAG_E,  0 };
    auto res = tr.decode(data);

    EXPECT_EQ(2u, res.size());

    expectEmoji(res.at(0), 0, 2, srh::EmojiLevel::UNKNOWN_FLAG, Emoji::UNKNOWN_FLAG);
    expectEmoji(res.at(1), 2, 2, srh::EmojiLevel::FULL, Emoji::ARAB_EMIRATES);
}


///
///  Incomplete flag, firm!
///
TEST (DecodeTrie, IncompleteFlags)
{
    Trie1 tr;
    const char32_t data[] { cp::FLAG_A, 'a', cp::FLAG_A, cp::FLAG_F,
                            'b', cp::FLAG_E, cp::FLAG_S, 0 };
    auto res = tr.decode(data);

    EXPECT_EQ(2u, res.size());

    expectEmoji(res.at(0), 2, 2, srh::EmojiLevel::FULL, Emoji::AFGHANISTAN);
    expectEmoji(res.at(1), 5, 2, srh::EmojiLevel::FULL, Emoji::SPAIN);
}


///
///  Decode
///
TEST (DecodeTrie, InterracialKissPart)
{
    Trie1 tr;
    const char32_t data[] { 'A',
                cp::WOMAN, cp::SKIN1, cp::ZWJ,
                cp::EMOJI_RED_HEART, cp::ZWJ,
                cp::KISS_MARK, cp::ZWJ,
                cp::MAN, cp::SKIN5, 'B', 'C' };
    auto res = tr.decode(data);

    EXPECT_EQ(1u, res.size());

    expectEmoji(res.at(0), 1,  9, srh::EmojiLevel::PART, Emoji::KISS_INTERRACIAL);
}


///
///  Decode
///
TEST (DecodeTrie, HeadYesFull)
{
    Trie1 tr;
    const char32_t data[] {
                cp::SMILE_SLIGHTLY_SMILING, cp::ZWJ,
                cp::ARROW_UP_DOWN, cp::VS16 };
    auto res = tr.decode(data);

    EXPECT_EQ(1u, res.size());

    expectEmoji(res.at(0), 0,  4, srh::EmojiLevel::FULL, Emoji::HEAD_YES);
}


///
///  Decode
///
TEST (DecodeTrie, HeadYesPart)
{
    Trie1 tr;
    const char32_t data[] {
                cp::SMILE_SLIGHTLY_SMILING, cp::ZWJ,
                cp::ARROW_UP_DOWN, cp::VS15 };
    auto res = tr.decode(data);

    EXPECT_EQ(1u, res.size());

    expectEmoji(res.at(0), 0,  3, srh::EmojiLevel::PART, Emoji::HEAD_YES);
}


///
///  Prereq — should decode read heart
///
TEST (DecodeTrie, HeartRedFull1)
{
    Trie1 tr;
    const char32_t data[] {
                'a', 'b', cp::EMOJI_RED_HEART, cp::VS16, 'b' };
    auto res = tr.decode(data);

    EXPECT_EQ(1u, res.size());

    expectEmoji(res.at(0), 2,  2, srh::EmojiLevel::FULL, Emoji::HEART_RED);
}




///
///  End works too
///
TEST (DecodeTrie, HeartRedFull2)
{
    Trie1 tr;
    const char32_t data[] {
                'a', 'b', cp::EMOJI_RED_HEART, cp::VS16 };
    auto res = tr.decode(data);

    EXPECT_EQ(1u, res.size());

    expectEmoji(res.at(0), 2,  2, srh::EmojiLevel::FULL, Emoji::HEART_RED);
}


///
///  Should not decode 1-char read heart
///
TEST (DecodeTrie, HeartRedPart1)
{
    Trie1 tr;
    const char32_t data[] {
                'a', 'b', cp::EMOJI_RED_HEART, 'c' };
    auto res = tr.decode(data);

    EXPECT_EQ(0u, res.size());
}


///
///  Same
///
TEST (DecodeTrie, HeartRedPart2)
{
    Trie1 tr;
    const char32_t data[] {
                'a', 'b', cp::EMOJI_RED_HEART };
    auto res = tr.decode(data);

    EXPECT_EQ(0u, res.size());
}
