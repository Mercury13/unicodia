// What we are testing
#include "u_DumbSp.h"

// Google test
#include "gtest/gtest.h"


// Testing fixture
struct Counter
{
    std::atomic<size_t> nCtors = 0, nDtors = 0;
};

struct Target : public dumb::SpTarget
{
    int tag;
    Counter& counter;

    Target(int aTag, Counter& aCounter)
        : tag(aTag), counter(aCounter) { ++counter.nCtors; }
    ~Target() { ++counter.nDtors; }
};


TEST (DumbSp, CtorDtor)
{
    dumb::Sp<Target> p;
    EXPECT_FALSE(p);
    EXPECT_EQ(nullptr, p.get());
    EXPECT_EQ(0, p.refCount());
}


TEST (DumbSp, ConstructByPtr)
{
    Counter ctr;
    {
        EXPECT_EQ(0, ctr.nCtors);
        EXPECT_EQ(0, ctr.nDtors);

        dumb::Sp<Target> p(new Target(41, ctr));

        EXPECT_TRUE(p);
        EXPECT_NE(nullptr, p.get());
        EXPECT_EQ(1, p.refCount());
        EXPECT_EQ(41, p->tag);

        EXPECT_EQ(1, ctr.nCtors);
        EXPECT_EQ(0, ctr.nDtors);
    }

    EXPECT_EQ(1, ctr.nCtors);
    EXPECT_EQ(1, ctr.nDtors);
}


TEST (DumbSp, AssignByPtr)
{
    Counter ctr;
    {
        dumb::Sp<Target> p;

        EXPECT_EQ(0, ctr.nCtors);
        EXPECT_EQ(0, ctr.nDtors);

        p = dumb::Sp<Target>(new Target(42, ctr));

        EXPECT_TRUE(p);
        EXPECT_NE(nullptr, p.get());
        EXPECT_EQ(1, p.refCount());
        EXPECT_EQ(42, p->tag);

        EXPECT_EQ(1, ctr.nCtors);
        EXPECT_EQ(0, ctr.nDtors);
    }

    EXPECT_EQ(1, ctr.nCtors);
    EXPECT_EQ(1, ctr.nDtors);
}


TEST (DumbSp, MoveConstructByPtr)
{
    Counter ctr;
    {
        dumb::Sp<Target> p, q;

        EXPECT_EQ(0, ctr.nCtors);
        EXPECT_EQ(0, ctr.nDtors);

        p = dumb::Sp<Target>(new Target(42, ctr));

        EXPECT_TRUE(p);
        EXPECT_NE(nullptr, p.get());
        EXPECT_EQ(1, p.refCount());
        EXPECT_EQ(42, p->tag);

        EXPECT_EQ(1, ctr.nCtors);
        EXPECT_EQ(0, ctr.nDtors);

        q = std::move(p);

        EXPECT_FALSE(p);
        EXPECT_EQ(nullptr, p.get());
        EXPECT_EQ(0, p.refCount());

        EXPECT_TRUE(q);
        EXPECT_NE(nullptr, q.get());
        EXPECT_EQ(1, q.refCount());
        EXPECT_EQ(42, q->tag);
    }

    EXPECT_EQ(1, ctr.nCtors);
    EXPECT_EQ(1, ctr.nDtors);
}


TEST (DumbSp, Reset)
{
    Counter ctr;
    {
        EXPECT_EQ(0, ctr.nCtors);
        EXPECT_EQ(0, ctr.nDtors);

        dumb::Sp<Target> p(new Target(43, ctr));

        EXPECT_EQ(1, ctr.nCtors);
        EXPECT_EQ(0, ctr.nDtors);
        EXPECT_TRUE(p);

        p.reset();

        EXPECT_EQ(1, ctr.nCtors);
        EXPECT_EQ(1, ctr.nDtors);
        EXPECT_FALSE(p);
    }
}


TEST (DumbSp, MakeSp)
{
    Counter ctr;
    {
        EXPECT_EQ(0, ctr.nCtors);
        EXPECT_EQ(0, ctr.nDtors);

        auto p = dumb::makeSp<Target>(43, ctr);

        EXPECT_TRUE(p);
        EXPECT_NE(nullptr, p.get());
        EXPECT_EQ(1, p.refCount());
        EXPECT_EQ(43, p->tag);

        EXPECT_EQ(1, ctr.nCtors);
        EXPECT_EQ(0, ctr.nDtors);
    }

    EXPECT_EQ(1, ctr.nCtors);
    EXPECT_EQ(1, ctr.nDtors);
}


