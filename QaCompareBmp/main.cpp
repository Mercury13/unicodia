#include "FmMain.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FmMain w;
    w.loadConfig();
    w.show();
    auto retCode = a.exec();
    if (retCode == 0) {
        w.saveConfig();
    }
    return retCode;
}
