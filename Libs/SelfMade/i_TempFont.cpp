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
#include "i_ByteSwap.h"


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

    class MemFont
    {
    public:
        bool load(const QString& fname);
        bool load(QIODevice& f);

        // Stream-like bhv (Motorola data here)
        const QByteArray& data() const { return d; }
        char* beg() { return d.begin(); }
        char* end() { return d.end(); }
        const char* beg() const { return d.begin(); }
        const char* end() const { return d.end(); }
        char* ptr() { return p; }
        const char* ptr() const { return p; }
        size_t size() const { return d.size(); }
        size_t pos() const { return p - beg(); }
        size_t remainder() const { return end() - p; }
        void seek(size_t x);
        void read(char* data, size_t dsize);
        template <size_t Len, class T>
        void readStruc(T& data)
        {
            static constexpr auto actualLen = sizeof(std::remove_reference_t<T>);
            static_assert(Len == actualLen, "Wrong Len, maybe mistake?");
            read(reinterpret_cast<char*>(&data), Len);
        }
        void write(const char* data, size_t dsize);
        template <size_t Len, class T>
        void writeStruc(const T& data)
        {
            static constexpr auto actualLen = sizeof(std::remove_reference_t<T>);
            static_assert(Len == actualLen, "Wrong Len, maybe mistake?");
            write(reinterpret_cast<const char*>(&data), Len);
        }

        Block2 findBlock(Char4 x);
        uint8_t readB() {
            uint8_t r;
            readStruc<1>(r);
            return r;
        }

        /// read Intel word (2 bytes)
        [[maybe_unused]] uint16_t readIW();

        /// read Motorola word (2 bytes)
        [[maybe_unused]] uint16_t readMW();

        /// read Intel dword (4 bytes)
        [[maybe_unused]] uint32_t readID();

        /// read Motorola dword (4 bytes)
        [[maybe_unused]] uint32_t readMD();

        /// write Motorola dword (4 bytes)
        [[maybe_unused]] void writeMD(uint32_t x);

        /// Skip dword (4 bytes)
        inline void skipD();
    private:
        QByteArray d;
        char* p = nullptr;
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

    uint16_t MemFont::readIW()
    {
        Word1 w;
        readStruc<2>(w);
        swapIW(w);
        return w.asWord;
    }

    uint16_t MemFont::readMW()
    {
        Word1 w;
        readStruc<2>(w);
        swapMW(w);
        return w.asWord;
    }

    uint32_t MemFont::readID()
    {
        Dword1 d;
        readStruc<4>(d);
        swapID(d);
        return d.asDword;
    }

    uint32_t MemFont::readMD()
    {
        Dword1 d;
        readStruc<4>(d);
        swapMD(d);
        return d.asDword;
    }

    void MemFont::writeMD(uint32_t x)
    {
        Dword1 d;
        d.asDword = x;
        swapMD(d);
        writeStruc<4>(d);
    }

    void MemFont::skipD()
    {
        uint32_t noMatter;
        readStruc<4>(noMatter);
    }

    void MemFont::seek(size_t x)
    {
        if (x > size()) {
            char buf[100];
            snprintf(buf, sizeof(buf), "[MF.seek] Seek past end: %llu/%llu, former pos=%llu",
                     static_cast<unsigned long long>(x), static_cast<unsigned long long>(size()),
                     static_cast<unsigned long long>(pos()));
            throw std::logic_error(buf);
        }
        x = std::min(x, size());
        p = beg() + x;
    }

    void MemFont::read(char* data, size_t dsize)
    {
        auto rem = remainder();
        if (dsize > rem) {
            char buf[100];
            snprintf(buf, sizeof(buf), "[MF.read] Cannot read %llu bytes: pos=%llu/%llu, rem=%llu",
                     static_cast<unsigned long long>(dsize), static_cast<unsigned long long>(pos()),
                     static_cast<unsigned long long>(size()), static_cast<unsigned long long>(rem));
            throw std::logic_error(buf);
        }
        auto pNew = p + dsize;
        std::copy(p, pNew, data);
        p = pNew;
    }

    void MemFont::write(const char* data, size_t dsize)
    {
        auto rem = remainder();
        if (dsize > rem) {
            char buf[100];
            snprintf(buf, sizeof(buf), "[MF.write] Cannot write %llu bytes: pos=%llu/%llu, rem=%llu",
                     static_cast<unsigned long long>(dsize), static_cast<unsigned long long>(pos()),
                     static_cast<unsigned long long>(size()), static_cast<unsigned long long>(rem));
            throw std::logic_error(buf);
        }
        auto pNew = p + dsize;
        p = std::copy(data, data + dsize, p);
        p = pNew;
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
