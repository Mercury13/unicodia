#pragma once

// C++
#include <vector>

// Project-local
#include "FileWrap.h"

class QFile;

struct Char4
{
    union {
        char asChars[4] { 0, 0, 0, 0 };
        uint32_t asInt;
    } d;

    constexpr Char4() = default;
    constexpr Char4(const char (&x)[5])
        : d {{ x[0], x[1], x[2], x[3] }} {}
};

constexpr bool operator == (Char4 x, Char4 y) { return x.d.asInt == y.d.asInt; }
constexpr bool operator != (Char4 x, Char4 y) { return x.d.asInt != y.d.asInt; }

struct PanoseData {
    std::array<uint8_t, 10> fit;
    std::array<uint8_t, 16> scripts;
};

class Ttf
{
public:
    struct Block {
        Char4 name;
        uint32_t posInDir = 0, posInFile = 0, length = 0;
    };

    FileWrap f;
    std::vector<Block> blocks;

    Ttf(QFile& aF);
    const Block* findBlock(Char4 name) const;
    const Block* rqBlock(Char4 name, uint32_t len = 0) const;
    void seek(const Block* block, uint32_t offset = 0);
    PanoseData readPanoseData();
    void writePanoseData(const PanoseData& x);
    void recomputeChecksum(const Block* block);
private:
    void readDir();
    Block readBlockEntry();
};
