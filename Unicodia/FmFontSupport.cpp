#include "FmFontSupport.h"
#include "ui_FmFontSupport.h"

FmFontSupport::FmFontSupport(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FmFontSupport)
{
    ui->setupUi(this);
}

FmFontSupport::~FmFontSupport()
{
    delete ui;
}
