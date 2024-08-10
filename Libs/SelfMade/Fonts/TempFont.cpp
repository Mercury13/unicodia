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
inline void msg0([[maybe_unused]] Args&&... x)
{
    if constexpr (debugTempFont) {
        ((std::cout << x), ...);
    }
}

template <class... Args>
inline void msg([[maybe_unused]] Args&&... x)
{
    if constexpr (debugTempFont) {
        ((std::cout << x), ...);
        // OK, this is debug and flushing is really needed
        std::cout << std::endl;
    }
}

inline void msgln()
{
    if constexpr (debugTempFont) {
        // OK, this is debug and flushing is really needed
        std::cout << std::endl;
    }
}


TempFont installTempFontFull(QString fname, [[maybe_unused]] char32_t trigger)
{
    MemFont mf;
    int id = -1;
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
    return { id, std::move(families), std::make_unique<Mems>(mf.giveStream()) };
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
