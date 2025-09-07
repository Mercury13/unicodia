#include "FmPopupChar.h"
#include "ui_FmPopupChar.h"

// Project-local
#include "u_Popup.h"

#include "UcAutoDefines.h"

FmPopupChar::FmPopupChar(QWidget *parent) :
    Super(parent),
    ui(new Ui::FmPopupChar)
{
    ui->setupUi(this);
    ui->wiSample->hideHead();
}

FmPopupChar::~FmPopupChar()
{
    delete ui;
}

auto FmPopupChar::setCp(const uc::Cp& cp) -> This&
{
    /// @todo [urgent] setCp: what to do?
    char buf[30];
    snprintf(buf, std::size(buf), "U+%04X", cp.subj.uval());
    ui->lbCode->setText(buf);

    return *this;
}
