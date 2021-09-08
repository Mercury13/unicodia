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

    TempFont installTempFontFull(QString fname)
    {
        /// @todo [urgent] What to do with fonts?
        QFile f(fname);
        f.open(QIODevice::ReadOnly);
        QByteArray ba = f.readAll();
        auto id = QFontDatabase::addApplicationFontFromData(ba);
        if (id < 0) {
            std::cout << "Cannot install " << fname.toStdString() << std::endl;
        }
        auto families = QFontDatabase::applicationFontFamilies(id);
        //for (auto& v : families) {
        //    std::cout << "Installed " << v.toStdString() << ", id=" << id << std::endl;
        //}
        return { id, std::move(families) };
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


TempFont installTempFontRel(std::string_view fname)
{
    QString absPath = expandTempFontName(fname);
    return installTempFontFull(absPath);
}
