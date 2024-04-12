// My header
#include "textbase.h"

// STL
#include <fstream>

// Libs
#include "u_Strings.h"

// Project-local
#include "loader.h"
#include "ucdcom.h"
#include "data.h"

//#define DUMP_EQUALS

using namespace std::string_view_literals;

void tx::Cp::eraseName(std::string_view x)
{
    if (x.empty())
        return;

    if (auto it1 = names.find(x);
            it1 != names.end()) {
        names.erase(it1);
    }

    if (auto it2 = std::find(abbrs.begin(), abbrs.end(), x);
            it2 != abbrs.end()) {
        abbrs.erase(it2);
    }

    if (auto it3 = std::find(controls.begin(), controls.end(), x);
            it3 != controls.end()) {
        controls.erase(it3);
    }
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
            if (lat::isSingleCase(name)) {
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

    { std::ifstream is(UCD_ALIASES);
        std::string line;
        while (std::getline(is, line)) {
            std::string_view trimmed = str::trimSv(line);
            if (trimmed.empty() || trimmed.starts_with('#'))
                continue;
            auto names = str::splitSv(trimmed, ';', false);
            if (names.size() < 3)
                continue;
            std::string_view sCode = names.at(0);
            char32_t code = fromHex(sCode);
            std::string_view value = names.at(1);
            std::string_view sType = names.at(2);
            auto& entry = r[code];
            if (sType == "correction"sv || sType == "figment"sv) {
                entry.correction = decapitalize(value, code);
            } else if (sType == "abbreviation"sv) {
                entry.abbrs.emplace_back(value);
            } else if (sType == "control"sv || sType == "alternate"sv) {
                entry.controls.emplace_back(decapitalize(value, code));
            }
        }
    }

    { std::ifstream is(UCD_NAMES);
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
                    std::string_view sCode = names.safeGetV(0, ""sv);
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
    }

    // Clean
    for (auto& [cp,u] : r) {
        for (auto& v1 : u.controls) {
            u.names.erase(v1);
        }
    }

    return r;
}


tx::Scripts tx::loadScripts(const ucd::PropBase& propBase)
{
    tx::Scripts r;

    std::ifstream is(UCD_SCRIPTS);
    std::string line;
    while (std::getline(is, line)) {
        std::string_view trimmed = str::trimSv(line);
        if (trimmed.empty() || trimmed.starts_with('#'))
            continue;

        if (auto pHash = trimmed.find('#'); pHash != std::string_view::npos) {
            trimmed = trimmed.substr(0, pHash);
        }

        auto vals = str::splitSv(trimmed, ';', false);
        // 2 is minimum: range, script
        if (vals.size() < 2)
            continue;

        auto cps = vals.at(0);
        auto pair = ucd::Range::from(cps);
        auto name = propBase.shortenScript(vals.at(1));
        r.add(pair, name);
    }

    return r;
}


tx::Ages tx::loadAges()
{
    tx::Ages r;

    std::ifstream is(DER_AGE);
    std::string line;
    while (std::getline(is, line)) {
        std::string_view trimmed = str::trimSv(line);
        if (trimmed.empty() || trimmed.starts_with('#'))
            continue;

        if (auto pHash = trimmed.find('#'); pHash != std::string_view::npos) {
            trimmed = trimmed.substr(0, pHash);
        }

        auto vals = str::splitSv(trimmed, ';', false);
        // 2 is minimum: range, age
        if (vals.size() < 2)
            continue;

        auto cps = vals.at(0);
        auto pair = ucd::Range::from(cps);
        auto age = vals.at(1);
        r.add(pair, age);
    }

    return r;
}


tx::Props tx::loadProps()
{
    tx::Props r;

    { std::ifstream is(DER_CORE);
        std::string line;
        while (std::getline(is, line)) {
            std::string_view trimmed = str::trimSv(line);
            if (trimmed.empty() || trimmed.starts_with('#'))
                continue;

            if (auto pHash = trimmed.find('#'); pHash != std::string_view::npos) {
                trimmed = trimmed.substr(0, pHash);
            }

            auto vals = str::splitSv(trimmed, ';', false);
            if (vals.size() < 2)
                continue;

            auto range = ucd::Range::from(vals.at(0));
            auto prop = vals.at(1);
            if (prop == "Default_Ignorable_Code_Point"sv) {
                r.defaultIgnorable.add(range);
            }
        }
    }

    { std::ifstream is(UCD_PROP);
        std::string line;
        while (std::getline(is, line)) {
            std::string_view trimmed = str::trimSv(line);
            if (trimmed.empty() || trimmed.starts_with('#'))
                continue;

            if (auto pHash = trimmed.find('#'); pHash != std::string_view::npos) {
                trimmed = trimmed.substr(0, pHash);
            }

            auto vals = str::splitSv(trimmed, ';', false);
            if (vals.size() < 2)
                continue;

            auto range = ucd::Range::from(vals.at(0));
            auto prop = vals.at(1);
            if (prop == "Deprecated"sv) {
                r.deprecated.add(range);
            }
        }
    }

    return r;
}
