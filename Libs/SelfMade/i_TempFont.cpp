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

#define MSG0(x) if constexpr (debugTempFont) { std::cout << x; }
#define MSG(x)  if constexpr (debugTempFont) { std::cout << x << std::endl; }
#define MSGLN   if constexpr (debugTempFont) { std::cout << std::endl; }

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

        Buf1d<char> toBuf(Buf1d<char> data);
    };

    Buf1d<char> Block::toBuf(Buf1d<char> data)
    {
        auto end = posInFile + length;
        if (end > data.size() || end < posInFile) {
            char buf[100];
            snprintf(buf, std::size(buf), "[Block.toBuf] File overrun in %.4s: pos %lld+%lld, file size %lld",
                     name.d.asChars,
                     static_cast<unsigned long long>(posInFile),
                     static_cast<unsigned long long>(length),
                     static_cast<unsigned long long>(data.size()));
            throw std::logic_error(buf);
        }
        return { length, data.buffer() + posInFile };
    }


    struct Block2
    {
        const Block* b = nullptr;
        Buf1d<char> d;

        operator bool() const { return b; }
    };

    class MemFont : public Mems
    {
    public:
        bool load(const QString& fname);
        bool load(QIODevice& f);

        // High-level bhv
        Block2 findBlock(Char4 name);
        Block2 rqBlock(Char4 name, uint32_t len = 0);
        /// @param bytes  ASCII only!!
        void mangleTtf(std::string_view bytes);
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
        auto sz = f.size();
        d.alloc(sz);
        f.read(d.buffer(), sz);
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
            MSG0("Read blocks:");
            for (auto& v : blocks) {
                MSG0(' ' << v.name.toSv());
            }
            MSGLN;
        }

        return true;
    }

    Block2 MemFont::findBlock(Char4 name)
    {
        for (auto& v : blocks)
            if (v.name == name)
                return { &v, v.toBuf(d) };
        return {};
    }


    Block2 MemFont::rqBlock(Char4 name, uint32_t len)
    {
        char buf[100];
        auto r = findBlock(name);
        if (!r) {
            snprintf(buf, sizeof(buf), "Block <%.4s> not found", name.d.asChars);
            throw std::logic_error(buf);
        }
        if (r.b->length <= len) {
            snprintf(buf, sizeof(buf), "Block <%.4s> wanted %lu, found %lu",
                     name.d.asChars, long(len), long(r.b->length));
            throw std::logic_error(buf);
        }
        return r;
    }

    void MemFont::mangleTtf(std::string_view bytes)
    {
        auto v = rqBlock("name", 32);
        Mems blk(v.d);

        blk.skipW();    // should be 0
        unsigned nRecs = blk.readMW();
        unsigned stringOffset = blk.readMW();
        MSG(nRecs << " records found");
        MSG0("Records found:");
        for (unsigned i = 0; i < nRecs; ++i) {
            blk.skip(6);    // platform, platformSpecific, language
            unsigned nameId = blk.readMW();
            unsigned length = blk.readMW();
            unsigned offset = blk.readMW();
            MSG0(' ' << nameId);
            if (nameId == 1 || nameId == 4 || nameId == 6) {
                if (length < bytes.length()) {
                    throw std::logic_error("Font name is too short");
                }
                auto oldPos = blk.pos();
                blk.seek(stringOffset + offset);
                for (auto v : bytes)
                    blk.writeMW(v);
                blk.seek(oldPos);
            }
        }
        MSGLN
    }

}


TempFont installTempFontFull(QString fname, [[maybe_unused]] char32_t trigger)
{
    int id = -1;
    if (!tempPrefix.empty() && fname.endsWith(".ttf")) {
        // TTF, load + rename
        try {
            MemFont mf;
            mf.load(fname);
            mf.mangleTtf(tempPrefix);
            id = QFontDatabase::addApplicationFontFromData(mf.qdata());
        } catch (const std::exception& e) {
            std::cout << "ERROR: " << e.what() << std::endl;
        }
    } else if (!tempPrefix.empty() && fname.endsWith(".otf")) {
        // OTF, try from memory
        try {
            MemFont mf;
            mf.load(fname);
            //mf.mangle(tempPrefix);
            id = QFontDatabase::addApplicationFontFromData(mf.qdata());
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
            MSG("Installed " << v.toStdString() << ", id=" << id << " for the sake of "
                << std::hex << static_cast<uint32_t>(trigger) << std::dec);
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


TempFont installTempFontRel(std::string_view fname, char32_t trigger)
{
    QString absPath = expandTempFontName(fname);
    return installTempFontFull(absPath, trigger);
}