TEST (DumbSp, CtorByCopy)
{
    Counter ctr;
    {
        EXPECT_EQ(0, ctr.nCtors);
        EXPECT_EQ(0, ctr.nDtors);

        auto p = dumb::makeSp<Target>(44, ctr);

        EXPECT_EQ(1, ctr.nCtors);
        EXPECT_EQ(0, ctr.nDtors);

        EXPECT_TRUE(p);
        EXPECT_NE(nullptr, p.get());
        EXPECT_EQ(1, p.refCount());
        EXPECT_EQ(44, p->tag);

        {
            auto q(p);
            EXPECT_EQ(2, p.refCount());
            EXPECT_EQ(2, q.refCount());
            EXPECT_EQ(44, q->tag);

            EXPECT_EQ(1, ctr.nCtors);
            EXPECT_EQ(0, ctr.nDtors);
        }

        EXPECT_EQ(1, p.refCount());
    }

    EXPECT_EQ(1, ctr.nCtors);
    EXPECT_EQ(1, ctr.nDtors);
}


TEST (DumbSp, AssignByCopy)
{
    Counter ctr;
    {
        auto p = dumb::makeSp<Target>(45, ctr);

        EXPECT_EQ(1, ctr.nCtors);
        EXPECT_EQ(0, ctr.nDtors);

        EXPECT_TRUE(p);
        EXPECT_NE(nullptr, p.get());
        EXPECT_EQ(1, p.refCount());
        EXPECT_EQ(45, p->tag);

        {
            dumb::Sp<Target> q;
            EXPECT_EQ(0, q.refCount());

            q = p;
            EXPECT_EQ(2, p.refCount());
            EXPECT_EQ(2, q.refCount());
            EXPECT_EQ(45, q->tag);

            EXPECT_EQ(1, ctr.nCtors);
            EXPECT_EQ(0, ctr.nDtors);
        }

        EXPECT_EQ(1, p.refCount());
    }

    EXPECT_EQ(1, ctr.nCtors);
    EXPECT_EQ(1, ctr.nDtors);
}


TEST (DumbSp, AssignToSelf)
{
    Counter ctr;
    {
        auto p = dumb::makeSp<Target>(46, ctr);

        EXPECT_EQ(1, ctr.nCtors);
        EXPECT_EQ(0, ctr.nDtors);

        EXPECT_TRUE(p);
        EXPECT_NE(nullptr, p.get());
        EXPECT_EQ(1, p.refCount());
        EXPECT_EQ(46, p->tag);

        p = p;

        EXPECT_EQ(1, ctr.nCtors);
        EXPECT_EQ(0, ctr.nDtors);

        EXPECT_TRUE(p);
        EXPECT_NE(nullptr, p.get());
        EXPECT_EQ(1, p.refCount());
        EXPECT_EQ(46, p->tag);
    }

    EXPECT_EQ(1, ctr.nCtors);
    EXPECT_EQ(1, ctr.nDtors);
}


TEST (DumbSp, AssignAdvanced)
{
    Counter ctr;
    {
        auto p = dumb::makeSp<Target>(47, ctr);

        EXPECT_EQ(1, ctr.nCtors);
        EXPECT_EQ(0, ctr.nDtors);

        EXPECT_EQ(1, p.refCount());
        EXPECT_EQ(47, p->tag);

        {
            auto q = dumb::makeSp<Target>(48, ctr);
            EXPECT_EQ(1, q.refCount());
            EXPECT_EQ(48, q->tag);

            EXPECT_EQ(2, ctr.nCtors);
            EXPECT_EQ(0, ctr.nDtors);

            q = p;
            EXPECT_EQ(2, p.refCount());
            EXPECT_EQ(2, q.refCount());
            EXPECT_EQ(47, q->tag);

            EXPECT_EQ(2, ctr.nCtors);
            EXPECT_EQ(1, ctr.nDtors);
        }

        EXPECT_EQ(1, p.refCount());
    }

    EXPECT_EQ(2, ctr.nCtors);
    EXPECT_EQ(2, ctr.nDtors);
}
