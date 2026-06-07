#include <iostream>
#include <filesystem>

#include "Xsvg.h"

constexpr const char* FNAME_IN = "../NotoEmoji/Remakes/Flags/A/1f1e6-1f1e8.svg";
constexpr const char* FNAME_OUT = "opt.svg";
constexpr int MAX_UPPER = 6;

std::filesystem::path findFile(const std::filesystem::path& path)
{
    if (std::filesystem::exists(path))
        return path;
    auto t = path;
    for (int i = 0; i < MAX_UPPER; ++i) {
        t = ".." / t;
        if (std::filesystem::exists(t))
            return t;
    }
    throw std::logic_error("Cannot find " + path.string());
}

int main()
{
    auto pathIn = findFile(FNAME_IN);
    std::cout << "Found " << pathIn.string() << '\n';

    xs::Svg svg;
    svg.loadFile(pathIn);

    xs::SaveSets sets;
    svg.saveFile(FNAME_OUT, sets);

    return 0;
}
