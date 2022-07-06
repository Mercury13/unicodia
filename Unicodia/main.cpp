// Qt
#include <QApplication>
#include <QWidget>
#include <QTranslator>

// Libs
#include <i_TempFont.h>

// Project-local
#include "d_Config.h"

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
        // OK: translateMe requires currLang
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

    // Load config
    {
        auto rect = w.geometry();
        auto order = BlockOrder::DEFAULT;
        config::init(rect, order);
        w.setBlockOrder(order);

        /// @todo [config] We’ve got a bad position for window divider
        //setGeometry(w, rect);
        //if (config::window::isMaximized)
        //    w.setWindowState(w.windowState() | Qt::WindowMaximized);
    }
    w.chooseFirstLanguage();

    w.show();

    { loc::AutoStop autoStop;
        int r = a.exec();
        config::save(w.normalGeometry(), w.isMaximized(), w.blockOrder());
        return r;
    }   // manager will stop erasing here → speed up exit
}
