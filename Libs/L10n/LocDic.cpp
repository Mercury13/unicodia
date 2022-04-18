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

    std::string head;

    while (std::getline(is, s)) {
        std::string_view line = s;
        line = str::trimSv(line);
        auto p = s.begin();
        // Trim left
        while (p != s.end() && *p <= ' ')
            ++p;
        // Comment?
        if (p == s.end() || *p == ';' || *p == '#')
            continue;
        //
    }
}
