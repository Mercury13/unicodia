// C++ I/O
#include <iostream>
#include <fstream>

// STL
#include <set>
#include <unordered_set>

// Libs
#include "u_Strings.h"
#include "Fonts/MemFont.h"

using namespace std::string_view_literals;


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


struct UoIterator
{
    std::unordered_set<char32_t>& slave;
    UoIterator(std::unordered_set<char32_t>& x) noexcept : slave(x) {}
    const UoIterator& operator * () const noexcept { return *this; }
    void operator = (char32_t x) const { slave.insert(x); }
    void operator ++ () noexcept {}
};


std::unordered_set<char32_t> getNonChineseChars()
{
    std::ifstream is("Unihan_IRGSources.txt");
    if (!is.is_open())
        throw std::logic_error("Cannot open Unihan sources");

    std::set<char32_t> chineseFound, nonChineseFound;
    std::string line;
    while (std::getline(is, line)) {
        auto line1 = str::trimSv(line);
        if (line1.empty() || line1.starts_with('#'))
            continue;
        auto cells = str::splitSv(line1, '\t');
        if (cells.size() < 2)
            continue;
        // Code
        auto sCode = cells[0];
        if (!sCode.starts_with("U+"))
            continue;
        sCode = sCode.substr(2);
        char32_t code = fromHex(sCode);
        // Value
        auto sParam = cells[1];
        if (sParam == "kIRG_GSource"sv) {
            // Chinese source
            chineseFound.insert(chineseFound.end(), code);
        } else if (sParam == "kIRG_JSource"sv
                || sParam == "kIRG_KSource"sv
                || sParam == "kIRG_TSource"sv) {
            // Other source
            nonChineseFound.insert(nonChineseFound.end(), code);
        }
    }

    std::unordered_set<char32_t> r;
    UoIterator rit(r);
    std::set_difference(
                nonChineseFound.begin(), nonChineseFound.end(),
                chineseFound.begin(), chineseFound.end(),
                rit);

    return r;
}

struct BsResult {
    unsigned nGlyphs = 0;
    unsigned nNonChinese = 0;
};

BsResult readBabelStoneFont(const std::unordered_set<char32_t>& nonChinese)
{
    MemFont font;
    if (!font.load("BabelStoneHan.ttf"))
        throw std::logic_error("Cannot load BableStone font");

    BsResult r;
    std::ofstream os("nonChinese.txt");
    font.traverseCps([&](char32_t cp, unsigned) {
        ++r.nGlyphs;
        if (nonChinese.contains(cp)) {
            ++r.nNonChinese;
            char buf[40];
            snprintf(buf, std::size(buf), "%X" "\n", int(cp));
            os << buf;
        }
    });

    return r;
}

int main()
{
    try {
        std::cout << "Reading Unihan sources..." << std::flush;
        auto nonChinese = getNonChineseChars();
        std::cout << "OK, " << nonChinese.size() << " non-G chars" "\n";

        std::cout << "Reading BabelStone font..." << std::flush;
        auto bsr = readBabelStoneFont(nonChinese);
        std::cout << "OK, " << bsr.nGlyphs << " glyphs, "
                            << bsr.nNonChinese << " non-G there" "\n";
        return 0;
    } catch (const std::exception& e) {
        std::cout << "ERROR: " << e.what() << '\n';
        return 1;
    }
}
