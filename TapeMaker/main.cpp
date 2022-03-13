#include <iostream>
#include <fstream>
#include <filesystem>
#include <bit>

// Libs
#include "i_ByteSwap.h"

struct TapeEntry
{
    unsigned subtape,
             offset,
             length;
    std::string filename;
};

class TapeWriter
{
public:
    void addFile(const std::filesystem::path& p, size_t fsize);
    void finish();
private:
    static constexpr int SUBTAPE_SIZE = 250'000;
    int iSubTape = 0;
    size_t subtapeSize = 0;
    std::string readySubtape;
    std::vector<TapeEntry> entries;
    void nextSubtape();
    void writeDirectory() const;
    void writeList() const;
};

void TapeWriter::addFile(const std::filesystem::path& p, size_t fsize)
{
    // Tape will not
    if (fsize == 0)
        return;

    // Create entry
    entries.emplace_back(
            iSubTape, subtapeSize, fsize, p.filename().generic_string() );

    // Add to subtape
    auto newSize = subtapeSize + fsize;
    if (newSize > readySubtape.size()) {
        auto allocSize = std::max<size_t>(newSize * 3 / 2, 2000);
        readySubtape.resize(allocSize);
    }
    std::ifstream is(p, std::ios::binary);
    is.read(readySubtape.data() + subtapeSize, fsize);
    subtapeSize = newSize;

    // Next??
    if (subtapeSize >= SUBTAPE_SIZE)
        nextSubtape();
}

void TapeWriter::nextSubtape()
{
    if (subtapeSize == 0)
        return;

    // Generate subtape name
    char buf[20];
    snprintf(buf, sizeof(buf), "%d.bin", iSubTape);

    // Write subtape
    std::ofstream os(buf, std::ios::binary);
    os.write(readySubtape.data(), subtapeSize);

    // ++
    ++iSubTape;
    subtapeSize = 0;
}

namespace {

    [[maybe_unused]] void writeIW(std::ostream& os, uint16_t x)
    {
        Word1 w { x };
        swapIW(w);
        os.write(w.asChars, 2);
    }

    [[maybe_unused]] void writeID(std::ostream& os, uint32_t x)
    {
        Dword1 d { x };
        swapID(d);
        os.write(d.asChars, 4);
    }

}   // anon namespace

void TapeWriter::writeDirectory() const
{
    std::ofstream os("tape.bin", std::ios::binary);
    writeID(os, entries.size());
    for (auto& entry : entries) {
        writeIW(os, entry.subtape);
        writeID(os, entry.offset);
        writeID(os, entry.length);
        writeIW(os, entry.filename.length());
        os.write(entry.filename.data(), entry.filename.length());
    }
}

void TapeWriter::writeList() const
{
    std::ofstream os("notoemoji.txt");
    for (auto& entry : entries) {
        os << entry.filename << '\n';
    }
}


void TapeWriter::finish()
{
    nextSubtape();
    writeDirectory();
    writeList();
}

void deleteBinaries()
{
    std::filesystem::path pExt(".bin");
    std::filesystem::directory_iterator di(".");
    for (const auto& entry: di) {
        if (entry.is_regular_file() && entry.path().extension() == pExt) {
            std::filesystem::remove(entry.path());
        }
    }
}

int main()
{
    deleteBinaries();
    TapeWriter tw;
    std::filesystem::path pExt(".svg");
    std::filesystem::directory_iterator di(".");
    for (const auto& entry: di) {
        if (entry.is_regular_file() && entry.path().extension() == pExt) {
            tw.addFile(entry.path(), entry.file_size());
            std::cout << entry.path().filename().generic_string() << std::endl;
        }
    }
    tw.finish();
}
