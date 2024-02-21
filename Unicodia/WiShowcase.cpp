#include "WiShowcase.h"
#include "ui_WiShowcase.h"

// Unicode
#include "UcData.h"

// Project-local
#include "MyWiki.h"
#include "Skin.h"

WiShowcase::WiShowcase(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WiShowcase)
{
    ui->setupUi(this);
    connect(ui->btCopy, &QPushButton::clicked, this, &This::btCopyClicked);
    connect(ui->btCopyEx, &QPushButton::clicked, this, &This::btCopyExClicked);
    connect(ui->lbCharCode, &QLabel::linkActivated, this, &This::lbCharCodeClicked);
    connect(ui->wiOsStyle, &WiOsStyle::linkActivated, this, &This::linkActivated);

    { // Copy ex
        auto font = ui->btCopyEx->font();
        QFontMetrics metrics(font);
        auto sz = metrics.horizontalAdvance("+000000");
        ui->btCopyEx->setFixedWidth(sz);
    }
}

WiShowcase::~WiShowcase()
{
    delete ui;
}

WiSample* WiShowcase::wiSample() const
    { return ui->wiSample; }

void WiShowcase::translateMe()
{
    Form::translateMe();
    loc::translateForm(ui->wiSample);
}

void WiShowcase::btCopyClicked()
    { emit charCopied(ui->btCopy); }

void WiShowcase::btCopyExClicked()
    { emit advancedCopied(ui->btCopyEx); }

void WiShowcase::lbCharCodeClicked(const QString& link)
    { emit linkActivated(ui->lbCharCode, link); }

void WiShowcase::setSilent(uc::MaybeChar ch)
{
    fShownCp = ch;
}

void WiShowcase::set(uc::MaybeChar ch, FontMatch& fonts)
{
    fShownCp = ch;

    // Code
    char buf[300];
    { QString ucName;
        uc::sprintUPLUS(buf, ch.code);
        mywiki::appendCopyable(ucName, buf, "' style='" STYLE_BIGCOPY);
        ui->lbCharCode->setText(ucName);
    }

    if (ch) {
        // Character
        if (ch->category().upCat == uc::UpCategory::MARK) {
            ui->btCopyEx->setText("+25CC");
            ui->btCopyEx->show();
        } else if (ch->isVs16Emoji()) {
            ui->btCopyEx->setText("+VS16");
            ui->btCopyEx->show();
        } else {
            ui->btCopyEx->hide();
        }

        // OS char
        ui->wiOsStyle->setCp(*ch, fonts);
    } else {
        // No character
        ui->wiOsStyle->setEmptyCode(ch.code);
        ui->btCopyEx->hide();
    }
}
