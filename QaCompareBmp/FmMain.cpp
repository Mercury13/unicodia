#include "FmMain.h"
#include "ui_FmMain.h"

///// Config ///////////////////////////////////////////////////////////////////


bool FmMain::Config::isPresent() const
{
    return !versioned.isEmpty()
        || !working.isEmpty();
}


bool FmMain::Config::isFull() const
{
    return !versioned.isEmpty()
        && !working.isEmpty();
}


///// FmMain ///////////////////////////////////////////////////////////////////


FmMain::FmMain(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::FmMain)
{
    ui->setupUi(this);
    ui->splitMain->setSizes({100, 200});
}

FmMain::~FmMain()
{
    delete ui;
}

FmMain::Config FmMain::config()
{
    return {
        .versioned = ui->edVersioned->text().trimmed(),
        .working = ui->edWorking->text().trimmed(),
    };
}


void FmMain::saveConfig()
{

}

void FmMain::loadConfig()
{

}


void FmMain::run()
{

}
