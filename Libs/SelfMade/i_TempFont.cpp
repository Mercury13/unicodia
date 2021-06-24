#include "i_TempFont.h"

// Qt
#include <QApplication>
#include <QDir>

#include "u_Qstrings.h"
#include <iostream>

//#define USE_WIN32_FONTS

#if defined(_WIN32) && defined(USE_WIN32_FONTS)
    #include <windows.h>

    intptr_t installTempFontFull(QString fname)
    {
        fname.replace('/', '\\');
        const QChar* data1 = fname.data();
        auto data2 = reinterpret_cast<const wchar_t*>(data1);
        AddFontResourceExW(data2, FR_PRIVATE, 0);
    }
#else
    #include <QFontDatabase>

    intptr_t installTempFontFull(QString fname)
    {
        auto id = QFontDatabase::addApplicationFont(fname);
        if (id < 0) {
            std::cout << "Cannot install " << fname.toStdString() << std::endl;
        }
//        auto families = QFontDatabase::applicationFontFamilies(id);
//        for (auto& v : families) {
//            std::cout << "Installed " << v.toStdString() << std::endl;
//        }
        return id;
    }
#endif


QString expandTempFontName(std::string_view fname)
{
    QString subPath("Fonts/");
    str::append(subPath, fname);

    auto path = QApplication::applicationDirPath();
    QDir dir(path);
    QString absPath = dir.absoluteFilePath(subPath);
    return absPath;
}


intptr_t installTempFontRel(std::string_view fname)
{
    QString absPath = expandTempFontName(fname);
    return installTempFontFull(absPath);
}
