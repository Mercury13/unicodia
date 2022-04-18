#include "LocDic.h"

// C++
#include <fstream>

// Libs
#include "u_Strings.h"

loc::Dic loc::dic;


///// Dic //////////////////////////////////////////////////////////////////////


void loc::Dic::add(std::string_view id, std::u8string translation)
{
    fMap.emplace(std::string{id}, std::move(translation));
}


///// Misc loaders /////////////////////////////////////////////////////////////


void loc::loadIni(Dic& d, const std::filesystem::path& path)
{
    d.clear();
    std::ifstream is(path);
    std::string s;

    std::string head;       // = empty
    std::string newId;

    while (std::getline(is, s)) {
        auto line = str::trimLeftSv(s);
        // Empty line?
        if (line.empty())
            continue;
        auto firstChar = line.front();
        // Comment?
        if (firstChar == ';' || firstChar == '#')
            continue;
        // Head
        if (firstChar == '[') {
            line = line.substr(1);
            if (line.ends_with(']'))
                line = line.substr(line.length() - 1);
            head = line;
            head.push_back('.');
        } else {
            auto pEq = line.find('=');
            if (pEq != std::string_view::npos) {
                auto key = str::trimRightSv(line.substr(0, pEq));
                if (key.empty())
                    continue;

                // Get new ID
                newId = head;
                newId.append(key);

                // Get value
                auto value = line.substr(pEq + 1);

                if (value.find('\\') != std::string_view::npos) {
                    // Unescape
                    std::u8string newValue;
                    newValue.reserve(value.length());   // string will be shorter, not longer
                    auto end = value.end();
                    for (auto p = value.begin(); p != end; ++p) {
                        auto c = *p;
                        if (c != '\\') {
                            newValue.push_back(c);
                        } else {
                            if ((++p) == end)
                                break;

                            switch (c = *p) {
                            case 'n':
                            case 'r':
                                c = '\n';
                                break;
                            case 's':
                                c = ' ';
                                break;
                            case 't':
                                c = '\t';
                                break;
                            default:;
                                break;
                            }
                            newValue.push_back(c);
                        }
                    }
                    d.add(key, newValue);
                } else {
                    // No need to unescape
                    d.add(key, str::toU8(value));
                }
            }
        }
    }
}
