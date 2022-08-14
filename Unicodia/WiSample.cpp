#include "WiSample.h"
#include "ui_WiSample.h"

WiSample::WiSample(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WiSample)
{
    ui->setupUi(this);
}

WiSample::~WiSample()
{
    delete ui;
}
