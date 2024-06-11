// My header
#include "legacy.h"

// STL
#include <map>
#include <fstream>

// Libs
#include "u_Strings.h"

// Unicode
#include "UcFlags.h"

// Project-local
#include "loader.h"
#include "ucdcom.h"


namespace {

    using Fgs = Flags<uc::OldComp>;

    using Dic = std::map<char32_t, Fgs>;

    void processMapping(Dic& dic, const char* fname, uc::OldComp comp)
    {
        std::ifstream is(fname);
        if (!is.is_open()) {
            char buf[100];
            snprintf(buf, std::size(buf), "Cannot open file %s", fname);
            throw std::logic_error(buf);
        }

        std::string s;
        while (std::getline(is, s)) {
            std::string_view sv = str::trimRightSv(s);
            if (sv.empty())
                continue;
            // Get rid of comment
            if (auto pHash = sv.find('#'); pHash != std::string_view::npos) {
                sv = sv.substr(0, pHash);
                sv = str::trimRightSv(s);
            }
            // Split line by TAB (TSV)
            auto data = str::splitSv(s, '\t', false);
            if (data.size() < 2)
                continue;
            // We don’t heed legacy code, just Unicode
            auto sCodes = data[1];
            // Split codes by +
            auto codes = str::splitSv(sCodes, '+');
            for (auto code : codes) {
                if (!code.starts_with("0x"))
                    continue;
                code = code.substr(2);
                char32_t q = fromHex(code);
                dic[q] |= comp;
            }
        }
    }

    #define OLDCOMP_CPP "UcOldComp.cpp"

    struct OldRange {
        Dic::const_iterator start, end;
    };

    using OldRanges = std::vector<OldRange>;

    [[nodiscard]] OldRanges splitIntoRanges(const Dic& dic)
    {
        constexpr int RANGE_GAP = 16;  // exceeds this gap → new range
        int oldCp = -10000;
        Dic::const_iterator rangeStart = dic.begin();
        OldRanges r;
        auto finishRange = [&](Dic::const_iterator x) {
            r.emplace_back(rangeStart, x);
        };
        for (auto it = dic.begin(); it != dic.end(); ++it) {
            auto cp = it->first;
            if (cp - oldCp > RANGE_GAP) {
                finishRange(it);
                oldCp = cp;
                rangeStart = it;
            }
        }
        finishRange(dic.end());
        return r;
    }

    [[nodiscard]] unsigned outBase(const Dic& dic)
    {
        auto ranges = splitIntoRanges(dic);
        std::ofstream os(OLDCOMP_CPP);
        if (!os.is_open()) {
            throw std::logic_error("Cannot open file " OLDCOMP_CPP);
        }
        return ranges.size();
    }

}   // anon namespace


old::Result old::process()
{
    Dic dic;
    processMapping(dic, OLD_APPLE2_1, uc::OldComp::APPLE);
    processMapping(dic, OLD_APPLE2_2, uc::OldComp::APPLE);
    processMapping(dic, OLD_APPLE2_3, uc::OldComp::APPLE);
    processMapping(dic, OLD_APPLE2_4, uc::OldComp::APPLE);

    auto nRanges = outBase(dic);

    return {
        .nCps = static_cast<unsigned>(dic.size()),
        .nRanges = nRanges,
    };
}
