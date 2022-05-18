// Qt
#include <QApplication>
#include <QWidget>
#include <QTranslator>

// Libs
#include <i_TempFont.h>

// L10n
#include "LocDic.h"
#include "LocList.h"
#include "LocQt.h"

// Qt forms
#include "FmMain.h"


void initTranslation()
{
    auto dir = QApplication::applicationDirPath();
    std::filesystem::path pDir = dir.toStdWString();

    loc::collectLangs(pDir);

    loc::loadFirstLang();

    /// @todo [L10n] They should be managed by loc::Manager
    uc::finishTranslation();
    mywiki::translateDatingLoc();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    initTranslation();

    uc::completeData();
    FmMain w;
    loc::translateForm(&w);

    w.installTempPrefix();

    w.show();
    return a.exec();
}
