#include "FmMain.h"
#include "ui_FmMain.h"

// Qt
#include <QPushButton>

// Qt ex
#include "i_OpenSave.h"

FmMain::FmMain(QWidget *parent)
    : Super(parent)
    , ui(new Ui::FmMain)
{
    ui->setupUi(this);

    connect(ui->btBrowseOld, &QPushButton::clicked, this, &This::browseForOld);
    connect(ui->btBrowseNew, &QPushButton::clicked, this, &This::browseForNew);
}

FmMain::~FmMain()
{
    delete ui;
}

void FmMain::browseFor(QLineEdit* ed)
{
    filedlg::Filters filters {
        { L"TTF fonts", L"*.ttf" }
    };
    auto text = filedlg::open(
                    this,
                    L"Browse for font",
                    filters,
                    L".ttf",
                    filedlg::AddToRecent::NO);
    if (!text.empty()) {
        ed->setText(QString::fromStdWString(text));
    }
}


void FmMain::browseForOld()
{
    browseFor(ui->edOld);
}

void FmMain::browseForNew()
{
    browseFor(ui->edNew);
}
