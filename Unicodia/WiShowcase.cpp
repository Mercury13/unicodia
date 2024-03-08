#include "WiShowcase.h"
#include "ui_WiShowcase.h"

// Qt
#include <QTextBrowser>

// Libs
#include "u_Strings.h"

// Unicode
#include "UcData.h"
#include "UcCp.h"
#include "UcClipboard.h"

// L10n
#include "LocDic.h"

// Wiki
#include "MyWiki.h"

// Project-local
#include "Skin.h"

template struct TinyOpt<char32_t>;


///// ShownObj /////////////////////////////////////////////////////////////////

/// Static_assert here for compile speed
static_assert(ec::size<ShownClass>()== std::variant_size_v<detail::ShownObjFather>,
              "ShownClass should correspond to ShownObj");

TinyOpt<char32_t> ShownObj::maybeCp() const
    { return std::get_if<char32_t>(this); }

TinyOpt<const uc::LibNode*> ShownObj::maybeNode() const
    { return std::get_if<const uc::LibNode*>(this); }

char32_t ShownObj::forceCp() const
    { return std::get<char32_t>(*this); }

const uc::LibNode* ShownObj::forceNode() const
    { return std::get<const uc::LibNode*>(*this); }


///// WiShowcase ///////////////////////////////////////////////////////////////

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
    ui->btCopyEx->hide();
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

bool WiShowcase::doCopy(uc::CopiedChannel channel)
{
    switch (fShownObj.clazz()) {
    case ShownClass::NONE: break;
    case ShownClass::CP:
        uc::copyCp(fShownObj.forceCp(), channel);
        return true;
    case ShownClass::LIB:
        return uc::copyNode(fShownObj.forceNode());
    }
    return false;
}


void WiShowcase::btCopyClicked()
{
    if (doCopy(uc::CopiedChannel::CHAR))
        emit copiedPopped(ui->btCopy);
}

void WiShowcase::btCopyExClicked()
{
    if (doCopy(uc::CopiedChannel::SAMPLE))
        emit copiedPopped(ui->btCopyEx);
}

void WiShowcase::lbCharCodeLinkActivated(const QString& link)
    { emit linkActivated(ui->lbCharCode, link); }

void WiShowcase::lbStyleHelpLinkActivated(const QString& link)
    { emit linkActivated(ui->lbStyleHelp, link); }

void WiShowcase::setSilent(char32_t ch)
{
    fShownObj = ch;
}

namespace {

    void setWiki(QTextBrowser* view, const QString& text)
    {
        view->setText(text);
        // I won’t hack: my underline is lower, and it’s nice.
        // What to do with that one — IDK.
        //mywiki::hackDocument(view->document());
    }

}   // anon namespace


void WiShowcase::redrawViewerCp(char32_t code, QTextBrowser* viewer)
{
    if (auto ch = uc::cpsByCode[code]) {
        // Normal CP
        QString text = mywiki::buildHtml(*ch);
        setWiki(viewer, text);
    } else if (uc::isNonChar(code)) {
        // Non-character
        QString text = mywiki::buildNonCharHtml(code);
        setWiki(viewer, text);
    } else {
        // Vacant (reserved) codepoint
        auto color = palette().color(QPalette::Disabled, QPalette::WindowText);
        QString text = mywiki::buildVacantCpHtml(code, color);
        setWiki(viewer, text);
    }
}


void WiShowcase::redrawViewerNode(const uc::LibNode& node, QTextBrowser* viewer)
{
    if (node.value.empty()) {
        // Folder
        auto color = palette().color(QPalette::Disabled, QPalette::WindowText);
        QString s = mywiki::buildLibFolderHtml(node, color);
        setWiki(viewer, s);
    } else {
        auto& parent = uc::libNodes[node.iParent];
        QString s = mywiki::buildHtml(node, parent);
        setWiki(viewer, s);
    }
}


void WiShowcase::set(
        char32_t code,
        QTextBrowser* viewer,
        FontMatch& fonts,
        const uc::GlyphStyleSets& glyphSets)
{
    fShownObj = code;
    auto ch = uc::cpsByCode[code];

    ui->btCopy->setEnabled(true);

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
    redrawViewerCp(code, viewer);
}


void WiShowcase::set(
        const uc::LibNode& node,
        QTextBrowser* viewer,
        FontMatch& fonts)
{
    fShownObj = &node;
    // Hide these widgets
    ui->wiGlyphStyle->hide();
    ui->btCopyEx->hide();
    // Depending on nodes
    if (node.value.empty()) {
        // Folder
        ui->wiSample->showNothing();
        ui->lbCharCode->clear();
        ui->btCopy->setEnabled(false);
        ui->wiOsStyle->setNothing();
    } else {
        // Show sample
        auto len = node.value.length();
        auto emojiDraw = node.emojiDraw();
        bool needGraph = (len > 1 && emojiDraw != uc::EmojiDraw::FORCE_TEXT);
        if (needGraph || node.flags.have(uc::Lfg::GRAPHIC_EMOJI)) {
            ui->wiSample->showEmoji(node.value);
        } else if (auto cp = uc::cpsByCode[node.value[0]]) {
            // Library uses default/empty settings
            ui->wiSample->showCp(*cp, emojiDraw, uc::GlyphStyleSets::EMPTY);
        } else  {
            ui->wiSample->showNothing();
        }

        if (node.flags.have(uc::Lfg::GRAPHIC_EMOJI)) {
            ui->wiOsStyle->setEmojiText(node.value, fonts);
        } else {
            ui->wiOsStyle->setCustomText(node.value, fonts);
        }

        // Codes
        char buf[150], shortBuf[50];
        node.sprintPlus(buf);
        switch (node.value.length()) {
        case 0:     // never happens
        case 1:
        case 2:
            strcpy_s(shortBuf, buf);
            break;
        default:
            uc::sprint(shortBuf, node.value[0]);
            strcat_s(shortBuf, "+…");
        }
        QString ucText;
        if (shortBuf[0] != '\0') {
            ucText = "U+";
            mywiki::appendCopyable2(ucText, buf, shortBuf, "' style='" STYLE_BIGCOPY);
        }
        ui->lbCharCode->setText(ucText);
        ui->btCopy->setEnabled(true);
    }
    redrawViewerNode(node, viewer);
}


void WiShowcase::redrawSampleChar(const uc::GlyphStyleSets& glyphSets)
{
    switch (fShownObj.clazz()) {
    case ShownClass::CP:
        if (auto cp = uc::cpsByCode[fShownObj.forceCp()]) {
            ui->wiSample->showCp(*cp, EMOJI_DRAW, glyphSets);
            radioGlyphStyle.set(glyphSets[fCurrChannel]);
            break;
        }
        // else
        [[fallthrough]];
    case ShownClass::LIB:
        // Unused in LIB mode, maybe someday
        break;
    case ShownClass::NONE:
        ui->wiSample->showNothing();
    }
}


void WiShowcase::glyphStyleClicked()
{
    emit glyphStyleChanged(fCurrChannel, radioGlyphStyle.get());
}
