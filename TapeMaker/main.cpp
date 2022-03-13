#include <iostream>
#include <fstream>
#include <filesystem>
#include <bit>

// Libs
#include "i_ByteSwap.h"
#include "u_Strings.h"

using namespace std::string_view_literals;

struct TapeEntry
{
    unsigned subtape,
             offset,
             length;
    std::u32string seq;

    std::string toName(std::string_view extension) const;
};


std::string TapeEntry::toName(std::string_view extension) const
{
    std::string r;
    char buf[20];
    for (auto v : seq) {
        if (!r.empty())
            r += '-';
        snprintf(buf, std::size(buf), "%x", v);
        r += buf;
    }
    r += extension;
    return r;
}


class TapeWriter
{
public:
    /// @return [0] not added [+] its filename
    std::string addFile(const std::filesystem::path& p, size_t fsize);
    void finish();
private:
    static constexpr int SUBTAPE_SIZE = 1'000'000;
    int iSubTape = 0;
    size_t subtapeSize = 0;
    std::string readySubtape;
    std::vector<TapeEntry> entries;
    void nextSubtape();
    void writeDirectory() const;
    void writeList() const;
};

bool isBlacklistedPart(std::string_view s)
{
    return (s == "emoji"sv);
}

std::u32string parseSeq(std::string s)
{
    str::replace(s, '_', '-');      // for Noto
    auto things = str::splitSv(s, '-');
    std::u32string r;
    for (auto v : things) {
        if (isBlacklistedPart(v))
            continue;
        if (v.starts_with('u'))
            v = v.substr(1);
        int value = -1;
        std::from_chars(
            std::to_address(v.begin()), std::to_address(v.end()), value, 16);
        if (value >= 0)
            r += char32_t(value);
    }
    return r;
}

std::string TapeWriter::addFile(const std::filesystem::path& p, size_t fsize)
{
    // Tape will not
    if (fsize == 0)
        return {};

    // Create entry
    auto fname = p.filename().generic_string();
    auto seq = parseSeq(fname);
    auto& entry = entries.emplace_back(
            iSubTape, subtapeSize, fsize, seq );

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

    return entry.toName(".svg");
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
        auto fname = entry.toName(".svg");
        writeIW(os, fname.length());
        os.write(fname.data(), fname.length());
    }
}

void TapeWriter::writeList() const
{
    std::ofstream os("single-char-emoji.txt");
    for (auto& entry : entries) {
        if (entry.seq.size() == 1)
            os << entry.toName({}) << '\n';
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
            auto name = tw.addFile(entry.path(), entry.file_size());
            if (name.empty()) {
                std::cout << "NOT ADDED: " << entry.path().filename().generic_string() << std::endl;
            } else {
                std::cout << name << std::endl;
            }
        }
    }
    tw.finish();
}
