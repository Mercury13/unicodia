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

    // Preinstall backup fonts w/o mangling…
    /// @todo [fonts] When the font engine is ready, we’ll install fonts when needed
    installTempFontRel("NotoSerif-Regular.ttf");
    installTempFontRel("NotoSansSymbols-Regular.ttf");
    installTempFontRel("NotoSansSymbols2-Regular.ttf");
    installTempFontRel("NotoSansMath-Regular.ttf");

    // …and then install temporary prefix
    w.installTempPrefix();

    w.show();
    return a.exec();
}
