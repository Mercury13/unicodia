#include "library.h"

#include "u_Strings.h"

using namespace std::string_view_literals;


unsigned fromHex(std::string_view x)
{
    long long r;
    auto beg = x.data();
    auto end = beg + x.length();
    auto res = std::from_chars(beg, end, r, 16);
    if (res.ec != std::errc() || res.ptr != end) {
        std::string errm = "[fromChars] Cannot parse hex ";
        errm.append(x);
        throw std::invalid_argument(errm);
    }
    return r;
}


lib::EmojiData lib::loadEmoji(const char* fname)
{
    std::ifstream is(fname);
    if (!is.is_open())
        throw std::logic_error(std::string("Cannot open ") + fname);

    Fix1d<char32_t, 20> codes;

    EmojiData r;
    std::string s;
    while (std::getline(is, s)) {
        std::string_view mainLine{str::trimSv(s)}, qualType{}, comment{};
        if (auto pPound = mainLine.find('#');
                pPound != std::string_view::npos) {
            comment = str::trimSv(mainLine.substr(pPound + 1));
            mainLine = str::trimSv(mainLine.substr(0, pPound));
        }
        if (auto pColon = mainLine.find(';');
                pColon != std::string_view::npos) {
            qualType = str::trimSv(mainLine.substr(pColon + 1));
            mainLine = str::trimSv(mainLine.substr(0, pColon));
        }
        auto hexCodes = str::splitSv(mainLine, ' ');
        auto nCodes = hexCodes.size();
        for (size_t i = 0; i < nCodes; ++i) {
            codes[i] = fromHex(hexCodes[i]);
        }
        if (nCodes == 2 && qualType == "fully-qualified"sv && codes[1] == 0xFE0F) {
            r.vs16.insert(codes[0]);
        }
    }
    return r;
}
