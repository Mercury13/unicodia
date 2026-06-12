// My header
#include "Xtypes.h"

#include "u_Strings.h"

using namespace std::string_view_literals;


///// xsin /////////////////////////////////////////////////////////////////////


void xsin::encodeAttr(std::string& dest, std::string_view x)
{
    dest.reserve(dest.length() + x.length());
    for (auto c : x) {
        switch (c) {
        case '"':
            dest += "&quot;";
            break;
        default:
            dest += c;
            break;
        }
    }
}

void xsin::writeAttrIf(std::string& dest, std::string_view key, std::string_view value)
{
    if (!value.empty())
        writeAttr(dest, key, value);
}

void xsin::startAttr(std::string& dest, std::string_view key)
{
    dest += ' ';
    dest += key;
    dest += R"(=")";
}

void xsin::writeAttr(std::string& dest, std::string_view key, std::string_view value)
{
    xsin::startAttr(dest, key);
    xsin::encodeAttr(dest, value);
    dest += '"';
}


///// IdLink ///////////////////////////////////////////////////////////////////


static_assert('0' == 48, "Non-ASCII machine");
static_assert('A' == 65, "Non-ASCII machine");
static_assert('a' == 97, "Non-ASCII machine");

namespace {

#define TWO_OF(x) (x), (x)
#define FOUR_OF(x) TWO_OF(x), TWO_OF(x)
#define EIGHT_OF(x) FOUR_OF(x), FOUR_OF(x)
#define TEN_OF(x) EIGHT_OF(x), TWO_OF(x)
#define ROW_OF(x) EIGHT_OF(x), EIGHT_OF(x)
#define ALPH_OF(x) ROW_OF(x), TEN_OF(x)  // 16+10
#define ba xs::CharType::BAN
#define ST xs::CharType::START
#define NX xs::CharType::NEXT

    constinit const xs::CharType charTypes[] {
        ROW_OF(ba), ROW_OF(ba), // 32 control
        ba,ba,ba,ba,  ba,ba,ba,ba,  ba,ba,ba,ba, ba,NX,NX,ba,  // only 2D -, 2E .
        TEN_OF(NX),                       NX,ba, ba,ba,ba,ba,  // digs and 3A :
        ba, ALPH_OF(ST),                     ba, ba,ba,ba,ST,  // letters and 5F _
        ba, ALPH_OF(ST),                     ba, ba,ba,ba,ba,  // letters only
        ROW_OF(ST), ROW_OF(ST), ROW_OF(ST), ROW_OF(ST),  // last 128
        ROW_OF(ST), ROW_OF(ST), ROW_OF(ST), ROW_OF(ST),
    };
    static_assert(std::size(charTypes) == 256);

#undef ba
#undef ST
#undef NX
#undef TWO_OF
#undef FOUR_OF
#undef EIGHT_OF
#undef TEN_OF
#undef ROW_OF
#undef ALPH_OF

}   // anon namespace

inline xs::CharType xs::IdLink::charType(unsigned char x) noexcept
{
    return charTypes[x];
}


bool xs::IdLink::isId(std::string_view x) noexcept
{
    if (x.empty())
        return false;
    if (charType(x[0]) != CharType::START)
        return false;
    x = x.substr(1);
    for (auto c : x)
        if (charType(c) == CharType::BAN)
            return false;
    return true;
}


std::optional<xs::IdLink> xs::IdLink::parse(std::string_view x)
{
    if (!x.starts_with("url"sv) || !x.ends_with(')'))
        return std::nullopt;
    x = str::trimSv(x.substr(3, x.length() - 4));
    if (!x.starts_with('('))
        return std::nullopt;
    x = str::trimLeftSv(x.substr(1));
    if (!x.starts_with('#'))
        return std::nullopt;
    x = str::trimLeftSv(x.substr(1));
    if (!isId(x))
        return std::nullopt;
    return xs::IdLink(x);
}


void xs::IdLink::encodeAttr(std::string& dest) const
{
    dest += "url(#"sv;
    xsin::encodeAttr(dest, refId);
    dest += ')';
}
