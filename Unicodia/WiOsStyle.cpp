#include "WiOsStyle.h"
#include "ui_WiOsStyle.h"

// Libs
#include "u_Qstrings.h"

// Unicode
#include "UcData.h"
#include "UcSearch.h"
#include "FontMatch.h"

// L10n
#include "LocDic.h"

// Project-local
#include "Skin.h"


namespace {
    constinit const char16_t U16_TOFU[] { 0xD807, 0xDEE0, 0 };     // Makasar Ka
}

WiOsStyle::WiOsStyle(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WiOsStyle),
    fontBig(str::toQ(FAM_DEFAULT), FSZ_BIG),
    fontTofu(str::toQ(FAM_TOFU), FSZ_BIG)
{
    ui->setupUi(this);

    // OS style
    fontTofu.setStyleStrategy(fst::TOFU);

    auto& font = uc::fontInfo[0];
    ui->lbOs->setFont(font.get(uc::FontPlace::SAMPLE, FSZ_BIG, false, NO_TRIGGER));

    connect(ui->lbOsTitle, &QLabel::linkActivated, this, &This::slotLinkActivated);
}

WiOsStyle::~WiOsStyle()
{
    delete ui;
}

void WiOsStyle::setEmptyCode(char32_t code)
{
    ui->lbOs->setText({});
    const auto& osTitle = loc::get(
            uc::isNonChar(code) ? "Prop.Os.NonChar" : "Prop.Os.Empty");
    ui->lbOsTitle->setText(osTitle);
}


QString qPopupLinkNoLoc(
        const QString& text, const char* target)
{
    return QString("<a href='") + target + "' style='" STYLE_POPUP "'>"
            + text + "</a>";
}


QString qPopupLink(
        std::string_view locKey, const char* target)
{
    return qPopupLinkNoLoc(loc::get(locKey), target);
}


void WiOsStyle::setTofu()
{
    ui->lbOs->setFont(fontTofu);
    ui->lbOs->setText(QString::fromUtf16(U16_TOFU));
    ui->lbOsTitle->setText(qPopupLink("Prop.Os.Tofu", "pt:tofu"));
}


void WiOsStyle::setCpEx(const uc::Cp& ch, const QString& display, FontMatch& fontMatch)
{
    const bool wantSysFont = !ch.isDefaultIgnorable() && ch.isGraphical();
    if (wantSysFont) {
        if (display.isEmpty()) {
            ui->lbOs->setFont(fontBig);
            ui->lbOs->setText({});
            ui->lbOsTitle->setText(loc::get("Prop.Os.Invisible"));
        } else {
            QFontDatabase::WritingSystem ws = ch.scriptEx().qtCounterpart;
            auto font = fontMatch.sysFontFor(ch, ws, FSZ_BIG);
            if (font) {
                char buf[300];
                ui->lbOs->setFont(*font);
                ui->lbOs->setText(display);
                snprintf(buf, std::size(buf),
                        "<a href='pf:%d/%d' style='" STYLE_POPUP "'>",
                        ch.subj.val(), static_cast<int>(ws));
                ui->lbOsTitle->setText(
                    buf + font->family().toHtmlEscaped() + "</a>");
            } else {
                setTofu();
            }
        }
    } else {
        ui->lbOs->setFont(fontBig);
        ui->lbOs->setText({});
        ui->lbOsTitle->setText(loc::get("Prop.Os.Invisible"));
    }
}


void WiOsStyle::setCp(const uc::Cp& ch, FontMatch& fontMatch)
{
    setCpEx(ch, ch.osProxy(), fontMatch);
}


void WiOsStyle::slotLinkActivated(const QString& url)
{
    auto snd = qobject_cast<QWidget*>(sender());
    if (!snd)
        snd = this;
    emit linkActivated(snd, url);
}


void WiOsStyle::setNothing()
{
    ui->lbOs->clear();
    ui->lbOsTitle->setText(loc::get("Prop.Os.Style"));
}


void WiOsStyle::setEmojiText(std::u32string_view text, FontMatch& fontMatch)
{
    if (text.empty()) {
        setNothing();
        return;
    }

    // 0x1F300 = very old emoji from Webdings
    constexpr char32_t SOME_EMOJI = 0x1F300;
    auto char0 = text[0];
    std::optional<QFont> font;
    if (char0 <= 0xFFFF) {
        // BMP emoji, really old
        font = fontMatch.sysFontFor(SOME_EMOJI, FSZ_BIG);
    } else {
        // SMP emoji
        font = fontMatch.sysFontFor(char0, FSZ_BIG);
    }
    if (font) {
        ui->lbOs->setFont(*font);
        ui->lbOs->setText(str::toQ(text));
        ui->lbOsTitle->setText(font->family());
    } else {
        setTofu();
    }
}


void WiOsStyle::setCustomText(
        std::u32string_view text, FontMatch& fontMatch)
{
    if (text.empty()) {
        setNothing();
    } else {
        auto c0 = text[0];
        auto cp = uc::cpsByCode[c0];
        if (cp) {
            if (text.length() == 1) {
                setCp(*cp, fontMatch);
            } else {
                setCpEx(*cp, str::toQ(text), fontMatch);
            }
        } else {
            setEmptyCode(c0);
        }
    }
}
