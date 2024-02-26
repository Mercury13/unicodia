#include "WiShowcase.h"
#include "ui_WiShowcase.h"

// Libs
#include "u_Strings.h"

// Unicode
#include "UcData.h"

// L10n
#include "LocDic.h"

// Project-local
#include "MyWiki.h"
#include "Skin.h"

template struct TinyOpt<char32_t>;

WiShowcase::WiShowcase(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WiShowcase)
{
    ui->setupUi(this);
    connect(ui->btCopy, &QPushButton::clicked, this, &This::btCopyClicked);
    connect(ui->btCopyEx, &QPushButton::clicked, this, &This::btCopyExClicked);
    connect(ui->lbCharCode, &QLabel::linkActivated, this, &This::lbCharCodeLinkActivated);
    connect(ui->wiOsStyle, &WiOsStyle::linkActivated, this, &This::linkActivated);

    { // Copy ex
        auto font = ui->btCopyEx->font();
        QFontMetrics metrics(font);
        auto sz = metrics.horizontalAdvance("+000000");
        ui->btCopyEx->setFixedWidth(sz);
    }

    // Glyph styles
    radioGlyphStyle.setRadio(0, ui->radioStyle0);
    radioGlyphStyle.setRadio(1, ui->radioStyle1);
    connect(ui->radioStyle0, &QRadioButton::clicked, this, &This::glyphStyleClicked);
    connect(ui->radioStyle1, &QRadioButton::clicked, this, &This::glyphStyleClicked);
    connect(ui->lbStyleHelp, &QLabel::linkActivated, this, &This::lbStyleHelpLinkActivated);

    ui->wiGlyphStyle->hide();
}

WiShowcase::~WiShowcase()
{
    delete ui;
}

void WiShowcase::translateMe()
{
    Form::translateMe();
    loc::translateForm(ui->wiSample);
}

void WiShowcase::btCopyClicked()
    { emit charCopied(ui->btCopy); }

void WiShowcase::btCopyExClicked()
    { emit advancedCopied(ui->btCopyEx); }

void WiShowcase::lbCharCodeLinkActivated(const QString& link)
    { emit linkActivated(ui->lbCharCode, link); }

void WiShowcase::lbStyleHelpLinkActivated(const QString& link)
    { emit linkActivated(ui->lbStyleHelp, link); }

void WiShowcase::setSilent(char32_t ch)
{
    fShownObj = ch;
}

void WiShowcase::set(char32_t code, FontMatch& fonts, const uc::GlyphStyleSets& glyphSets)
{
    fShownObj = code;
    auto ch = uc::cpsByCode[code];

    // Code
    char buf[300];
    { QString ucName = "U+";
        uc::sprint(buf, code);
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

        // Style channel
        if (auto& var = ch->styleChannel()) {
            // We never have three things here
            auto goodFont = ui->wiGlyphStyle->font();
            auto badFont = goodFont;
            badFont.setStrikeOut(true);
            auto prefix = str::cat("GlyphVar.", var.name, '.');
            for (unsigned i = 0; i < var.count; ++i) {
                auto button = radioGlyphStyle.buttonAt(i);
                button->setText(loc::get(str::cat(prefix, char('0' + i))));
                auto flag = uc::Cfg::G_STYLE_0 << i;
                button->setFont(ch->flags.have(flag) ? goodFont : badFont);
            }
            fCurrChannel = ch->ecStyleChannel();
            snprintf(buf, std::size(buf), "pgs:%d", static_cast<int>(ch->ecStyleChannel()));
            ui->lbStyleHelp->setText(qPopupLinkNoLoc("&nbsp;?&nbsp;", buf));
            ui->wiGlyphStyle->show();
        } else {
            ui->wiGlyphStyle->hide();
        }
    } else {
        // No character
        ui->wiOsStyle->setEmptyCode(code);
        ui->btCopyEx->hide();
        fCurrChannel = uc::EcGlyphStyleChannel::NONE;
    }

    redrawSampleChar(glyphSets);
}


void WiShowcase::redrawSampleChar(const uc::GlyphStyleSets& glyphSets)
{
    switch (toUnderlying(fShownObj)) {
    case ShownClass::CP:
        if (auto cp = uc::cpsByCode[*shownCode()]) {
            ui->wiSample->showCp(*cp, EMOJI_DRAW, glyphSets);
            radioGlyphStyle.set(glyphSets[fCurrChannel]);
            break;
        }
        // else
        [[fallthrough]];
    case ShownClass::NONE:
        ui->wiSample->showNothing();
    }
}


void WiShowcase::glyphStyleClicked()
{
    emit glyphStyleChanged(fCurrChannel, radioGlyphStyle.get());
}

TinyOpt<char32_t> WiShowcase::shownCode() const
    { return std::get_if<char32_t>(&fShownObj); }
