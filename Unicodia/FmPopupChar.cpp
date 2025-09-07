#include "FmPopupChar.h"
#include "ui_FmPopupChar.h"

// Project-local
#include "u_Popup.h"

FmPopupChar::FmPopupChar(QWidget *parent) :
    QWidget(parent, WF_POPUP),
    ui(new Ui::FmPopupChar)
{
    ui->setupUi(this);
}

FmPopupChar::~FmPopupChar()
{
    delete ui;
}
