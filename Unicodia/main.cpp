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
#include "LocManager.h"

// Qt forms
#include "FmMain.h"

namespace {

    ///
    ///  L10n of static data: Unicode DB and some wiki table
    ///
    class LocData : public loc::Subject
    {
    public:
        void translateMe() override;
        static LocData INST;
    };

    LocData LocData::INST;

    void LocData::translateMe()
    {
        uc::finishTranslation(loc::currLang->sortOrder);
        mywiki::translateDatingLoc();
    }

}   // anon namespace


void initTranslation()
{
    loc::man.add(LocData::INST);
    auto dir = QApplication::applicationDirPath();
    std::filesystem::path pDir = dir.toStdWString();

    loc::allLangs.collect(pDir);
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    uc::completeData();  // …runs once and should not depend on L10n
    initTranslation();

    FmMain w;    
    w.installTempPrefix();
    loc::man.add(w);

    w.chooseFirstLanguage();

    w.show();

    { loc::AutoStop autoStop;
        return a.exec();
    }   // manager will stop erasing here → speed up exit
}
