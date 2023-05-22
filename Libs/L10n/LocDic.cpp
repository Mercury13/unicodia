#include "LocDic.h"

// C++
#include <fstream>

// Libs
#include "u_Strings.h"

loc::Dic loc::dic;


///// Text /////////////////////////////////////////////////////////////////////


std::u8string loc::Text::arg(std::u8string_view x) const
{
    std::u8string r = fSrc;
    str::replace(r, u8"{1}", x);
    return r;
}


std::u8string loc::Text::arg(int x) const
{
    std::u8string r = fSrc;
    char c[30];
    auto u8 = str::toCharsU8(c, x);
    str::replace(r, u8"{1}", u8);
    return r;
}


std::u8string loc::Text::arg(
        std::u8string_view x, std::u8string_view y) const
{
    std::u8string r = fSrc;
    str::replace(r, u8"{1}", x);
    str::replace(r, u8"{2}", y);
    return r;
}


std::u8string loc::Text::arg(
        std::u8string_view x, std::u8string_view y, std::u8string_view z) const
{
    std::u8string r = fSrc;
    str::replace(r, u8"{1}", x);
    str::replace(r, u8"{2}", y);
    str::replace(r, u8"{3}", z);
    return r;
}


///// Dic //////////////////////////////////////////////////////////////////////


void loc::Dic::add(std::string_view id, std::u8string translation)
{
    fMap.emplace(std::string{id}, std::move(translation));
}


void loc::Dic::dump(size_t maxSize)
{
    for (auto& v : fMap) {
        std::cout << v.first << " = " << v.second.c_str() << std::endl;
        if ((--maxSize) == 0)
            break;
    }
}


const loc::Text& loc::Dic::get(std::string_view id)
{
    auto& data = fMap[std::string{id}];
    if (!data.isFull()) {
        auto r = str::cat(u8'[', str::toU8sv(id), u8']');
        data = std::move(r);
    }
    return data;
}


const loc::Text* loc::Dic::getIf(std::string_view id) const
{
    auto it = fMap.find(std::string{id});
    if (it == fMap.end())
        return nullptr;
    return &it->second;
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
            line = str::trimRightSv(line);
            line = str::remainderSv(line, "[", "]");
            if (!line.empty()) {
                head = line;
                head.push_back('.');
            }
        } else {
            auto pEq = line.find('=');
            if (pEq != std::string_view::npos) {
                auto key = str::trimRightSv(line.substr(0, pEq));
                if (key.empty())
                    continue;

                // Get new ID
                str::catIp(newId, head, key);

                // Get value
                std::string_view value = line.substr(pEq + 1);

                    // —warn, actually value.find
                if (line.find('\\', pEq + 1) != std::string_view::npos) {
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
                    d.add(newId, newValue);
                } else {
                    // No need to unescape
                    d.add(newId, str::toU8(value));
                }
            }
        }
    }
}
