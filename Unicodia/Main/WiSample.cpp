#include "WiSample.h"
#include "ui_WiSample.h"

// Data
#include "UcData.h"
#include "Skin.h"

// Painting
#include "CharPaint/emoji.h"

// L10n
#include "LocDic.h"


WiSample::WiSample(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WiSample)
{
    ui->setupUi(this);

    // Sample
    ui->pageSampleCustom->init();

    connect(ui->lbSampleTitle, &QLabel::linkActivated, this, &This::labelLinkActivated);
}

WiSample::~WiSample()
{
    delete ui;
}


void WiSample::switchToLib()
{
    isLib = true;
}


void WiSample::labelLinkActivated(const QString& link)
{
    emit linkActivated(qobject_cast<QWidget*>(sender()), link);
}


const uc::Font* WiSample::setFont(const uc::Cp& ch, const uc::FontMatcher& matcher)
{
    auto font = ch.font(matcher);
    if (!font) {
        clearSample();
        return nullptr;
    }
    Flags<uc::FontGetFg> fgs;
    if (ch.flags.have(uc::Cfg::DYN_SYSTEM_TOFU))
        fgs |= uc::FontGetFg::KNOWN_TOFU;
    auto qfont = font->get(uc::FontPlace::SAMPLE, FSZ_BIG, fgs, &ch);
    ui->lbSample->setFont(qfont);
    return font;
}


void WiSample::setAbbrFont(const uc::Cp& ch)
{
    if (ch.block().flags.have(uc::Bfg::BIG_CONTROLS)) {
        if (auto font = setFont(ch, match::MainFont::INST)) {
            showBriefly(font);
        }
    } else {
        clearSample();
    }
}


const uc::Font* WiSample::drawWithQt(
        const uc::Cp& ch, uc::EmojiDraw emojiDraw,
        const uc::GlyphStyleSets& glyphSets,
        bool haltIfGraphic)
{
    ui->pageSampleCustom->setNormal();
    auto font = setFont(ch, match::Normal::INST);
    if (haltIfGraphic && font && font->flags.have(uc::Ffg::GRAPHIC_SAMPLE)) {
        clearSample();
        return font;
    }

    // Sample char
    ui->stackSample->setCurrentWidget(ui->pageSampleQt);
    shownBrieflyFont = font;
    auto proxy = ch.sampleProxy(uc::ProxyType::EXTENDED, emojiDraw, glyphSets);
    // Color
    if (ch.isTrueSpace()) {
        auto c = palette().text().color();
        c.setAlpha(ALPHA_SPACE);
        QString css = "color: " + c.name(QColor::HexArgb) + ';';
        ui->lbSample->setStyleSheet(css);
    } else {
        ui->lbSample->setStyleSheet(proxy.styleSheet);
    }
    ui->lbSample->setText(proxy.text);
    return nullptr;
}


void WiSample::clearSample()
{
    ui->lbSample->clear();
    ui->lbSample->setStyleSheet({});
    ui->lbSample->setFont(QFont());
    shownBrieflyFont = nullptr;
}


void WiSample::showBriefly(const uc::Font* font)
{
    ui->lbSample->setText(" ");
    if (shownBrieflyFont != font) {
        ui->stackSample->setCurrentWidget(ui->pageSampleQt);
        shownBrieflyFont = font;
    }
}


QFont WiSample::showCpBriefly(const uc::Cp& ch)
{
    auto font = ch.font(match::Normal::INST);
    auto qfont = font->get(uc::FontPlace::SAMPLE, FSZ_BIG, uc::FontGetFg::FAST, &ch);
    if (!font->flags.have(uc::Ffg::GRAPHIC_SAMPLE)) {
        ui->lbSample->setFont(qfont);
        showBriefly(font);
    } else {
        clearSample();
    }
    return qfont;
}


