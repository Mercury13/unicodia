// Qt
#include <QApplication>
#include <QWidget>
#include <QTranslator>
#include <QScreen>

// Libs
#include <Fonts/TempFont.h>
#include "i_DarkMode.h"
#include "RememberWindow.h"

// Project-local
#include "d_Config.h"

// QA
#include "cmdline.h"
#include "qa.h"

// L10n
#include "LocList.h"
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
        uc::finishTranslation(
                    loc::currLang->sortOrder,
                    loc::currLang->engTerms,
                    loc::currLang->ellipsis.blocks,
                    loc::currLang->alphaFixup);
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
    //qputenv("QT_SCALE_FACTOR", "1.5");
    config::init1();
    dark::init1();
    QApplication a(argc, argv);
    dark::init2(":/Dark.qss");

    uc::completeData();  // …runs once and should not depend on L10n

    FmMain w;
    w.installTempPrefix();

    auto cmdline = Cmdline::parse();
    if (cmdline.qa) {
        if (cmdline.qa.hasFonts()) {
            qa::testFonts(cmdline.qa.fonts);
        }
        if (cmdline.qa.hasHttp()) {
            qa::testHttp(cmdline.qa.http);
        }
        return 0;
    }

    initTranslation();

    loc::man.add(w);

    // Load config
    {
        auto order = BlockOrder::DEFAULT;
        config::window::State state(w);

        config::init2(state, order);

        w.chooseFirstLanguage();
        w.setBlockOrder(order);  // Strange interaction: first language, then order, not vice-versa
        w.configLoaded();

        config::window::setGeometry(w, state);
    }

    w.show();

    { loc::AutoStop autoStop;
        int r = a.exec();
        config::save(config::window::State(w), w.blockOrder());
        return r;
    }   // manager will stop erasing here → speed up exit
}
