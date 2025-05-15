#include <iostream>
#include <filesystem>
#include <stdexcept>
#include <map>
#include <fstream>

// C
#include "cstdlib"

// Libs
#include "u_Strings.h"

enum class FileVer : unsigned char { COL3, COL2, UCD };

struct Version {
    const char* name;
    const char* suffix;
    FileVer version;
    bool isBeta = false;

    std::string remoteFileName() const;
    std::string localFileName() const;
};

std::string Version::remoteFileName() const
{
    std::string r = str::cat("https://unicode.org/Public/", name, "/");
    switch (version) {
    case FileVer::UCD:
        r += "ucd/";
        break;
    case FileVer::COL3:
    case FileVer::COL2:
        break;
    }
    str::append(r, "Blocks", suffix, ".txt");
    return r;
}

std::string Version::localFileName() const
{
    return str::cat(
            "Blocks-",
            isBeta ? "beta" : name,
            ".txt");
}

Version versions[] {
    { "2.0-Update",  "-1",     FileVer::COL3 },
    { "2.1-Update4", "-2",     FileVer::COL3 },
    { "3.0-Update",  "-3",     FileVer::COL3 },
    { "3.1-Update",  "-4",     FileVer::COL2 },
    { "3.2-Update",  "-3.2.0", FileVer::COL2 },
    { "4.0-Update",  "-4.0.0", FileVer::COL2 },
    { "4.0-Update1", "-4.0.1", FileVer::COL2 },
    { "4.1.0",       "",       FileVer::UCD  },
    { "5.0.0",       "",       FileVer::UCD  },
    { "5.1.0",       "",       FileVer::UCD  },
    { "5.2.0",       "",       FileVer::UCD  },
    { "6.0.0",       "",       FileVer::UCD  },
    { "6.1.0",       "",       FileVer::UCD  },
    { "6.2.0",       "",       FileVer::UCD  },
    { "6.3.0",       "",       FileVer::UCD  },
    { "7.0.0",       "",       FileVer::UCD  },
    { "8.0.0",       "",       FileVer::UCD  },
    { "9.0.0",       "",       FileVer::UCD  },
    { "10.0.0",      "",       FileVer::UCD  },
    { "11.0.0",      "",       FileVer::UCD  },
    { "12.0.0",      "",       FileVer::UCD  },
    { "12.1.0",      "",       FileVer::UCD  },
    { "13.0.0",      "",       FileVer::UCD  },
    { "14.0.0",      "",       FileVer::UCD  },
    { "15.0.0",      "",       FileVer::UCD  },
    { "15.1.0",      "",       FileVer::UCD  },
    { "16.0.0",      "",       FileVer::UCD  },
    { "17.0.0",      "",       FileVer::UCD, true },
    // Now Unicode
};


void ensureLocalFiles()
{
    std::cout << "Checking for local files, downloading remote..." "\n";
    for (auto& v : versions) {
        auto localName = v.localFileName();
        // Beta is always loaded!
        if (!v.isBeta && std::filesystem::exists(localName))
            continue;
        // Load
        auto remoteName = v.remoteFileName();
        std::string cmdline = str::cat("curl ", remoteName, " -L --output ", localName);
        std::system(cmdline.c_str());
        // Check again
        if (!std::filesystem::exists(localName))
            throw std::logic_error(str::cat(
                "File ", localName, " still not found!"));
    }
    std::cout << "Local files are here!" "\n";
}


struct Range {
    unsigned start = 999999, end = 999999;
    bool operator == (const Range& x) const = default;
};


struct Block {
    Range range;
    const Version* version = nullptr;
};


struct Cmp {
    using is_transparent = void;
    bool operator () (std::string_view x, std::string_view y) const { return (x < y); }
};


class String : public std::string
{
public:
    using std::string::string;
    // Implicit ctor
    String(std::string_view x) : std::string(x) {}
    String(const std::string& x) : std::string(x) {}
    String(std::string&& x) : std::string(std::move(x)) {}
};

using Blocks = std::map<String, Block, Cmp>;


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


void testNewBlock(Blocks& blocks, std::string_view name, Range newRange,
                  const Version& version)
{
    auto upName = str::toUpper(name);
    if (newRange.start == 0xFEFF  // Block consisting of 1 char
            || name == "Private Use")  // Do not check these
        return;
    // Known renames
    if (upName == "GREEK")
        upName = "GREEK AND COPTIC";
    if (upName == "COMBINING MARKS FOR SYMBOLS")
        upName = "COMBINING DIACRITICAL MARKS FOR SYMBOLS";
    if (upName == "CYRILLIC SUPPLEMENTARY")
        upName = "CYRILLIC SUPPLEMENT";
    newRange.end |= 0x0F;
    auto& oldBlock = blocks[upName];
    if (oldBlock.version && oldBlock.range != newRange) {
        std::cout << std::hex
              << "Version '" << version.name << "' changed block '" << name
              << "' from " << oldBlock.range.start << ".." << oldBlock.range.end
              << " to " << newRange.start << ".." << newRange.end << "." "\n";
    }
    oldBlock.version = &version;
    oldBlock.range = newRange;
}


void loadBlocks(Blocks& blocks, const Version& version)
{
    std::string fname = version.localFileName();
    std::ifstream is(fname);
    std::string s;
    while (std::getline(is, s)) {
        if (s.empty())
            continue;
        auto sTrimmed = str::trim(s);
        if (sTrimmed.empty() || sTrimmed.starts_with('#'))
            continue;
        auto cols = str::splitSv(sTrimmed, ';', false);
        switch (version.version) {
        case FileVer::COL3: { // Three columns
                if (cols.size() < 3)
                    throw std::logic_error(str::cat("Need 3 cols in ", fname));
                Range newRange {
                    .start = fromHex(cols[0]),
                    .end   = fromHex(cols[1]), };
                auto name = cols[2];
                testNewBlock(blocks, name, newRange, version);
            } break;
        case FileVer::COL2:
        case FileVer::UCD: {   // Two columns
                if (cols.size() < 2)
                    throw std::logic_error(str::cat("Need 2 cols in ", fname));
                auto sRange = cols[0];
                auto pos = sRange.find("..");
                if (pos == std::string_view::npos)
                    throw std::logic_error(str::cat("Cannot split ", sRange));
                auto sStart = sRange.substr(0, pos);
                auto sEnd = sRange.substr(pos + 2);
                Range newRange {
                    .start = fromHex(sStart),
                    .end   = fromHex(sEnd), };
                auto name = cols[1];
                testNewBlock(blocks, name, newRange, version);
            } break;
        }
    }
}


void checkForDeleted(const Blocks& blocks, const Version* oldVersion, const Version& newVersion)
{
    for (auto& [name, blk] : blocks) {
        if (blk.version == oldVersion) {
            std::cout << std::hex
                  << "Version '" << newVersion.name << "' deleted block '" << name << "'." "\n";
        }
    }
}


void checkHistory()
{
    Blocks blocks;
    Version* prevVersion = nullptr;
    for (auto& v : versions) {
        loadBlocks(blocks, v);
        checkForDeleted(blocks, prevVersion, v);
        prevVersion = &v;
    }
}


int main()
{
    try {
        ensureLocalFiles();
        checkHistory();
    } catch (const std::exception& e) {
        std::cout << "ERROR: " << e.what() << '\n';
    }
    return 0;
}
