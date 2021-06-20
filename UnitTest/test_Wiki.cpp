// What we are testing
#include "Wiki.h"

// Google test
#include "gtest/gtest.h"


///// skipParam ////////////////////////////////////////////////////////////////

TEST (SkipParam, FoundNothing)
{
    std::string_view s = "alpha";
    auto x = wiki::skipParam(s, '}');
    EXPECT_EQ(wiki::ParamType::STRING_END, x.type);
    EXPECT_EQ(s.data() + 5, x.posEnd);
    EXPECT_EQ(s.data() + 5, x.posNext);
}


TEST (SkipParam, FoundWrongBracket)
{
    std::string_view s = "alpha]]bravo";
    auto x = wiki::skipParam(s, '}');
    EXPECT_EQ(wiki::ParamType::STRING_END, x.type);
    EXPECT_EQ(s.data() + 12, x.posEnd);
    EXPECT_EQ(s.data() + 12, x.posNext);
}

TEST (SkipParam, FoundRightBracket)
{
    std::string_view s = "alpha}}bravo";
    auto x = wiki::skipParam(s, '}');
    EXPECT_EQ(wiki::ParamType::THING_END, x.type);
    EXPECT_EQ(s.data() + 5, x.posEnd);
    EXPECT_EQ(s.data() + 7, x.posNext);
}

TEST (SkipParam, FoundSingleBracket)
{
    std::string_view s = "alpha}bravo";
    auto x = wiki::skipParam(s, '}');
    EXPECT_EQ(wiki::ParamType::STRING_END, x.type);
    EXPECT_EQ(s.data() + 11, x.posEnd);
    EXPECT_EQ(s.data() + 11, x.posNext);
}

TEST (SkipParam, FoundBar)
{
    std::string_view s = "alpha|bravo";
    auto x = wiki::skipParam(s, '}');
    EXPECT_EQ(wiki::ParamType::BAR, x.type);
    EXPECT_EQ(s.data() + 5, x.posEnd);
    EXPECT_EQ(s.data() + 6, x.posNext);
}

TEST (SkipParam, Nested1)
{
    std::string_view s = "alpha{{t}}br}}avo";
    auto x = wiki::skipParam(s, '}');
    EXPECT_EQ(wiki::ParamType::THING_END, x.type);
    EXPECT_EQ(s.data() + 12, x.posEnd);
    EXPECT_EQ(s.data() + 14, x.posNext);
}

TEST (SkipParam, Nested2)
{
    std::string_view s = "alpha{{t}}}}bravo";
    auto x = wiki::skipParam(s, '}');
    EXPECT_EQ(wiki::ParamType::THING_END, x.type);
    EXPECT_EQ(s.data() + 10, x.posEnd);
    EXPECT_EQ(s.data() + 12, x.posNext);
}

TEST (SkipParam, Nested3)
{
    std::string_view s = "alpha{{{{t}}br}}avo";
    auto x = wiki::skipParam(s, '}');
    EXPECT_EQ(wiki::ParamType::STRING_END, x.type);
    EXPECT_EQ(s.data() + 19, x.posEnd);
    EXPECT_EQ(s.data() + 19, x.posNext);
}

TEST (SkipParam, NestedBad)
{
    std::string_view s = "alpha{{{t}}br}}avo";
    auto x = wiki::skipParam(s, '}');
    EXPECT_EQ(wiki::ParamType::THING_END, x.type);
    EXPECT_EQ(s.data() + 13, x.posEnd);
    EXPECT_EQ(s.data() + 15, x.posNext);
}

TEST (SkipParam, Nested4)
{
    std::string_view s = "alpha{{t|[[u]]|v}}br|avo";
    auto x = wiki::skipParam(s, '}');
    EXPECT_EQ(wiki::ParamType::BAR, x.type);
    EXPECT_EQ(s.data() + 20, x.posEnd);
    EXPECT_EQ(s.data() + 21, x.posNext);
}

TEST (SkipParam, WrongClosing1)
{
    std::string_view s = "alpha{{t]]br}}avo";
    auto x = wiki::skipParam(s, '}');
    EXPECT_EQ(wiki::ParamType::STRING_END, x.type);
    EXPECT_EQ(s.data() + 17, x.posEnd);
    EXPECT_EQ(s.data() + 17, x.posNext);
}


///// findThing ////////////////////////////////////////////////////////////////


