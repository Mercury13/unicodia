#include "i_TempFont.h"

// STL
#include <iostream>
#include <bit>

// Qt
#include <QApplication>
#include <QDir>
#include <QFontDatabase>

// Qt misc
#include "u_Qstrings.h"

// Libs
#include "u_Vector.h"
#include "i_MemStream.h"

std::string tempPrefix;

constexpr bool debugTempFont = true;

namespace {

    namespace HEADER {
        constexpr auto OFS_NTABLES = 4;
        constexpr auto SIZE = 12;
    }

    struct Char4
    {
        union {
            char asChars[4] { 0, 0, 0, 0 };
            uint32_t asInt;
        } d;

        constexpr Char4() = default;
        constexpr Char4(const char (&x)[5])
            : d {{ x[0], x[1], x[2], x[3] }} {}
        std::string_view toSv() const { return { d.asChars, 4}; }
    };

    [[maybe_unused]] constexpr bool operator == (Char4 x, Char4 y) { return x.d.asInt == y.d.asInt; }
    [[maybe_unused]] constexpr bool operator != (Char4 x, Char4 y) { return x.d.asInt != y.d.asInt; }

    struct Block
    {
        Char4 name;
        uint32_t posInDir = 0, posInFile = 0, length = 0;
    };

    struct Block2
    {
        const Block* b = nullptr;
        QByteArray d {};

        operator bool() const { return b; }
    };

    class MemFont : public Mems
    {
    public:
        bool load(const QString& fname);
        bool load(QIODevice& f);

        // High-level bhv
        Block2 findBlock(Char4 name);
        const Block* rqBlock(Char4 name, uint32_t len = 0) const;
    private:
        SafeVector<Block> blocks;

        bool readDir();
        Block readBlockEntry();
    };

    bool MemFont::load(const QString& fname)
    {
        QFile f(fname);
        if (!f.open(QIODevice::ReadOnly))
            return false;
        return load(f);
    }

    bool MemFont::load(QIODevice& f)
    {
        d = f.readAll();
        p = beg();
        return readDir();
    }

    Block MemFont::readBlockEntry()
    {
        Block r;
        r.posInDir = pos();
        readStruc<4>(r.name);
        skipD();  // skip checksum;
        r.posInFile = readMD();
        r.length = readMD();
        return r;
    }

    bool MemFont::readDir()
    {
        // All data is Motorola here
        seek(HEADER::OFS_NTABLES);
        unsigned nTables = readMW();
        blocks.reserve(nTables);

        seek(HEADER::SIZE);
        for (unsigned iTable = 0; iTable < nTables; ++iTable) {
            Block table = readBlockEntry();
            blocks.push_back(table);
        }

        if constexpr (debugTempFont) {
            std::cout << "Read blocks:";
            for (auto& v : blocks) {
                std::cout << ' ' << v.name.toSv();
            }
            std::cout << std::endl;
        }

        return true;
    }
}


TempFont installTempFontFull(QString fname)
{
    /// @todo [urgent] What to do with fonts?
    int id = -1;
    if (!tempPrefix.empty() && fname.endsWith(".ttf")) {
        // TTF, load + rename
        try {
            MemFont mf;
            mf.load(fname);
            id = QFontDatabase::addApplicationFontFromData(mf.data());
        } catch (const std::exception& e) {
            std::cout << "ERROR: " << e.what() << std::endl;
        }
    } else {
        // Load exactly
        id = QFontDatabase::addApplicationFont(fname);
    }
    if (id < 0) {
        std::cout << "Cannot install " << fname.toStdString() << std::endl;
        return { FONT_BADLY_INSTALLED, {} };
    }
    auto families = QFontDatabase::applicationFontFamilies(id);
    if constexpr (debugTempFont) {
        for (auto& v : families) {
            std::cout << "Installed " << v.toStdString() << ", id=" << id << std::endl;
        }
    }
    return { id, std::move(families) };
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


TempFont installTempFontRel(std::string_view fname)
{
    QString absPath = expandTempFontName(fname);
    return installTempFontFull(absPath);
}
