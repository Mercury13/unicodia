// STL
#include <iostream>
#include <filesystem>
#include <vector>

// XML
#include "pugixml.hpp"

#define PATH_WORK "../Font-Source/GlyphWiki/"
constexpr std::string_view DIR_IN = PATH_WORK "Raw";
constexpr std::string_view DIR_OUT = PATH_WORK "AutoRemade";

struct Baddy {
    std::string fname;
    std::string problem;
};

void printBaddies(const std::vector<Baddy>& x)
{
    std::cout << "GOT " << x.size() << " BADDIES" << std::endl;
    for (auto& v: x) {
        std::cout << v.fname << ": " << v.problem << std::endl;
    }
}

void remakeSvg(
        const std::filesystem::path& fnIn,
        const std::filesystem::path& fnOut)
{
    pugi::xml_document doc;
    doc.load_file(fnIn.c_str());
    doc.save_file(fnOut.c_str());
}

int main()
{
    std::filesystem::directory_iterator itIn(DIR_IN);
    std::filesystem::path pathOut(DIR_OUT);
    std::vector<Baddy> baddies;
    int nFiles = 0;
    for (auto& entryIn : itIn) {
        auto& fnIn = entryIn.path();
        auto fnFile = fnIn.filename();
        auto extFile = fnFile.extension();
        if (extFile == ".svg") {
            ++nFiles;
            std::cout << "#" << nFiles << ": " << fnFile.generic_string() << std::endl;
            auto fnOut = pathOut / fnFile;
            remakeSvg(fnIn, fnOut);
        }
    }

    printBaddies(baddies);
    return 0;
}
