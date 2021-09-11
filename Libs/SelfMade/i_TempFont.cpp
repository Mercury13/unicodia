#include "i_TempFont.h"

// Qt
#include <QApplication>
#include <QDir>
#include <QFontDatabase>

#include "u_Qstrings.h"
#include <iostream>

std::string tempPrefix;


namespace {

    class MemFont
    {
    public:
        bool load(const QString& fname);
        bool load(QIODevice& f);
        const QByteArray& data() const { return d; }
    private:
        QByteArray d;
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
        return true;
    }
}


TempFont installTempFontFull(QString fname)
{
    /// @todo [urgent] What to do with fonts?
    int id = -1;
    if (!tempPrefix.empty() && fname.endsWith(".ttf")) {
        MemFont mf;
        mf.load(fname);
        id = QFontDatabase::addApplicationFontFromData(mf.data());
    } else {
        id = QFontDatabase::addApplicationFont(fname);
    }
    if (id < 0) {
        std::cout << "Cannot install " << fname.toStdString() << std::endl;
    }
    auto families = QFontDatabase::applicationFontFamilies(id);
    //for (auto& v : families) {
    //    std::cout << "Installed " << v.toStdString() << ", id=" << id << std::endl;
    //}
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
