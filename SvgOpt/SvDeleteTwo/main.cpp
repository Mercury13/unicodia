#include <iostream>
#include <filesystem>
#include <fstream>

#include "u_Strings.h"

int main()
{
    std::cout << "Current dir: " << std::filesystem::current_path() << '\n';

    std::string buf;

    for (auto& v : std::filesystem::directory_iterator(".")) {
        if (v.is_regular_file() && v.path().extension() == ".svg") {
            // Read file
            { std::ifstream is(v.path(), std::ios::binary | std::ios::ate);
                size_t size = is.tellg();
                is.seekg(0);
                buf.resize(size);
                is.read(buf.data(), size);
            }
            // Replace
            unsigned nRepl = 0;
            nRepl += str::replace(buf, " viewBox=\"0 0 16 16\"", "");
            nRepl += str::replace(buf, " xmlns=\"http://www.w3.org/2000/svg\"", "");
            auto fname = v.path().filename();
            if (nRepl > 0) {
                std::ofstream os(v.path(), std::ios::binary);
                os.write(buf.data(), buf.size());
                std::cout << fname << " written!" "\n";
            } else {
                std::cout << fname << " skipped!" "\n";
            }
        }
    }
    std::cout << "Press Enter!";
    std::cin.ignore(1);
    return 0;
}
