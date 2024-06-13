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

    bool isCpGood(char32_t x)
    {
        return (x >= 0x02B0 && x <= 0x02FF)  // Spacing modifiers, all
            || (x >= 0x2010 && x <= 0x2027)  // Some punctuation, no spaces and marks
            || (x >= 0x2030 && x <= 0x205E)  // More punctuation, no spaces
            || (x >= 0x2070 && x <= 0x20CF)  // Super/sub, currencies
            || (x >= 0x2100 && x <= 0x27FF)  // Technical, control pics etc
            || (x >= 0x2900 && x <= 0x2BFF)  // Arrows etc
            || (x >= 0x2E00 && x <= 0x2E7F)  // Supp punctuation
            || (x >= 0x1CC00 && x <= 0x1CEBF)  // Legacy supp
            || (x >= 0x1F100 && x <= 0x1F2FF)  // Enclosed things
            || (x >= 0x1F780 && x <= 0x1F8FF)  // Shapes ex, arrows C
            || (x >= 0x1FB00 && x <= 0x1FBFF); // Legacy
    }

    void processMapping(Dic& dic, const std::filesystem::path& fname, uc::OldComp comp)
    {
        std::ifstream is(fname);
        if (!is.is_open()) {
            char buf[100];
            snprintf(buf, std::size(buf), "Cannot open file %s", fname.string().c_str());
            throw std::logic_error(buf);
        }

        std::string s;
        int iLine = 0;
        while (std::getline(is, s)) {
            ++iLine;
            std::string_view sv = str::trimRightSv(s);
            if (sv.empty())
                continue;
            // Get rid of comment
            if (auto pHash = sv.find('#'); pHash != std::string_view::npos) {
                sv = sv.substr(0, pHash);
                sv = str::trimRightSv(sv);
            }
            // Split line by TAB (TSV)
            auto data = str::splitSv(sv, '\t', false);
            if (data.size() < 2)
                continue;
            // Just check legacy code
            auto legacyCode = data[0];
            if (legacyCode != "?") {    // ? = IDK legacy code
                if (!legacyCode.starts_with("0x"))
                    throw std::logic_error(str::cat(
                            "File ", fname.string(), " line ", std::to_string(iLine),
                            ": legacy code <", legacyCode, "> should start with 0x"));
                legacyCode = legacyCode.substr(2);
                try {
                    fromHex(legacyCode);    // just check validity
                } catch (const std::exception& e) {
                    throw std::logic_error(str::cat(
                            "File ", fname.string(), " line ", std::to_string(iLine),
                            ": ", e.what()));
                }
            }
            // Decode Unicode
            auto sCodes = data[1];
            // Split codes by +
            auto codes = str::splitSv(sCodes, '+');
            for (auto code : codes) {
                if (code.starts_with('<'))
                    continue;
                if (!code.starts_with("0x"))
                    throw std::logic_error("Unicode should start with 0x");
                code = code.substr(2);
                char32_t q = fromHex(code);
                if (isCpGood(q)) {
                    dic[q] |= comp;
                }
            }
        }
    }

    #define OLDCOMP_CPP "UcAutoOldComp.cpp"

    struct OldSpan {
        Dic::const_iterator st, en;
        auto begin() const { return st; }
        auto end()   const { return en; }
        size_t nCps() const;
    };

    size_t OldSpan::nCps() const
    {
        if (st == en)
            return 0;
        auto en1 = en; --en1;
        return en1->first + 1 - st->first;
    }


    using OldSpans = std::vector<OldSpan>;

    [[nodiscard]] OldSpans splitIntoSpans(const Dic& dic)
    {
        if (dic.empty())
            throw std::logic_error("Old computer dictionary is empty!");

        constexpr int RANGE_GAP = 16;  // exceeds this gap → new range
        int oldCp = dic.begin()->first;
        Dic::const_iterator rangeStart = dic.begin();
        OldSpans r;
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
        "AMSTRAD_CPC",
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
        auto spans = splitIntoSpans(dic);
        std::ofstream os(OLDCOMP_CPP);
        if (!os.is_open()) {
            throw std::logic_error("Cannot open file " OLDCOMP_CPP);
        }

        os << "// Automatically generated, do not edit!"  "\n\n"
              R"(#include "UcAutoDefines.h")"  "\n\n"
              "using OC = uc::OldComp;"        "\n\n";

        char bufId[40];
        size_t iRange = 0;
        for (auto& spn : spans) {
            auto nCps = spn.nCps();
            if (nCps == 0)
                throw std::logic_error("Found empty range");

            writeId(bufId, iRange);
            os << "constinit const Flags<uc::OldComp> " << bufId << "[] {" "\n";

            auto oldCp = spn.begin()->first;
            for (auto& [cp, compMask] : spn) {
                if (compMask.empty())
                    throw std::logic_error("CP has no old computers");
                // Skipped flags
                for (char32_t skippedCp = oldCp + 1; skippedCp < cp; ++skippedCp) {
                    os << "OC::NONE";
                    writeTail(os, skippedCp);
                }
                bool hasAny = false;
                auto comps = compMask;
                while (comps) {
                    // Extract and remove bit
                    auto bit = comps.smallest();
                    comps.remove(bit);
                    // Turn bit to index
                    auto iBit = std::countr_zero(Flags<uc::OldComp>::toUnsignedStorage(bit));
                    // Write what we got
                    if (hasAny) {
                        os << " | ";
                    }
                    os << "OC::" << oldCompNames[iBit];
                    hasAny = true;
                }
                writeTail(os, cp);
                oldCp = cp;
            }
            os << "};" "\n";
            os << "static_assert(std::size(" << bufId << ") == "
                 << std::dec << spn.nCps() << R"(, "Autotool failed");)" "\n\n";
            ++iRange;
        }

        os << "constinit const uc::OldCompSpan uc::oldCompSpans[] {" "\n";
        iRange = 0;
        for (auto& spn : spans) {
            writeId(bufId, iRange);
            auto firstCp = spn.begin()->first;
            os << "{ .span = " << bufId << ", .firstCp = 0x"
                    << std::hex << static_cast<int>(firstCp) << "}," "\n";
            ++iRange;
        }
        os << "};" "\n"
              R"(static_assert(std::size(uc::oldCompSpans) == uc::N_OLDCOMP_SPANS, "Autotool failed");)" "\n";

        return spans.size();
    }

}   // anon namespace


old::Result old::process()
{
    Dic dic;
    for (auto& x : allOldComps()) {
        processMapping(dic, x.localName(), x.flag);
    }

    // Add teletext chars
    for (char32_t c = 0x1CE51; c <= 0x1CE8F; ++c) {
        dic[c] |= uc::OldComp::TELETEXT;
    }

    auto nSpans = outBase(dic);

    return {
        .nCps = static_cast<unsigned>(dic.size()),
        .nSpans = nSpans,
    };
}
