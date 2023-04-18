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

//#define DUMP_EQUALS

using namespace std::string_view_literals;

void tx::Cp::eraseName(std::string_view x)
{
    auto it = names.find(x);
    if (it != names.end())
        names.erase(it);
}

namespace {

#ifdef DUMP_EQUALS
    std::ofstream osEqual("!equal.log");
#endif

    void extractSynonym(
            tx::Base& base, char32_t currChar,
            std::string_view mainName,
            std::string_view line)
    {
    #ifdef DUMP_EQUALS
        if (line.find(',') != std::string_view::npos) {
            osEqual << line << std::endl;
        }
    #endif

        switch (currChar) {
            // Do not collect synonyms
        case 0xFEFF:    // those abbreviations are just repeated
            return;
        default:
            break;
        }

        // Is synonym extraneous?
        line = str::trimSv(line);
        auto up = str::toUpper(line);
        if (str::containsWord(mainName, up))
            return;

        Flags<Dcfg> flags;
        if (str::containsWord(mainName, "CAPITAL")) {
            flags |= Dcfg::UPCASE;
        } else if (str::containsWord(mainName, "SMALL")) {
            flags |= Dcfg::LOCASE;
        }

        auto convertName = [currChar, flags](std::string_view name) -> std::string
        {
            // We do not check for abbreviation
            if (currChar <= ' '             // control/space
                || (currChar >= 0x7F && currChar <= 0x9F)   // another control range
                || name.ends_with("(1.0)")
                || name.ends_with("(1.1)")) {
                name = str::prefixSv(name, '(');
            }
            name = str::trimSv(name);
            if (name == "etc."sv || name == "..."sv)
                return {};
            if (str::latIsSingleCase(name)) {
                return decapitalize(name, currChar, flags);
            } else {
                return decapitalize(name, currChar, flags | Dcfg::SHORTCUT);
            }
        };

        auto& cp = base[currChar];
        if (line.ends_with(';')   // “ISOtech entity &iinfin;”
                || str::containsWord(mainName, "ANATOLIAN")) {
            // The entire line
            cp.names.insert(std::string{line});
        } else {
            // Convert to upper one by one, then stick together
            auto names = str::splitByAnySv(line, ",;", true);
            if (names.empty())
                return;
            std::string r;
            for (auto name : names) {
                auto conv = convertName(name);
                if (!conv.empty()) {
                    if (!r.empty())
                        r += ", ";
                    r += conv;
                }
            }

            cp.names.insert(r);
        }
    }

}   // anon namespace

tx::Base tx::loadBase()
{
    tx::Base r;

    std::ifstream is(UCD_NAMES);
    std::string mainName, line;
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
                auto names = str::splitSv(trimmed, '\t');
                std::string_view sCode = names.safeGetV(0, "");
                currChar = fromHex(sCode);
                mainName = names.safeGetV(1, "");
            }
        } else {
            // Its info
            switch (trimmed[0]) {
            case '%':   // character correction
                mainName = str::trim(trimmed.substr(1));
                break;
            case '=':
                extractSynonym(r, currChar, mainName, trimmed.substr(1));
                break;
            default: ;
            }
        }
    }

    return r;
}
