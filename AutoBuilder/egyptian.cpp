// My header
#include "egyptian.h"

// STL
#include <fstream>
#include <stdexcept>

// Libs
#include "u_Strings.h"

// Project-local
#include "loader.h"

// XML
#include "pugixml.hpp"

// Unicode
#include "ucdcom.h"

using namespace std::string_view_literals;

namespace {

    void loadDutchBase(egyp::Base& r)
    {
        std::unordered_map<std::string, char32_t> mNameToUnicode;

        pugi::xml_document doc;
        { doc.load_file(EGYP_UNICODE);
            auto hRoot = doc.root().child("signlist");

            for (auto h : hRoot.children("sign")) {
                auto id = h.attribute("id").as_string();
                auto code = h.attribute("code").as_int();
                mNameToUnicode[id] = code;
            }
        }

        { doc.load_file(EGYP_DESCRIPTION);
            auto hRoot = doc.root().child("signlist");
            std::string desc;

            for (auto h : hRoot.children("sign")) {
                auto id = h.attribute("id").as_string();
                if (auto it = mNameToUnicode.find(id); it != mNameToUnicode.end()) {
                    if (auto hDesc = h.child("descr")) {
                        desc.clear();
                        for (auto part : hDesc.children()) {
                            switch (part.type()) {
                            case pugi::node_cdata:
                            case pugi::node_pcdata:
                                desc += part.value();
                                break;
                            case pugi::node_element:
                                if (part.name() == "signref"sv) {
                                    auto name = part.attribute("name").as_string();
                                    desc += name;
                                    if (auto it2 = mNameToUnicode.find(name); it2 != mNameToUnicode.end()) {
                                        char buf[20];
                                        snprintf(buf, std::size(buf), " (U+%04X)", int(it2->second));
                                        desc += buf;
                                    }
                                } else if (part.name() == "al"sv) {
                                    for (auto part2 : part.children()) {
                                        switch (part2.type()) {
                                        case pugi::node_cdata:
                                        case pugi::node_pcdata:
                                            desc += part2.value();
                                            break;
                                        default:
                                            throw std::logic_error("Unknown child in Egyptian AL");
                                        }
                                    }
                                } else {
                                    throw std::logic_error(
                                        str::cat("Unknown tag <", part.name(), "> in Egyptian base"));
                                }
                                break;
                            default:
                                throw std::logic_error("Unknown child in Egyptian description");
                            }
                        }

                        str::replace(desc, '\r', ' ');
                        str::replace(desc, '\n', ' ');
                        str::replace(desc, "  ", " ");
                        desc = str::trim(desc);
                        r[it->second].descEwp = desc;
                    }
                } else {
                    throw std::logic_error(str::cat("Cannot find sign <", id, ">"));
                }
            }
        }
    }

    void loadUnikemet(egyp::Base& r)
    {
        std::ifstream is(UCD_UNIKEMET);
        if (!is.is_open())
            throw std::logic_error("Cannot open Unikemet base!");
        for (std::string rawLine; std::getline(is, rawLine); ) {
            std::string_view line = str::trimSv(rawLine);
            if (line.empty() || line.starts_with('#'))
                continue;
            auto d = str::splitSv(line, '\t', false);
            if (d.size() < 3)
                throw std::logic_error("Unikemet line too short");
            auto sUnicode = d[0];
            if (!sUnicode.starts_with("U+"))
                throw std::logic_error("Unikemet line should start with 'U+'");
            sUnicode = sUnicode.substr(2);
            auto cp = fromHex(sUnicode);
            auto sField = d[1];
            auto sValue = d[2];
            if (sField == "kEH_Desc") {
                auto& du = r[cp].descUnicode;
                du = sValue;
                str::replace(du, " syrian ", " Syrian ");
            }
        }
    }
}

egyp::Base egyp::loadBase()
{
    Base r;
    loadDutchBase(r);
    loadUnikemet(r);
    return r;
}
