#pragma once

#include <QList>
#include <QString>

// C++
#include <bit>

// STL
#include <vector>

constexpr auto FONT_NOT_INSTALLED = -1000;
constexpr auto FONT_BADLY_INSTALLED = -1;
extern std::string tempPrefix;

constexpr bool isPowerOfTwo(unsigned x) { return ((x & (x - 1)) == 0); }

/// Specific platforms we perform “static-assert” unit tests on
///   (structure itself is platform-independent)
enum class TfPlat : unsigned char { W64, UNK };
#ifdef _WIN64
    constexpr TfPlat TF_PLAT = TfPlat::W64;
#else
    constexpr TfPlat TF_PLAT = TfPlat::UNK;
#endif
constexpr bool TF_PLAT_W64 = (TF_PLAT == TfPlat::W64);
constexpr bool UNTESTED = true;

class CompressedBits
{
public:
    bool isEmpty() const noexcept { return blocks.empty(); }
    bool hasSmth() const noexcept { return !isEmpty(); }

    void add(unsigned x);
    bool have(unsigned x) const noexcept;
private:
    static constexpr unsigned BYTES_PER_ITEM = sizeof(size_t);
    static constexpr unsigned BITS_PER_ITEM = BYTES_PER_ITEM * 8;
    static_assert(isPowerOfTwo(BYTES_PER_ITEM));
    // Unit tests: W64
    static_assert(TF_PLAT_W64 ? (BITS_PER_ITEM == 64) : UNTESTED);

    static constexpr size_t BITS_PER_BLOCK = 4096;  // 512 bytes per block
    static constexpr size_t ITEMS_PER_BLOCK = BITS_PER_BLOCK / BITS_PER_ITEM;

    static constexpr unsigned BLOCK_HI_SHIFT = std::countr_zero(BITS_PER_BLOCK);
    static constexpr unsigned BLOCK_LO_MASK = BITS_PER_BLOCK - 1;
    // # of blocks to preallocate, plus 1
    static constexpr unsigned PREALLOC1 = 8;

    static_assert(isPowerOfTwo(BITS_PER_BLOCK));
    class Block {
    public:
        Block() { std::fill_n(items, ITEMS_PER_BLOCK, 0); }
        void set(unsigned n);
        bool have(unsigned n);
    private:
        size_t items[ITEMS_PER_BLOCK];

        static constexpr unsigned ITEM_HI_SHIFT = std::countr_zero(BITS_PER_ITEM);
        static constexpr unsigned ITEM_LO_MASK = BITS_PER_ITEM - 1;
        // Unit tests: W64
        static_assert(TF_PLAT_W64 ? (ITEM_HI_SHIFT == 6) : UNTESTED);
        static_assert(TF_PLAT_W64 ? (ITEM_LO_MASK == 63) : UNTESTED);
        static constexpr size_t ONE = 1;
    };
    std::vector<std::unique_ptr<Block>> blocks;
};

struct TempFont {
    intptr_t id = FONT_NOT_INSTALLED;
    QList<QString> families;
    CompressedBits cps;
};

///
/// @param fname        file name
/// @param dehintDotc   [+] remove hinting instructions from dotted circle
/// @param trigger      CP that triggered installation
/// @return  font info
///
TempFont installTempFontFull(
        const QString& fname, bool dehintDotc, char32_t trigger);
QString expandTempFontName(std::string_view fname);
TempFont installTempFontRel(
        std::string_view fname, bool dehintDotc, char32_t trigger);
