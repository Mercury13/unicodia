#include <iostream>
#include <filesystem>

#include "Xsvg.h"

constexpr const char* FNAME_IN = "../NotoEmoji/Remakes/emoji_u1f9d1_200d_1f692.svg";
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

    xs::OptSets osets;
    osets.styleToAttr = xs::StyleToAttr::IF_POSSIBLE;
    svg.optimize(osets);

    xs::SaveSets sets;
    sets.writeDocType = false;
    svg.saveFile(FNAME_OUT, sets);

    return 0;
}
