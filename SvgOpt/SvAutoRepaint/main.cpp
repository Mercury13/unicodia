#include <iostream>
#include <filesystem>
#include <fstream>

#include "u_Strings.h"

int main()
{
    std::string buf;
    buf.resize(100);  // initial capacity

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
            static constexpr const char* STUB = "#c01c28";
            nRepl += str::replace(buf, "#f57900", STUB); // Asia →
            // Europe lost (cancelled with bad SVGs) but almost done
            // WARNING: America may do bad on Asia ← (Phlp), but it’s done
            //   manually when looked for nice sandy colours
            nRepl += str::replace(buf, "#cc0000", STUB); // America

            // Save (or do not save)
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
