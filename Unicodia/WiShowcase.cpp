#include "WiShowcase.h"
#include "ui_WiShowcase.h"

WiShowcase::WiShowcase(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WiShowcase)
{
    ui->setupUi(this);
    connect(ui->btCopy, &QPushButton::clicked, this, &This::btCopyClicked);
}

WiShowcase::~WiShowcase()
{
    delete ui;
}

QPushButton* WiShowcase::btCopyEx() const
    { return ui->btCopyEx; }

QLabel* WiShowcase::lbCharCode() const
    { return ui->lbCharCode; }

WiSample* WiShowcase::wiSample() const
    { return ui->wiSample; }

WiOsStyle* WiShowcase::wiOsStyle() const
    { return ui->wiOsStyle; }

void WiShowcase::translateMe()
{
    Form::translateMe();
    loc::translateForm(ui->wiSample);
}

void WiShowcase::btCopyClicked()
{
    emit charCopied(ui->btCopy);
}
