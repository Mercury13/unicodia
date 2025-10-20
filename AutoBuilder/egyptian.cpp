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

    std::u32string parseCpSequence(std::string_view x)
    {
        std::u32string s;
        auto vals = str::splitSv(x, ' ', true);
        s.reserve(vals.size());
        for (auto& v : vals) {
            auto cp = fromHex(v);
            s += char32_t(cp);
        }
        return s;
    }

    std::string simplifyIndex(std::string_view x)
    {
        if (x.starts_with("HJ "sv)) {
            x = x.substr(3);
        }
        if (x.find(' ') != std::string_view::npos) {
            throw std::logic_error(str::cat(
                "Egyptian index <", x, "> somehow contains spaces"));
        }
        std::string r;
        const char* p = std::to_address(x.begin());
        const char* const end = std::to_address(x.end());
        // 1. First letter
        if (p == end)
            throw std::logic_error(str::cat(
                "Egyptian index <", x, "> has nothing in front"));
        auto firstLetter = *(p++);
        if (!std::isalpha(firstLetter)) {
            throw std::logic_error(str::cat(
                "Egyptian index <", x, "> has no letters in front"));
        }
        r += lat::toUpper(firstLetter);
        // 2. Rest letters
        for (; p != end && std::isalpha(*p); ++p)
            r += std::tolower(*p);
        // 3. Heading zeros
        for (; p != end && *p == '0'; ++p) ;
        // 4. Digits
        auto p1 = p;
        for (; p != end && std::isdigit(*p); ++p) {
            r += *p;
        }
        if (p1 == p) {
            throw std::logic_error(str::cat(
                "Egyptian index <", x, "> has no digits or is 000"));
        }
        // 4. Tail
        for (; p != end && std::isalpha(*p); ++p) {
            r += std::tolower(*p);
        }
        if (p != end) {
            throw std::logic_error(str::cat(
                "Egyptian index <", x, "> is no alpha-d1g1t5-alpha"));
        }
        return r;
    }

    void repl(std::string& x)
    {
        str::replace(x, "\uA7BD"sv, "ĭ"sv);
    }

    void loadUnikemet(egyp::Base& r)
    {
        for (char32_t c = 0x13430; c <= 0x1345F; ++c) {
            auto& q = r[c];
            q.reliability = uc::EgypReliability::SPECIAL;
        }

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
            if (sField == "kEH_Desc"sv) {
                auto& du = r[cp].descUnicode;
                du = sValue;
                // Known typo
                str::replace(du, " syrian ", " Syrian ");
                // No need period at end
                if (du.ends_with('.'))
                    du.pop_back();
                repl(du);
            } else if (sField == "kEH_Core"sv) {
                auto& en = r[cp];
                if (sValue == "C"sv) {
                    en.reliability = uc::EgypReliability::CORE;
                } else if (sValue == "L"sv) {
                    en.reliability = uc::EgypReliability::LEGACY;
                } else {
                    throw std::logic_error(str::cat(
                        "Unknown Egyp reliability level '", sValue, '\''));
                }
            } else if (sField == "kEH_AltSeq"sv) {
                auto& en = r[cp];
                en.equivSequence = parseCpSequence(sValue);
            } else if (sField == "kEH_Func"sv) {
                auto& en = r[cp];
                en.meaning = sValue;
                repl(en.meaning);
            } else if (sField == "kEH_FVal"sv) {
                auto& en = r[cp];
                en.pronunciation = sValue;
                repl(en.pronunciation);
            } else if (sField == "kEH_UniK"sv) {
                auto& en = r[cp];
                en.index = simplifyIndex(sValue);
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
