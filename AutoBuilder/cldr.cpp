#include "cldr.h"

#include "pugixml.hpp"
#include "mojibake.h"
#include "u_Strings.h"

#include "loader.h"
#include "UcCp.h"

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
        auto sType = hAnno.attribute("type").as_string();
        if (*sType != 0)
            continue;
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

        auto dest = r.emoji[cp];
        auto text = hAnno.text().as_string();
        auto texts = str::splitSv(text, '|');
        for (auto& v : texts) {
            // All are related to Japanese buttons
            if (v.starts_with("“") && v.ends_with("”"))
                continue;
            // Too many of these, just contaminate search
            if (v == "ideograph"
                    || v == "Japanese")
                continue;
            dest.names.emplace_back(v);
        }
    }

    return r;
}
