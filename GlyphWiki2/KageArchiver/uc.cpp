// My header
#include "uc.h"

// C++
#include <fstream>

// Libs
#include "u_Strings.h"

using namespace std::string_view_literals;

UnicodeList readUnicodeList()
{
    UnicodeList r;
    r.rehash(140'000);

    std::ifstream is("Unihan_IRGSources.txt");
    if (!is.is_open())
        throw std::logic_error("Cannot find 'Unihan_IRGSources.txt', should be in 'MiscFiles/RawData'");

    std::string s;
    char buf[200];
    while (std::getline(is, s)) {
        auto line = str::trimSv(s);
        if (line.empty() || line.starts_with('#'))
            continue;
        auto cols = str::splitSv(line, '\t', false);
        if (cols.size() != 3)
            throw BadUnicode("Line should consists of exactly 3 cols");
        auto sCp = cols[0];
        if (!sCp.starts_with("U+")) {
            snprintf(buf, std::size(buf),
                     "Codepoint '%.*s' should start with 'U+'",
                     PRF_SV(sCp));
            throw BadUnicode(buf);
        }
        auto sCp1 = sCp.substr(2);
        unsigned cp;
        auto q = str::fromChars(sCp1, cp, 16);
        if (q.ec != std::errc{}) {
            snprintf(buf, std::size(buf),
                     "Codepoint '%.*s' is not valid hex",
                     PRF_SV(sCp));
            throw BadUnicode(buf);
        }

        auto sKey = cols[1];

        static constexpr std::string_view HEAD = "kIRG_";
        static constexpr unsigned HEAD_L = HEAD.length();
        if (sKey.starts_with(HEAD)) {
            CountryFg country = CountryFg::DUMMY;
            auto tail = sKey.substr(HEAD_L);
            if (tail == "GSource"sv) {
                country = CountryFg::CHINA;
            } else if (tail == "HSource"sv) {
                country = CountryFg::HONGKONG;
            } else if (tail == "JSource"sv) {
                country = CountryFg::JAPAN;
            } else if (tail == "KPSource"sv) {
                country = CountryFg::KOREA_N;
            } else if (tail == "KSource"sv) {
                country = CountryFg::KOREA;
            } else if (tail == "MSource"sv) {
                country = CountryFg::MACAO;
            } else if (tail == "SSource"sv) {
                country = CountryFg::DAIZO;
            } else if (tail == "TSource") {
                country = CountryFg::TAIWAN;
            } else if (tail == "UKSource") {
                country = CountryFg::WESTERN;
            } else if (tail == "USource") {
                country = CountryFg::CONSORTIUM;
            } else if (tail == "VSource") {
                country = CountryFg::VIETNAM;
            } else {
                snprintf(buf, std::size(buf),
                         "Unknown property '%.*s'", PRF_SV(sKey));
            }
            auto& v = r[cp];
            v.countries |= country;
        }
    }

    return r;
}
