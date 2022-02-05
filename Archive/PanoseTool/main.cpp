#include "FmMain.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FmMain w;
    w.show();
    return a.exec();
}
