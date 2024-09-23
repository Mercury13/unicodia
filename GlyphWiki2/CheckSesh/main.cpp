// This program checks whether Unikemet has all files needed,
// before writing Python script for building some font
//
// FILES NEEDED:
//  Unikemet.txt
//  svg\*.svg from JSesh
//
// GOT RESULT:
//  Cannot use just JSesh for now, a big lot of hieros have no JSesh counterpart

#include <iostream>
#include <fstream>

#include "u_Strings.h"

unsigned fromHex(std::string_view x)
{
    unsigned r;
    auto beg = x.data();
    auto end = beg + x.length();
    auto res = std::from_chars(beg, end, r, 16);
    if (res.ec != std::errc() || res.ptr != end) {
        throw std::invalid_argument(
                str::cat("[fromHex] Cannot parse hex ", x));
    }
    return r;
}

int main()
{
    try {
        std::ifstream is("Unikemet.txt");
        if (!is.is_open()) {
            throw std::logic_error("No file");
        }

        std::cout.setf(std::ios_base::uppercase);

        std::string s;
        char32_t lastCp = 0;
        bool hasFileLine = true;
        int nHierosWoSesh = 0;
        while (std::getline(is, s)) {
            auto s1 = str::trimSv(s);
            // Empty/comment?
            if (s1.empty() || s1.starts_with('#'))
                continue;
            auto cells = str::splitSv(s1, '\t', false);
            if (cells.size() < 3) {
                throw std::logic_error("Too short line");
            }
            // Parse codepoint
            auto sCp = cells[0];
            if (!sCp.starts_with("U+")) {
                throw std::logic_error("No U+ in codepoint");
            }
            sCp = sCp.substr(2);
            char32_t cp = fromHex(sCp);
            if (cp != lastCp) {
                if (!hasFileLine) {
                    std::cout << "CP " << std::hex << (unsigned)lastCp << " nas no file specified" "\n";
                    ++nHierosWoSesh;
                }
                hasFileLine = false;
                lastCp = cp;
            }
            // Parse command
            if (cells[1] == "kEH_JSesh") {
                hasFileLine = true;
            }
        }

        std::cout << std::dec << nHierosWoSesh << " hieros w/o JSesh source found." "\n";
    } catch (const std::exception& e) {
        std::cout << "ERROR: " << e.what() << '\n';
    }
}
