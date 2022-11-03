// My header
#include "egyptian.h"

// STL
#include <iostream>
#include <stdexcept>

// Libs
#include "u_Strings.h"

// Project-local
#include "loader.h"

// XML
#include "pugixml.hpp"

using namespace std::string_view_literals;

egyp::Base egyp::loadBase()
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

    Base r;
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
                    r[it->second] = desc;
                }
            } else {
                throw std::logic_error(str::cat("Cannot find sign <", id, ">"));
            }
        }
    }

    return r;
}