TEST (FindThing, Nothing)
{
    std::string_view s = "alpha";
    auto x = wiki::findThing(s);
    EXPECT_EQ(wiki::Type::STRING_END, x.type);
    EXPECT_EQ(s.data() + 5, x.posStart);
    EXPECT_EQ(s.data() + 5, x.posNext);
    EXPECT_TRUE(x.params.empty());
}


TEST (FindThing, EmptyLink)
{
    std::string_view s = "alpha[[|bravo]]c";
    auto x = wiki::findThing(s);
    EXPECT_EQ(wiki::Type::STRING_END, x.type);
    EXPECT_EQ(s.data() + 16, x.posStart);
    EXPECT_EQ(s.data() + 16, x.posNext);
    EXPECT_TRUE(x.params.empty());
}


TEST (FindThing, SimpleLink)
{
    std::string_view s = "alpha[[ bravo  ]]c";
    auto x = wiki::findThing(s);
    EXPECT_EQ(wiki::Type::LINK, x.type);
    EXPECT_EQ(s.data() + 5, x.posStart);
    EXPECT_EQ(s.data() + 17, x.posNext);
    EXPECT_EQ(2, x.params.size());
    EXPECT_EQ("bravo", x.params[0]);
    EXPECT_EQ(" bravo  ", x.params[1]);
}


TEST (FindThing, SimpleTemplate)
{
    std::string_view s = "alpha{{ bravo  }}c";
    auto x = wiki::findThing(s);
    EXPECT_EQ(wiki::Type::TEMPLATE, x.type);
    EXPECT_EQ(s.data() + 5, x.posStart);
    EXPECT_EQ(s.data() + 17, x.posNext);
    EXPECT_EQ(1, x.params.size());
    EXPECT_EQ("bravo", x.params[0]);
}


TEST (FindThing, ComplexLink)
{
    std::string_view s = "alpha[[ bravo  | charlie  | delta  ]]e";
    auto x = wiki::findThing(s);
    EXPECT_EQ(wiki::Type::LINK, x.type);
    EXPECT_EQ(s.data() + 5, x.posStart);
    EXPECT_EQ(s.data() + 37, x.posNext);
    EXPECT_EQ(3, x.params.size());
    EXPECT_EQ("bravo", x.params[0]);
    EXPECT_EQ(" charlie  ", x.params[1]);
    EXPECT_EQ(" delta  ", x.params[2]);
}


///// appendHtml ///////////////////////////////////////////////////////////////


TEST (appendHtml, Simple)
{
    std::string_view s = "alpha<bravo>&charlie";
    std::string r;
    wiki::appendHtml(r, s);
    EXPECT_EQ("alpha&lt;bravo&gt;&amp;charlie", r);
}


///// run //////////////////////////////////////////////////////////////////////

namespace {

    class Eng : public wiki::Engine
    {
    public:
        std::string s;
        void appendPlain(std::string_view x) override;
        void appendLink(const SafeVector<std::string_view> x) override;
        void appendTemplate(const SafeVector<std::string_view> x) override;
    };

    void Eng::appendPlain(std::string_view x)
    {
        s.append("Plain:");
        s.append(x);
        s.append("\n");
    }

    void Eng::appendLink(const SafeVector<std::string_view> x)
    {
        s.append("Link:");
        for (size_t i = 0; i < x.size(); ++i) {
            if (i != 0)
                s.push_back(',');
            s.append(x[i]);
        }
        s.append("\n");
    }

    void Eng::appendTemplate(const SafeVector<std::string_view> x)
    {
        s.append("Template:");
        for (size_t i = 0; i < x.size(); ++i) {
            if (i != 0)
                s.push_back(',');
            s.append(x[i]);
        }
        s.append("\n");
    }

}   // anon namespace


TEST (Run, Plain)
{
    std::string_view s = "alpha";
    Eng eng;
    wiki::run(eng, s);
    EXPECT_EQ("Plain:alpha\n", eng.s);
}


///
///  [[wiki]]s: No prefixes/suffixes
///  '''HTML''': No bold/italic
///
TEST (Run, Simple)
{
    std::string_view s = "The [[wiki]]s are much {{big|simpler}} than '''HTML'''.";
    Eng eng;
    wiki::run(eng, s);
    std::string_view expected =
            "Plain:The \n"
            "Link:wiki,wiki\n"
            "Plain:s are much \n"
            "Template:big,simpler\n"
            "Plain: than '''HTML'''.\n";
    EXPECT_EQ(expected, eng.s);
}
