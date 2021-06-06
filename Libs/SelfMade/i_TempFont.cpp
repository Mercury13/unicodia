#include "i_TempFont.h"

// Qt
#include <QApplication>
#include <QDir>

#include "u_Qstrings.h"

//#define USE_WIN32_FONTS

#if defined(_WIN32) && defined(USE_WIN32_FONTS)
    #include <windows.h>

    void installTempFontFull(QString fname)
    {
        fname.replace('/', '\\');
        const QChar* data1 = fname.data();
        auto data2 = reinterpret_cast<const wchar_t*>(data1);
        AddFontResourceExW(data2, FR_PRIVATE, 0);
    }
#else
    #include <QFontDatabase>

    void installTempFontFull(QString fname)
    {
        QFontDatabase::addApplicationFont(fname);
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


void installTempFontRel(std::string_view fname)
{
    QString absPath = expandTempFontName(fname);
    installTempFontFull(absPath);
}
