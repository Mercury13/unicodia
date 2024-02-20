#include "WiSample.h"
#include "ui_WiSample.h"

// Libs
#include "u_Qstrings.h"

// Data
#include "UcData.h"
#include "Skin.h"


WiSample::WiSample(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WiSample)
{
    ui->setupUi(this);

    // Sample
    ui->pageSampleCustom->init();
}

WiSample::~WiSample()
{
    delete ui;
}


bool WiSample::setFont(const uc::Cp& ch, const uc::FontMatcher& matcher)
{
    auto font = ch.font(matcher);
    if (!font) {
        clearSample();
        return false;
    }
    auto qfont = font->get(uc::FontPlace::SAMPLE, FSZ_BIG, false, ch.subj);
    ui->lbSample->setFont(qfont);
    return true;
}


void WiSample::setAbbrFont(const uc::Cp& ch)
{
    if (ch.block().flags.have(uc::Bfg::BIG_CONTROLS)) {
        if (setFont(ch, match::MainFont::INST)) {
            ui->lbSample->setText(" ");
            // Show just to check layout, then revert again
            auto prevPage = ui->stackSample->currentIndex();
            ui->stackSample->setCurrentWidget(ui->pageSampleQt);
            ui->stackSample->setCurrentIndex(prevPage);
        }
    } else {
        clearSample();
    }
}


void WiSample::drawWithQt(
        const uc::Cp& ch, uc::EmojiDraw emojiDraw,
        const uc::GlyphStyleSets& glyphSets)
{
    ui->pageSampleCustom->setNormal();
    setFont(ch, match::Normal::INST);

    // Sample char
    ui->stackSample->setCurrentWidget(ui->pageSampleQt);
    auto proxy = ch.sampleProxy(uc::ProxyType::EXTENDED, emojiDraw, glyphSets);
    // Color
    if (ch.isTrueSpace()) {
        auto c = palette().text().color();
        c.setAlpha(ALPHA_SPACE);
        QString css = "color: " + c.name(QColor::HexArgb) + ';';
        ui->lbSample->setStyleSheet(css);
    } else {
        ui->lbSample->setStyleSheet(str::toQ(proxy.styleSheet));
    }
    ui->lbSample->setText(proxy.text);
}


void WiSample::clearSample()
{
    ui->lbSample->clear();
    ui->lbSample->setStyleSheet({});
    ui->lbSample->setFont(QFont());
}


void WiSample::showCp(
        const uc::Cp& ch, uc::EmojiDraw emojiDraw,
        const uc::GlyphStyleSets& glyphSets)
{
    if (ch.isTrueSpace()) {
            auto palette = this->palette();
            auto color = palette.windowText().color();
            color.setAlpha(ALPHA_SPACE);
            palette.setColor(QPalette::WindowText, color);
            ui->lbSample->setPalette(palette);
    } else {
        ui->lbSample->setPalette(this->palette());
    }

    // Sample char
    auto method = ch.drawMethod(emojiDraw, glyphSets);
    switch (method) {
    case uc::DrawMethod::CUSTOM_CONTROL:
        setAbbrFont(ch);
        ui->stackSample->setCurrentWidget(ui->pageSampleCustom);
        ui->pageSampleCustom->setCustomControl(ch.subj);
        break;
    case uc::DrawMethod::ABBREVIATION:
        setAbbrFont(ch);
        ui->stackSample->setCurrentWidget(ui->pageSampleCustom);
        ui->pageSampleCustom->setAbbreviation(ch.abbrev());
        break;
    case uc::DrawMethod::SPACE: {
            clearSample();
            ui->stackSample->setCurrentWidget(ui->pageSampleCustom);
            auto font = ch.font(match::Normal::INST);
            auto qfont = font->get(uc::FontPlace::SAMPLE, FSZ_BIG, false, ch.subj);
            ui->lbSample->clear();
            ui->lbSample->setFont(qfont);
            ui->pageSampleCustom->setSpace(qfont, ch.subj);
        } break;
    case uc::DrawMethod::VERTICAL_CW:
    case uc::DrawMethod::VERTICAL_CCW: {
            // set dummy font
            auto font = ch.font(match::Normal::INST);
            ui->lbSample->setText(QString{});
            QFont qfont = font->get(uc::FontPlace::SAMPLE, FSZ_BIG, false, ch.subj);
            ui->lbSample->setFont(qfont);
            // set vertical mode
            auto angle = (method == uc::DrawMethod::VERTICAL_CW) ? ROT_CW : ROT_CCW;
            // EMPTY: we want text anyway
            auto proxy = ch.sampleProxy(uc::ProxyType::EXTENDED, emojiDraw, uc::GlyphStyleSets::EMPTY);
            // Vertical fonts do not have special stylesheets
            ui->pageSampleCustom->setVertical(qfont, proxy.text, angle);
            ui->stackSample->setCurrentWidget(ui->pageSampleCustom);
        } break;
    case uc::DrawMethod::SAMPLE:
    case uc::DrawMethod::MARCHEN:   // same as Sample, Marchen is drawn in table only
    case uc::DrawMethod::SAMPLED_CONTROL:  // same as Sample, maybe we’ll make them somehow
        drawWithQt(ch, emojiDraw, glyphSets);
        break;
    case uc::DrawMethod::SVG_EMOJI:
        clearSample();
        ui->stackSample->setCurrentWidget(ui->pageSampleCustom);
        ui->pageSampleCustom->setEmoji(ch.subj);
        break;
    }
}


void WiSample::showNothing()
{
    ui->stackSample->setCurrentWidget(ui->pageSampleQt);
    ui->lbSample->setText({});
}


void WiSample::showEmoji(std::u32string_view text)
{
    ui->stackSample->setCurrentWidget(ui->pageSampleCustom);
    ui->pageSampleCustom->setEmoji(text);
}
