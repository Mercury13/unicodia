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


QPalette darkPalette()
{
    QPalette pal;
    pal.setColor(QPalette::Window, QColor(53, 53, 53));
    pal.setColor(QPalette::WindowText, Qt::white);
    pal.setColor(QPalette::Base, QColor(35, 35, 35));
    pal.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    pal.setColor(QPalette::ToolTipBase, QColor(25, 25, 25));
    pal.setColor(QPalette::ToolTipText, Qt::white);
    pal.setColor(QPalette::Text, Qt::white);
    pal.setColor(QPalette::Button, QColor(53, 53, 53));
    pal.setColor(QPalette::ButtonText, Qt::white);
    pal.setColor(QPalette::BrightText, Qt::red);
    pal.setColor(QPalette::Link, QColor(42, 130, 218));
    pal.setColor(QPalette::Highlight, QColor(42, 130, 218));
    pal.setColor(QPalette::HighlightedText, QColor(35, 35, 35));
    pal.setColor(QPalette::Active, QPalette::Button, QColor(53, 53, 53));
    pal.setColor(QPalette::Disabled, QPalette::ButtonText, Qt::darkGray);
    pal.setColor(QPalette::Disabled, QPalette::WindowText, Qt::darkGray);
    pal.setColor(QPalette::Disabled, QPalette::Text, Qt::darkGray);
    pal.setColor(QPalette::Disabled, QPalette::Light, QColor(53, 53, 53));
    return pal;
}


int main(int argc, char *argv[])
{
    //qputenv("QT_SCALE_FACTOR", "1.25");
    //qputenv("QT_QPA_PLATFORM", "windows:darkmode=2");
    QApplication a(argc, argv);
    //a.setStyle("fusion");  Try other styles

    dark::fileName = ":/Combinear.qss";
    dark::palette = darkPalette();
    //dark::turnOn();

    uc::completeData();  // …runs once and should not depend on L10n

    FmMain w;
    w.installTempPrefix();

    auto cmdline = Cmdline::parse();
    if (cmdline.qa) {
        // Sanity-check is inside
        qa::testFonts(cmdline.qa.fonts);
        return 0;
    }

    initTranslation();

    loc::man.add(w);

    // Load config
    {
        auto order = BlockOrder::DEFAULT;
        config::window::State state(w);

        config::init(state, order);

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
