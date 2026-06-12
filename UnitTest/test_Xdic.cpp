// What we are testing
#include "Xdic.h"
#include <charconv>

// Google test
#include "gtest/gtest.h"


TEST (Xdic, Empty)
{
    xs::Dic dic;
    EXPECT_EQ(0u, dic.countAttr());
    EXPECT_EQ(0u, dic.countStyle());
    EXPECT_FALSE(dic.hasAttr());
    EXPECT_FALSE(dic.hasStyle());
}

TEST (Xdic, OneAttr)
{
    xs::Dic dic;
    dic.putAt(xid::COLOR, xs::Place::ATTR) = xs::IdLink{"test"};
    EXPECT_EQ(1u, dic.countAttr());
    EXPECT_EQ(0u, dic.countStyle());
    EXPECT_TRUE(dic.hasAttr());
    EXPECT_FALSE(dic.hasStyle());

    dic.traverseAttr([](const xs::Kv& k) {
        EXPECT_EQ(xid::COLOR, k.id);
        EXPECT_EQ("color", k.key());
        auto* q = std::get_if<xs::IdLink>(&k.value);
        EXPECT_TRUE(q);
        EXPECT_EQ("test", q->refId);
    });
}

TEST (Xdic, OneStyle)
{
    xs::Dic dic;
    dic.putAt(xid::FILL, xs::Place::STYLE) = xs::IdLink{"test"};
    EXPECT_EQ(0u, dic.countAttr());
    EXPECT_EQ(1u, dic.countStyle());
    EXPECT_FALSE(dic.hasAttr());
    EXPECT_TRUE(dic.hasStyle());

    dic.traverseStyle([](const xs::Kv& k) {
        EXPECT_EQ(xid::FILL, k.id);
        EXPECT_EQ("fill", k.key());
        auto* q = std::get_if<xs::IdLink>(&k.value);
        EXPECT_TRUE(q);
        EXPECT_EQ("test", q->refId);
    });
}

TEST (Xdic, OnePlusOne)
{
    xs::Dic dic;
    dic.putAt(xid::COLOR, xs::Place::ATTR) = xs::IdLink{"alpha"};
    dic.putAt(xid::AZIMUTH, xs::Place::STYLE) = xs::IdLink{"bravo"};
    EXPECT_EQ(1u, dic.countAttr());
    EXPECT_EQ(1u, dic.countStyle());
    EXPECT_TRUE(dic.hasAttr());
    EXPECT_TRUE(dic.hasStyle());

    dic.traverseAttr([](const xs::Kv& k) {
        EXPECT_EQ(xid::COLOR, k.id);
        EXPECT_EQ("color", k.key());
        auto* q = std::get_if<xs::IdLink>(&k.value);
        EXPECT_TRUE(q);
        EXPECT_EQ("alpha", q->refId);
    });
    dic.traverseStyle([](const xs::Kv& k) {
        EXPECT_EQ(xid::AZIMUTH, k.id);
        EXPECT_EQ("azimuth", k.key());
        auto* q = std::get_if<xs::IdLink>(&k.value);
        EXPECT_TRUE(q);
        EXPECT_EQ("bravo", q->refId);
    });
}

TEST (Xdic, MultiAttr)
{
    xs::Dic dic;
    dic.putAt(xid::COLOR, xs::Place::ATTR) = xs::IdLink{"alpha"};
    dic.putAt(xid::AZIMUTH, xs::Place::ATTR) = xs::IdLink{"bravo"};
    dic.putAt(xid::STROKE_OPACITY, xs::Place::ATTR) = xs::IdLink{"charlie"};
    dic.putAt(xid::MEDIA, xs::Place::ATTR) = xs::IdLink{"delta"};

    EXPECT_EQ(4u, dic.countAttr());
    EXPECT_EQ(0u, dic.countStyle());
    EXPECT_TRUE(dic.hasAttr());
    EXPECT_FALSE(dic.hasStyle());

    std::string sum;
    dic.traverseAttr([&sum](const xs::Kv& k) {
        auto* q = std::get_if<xs::IdLink>(&k.value);
        EXPECT_TRUE(q);
        sum += q->refId;
    });
    EXPECT_EQ("alphabravocharliedelta", sum);
}

TEST (Xdic, MultiStyle)
{
    xs::Dic dic;
    dic.putAt(xid::COLOR, xs::Place::STYLE) = xs::IdLink{"alpha"};
    dic.putAt(xid::AZIMUTH, xs::Place::STYLE) = xs::IdLink{"bravo"};
    dic.putAt(xid::STROKE_OPACITY, xs::Place::STYLE) = xs::IdLink{"charlie"};
    dic.putAt(xid::MEDIA, xs::Place::STYLE) = xs::IdLink{"delta"};

    EXPECT_EQ(0u, dic.countAttr());
    EXPECT_EQ(4u, dic.countStyle());
    EXPECT_FALSE(dic.hasAttr());
    EXPECT_TRUE(dic.hasStyle());

    std::string sum;
    dic.traverseStyle([&sum](const xs::Kv& k) {
        auto* q = std::get_if<xs::IdLink>(&k.value);
        EXPECT_TRUE(q);
        sum += q->refId;
    });
    EXPECT_EQ("alphabravocharliedelta", sum);
}

