// My header
#include "ucdcom.h"

// C++
#include <stdexcept>
#include <charconv>
#include <fstream>

// Unicode loaders
#include "loader.h"

// Libs
#include "u_Strings.h"


using namespace std::string_view_literals;


std::string encodeC(std::string_view x)
{
    std::string r;
    r.reserve(x.size());
    for (auto c : x) {
        switch (c) {
        case '"':
            r += R"(\")";
            break;
        case '\\':
            r += R"(\\)";
            break;
        default:
            r += c;
            break;
        }
    }
    return r;
}


unsigned fromHex(std::string_view x)
{
    unsigned r;
    auto beg = x.data();
    auto end = beg + x.length();
    auto res = std::from_chars(beg, end, r, 16);
    if (res.ec != std::errc() || res.ptr != end) {
        throw std::invalid_argument(
                str::cat("[fromHex] Cannot parse hex ", x));
    }
    return r;
}


int fromDec(std::string_view x)
{
    int r;
    auto beg = x.data();
    auto end = beg + x.length();
    auto res = std::from_chars(beg, end, r);
    if (res.ec != std::errc() || res.ptr != end) {
        throw std::invalid_argument(
                str::cat("[fromDec] Cannot parse number ", x));
    }
    return r;
}


bool fromHexIf(std::string_view x, unsigned& y)
{
    auto beg = x.data();
    auto end = beg + x.length();
    auto res = std::from_chars(beg, end, y, 16);
    return (res.ec == std::errc() && res.ptr == end);
}


ucd::Range ucd::Range::from(std::string_view x)
{
    auto p = x.find(".."sv);
    if (p == std::string_view::npos) {  // ONE
        auto q = fromHex(x);
        return { q, q };
    } else {
        auto x1 = x.substr(0, p);
        auto x2 = x.substr(p + 2);
        return { fromHex(x1), fromHex(x2) };
    }
}


ucd::PropBase ucd::loadPropBase()
{
    PropBase r;

    std::ifstream is(UCD_PVA);
    std::string line;
    while (std::getline(is, line)) {
        std::string_view trimmed = str::trimSv(line);
        if (trimmed.empty() || trimmed.starts_with('#'))
            continue;

        auto vals = str::splitSv(trimmed, ';', false);
        // 3 is minimum: cat, key, value
        if (vals.size() < 3)
            continue;

        auto cat = vals.at(0);
        if (cat == "sc") {  // SCRIPT
            auto shortn = vals.at(1);
            auto longn = vals.at(2);
            r.addScript(longn, shortn);
        }
    }

    return r;
}


///// PropBase /////////////////////////////////////////////////////////////////


void ucd::PropBase::addScript(std::string_view longv, std::string_view shortv)
{
    scriptsLongToShort.emplace(longv, shortv);
}


ucd::PropBase::MStoS ucd::PropBase::buildShortToLong() const
{
    MStoS r;
    for (auto& [l, s] : scriptsLongToShort) {
        r.emplace(s, l);
    }
    return r;
}


std::string_view ucd::PropBase::shortenScript(std::string_view text) const
{
    auto q = scriptsLongToShort.find(text);
    if (q == scriptsLongToShort.end())
        throw std::logic_error(str::cat("Cannot shorten script: ", text));
    return q->second;
}
