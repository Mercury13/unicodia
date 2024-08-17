// My header
#include "TempFont.h"

// Nearby libs
#include "MemFont.h"

// STL
#include <iostream>

// Qt
#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QFontDatabase>

// Qt misc
#include "u_Qstrings.h"

std::string tempPrefix;

constexpr bool debugTempFont = false;

template <class... Args>
inline void msg([[maybe_unused]] Args&&... x)
{
    if constexpr (debugTempFont) {
        ((std::cout << x), ...);
        // OK, this is debug and flushing is really needed
        std::cout << std::endl;
    }
}


//// CompressedBits ////////////////////////////////////////////////////////////


void CompressedBits::Block::set(unsigned n)
{
    if (n >= BITS_PER_BLOCK)
        return;
    auto hi = n >> ITEM_HI_SHIFT;
    auto lo = n & ITEM_LO_MASK;
    items[hi] |= (ONE << lo);
}


bool CompressedBits::Block::have(unsigned n)
{
    if (n >= BITS_PER_BLOCK)
        return false;
    auto hi = n >> ITEM_HI_SHIFT;
    auto lo = n & ITEM_LO_MASK;
    return items[hi] & (ONE << lo);
}


void CompressedBits::add(unsigned x)
{
    auto hi = x >> BLOCK_HI_SHIFT;
    if (hi >= blocks.size()) {
        blocks.resize(hi + PREALLOC1);
    }
    auto& blk = blocks[hi];
    if (!blk)
        blk = std::make_unique<Block>();
    blk->set(x & BLOCK_LO_MASK);
}


bool CompressedBits::have(unsigned x) const noexcept
{
    auto hi = x >> BLOCK_HI_SHIFT;
    if (hi >= blocks.size())
        return false;
    auto& blk = blocks[hi];
    if (!blk)
        return false;
    return blk->have(x & BLOCK_LO_MASK);
}



//// TempFont //////////////////////////////////////////////////////////////////


TempFont installTempFontFull(
        const QString& fname, bool dehintDotc, char32_t trigger)
{
    MemFont mf;
    int id = -1;
    CompressedBits cps;
    if (!tempPrefix.empty() &&
            (fname.endsWith(".ttf") || fname.endsWith(".otf"))) {
        msg("Loading TTF/OTF ",
            QFileInfo(fname).fileName().toStdString(),
            " for the sake of ", std::hex,
            static_cast<int32_t>(trigger), std::dec);
        // TTF, load + rename
        try {
            mf.load(fname);
            mf.mangle(tempPrefix);
            unsigned giDotc = 0;    // glyph index of dotted circle
            mf.traverseCps([&cps, &giDotc]
                    (uint32_t cp, unsigned glyph) {
                        cps.add(cp);
                        if (cp == 0x25CC)
                            giDotc = glyph;
                    });
            if (dehintDotc && giDotc != 0) {
                /// @todo [future] dehint dotted circle
            }
            id = QFontDatabase::addApplicationFontFromData(mf.qdata());
        } catch (const std::exception& e) {
            std::cout << "ERROR: " << e.what() << '\n';
        }
    } else {
        // Load exactly
        id = QFontDatabase::addApplicationFont(fname);
    }
    if (id < 0) {
        std::cout << "Cannot install " << fname.toStdString() << '\n';
        return { FONT_BADLY_INSTALLED, {}, {} };
    }
    auto families = QFontDatabase::applicationFontFamilies(id);
    if constexpr (debugTempFont) {
        for (auto& v : families) {
            msg("Installed ", v.toStdString(), ", id=", id, " for the sake of ",
                std::hex, static_cast<uint32_t>(trigger), std::dec);
        }
    }
    TempFont r { .id = id, .families = std::move(families), .cps = std::move(cps) };
    return r;
}


QString expandTempFontName(std::string_view fname)
{
    QString subPath("Fonts/");
    str::append(subPath, fname);

    auto path = QApplication::applicationDirPath();
    QDir dir(path);
    QString absPath = dir.absoluteFilePath(subPath);
    return absPath;
}


TempFont installTempFontRel(
        std::string_view fname, bool dehintDotc, char32_t trigger)
{
    QString absPath = expandTempFontName(fname);
    return installTempFontFull(absPath, dehintDotc, trigger);
}
