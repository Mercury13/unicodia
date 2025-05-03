#include "FmMain.h"
#include "ui_FmMain.h"

FmMain::FmMain(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::FmMain)
{
    ui->setupUi(this);
}

FmMain::~FmMain()
{
    delete ui;
}