void WiSample::showCp(
        const uc::Cp& ch, uc::EmojiDraw emojiDraw,
        const uc::GlyphStyleSets& glyphSets,
        bool isMainEmoji)
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
        headToSample();
        break;
    case uc::DrawMethod::VIRTUAL_VIRAMA:
        setAbbrFont(ch);
        ui->stackSample->setCurrentWidget(ui->pageSampleCustom);
        ui->pageSampleCustom->setVirtualVirama(ch.subj);
        headToSample();
        break;
    case uc::DrawMethod::ABBREVIATION:
        setAbbrFont(ch);
        ui->stackSample->setCurrentWidget(ui->pageSampleCustom);
        ui->pageSampleCustom->setAbbreviation(ch.abbrev());
        headToSample();
        break;
    case uc::DrawMethod::SPACE: {
            auto qfont = showCpBriefly(ch);
            ui->stackSample->setCurrentWidget(ui->pageSampleCustom);
            ui->pageSampleCustom->setSpace(qfont, ch.subj);
            headToSample();
        } break;
    case uc::DrawMethod::VERTICAL_CW:
    case uc::DrawMethod::VERTICAL_CCW: {
            auto qfont = showCpBriefly(ch);
            // set vertical mode
            auto angle = (method == uc::DrawMethod::VERTICAL_CW) ? ROT_CW : ROT_CCW;
            // EMPTY: we want text anyway
            auto proxy = ch.sampleProxy(uc::ProxyType::EXTENDED, emojiDraw, uc::GlyphStyleSets::EMPTY);
            // Vertical fonts do not have special stylesheets
            ui->pageSampleCustom->setVertical(qfont, proxy, angle);
            ui->stackSample->setCurrentWidget(ui->pageSampleCustom);
            headToSample();
        } break;
    case uc::DrawMethod::SAMPLED_CONTROL: {
            auto qfont = showCpBriefly(ch);
            auto proxy = ch.sampleProxy(uc::ProxyType::EXTENDED, emojiDraw, uc::GlyphStyleSets::EMPTY);
            ui->stackSample->setCurrentWidget(ui->pageSampleCustom);
            ui->pageSampleCustom->setSampledControl(qfont, proxy);
            headToSample();
        } break;
    case uc::DrawMethod::SAMPLE:
        // Library never shows text+emoji
        if (!isLib && ch.isVs16Emoji()) {
            ui->stackSample->setCurrentWidget(ui->pageSampleCustom);
            ui->pageSampleCustom->setCharOverEmoji(ch.subj);
            headToLib(ch.subj);
            break;
        }
        [[fallthrough]];
    case uc::DrawMethod::MARCHEN: {  // Sample and Marchen differ in table
            auto retFont = drawWithQt(ch, emojiDraw, glyphSets, true);
            if (retFont) {
                auto proxy = ch.sampleProxy(uc::ProxyType::EXTENDED, emojiDraw, glyphSets);
                ui->stackSample->setCurrentWidget(ui->pageSampleCustom);
                Flags<uc::FontGetFg> fgs;
                if (ch.flags.have(uc::Cfg::DYN_SYSTEM_TOFU))
                    fgs |= uc::FontGetFg::KNOWN_TOFU;
                auto qfont = retFont->get(uc::FontPlace::SAMPLE, FSZ_BIG, fgs, &ch);
                ui->pageSampleCustom->setGraphicSample(qfont, proxy);
            }
            headToSample();
        } break;
    case uc::DrawMethod::SVG_EMOJI:
        clearSample();
        ui->stackSample->setCurrentWidget(ui->pageSampleCustom);
        ui->pageSampleCustom->setEmoji(ch.subj);
        headToLibIf(ch.subj, isMainEmoji);
        break;
    }
}


void WiSample::showNothing()
{
    ui->stackSample->setCurrentWidget(ui->pageSampleQt);
    ui->lbSample->setText({});
    headToSample();
}


void WiSample::showEmoji(std::u32string_view text, bool isMainEmoji)
{
    ui->stackSample->setCurrentWidget(ui->pageSampleCustom);
    ui->pageSampleCustom->setEmoji(text);
    headToLibIf(text, isMainEmoji);
}

void WiSample::translateMe()
{
    sSample = loc::get("Sample.Sample");
    sToLib = loc::get("Sample.ToLib");
    // Wait for reset after translation
    headMode = HeadMode::NONE;
}


void WiSample::headToSample()
{
    if (headMode == HeadMode::SAMPLE)
        return;
    headMode = HeadMode::SAMPLE;
    ui->lbSampleTitle->setText(sSample);
}


void WiSample::headToLib(char32_t subj)
{
    headMode = HeadMode::TOLIB;
    char buf[200];
    snprintf(buf, std::size(buf), "<a href='glc:%04X' style='" STYLE_INET "'>", int(subj));
    ui->lbSampleTitle->setText(buf + sToLib + "</a>");
}


void WiSample::headToLibIf(char32_t subj, bool isMainEmoji)
{
    if (isMainEmoji) {
        headToSample();
    } else {
        headToLib(subj);
    }
}


void WiSample::headToLibIf(std::u32string_view text, bool isMainEmoji)
{
    if (!isMainEmoji) {
        if (auto c = EmojiPainter::getCp(text)) {
            /// @todo [future] We rely here on the fact that Library has no complex emoji
            ///                outside main folder
            headToLib(c.cp);
            return;
        }
    }
    headToSample();
}
