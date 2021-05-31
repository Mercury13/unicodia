#include "i_TempFont.h"

// Qt
#include <QApplication>
#include <QDir>

#include "u_Qstrings.h"

#ifdef _WIN32
    #include <windows.h>

    void installTempFontFull(QString fname)
    {
        const QChar* data1 = fname.data();
        auto data2 = reinterpret_cast<const wchar_t*>(data1);
        AddFontResourceExW(data2, FR_PRIVATE, 0);
    }

#else
    #error Not ready for this OS
#endif

void installTempFontRel(std::string_view fname)
{
    QString subPath("Fonts/");
    str::append(subPath, fname);

    auto path = QApplication::applicationDirPath();
    QDir dir(path);
    QString absPath = dir.absoluteFilePath(subPath);
    installTempFontFull(absPath);
}
