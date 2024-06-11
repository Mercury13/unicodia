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

    #define OLDCOMP_CPP "UcAutoOldComp.cpp"

    struct OldRange {
        Dic::const_iterator st, en;
        auto begin() const { return st; }
        auto end()   const { return en; }
        size_t nCps() const;
    };

    size_t OldRange::nCps() const
    {
        if (st == en)
            return 0;
        auto en1 = en; --en1;
        return en1->first + 1 - st->first;
    }


    using OldRanges = std::vector<OldRange>;

    [[nodiscard]] OldRanges splitIntoRanges(const Dic& dic)
    {
        if (dic.empty())
            throw std::logic_error("Old computer dictionary is empty!");

        constexpr int RANGE_GAP = 16;  // exceeds this gap → new range
        int oldCp = dic.begin()->first;
        Dic::const_iterator rangeStart = dic.begin();
        OldRanges r;
        auto finishRange = [&](Dic::const_iterator x) {
            r.emplace_back(rangeStart, x);
        };
        for (auto it = dic.begin(); it != dic.end(); ++it) {
            auto cp = it->first;
            if (cp - oldCp > RANGE_GAP) {
                finishRange(it);
                rangeStart = it;
            }
            oldCp = cp;
        }
        finishRange(dic.end());
        return r;
    }

    void writeId(char (&buf)[40], unsigned i)
    {
        snprintf(buf, std::size(buf), "oldCompRange_%u", i);
    }

    constinit const char* oldCompNames[] {
        "AMSTRAD",
        "APPLE",
        "AQUARIUS",
        "ATARI_ST",
        "ATARI_8BIT",
        "MINITEL",
        "MSX",
        "ORIC",
        "COMMODORE",
        "RISC_OS",
        "SINCLAIR",
        "TELETEXT",
        "TANDY_TRS80",
        "TANDY_COCO",
        "KORVET",
        "SMALLTALK",
        "SHARP_MZ",
        "OHIO_SCIENTIFIC",
        "ROBOTRON",
        "HP_TERMINAL",
        "KAYPRO",
    };

    constexpr auto I_LAST_OLD_COMP = std::size(oldCompNames) - 1;
    static_assert((1 << I_LAST_OLD_COMP) == static_cast<int>(uc::OldComp::LAST));

    void writeTail(std::ostream& os, int cp)
    {
        os << ",  // " << std::hex << static_cast<int>(cp) << '\n';
    }

    [[nodiscard]] unsigned outBase(const Dic& dic)
    {
        auto ranges = splitIntoRanges(dic);
        std::ofstream os(OLDCOMP_CPP);
        if (!os.is_open()) {
            throw std::logic_error("Cannot open file " OLDCOMP_CPP);
        }

        os << "// Automatically generated, do not edit!"  "\n\n"
              R"(#include "UcAutoDefines.h")"  "\n\n"
              "using OC = uc::OldComp;"        "\n\n";

        char bufId[40];
        size_t iRange = 0;
        for (auto& rng : ranges) {
            auto nCps = rng.nCps();
            if (nCps == 0)
                throw std::logic_error("Found empty range");

            writeId(bufId, iRange);
            os << "constinit const Flags<uc::OldComp> " << bufId << "[] {" "\n";

            auto oldCp = rng.begin()->first;
            for (auto& [cp, compMask] : rng) {
                if (compMask.empty())
                    throw std::logic_error("CP has no old computers");
                // Skipped flags
                for (char32_t skippedCp = oldCp + 1; skippedCp < cp; ++skippedCp) {
                    os << "OC::NONE";
                    writeTail(os, skippedCp);
                }
                size_t bit = 0;
                auto numMask = compMask.numeric();
                bool hasAny = false;
                for (; numMask != 0;
                         numMask >>=1, ++bit) {
                    if (numMask & 1) {
                        if (hasAny) {
                            os << " | ";
                        }
                        os << "OC::" << oldCompNames[bit];
                        hasAny = true;
                    }
                }
                writeTail(os, cp);
                oldCp = cp;
            }
            os << "};" "\n";
            os << "static_assert(std::size(" << bufId << ") == "
                 << std::dec << rng.nCps() << R"(, "Autotool failed");)" "\n\n";
            ++iRange;
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
