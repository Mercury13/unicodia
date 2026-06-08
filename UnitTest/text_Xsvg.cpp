// What we are testing
#include "Xsvg.h"

// Google test
#include "gtest/gtest.h"

// Libs
#include "pugixml.hpp"


xsin::NsInfo getNsInfo(std::string_view x)
{
    pugi::xml_document doc;
    doc.load_buffer(x.data(), x.size());
    return xsin::detectNamespace(doc.root().first_child());
}

TEST(NsInfo, Normal)
{
    auto r = getNsInfo(R"(
<svg width="5cm" height="4cm" version="1.1"
     xmlns="http://www.w3.org/2000/svg">
  <desc>Four separate rectangles
  </desc>
    <rect x="0.5cm" y="0.5cm" width="2cm" height="1cm"/>
    <rect x="0.5cm" y="2cm" width="1cm" height="1.5cm"/>
    <rect x="3cm" y="0.5cm" width="1.5cm" height="2cm"/>
    <rect x="3.5cm" y="3cm" width="1cm" height="0.5cm"/>

  <!-- Show outline of viewport using 'rect' element -->
  <rect x=".01cm" y=".01cm" width="4.98cm" height="3.98cm"
        fill="none" stroke="blue" stroke-width=".02cm" />

</svg>
    )");
    EXPECT_TRUE(r.isNsFound);
    EXPECT_EQ("", r.prefix);
    EXPECT_EQ("xmlns", r.triggerAttr);
}

TEST(NsInfo, NoNs)
{
    auto r = getNsInfo(R"(
<svg width="5cm" height="4cm" version="1.1">
  <desc>Four separate rectangles
  </desc>
    <rect x="0.5cm" y="0.5cm" width="2cm" height="1cm"/>
    <rect x="0.5cm" y="2cm" width="1cm" height="1.5cm"/>
    <rect x="3cm" y="0.5cm" width="1.5cm" height="2cm"/>
    <rect x="3.5cm" y="3cm" width="1cm" height="0.5cm"/>

  <!-- Show outline of viewport using 'rect' element -->
  <rect x=".01cm" y=".01cm" width="4.98cm" height="3.98cm"
        fill="none" stroke="blue" stroke-width=".02cm" />

</svg>
    )");
    EXPECT_FALSE(r.isNsFound);
    EXPECT_EQ("", r.prefix);
    EXPECT_EQ("", r.triggerAttr);
}

TEST(NsInfo, CustomNs)
{
    auto r = getNsInfo(R"(
<?xml version="1.0" standalone="yes"?>
   <svg:svg xmlns:svg="http://www.w3.org/2000/svg" width="4cm" height="8cm">
      <svg:ellipse cx="2cm" cy="4cm" rx="2cm" ry="1cm" />
   </svg:svg>
    )");
    EXPECT_TRUE(r.isNsFound);
    EXPECT_EQ("svg:", r.prefix);
    EXPECT_EQ("xmlns:svg", r.triggerAttr);
}

TEST(NsInfo, CustomNsBad)
{
    auto r = getNsInfo(R"(
<?xml version="1.0" standalone="yes"?>
   <svg xmlns:="http://www.w3.org/2000/svg" width="4cm" height="8cm">
      <ellipse cx="2cm" cy="4cm" rx="2cm" ry="1cm" />
   </svg>
    )");
    EXPECT_FALSE(r.isNsFound);
    EXPECT_EQ("", r.prefix);
    EXPECT_EQ("", r.triggerAttr);
}
