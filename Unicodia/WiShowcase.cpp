// My header
#include "WiShowcase.h"
#include "ui_WiShowcase.h"

// Qt
#include <QTextBrowser>
#include <QToolBar>

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

// Painters
#include "CharPaint/emoji.h"

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

std::optional<char32_t> ShownObj::digCp() const
{
    switch (clazz()) {
    case ShownClass::NONE:
        return std::nullopt;
    case ShownClass::CP:
        return forceCp();
    case ShownClass::LIB: {
            auto node = forceNode();
            if (!node)
                return std::nullopt;
            auto q = EmojiPainter::getCp(node->value);
            if (!q)
                return std::nullopt;
            return q.cp;
        }
    }
    __builtin_unreachable();
}


///// BangButton ///////////////////////////////////////////////////////////////


BangButton::BangButton(QWidget* parent) : Super(parent)
{
    installEventFilter(this);
}


bool BangButton::eventFilter(QObject *object, QEvent *event)
{
    if (object == this && event->type() == QEvent::MouseButtonPress) {
        if (!isEnabled()) {
            emit banged();
        }
    }
    return false;
}


///// WiShowcase ///////////////////////////////////////////////////////////////

WiShowcase::WiShowcase(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WiShowcase)
{
    ui->setupUi(this);
    connect(ui->wiSample, &WiSample::linkActivated, this, &This::linkActivated);

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

void WiShowcase::switchToLib()
{
    ui->wiSample->switchToLib();
}

void WiShowcase::translateMe()
{
    Form::translateMe();
    ui->wiSample->translateMe();
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
        if (hasGoto) {
            auto& font = uc::fontInfo[static_cast<int>(uc::EcFont::DEJAVU)];
            font.load(NO_TRIGGER);
            ucName += loc::Fmt(" <a href='gc:{1}' style='{2}; font-family:{3}'>↪</a>")
                              (buf)(STYLE_BIGINET)  // 1-2
                              (font.familiesComma().toStdString()).q();
        }
        ui->lbCharCode->setText(ucName);
    }

    if (ch) {
        // Character
        if (ch->category().upCat == uc::EcUpCategory::MARK) {
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
        ui->wiGlyphStyle->hide();
        fCurrChannel = uc::EcGlyphStyleChannel::NONE;
    }

    reenableFavs();
    redrawSampleChar(glyphSets);
    redrawViewerCp(code, viewer);
}


void WiShowcase::reset()
{
    ui->wiSample->showNothing();
    ui->lbCharCode->clear();
    ui->btCopy->setEnabled(false);
    ui->wiOsStyle->setNothing();
    ui->wiGlyphStyle->hide();
    fCurrChannel = uc::EcGlyphStyleChannel::NONE;
}


void WiShowcase::reenableFavs()
{
    if (!acFavs)
        return;
    auto dug = fShownObj.digCp();
    acFavs->setEnabled(dug.has_value());
    acFavs->setChecked(dug.has_value() && setFavs->contains(*dug));
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
        reset();
    } else {
        // Show sample
        auto len = node.value.length();
        auto emojiDraw = node.emojiDraw();
        bool needGraph = (len > 1 && emojiDraw != uc::EmojiDraw::FORCE_TEXT);
        bool isMainEmoji = node.flags.have(uc::Lfg::GRAPHIC_EMOJI);
        if (needGraph || isMainEmoji) {
            ui->wiSample->showEmoji(node.value, isMainEmoji);
        } else if (auto cp = uc::cpsByCode[node.value[0]]) {
            // Library uses default/empty settings
            ui->wiSample->showCp(*cp, emojiDraw, uc::GlyphStyleSets::EMPTY, isMainEmoji);
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
    reenableFavs();
    redrawViewerNode(node, viewer);
}


void WiShowcase::redrawSampleChar(const uc::GlyphStyleSets& glyphSets)
{
    switch (fShownObj.clazz()) {
    case ShownClass::CP:
        if (auto cp = uc::cpsByCode[fShownObj.forceCp()]) {
            ui->wiSample->showCp(*cp, EMOJI_DRAW, glyphSets, false);
            radioGlyphStyle.set(glyphSets[fCurrChannel]);
        } else {
            ui->wiSample->showNothing();
        }
        break;
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


void WiShowcase::syncGlyphStyle(
        const uc::GlyphStyleSets& glyphSets, uc::EcGlyphStyleChannel channel)
{
    // Are we showing this channel?
    if (channel == uc::EcGlyphStyleChannel::NONE || fCurrChannel != channel)
        return;
    radioGlyphStyle.set(glyphSets[fCurrChannel]);
    redrawSampleChar(glyphSets);
}


QHBoxLayout* WiShowcase::toolbarLayout()
    { return ui->layToolbar; }


QToolBar* WiShowcase::toolbar()
{
    if (!fToolbar) {
        auto lay = toolbarLayout();
        fToolbar = new QToolBar(lay->parentWidget());
        lay->addWidget(fToolbar);
    }
    return fToolbar;
}
