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

    struct BigChar {
        uint16_t fills[sw::N_FILL] { 0, 0, 0, 0, 0, 0 };

        constexpr bool operator == (const BigChar& x) const noexcept = default;
        [[maybe_unused]] bool hasFill(unsigned i) const;
        [[maybe_unused]] bool hasRotation(unsigned i) const;

        /// @return  mask of used rotations: 5 = used rotations ⌀ and R3
        uint16_t usedRotations() const;
        /// @return  mask of used fills: 5 = used fills ⌀ and F3
        uint8_t usedFills() const;

        /// Removes a loner (char w/o fill)
        /// if other rotations are larger
        void removeLoner();

        /// @return [+] all used fills have equal set of rotations
        ///         (=all used rotations have equal set of fills)
        ///      e.g. 0, 5, 0, 5, 0, 0 → true
        ///      e.g. 0, 5, 0, 6, 0, 0 → false
        bool isEqual() const;
    };

    bool BigChar::hasFill(unsigned i) const
    {
        assert(i < sw::N_FILL);
        return fills[i];
    }

    bool BigChar::hasRotation(unsigned i) const
    {
        assert(i < sw::N_ROT);
        const uint16_t flag = 1 << i;
        for (auto& v : fills) {
            if (v & flag)
                return true;
        }
        return false;
    }

    uint16_t BigChar::usedRotations() const
    {
        uint16_t r = 0;
        for (auto& v : fills)
            r |= v;
        return r;
    }

    uint8_t BigChar::usedFills() const
    {
        uint8_t r = 0;
        for (int i = 0; i < sw::N_FILL; ++i) {
            if (fills[i] != 0) {
                r |= (1 << i);
            }
        }
        return r;
    }

    void BigChar::removeLoner()
    {
        // Loner is a surely unused bit (0,0)
        if (fills[0] != 1)
            return;
        for (auto v : fills) {
            if (v > 1) {    // 0 and 1 do not make a loner
                fills[0] = 0;
                return;
            }
        }
    }

    bool BigChar::isEqual() const
    {
        uint16_t sample = 0;
        for (auto& v : fills) {
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

    char32_t checkForEquality(const BigChar (&swdata)[sw::CLEN])
    {
        for (int i = 0; i < sw::CLEN; ++i) {
            auto& d = swdata[i];
            if (!d.isEqual()) {
                return i + sw::CMIN;
            }
        }
        return 0;
    }

}   // anon namespace

sw::Result sw::process()
{
    BigChar swdata[sw::CLEN];
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
        v.removeLoner();
    }

    // Check again
    r.firstInequal = checkForEquality(swdata);

    // Write data
    std::ofstream os("UcAutoSutton.cpp");
    os << "// Automatically generated, do not edit!" "\n";
    os << R"(#include "UcFlags.h")" "\n";
    os << "constinit const sw::Char sw::data[sw::CLEN] = {" "\n";

    for (int i = 0; i < sw::CLEN; ++i) {
        auto& d = swdata[i];
        os << "{.rot=" << d.usedRotations() << ",.fill=" << int(d.usedFills())
           << "},  // " << std::hex << (i + sw::CMIN) << std::dec << '\n';
    }

    os << "};" "\n";

    return r;
}
