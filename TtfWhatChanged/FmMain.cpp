#include "FmMain.h"
#include "ui_FmMain.h"

// Qt
#include <QPushButton>
#include <QMessageBox>

// Qt ex
#include "i_OpenSave.h"

// Libs
#include "u_EcArray.h"

// Fonts
#include "MemFont.h"
#include "MemWhatChanged.h"

FmMain::FmMain(QWidget *parent)
    : Super(parent)
    , ui(new Ui::FmMain)
{
    ui->setupUi(this);

    connect(ui->btBrowseOld, &QPushButton::clicked, this, &This::browseForOld);
    connect(ui->btBrowseNew, &QPushButton::clicked, this, &This::browseForNew);
    connect(ui->btGo, &QPushButton::clicked, this, &This::go);
}

FmMain::~FmMain()
{
    delete ui;
}

void FmMain::browseFor(QLineEdit* ed)
{
    filedlg::Filters filters {
        { L"TTF fonts", L"*.ttf" },
        filedlg::ALL_FILES,
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

namespace {

    ec::Array<const char*, mf::ChangeAction> actionNames {
        "add", "del", "chg", "BAD!!!!!!!!",
    };

}   // anon namespace

void FmMain::go()
{
    MemFont mfOld;
    if (!mfOld.load(ui->edOld->text())) {
        QMessageBox::critical(this, "Error", "Cannot load old file");
        return;
    }

    MemFont mfNew;
    if (!mfNew.load(ui->edNew->text())) {
        QMessageBox::critical(this, "Error", "Cannot load new file");
        return;
    }

    ec::Array<bool, mf::ChangeAction> filter {
        ui->chkAdd->isChecked(),
        ui->chkDel->isChecked(),
        ui->chkChg->isChecked(),
        true,
    };
    auto end = filter.itOf(mf::ChangeAction::BAD);
    auto where = std::find(filter.begin(), end, true);
    if (where == end) {
        QMessageBox::critical(this, "Error", "Check at least one event: add/del/chg");
        return;
    }

    ec::Array<unsigned, mf::ChangeAction> counts { 0u, 0u, 0u, 0u };

    auto changeList = mf::whatChanged(mfOld, mfNew);
    QString text;
    for (auto& q : changeList) {
        if (filter[q.action]) {
            char buf[40];
            snprintf(buf, std::size(buf), "%04X %s *%u\n",
                     int(q.cp), actionNames[q.action],
                     ++counts[q.action]);
            text += buf;
        }
    }
    ui->memoResult->setPlainText(text);
}
