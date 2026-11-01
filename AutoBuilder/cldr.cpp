#include "cldr.h"

#include <algorithm>

#include "pugixml.hpp"
#include "mojibake.h"
#include "u_Strings.h"

#include "loader.h"
#include "UcCp.h"

void cl::EmojiEntry::eraseName(std::string_view text)
{
    auto where = std::find(names.begin(), names.end(), text);
    if (where != names.end()) {
        names.erase(where);
    }
}

cl::EmojiEntry* cl::Base::findEmoji(char32_t c)
{
    auto it = emoji.find(c);
    return (it == emoji.end()) ? nullptr : &it->second;
}

cl::EmojiEntry& cl::Base::findEmojiOr(char32_t c, EmojiEntry& fallback)
{
    auto it = emoji.find(c);
    return (it == emoji.end()) ? fallback : it->second;
}

namespace {

    bool containsWord(std::string_view haystack, std::string_view needle)
    {
        auto pos = haystack.find(needle);
        if (pos == std::string_view::npos)
            return false;
        if (pos == 0)
            return true;
        switch (haystack[pos - 1]) {
        case ' ':
        case '-':
            return true;
        default:
            return false;
        }
    }

}   // anon namespace

cl::Base cl::load()
{
    pugi::xml_document doc;
    auto res = doc.load_file(ANNO_EN);
    if (!res)
        throw std::logic_error("Cannot load CLDR annotations");

    cl::Base r;

    auto hRoot = doc.root();
    auto hRoot1 = hRoot.child("ldml");
    auto hAnnos = hRoot1.child("annotations");
    for (auto hAnno : hAnnos.children("annotation")) {
        char32_t cp = 0;
        auto u8Cp = hAnno.attribute("cp").as_string();
        auto u32Cp = mojibake::toM<std::u32string>(u8Cp);
        if (u32Cp.find(cp::MOJIBAKE) != std::u32string::npos)
            throw std::logic_error("Mojibake found in CLDR");

        switch (u32Cp.length()) {
        case 2:
            if (u32Cp[1] != cp::VS16)
                continue;
            [[fallthrough]];
        case 1:
            cp = u32Cp[0];
            break;
        default:
            continue;
        }

        auto& dest = r.emoji[cp];
        auto text = hAnno.text().as_string();
        auto texts = str::splitSv(text, '|');
        for (auto& v : texts) {
            // All are related to Japanese buttons
            if (v.starts_with("“") && v.ends_with("”"))
                continue;
            // Too many of these, just contaminate search
            if (v == "ideograph"
                    || v == "Japanese"
                    || v == "eye"
                    || v == "face"
                    || v == "pickle")  // old CLDR
                continue;
            dest.names.emplace_back(v);
        }
    }

    // Erase substrings (they do nothing to search)
    for (auto& [_, v] : r.emoji) {
        // Go back, it’s simpler
        for (size_t iSmall = v.names.size(); iSmall > 0; ) { --iSmall;
            auto small = v.names[iSmall];
            for (size_t iBig = 0; iBig < v.names.size(); ++iBig) {
                if (iBig == iSmall)
                    continue;
                auto big = v.names[iBig];
                if (containsWord(big, small)) {
                    v.names.erase(v.names.begin() + iSmall);
                    break;
                }
            }
        }
    }

    return r;
}
