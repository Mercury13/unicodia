#include "FmMain.h"
#include "ui_FmMain.h"

#include "i_OpenSave.h"

#include "work.h"

FmMain::FmMain(QWidget *parent)
    : Super(parent)
    , ui(new Ui::FmMain)
{
    ui->setupUi(this);
    connect(ui->btBrowse, &QPushButton::clicked, this, &This::browseForDirectory);
    connect(ui->btGo, &QPushButton::clicked, this, &This::go);
}

FmMain::~FmMain()
{
    delete ui;
}

void FmMain::browseForDirectory()
{
    filedlg::Filter filter { L"SVG files", L"*.svg" };
    if (!filedlg::browseLineEdit(
            this, L"Choose directory", filter, L".svg",
            filedlg::BrowseMode::DIRECTORY, ui->edDirectory))
        return;
}

void FmMain::go()
{
    std::filesystem::path dir = ui->edDirectory->text().toStdWString();
    auto r = processDir(dir);
}
