// My header
#include "Xtypes.h"

#include <cmath>
#include <stdexcept>
#include <algorithm>

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


///// Number ///////////////////////////////////////////////////////////////////


void xs::Number::encodeAttr(double value, std::string& dest, int precision)
{
    if (!std::isfinite(value)) {
        throw std::logic_error("Finished out with an infinite number");
    }
    char buf[40];
    auto res = std::to_chars(std::begin(buf), std::end(buf) - 1,
            value, std::chars_format::general, precision);
    if (res.ec != std::errc()) {
        throw std::logic_error("Somehow cannot convert number");
    }
    auto beg = buf;
    auto end = res.ptr;
    *end = '\0';   // just for simplicity
    auto len = end - beg;
    // Ways to shorten the string
    if (len >= 3) {
        // 0.5 → .5
        if (buf[0] == '0' && buf[1] == '.') {
            ++beg;
        } else if (buf[0] == '-' && buf[1] == '0' && buf[2] == '.') {
            // -0.5 → -.5
            ++beg;
            *beg = '-';
        }
        static std::string_view EXP = "Ee";
        auto pE = std::find_first_of(beg, end, EXP.begin(), EXP.end());
        if (pE != end) {
            // Parse numeric exp part and write again
            *pE = 'e';
            auto pExpNum = pE + 1;
            int expValue;
            // from_chars cannot parse +1
            auto pParse = pExpNum;
            if (*pParse == '+')
                ++pParse;
            auto res2 = std::from_chars(pParse, end, expValue);
            if (res2.ec != std::errc{}) {
                throw std::logic_error("Cannot parse exponent back");
            }
            if (auto pPoint = std::find(beg, pE, '.');
                    pPoint != pE) {   // Have dec.point
                auto nDigs = pE - pPoint - 1;
                expValue -= nDigs;
                if (expValue == 0) {
                    throw std::logic_error("Strange, wrote in exp.form, though can in simple");
                }
                pExpNum = std::copy(pPoint + 1, pExpNum, pPoint);
            }
            auto res3 = std::to_chars(pExpNum, std::end(buf), expValue);
            if (res3.ec != std::errc{}) {
                throw std::logic_error("Cannot write exponent once again");
            }
            end = res3.ptr;
        } else {
            // No exponent, but 1000=1e3 would be cool!
            if (len >= 4) {
                unsigned expValue = 0;
                auto p = end - 1;
                while (*p == '0') {
                    ++expValue;
                    --p;
                }
                if (expValue >= 3) {
                    ++p;
                    *(p++) = 'e';
                    auto res4 = std::to_chars(p, end, expValue);
                    if (res4.ec != std::errc{}) {
                        throw std::logic_error("Cannot write exponent, or it is not smaller");
                    }
                    end = res4.ptr;
                }
            }
        }
    }
    dest.append(beg, end);
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


///// ValueVar /////////////////////////////////////////////////////////////////


void xs::ValueVar::encodeAttr(std::string& dest) const
{
    std::visit(
        [&dest](const auto& x) {
            x.encodeAttr(dest);
        },
        *this);
}
