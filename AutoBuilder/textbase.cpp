// My header
#include "textbase.h"

// STL
#include <fstream>

// Libs
#include "u_Strings.h"

// Project-local
#include "loader.h"
#include "library.h"    // for fromHex
#include "data.h"

using namespace std::string_view_literals;

void tx::Cp::eraseName(std::string_view x)
{
    auto it = names.find(x);
    if (it != names.end())
        names.erase(it);
}

namespace {

    void extractSynonym(tx::Base& base, char32_t currChar, std::string_view line)
    {
        auto names = str::splitSv(line, ',', true);
        if (names.empty())
            return;
        auto& cp = base[currChar];
        for (auto name : names) {
            // We do not check for abbreviation
            if (currChar <= ' '             // control/space
                    || (currChar >= 0x7F && currChar <= 0x9F)   // another control range
                    || name.ends_with("(1.0)")
                    || name.ends_with("(1.1)")) {
                name = str::prefixSv(name, '(');
            }
            name = str::trimSv(name);
            if (name == "etc."sv
                    || name == "..."sv)
                continue;
            std::string decap;
            if (str::latIsSingleCase(name)) {
                decap = decapitalize(name, currChar);
            } else {
                decap = decapitalizeByTable(name);
            }
            cp.names.emplace(std::move(decap));
        }
    }

}   // anon namespace

tx::Base tx::loadBase()
{
    tx::Base r;

    std::ifstream is(UCD_NAMES);
    std::string line;
    char32_t currChar = 0;
    while (std::getline(is, line)) {
        std::string_view trimmed = str::trimSv(line);
        if (trimmed.empty() || trimmed.starts_with(';'))
            continue;
        if (std::to_address(line.begin()) == std::to_address(trimmed.begin())) {
            // Character
            if (trimmed.starts_with('@')) {
                // Subtitle, do nothing
            } else {
                std::string_view sCode = str::prefixSv(trimmed, '\t');
                currChar = fromHex(sCode);
            }
        } else {
            // Its info
            switch (trimmed[0]) {
            case '=':
                extractSynonym(r, currChar, trimmed.substr(1));
                break;
            default: ;
            }
        }
    }

    return r;
}
