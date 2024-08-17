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
    TempFont r { .id = -1, .families{}, .cps{} };
    bool wantChanges = !tempPrefix.empty() || dehintDotc;
    if (wantChanges &&
            (fname.endsWith(".ttf") || fname.endsWith(".otf"))) {
        msg("Loading TTF/OTF ",
            QFileInfo(fname).fileName().toStdString(),
            " for the sake of ", std::hex,
            static_cast<int32_t>(trigger), std::dec);
        // TTF, load + rename
        try {
            mf.load(fname);
            mf.mangle(tempPrefix);
            int giDotc = -1;    // glyph index of dotted circle;
            mf.traverseCps([&r, &giDotc]
                    (uint32_t cp, unsigned glyph) {
                        r.cps.add(cp);
                        if (cp == 0x25CC)
                            giDotc = glyph;
                    });
            if (dehintDotc && giDotc >= 0) {
                mf.dehintGlyph(giDotc);
            }
            r.id = QFontDatabase::addApplicationFontFromData(mf.qdata());
        } catch (const std::exception& e) {
            std::cout << "ERROR: " << e.what() << '\n';
        }
    } else {
        // Load exactly
        r.id = QFontDatabase::addApplicationFont(fname);
    }
    if (r.id < 0) {
        std::cout << "Cannot install " << fname.toStdString() << '\n';
        return { FONT_BADLY_INSTALLED, {}, {} };
    }
    r.families = QFontDatabase::applicationFontFamilies(r.id);
    if constexpr (debugTempFont) {
        for (auto& v : r.families) {
            msg("Installed ", v.toStdString(), ", id=", r.id, " for the sake of ",
                std::hex, static_cast<uint32_t>(trigger), std::dec);
        }
    }
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
