// Qt
#include <QApplication>
#include <QWidget>
#include <QTranslator>

// Libs
#include <i_TempFont.h>

// Qt forms
#include "FmMain.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Install standard Qt translation
    QTranslator tr;
    if (tr.load("qtbase_ru", QApplication::applicationDirPath()))
        QApplication::installTranslator(&tr);

    uc::completeData();
    FmMain w;
    w.installTempPrefix();

    w.show();
    return a.exec();
}
