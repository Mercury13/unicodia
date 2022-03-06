#include <iostream>
#include <filesystem>
#include <fstream>

namespace ret {
    constexpr int OK = 0;
    constexpr int USAGE = 1;
    constexpr int NO_SRC = 2;
    constexpr int CANT_READ_SRC = 3;
    constexpr int CANT_WRITE_DEST = 4;
}   // ns ret

enum class Rs { EQUAL, INEQUAL, NO_SRC, CANT_READ };

struct CmpRet {
    Rs state = Rs::NO_SRC;
    size_t size = 0;
    std::unique_ptr<char[]> srcFile {};
};

CmpRet compareFiles(
        const std::filesystem::path& p1,
        const std::filesystem::path& p2)
{
    // Check for p1
    if (!std::filesystem::exists(p1))
        return {};

    // Load p1
    CmpRet r;
    std::istream::pos_type sz1;
    { std::ifstream is1(p1, std::ios::binary);
        if (!is1.is_open())
            return {};
        is1.seekg(0, std::ios::end);
        sz1 = is1.tellg();
        if (sz1 >= 2'000'000'000)
            return { Rs::CANT_READ };
        r.srcFile = std::make_unique<char[]>(sz1);
        r.size = sz1;
        is1.seekg(0, std::ios::beg);
        if (!is1.read(r.srcFile.get(), sz1))
            return { Rs::CANT_READ };
    }

    // Now temporarily inequal
    r.state = Rs::INEQUAL;

    // Check for p2
    if (!std::filesystem::exists(p2))
        return r;

    // Load p2
    { std::ifstream is2(p2, std::ios::binary);
        if (!is2.is_open())
            return r;
        is2.seekg(0, std::ios::end);
        auto sz2 = is2.tellg();
        if (sz2 != sz1)
            return r;
        auto d2 = std::make_unique<char[]>(sz1);
        is2.seekg(0);
        if (!is2.read(d2.get(), sz1))
            return r;
        auto eq = std::equal(r.srcFile.get(), r.srcFile.get() + sz1, d2.get());
        if (eq)
            r.state = Rs::EQUAL;
    }
    return r;
}

int main(int argc, char** argv)
{
    if (argc < 3) {
        std::cout << std::endl
                  << "SmartCopy file_in file_out" << std::endl
                  << std::endl;
        return ret::USAGE;
    }

    std::filesystem::path p1(argv[1]), p2(argv[2]);
    auto cmpr = compareFiles(p1, p2);

    switch (cmpr.state) {
    case Rs::NO_SRC:
        std::cout << "No source file " << p1.filename() << std::endl;
        return ret::NO_SRC;
    case Rs::CANT_READ:
        std::cout << "Can't read source file " << p1.filename() << std::endl;
        return ret::CANT_READ_SRC;
    case Rs::EQUAL:
        std::cout << p2.filename().generic_string() << ": files are equal, nothing to do" << std::endl;
        return ret::OK;
    case Rs::INEQUAL: {
            std::ofstream os(p2, std::ios::binary);
            if (!os.is_open()) {
                std::cout << "Cannot open " << p2.filename().generic_string() << std::endl;
                return ret::CANT_WRITE_DEST;
            }
            if (!os.write(cmpr.srcFile.get(), cmpr.size)) {
                std::cout << "Cannot write " << p2.filename().generic_string() << std::endl;
                return ret::CANT_WRITE_DEST;
            }
            std::cout << "Written " << p2.filename().generic_string() << std::endl;
            return ret::OK;
        }
    }
}
