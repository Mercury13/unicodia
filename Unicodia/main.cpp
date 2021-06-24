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
    FmMain w;

    //installTempFontRel("unifont-13.0.06.ttf");

    uc::completeData();
    w.show();
    return a.exec();
}
