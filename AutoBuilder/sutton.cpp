// My header
#include "sutton.h"

// STL
#include <iostream>
#include <fstream>
#include <unordered_set>
#include <span>

// Libs
#include "u_Strings.h"

// Project-local
#include "loader.h"
#include "library.h"
#include "UcFlags.h"

namespace {

    SafeVector<char32_t> decodeCps(std::string_view line)
    {
        auto s1 = str::trimSv(line);
        if (s1.empty())
            return {};
        if (s1.starts_with('#'))
            return {};
        auto s2 = str::trimSv(str::prefixSv(s1, ';'));
        if (s2.empty())
            return {};
        auto strcps = str::splitSv(s2, ' ');
        if (strcps.size() > 3)
            throw std::runtime_error("Line too long");
        // Get CPs
        SafeVector<char32_t> cps;
        cps.reserve(strcps.size());
        for (auto v : strcps) {
            cps.push_back(fromHex(v));
        }
        return cps;
    }

    struct Combo {
        char32_t mainCp = 0;
        int fill = 0, rotation = 0;
    };

    Combo decodeCombo(std::span<char32_t> cps, std::string_view line)
    {
        Combo r;
        // Get main CP
        r.mainCp = cps[0];
        if (r.mainCp < sw::CMIN || r.mainCp > sw::CMAX)
            throw std::runtime_error("Wrong main codepoint");
        // Get fill
        size_t iCp = 1;
        if (iCp < cps.size()) {
            auto cpFill = cps[iCp];
            if (cpFill >= sw::F2 && cpFill <= sw::F6) {
                r.fill = cpFill - sw::VIRTUAL_F1;
                ++iCp;
            }
        }
        // Get rotation
        if (iCp < cps.size()) {
            auto cpRot = cps[iCp];
            if (cpRot >= sw::R2 && cpRot <= sw::R16) {
                r.rotation = cpRot - sw::VIRTUAL_R1;
                ++iCp;
            }
        }
        // Check the rest
        if (iCp < cps.size()) {
            std::string text = "Cannot process the entire line: ";
            text += line;
            throw std::runtime_error(text);
        }
        return r;
    }

    void removeLoner(sw::Char& c)
    {
        // Loner is a surely unused bit (0,0)
        if (c.fills[0] != 1)
            return;
        for (auto v : c.fills) {
            if (v > 1) {    // 0 and 1 do not make a loner
                c.fills[0] = 0;
                return;
            }
        }
    }

    bool isEqual(const sw::Char& x)
    {
        uint16_t sample = 0;
        for (auto& v : x.fills) {
            if (v != 0) {
                if (sample == 0) {
                    sample = v;
                } else if (v != sample) {
                    return false;
                }
            }
        }
        return true;
    }

    char32_t checkForEquality(const sw::Char (&swdata)[sw::CLEN])
    {
        for (int i = 0; i < sw::CLEN; ++i) {
            auto& d = swdata[i];
            if (!isEqual(d)) {
                return i + sw::CMIN;
            }
        }
        return 0;
    }

}   // anon namespace

sw::Result sw::process()
{
    sw::Char swdata[sw::CLEN];
    std::unordered_set<char32_t> usedCps;
    Result r;

    // Read text line by line
    { std::ifstream is(SUTTON_TXT);

        std::string line;
        while (std::getline(is, line)) {
            auto cps = decodeCps(line);
            if (cps.empty())
                continue;
            Combo combo = decodeCombo(cps, line);
            // Process
            auto index = combo.mainCp - sw::CMIN;
            if (index >= std::size(swdata))
                throw std::runtime_error("Out-of-range in swdata");
            auto& d = swdata[index];
            d.fills[combo.fill] |= (1 << combo.rotation);
            // Add to stats
            usedCps.insert(combo.mainCp);
            ++r.nLines;
        }
    }

    // Check data
    if (usedCps.size() != sw::NGOODCPS)
        throw std::logic_error("Not all CPs are touched");

    // Remove loners
    for (auto& v : swdata) {
        removeLoner(v);
    }

    // Check again
    r.firstInequal = checkForEquality(swdata);

    // Write data
    std::ofstream os("UcAutoSutton.cpp");
    os << "// Automatically generated, do not edit!" "\n";
    os << R"(#include "UcFlags.h")" "\n";
    os << "constinit const sw::Char sw::data[sw::CLEN] = {" "\n";

    for (int i = 0; i < sw::CLEN; ++i) {
        os << "{{";
        auto& d = swdata[i];
        for (auto& v : d.fills) {
            os << v << ",";
        }
        os << "}},  // " << std::hex << (i + sw::CMIN) << std::dec << '\n';
    }

    os << "};" "\n";

    return r;
}
