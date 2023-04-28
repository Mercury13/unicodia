// My header
#include "sutton.h"

// STL
#include <iostream>
#include <fstream>

// Libs
#include "u_Strings.h"

// Project-local
#include "loader.h"
#include "library.h"
#include "UcFlags.h"

int sw::process()
{
    sw::Char swdata[sw::CLEN];
    int nLines = 0;

    { std::ifstream is(SUTTON_TXT);

        std::string s;
        while (std::getline(is, s)) {
            auto s1 = str::trimSv(s);
            if (s1.empty())
                continue;
            if (s1.starts_with('#'))
                continue;
            auto s2 = str::trimSv(str::prefixSv(s1, ';'));
            if (s2.empty())
                continue;
            auto strcps = str::splitSv(s2, ' ');
            // Get CPs
            SafeVector<char32_t> cps;
            cps.reserve(strcps.size());
            for (auto v : strcps) {
                cps.push_back(fromHex(v));
            }
            if (cps.size() > 3) {
                throw std::runtime_error("Line too long");
            }
            ++nLines;
        }
    }
    return nLines;
}