TEST (Xdic, Change)
{
    xs::Dic dic;
    dic.putAt(xid::COLOR, xs::Place::STYLE) = xs::IdLink{"alpha"};
    dic.putAt(xid::AZIMUTH, xs::Place::STYLE) = xs::IdLink{"bravo"};
    dic.putAt(xid::STROKE_OPACITY, xs::Place::STYLE) = xs::IdLink{"charlie"};
    dic.putAt(xid::MEDIA, xs::Place::STYLE) = xs::IdLink{"delta"};
    dic.putAt(xid::STROKE_OPACITY, xs::Place::STYLE) = xs::IdLink{"ECHO"};

    EXPECT_EQ(0u, dic.countAttr());
    EXPECT_EQ(4u, dic.countStyle());
    EXPECT_FALSE(dic.hasAttr());
    EXPECT_TRUE(dic.hasStyle());

    std::string sum;
    dic.traverseStyle([&sum](const xs::Kv& k) {
        auto* q = std::get_if<xs::IdLink>(&k.value);
        EXPECT_TRUE(q);
        sum += q->refId;
    });
    EXPECT_EQ("alphabravoECHOdelta", sum);
}

///
///  Moving style to attribute works
///
TEST (Xdic, StyleToAttr)
{
    xs::Dic dic;
    dic.putAt(xid::COLOR, xs::Place::STYLE) = xs::IdLink{"alpha"};
    dic.putAt(xid::AZIMUTH, xs::Place::STYLE) = xs::IdLink{"bravo"};
    dic.putAt(xid::STROKE_OPACITY, xs::Place::STYLE) = xs::IdLink{"charlie"};
    dic.putAt(xid::MEDIA, xs::Place::STYLE) = xs::IdLink{"delta"};
    dic.putAt(xid::STROKE_OPACITY, xs::Place::ATTR) = xs::IdLink{"ECHO"};

    EXPECT_EQ(1u, dic.countAttr());
    EXPECT_EQ(3u, dic.countStyle());
    EXPECT_TRUE(dic.hasAttr());
    EXPECT_TRUE(dic.hasStyle());

    std::string sum;
    dic.traverseAttr([&sum](const xs::Kv& k) {
        auto* q = std::get_if<xs::IdLink>(&k.value);
        EXPECT_TRUE(q);
        sum += q->refId;
    });
    EXPECT_EQ("ECHO", sum);

    sum.clear();
    dic.traverseStyle([&sum](const xs::Kv& k) {
        auto* q = std::get_if<xs::IdLink>(&k.value);
        EXPECT_TRUE(q);
        sum += q->refId;
    });
    EXPECT_EQ("alphabravodelta", sum);
}

///
///  Moving style to attribute, more things
///
TEST (Xdic, StyleToAttr2)
{
    xs::Dic dic;
    dic.putAt(xid::COLOR, xs::Place::STYLE) = xs::IdLink{"alpha"};
    dic.putAt(xid::AZIMUTH, xs::Place::STYLE) = xs::IdLink{"bravo"};
    dic.putAt(xid::STROKE_OPACITY, xs::Place::STYLE) = xs::IdLink{"charlie"};
    dic.putAt(xid::MEDIA, xs::Place::STYLE) = xs::IdLink{"delta"};
    dic.putAt(xid::FONT_WEIGHT, xs::Place::ATTR) = xs::IdLink{"echo"};
    dic.putAt(xid::STROKE_OPACITY, xs::Place::ATTR) = xs::IdLink{"FOXTROT"};
    dic.putAt(xid::AZIMUTH, xs::Place::ATTR) = xs::IdLink{"GOLF"};

    EXPECT_EQ(3u, dic.countAttr());
    EXPECT_EQ(2u, dic.countStyle());
    EXPECT_TRUE(dic.hasAttr());
    EXPECT_TRUE(dic.hasStyle());

    std::string sum;
    dic.traverseAttr([&sum](const xs::Kv& k) {
        auto* q = std::get_if<xs::IdLink>(&k.value);
        EXPECT_TRUE(q);
        sum += q->refId;
    });
    EXPECT_EQ("echoFOXTROTGOLF", sum);

    sum.clear();
    dic.traverseStyle([&sum](const xs::Kv& k) {
        auto* q = std::get_if<xs::IdLink>(&k.value);
        EXPECT_TRUE(q);
        sum += q->refId;
    });
    EXPECT_EQ("alphadelta", sum);
}

///
///  Moving style to attribute and back works,
///  and does not preserve order
///
TEST (Xdic, StyleToAttrAndBack)
{
    xs::Dic dic;
    dic.putAt(xid::COLOR, xs::Place::STYLE) = xs::IdLink{"alpha"};
    dic.putAt(xid::AZIMUTH, xs::Place::STYLE) = xs::IdLink{"bravo"};
    dic.putAt(xid::STROKE_OPACITY, xs::Place::STYLE) = xs::IdLink{"charlie"};
    dic.putAt(xid::MEDIA, xs::Place::STYLE) = xs::IdLink{"delta"};
    dic.putAt(xid::STROKE_OPACITY, xs::Place::ATTR) = xs::IdLink{"ECHO"};
    dic.putAt(xid::STROKE_OPACITY, xs::Place::STYLE) = xs::IdLink{"FOXTROT"};

    EXPECT_EQ(0u, dic.countAttr());
    EXPECT_EQ(4u, dic.countStyle());
    EXPECT_FALSE(dic.hasAttr());
    EXPECT_TRUE(dic.hasStyle());

    std::string sum;
    dic.traverseStyle([&sum](const xs::Kv& k) {
        auto* q = std::get_if<xs::IdLink>(&k.value);
        EXPECT_TRUE(q);
        sum += q->refId;
    });
    EXPECT_EQ("alphabravodeltaFOXTROT", sum);
}
