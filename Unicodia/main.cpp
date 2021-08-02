// Qt
#include <QApplication>
#include <QWidget>

// Libs
#include <i_TempFont.h>

// Qt forms
#include "FmMain.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Preinstall backup font!
    installTempFontRel("NotoSerif-Regular.ttf");
    installTempFontRel("NotoSansSymbols-Regular.ttf");
    installTempFontRel("NotoSansSymbols2-Regular.ttf");

    FmMain w;

    uc::completeData();
    w.show();
    return a.exec();
}
