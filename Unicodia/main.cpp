// Qt
#include <QApplication>
#include <QWidget>
#include <QTranslator>

// Libs
#include <i_TempFont.h>

// L10n
#include "LocDic.h"

// Qt forms
#include "FmMain.h"

void doTranslate()
{
    auto dir = QApplication::applicationDirPath();

    // Install standard Qt translation
    QTranslator tr;
    if (tr.load("qtbase_ru", dir))
        QApplication::installTranslator(&tr);

    std::filesystem::path pDir = dir.toStdWString();
    pDir /= "lang.ini";
    loc::loadIni(loc::dic, pDir);

    loc::dic.dump(20);
    uc::finishTranslation();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    doTranslate();

    uc::completeData();
    FmMain w;
    w.installTempPrefix();

    w.show();
    return a.exec();
}
