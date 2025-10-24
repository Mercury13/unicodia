// My header
#include "MainGui.h"

// Qt
#include <QDesktopServices>
#include <QApplication>
#include <QClipboard>

// Project-local
#include "Skin.h"
#include "FmPopup.h"
#include "FmMessage.h"
#include "FmPopupChar.h"
#include "ui_FmPopupChar.h"

// L10n
#include "LocDic.h"

// Unicode
#include "UcData.h"
#include "UcCp.h"


///// FmPopup2 /////////////////////////////////////////////////////////////////


class FmPopup2 : public FmPopup
{
    using Super = FmPopup;
    using This = FmPopup2;
public:
    FmPopup2(MyGui& owner);
    ~FmPopup2() override = default;
};


FmPopup2::FmPopup2(MyGui& owner) : Super(owner.wiMain, owner.memory, CNAME_BG_POPUP)
{
    auto vw = viewport();
    vw->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::TextSelectableByMouse);
    connect(vw, &QLabel::linkActivated, &owner, &MyGui::popupLinkActivated);
}


///// FmPopupChar2 /////////////////////////////////////////////////////////////


class FmPopupChar2 : public FmPopupChar
{
    using Super = FmPopupChar;
    using This = FmPopupChar2;
public:
    FmPopupChar2(MyGui& owner);
    ~FmPopupChar2() override = default;
};


FmPopupChar2::FmPopupChar2(MyGui& owner)
    : Super(owner.wiMain, owner.memory)
{
    connect(ui->lbText, &QLabel::linkActivated, &owner, &MyGui::popupLinkActivated);
    connect(ui->lbCode, &QLabel::linkActivated, &owner, &MyGui::popupLinkActivated);
    connect(ui->lbToBlocks, &QLabel::linkActivated,
            this, // context object — who owns the lambda function
            [this, &owner]() {
                hide();
                owner.linkWalker().gotoCp(nullptr, charCode);
            });
    connect(ui->lbCopy, &QLabel::linkActivated,
            this, // context object — who owns the lambda function
            [this, &owner]() {
                QString text = QString::fromUcs4(&charCode, 1);
                owner.copyTextRel(ui->lbCopy, {}, text, LK_COPIED);
            });
    connect(ui->lbCopyVs16, &QLabel::linkActivated,
            this, // context object — who owns the lambda function
            [this, &owner]() {
                char32_t cps[2] { charCode, cp::VS16 };
                QString text = QString::fromUcs4(cps, 2);
                owner.copyTextRel(ui->lbCopyVs16, {}, text, LK_COPIED);
            });
}

///// MyGui ////////////////////////////////////////////////////////////////////


MyGui::MyGui(
        QWidget* aWiMain, FontSource& aFontSource,
        mywiki::InternalLinkWalker& aInternalWalker)
    : wiMain(aWiMain), fFontSource(aFontSource),
      fInternalWalker(aInternalWalker) {}


MyGui::~MyGui() = default;


void MyGui::popupLinkActivated(const QString& link)
{
    if (auto wi = qobject_cast<QWidget*>(sender())) {
        emit linkActivated(wi, link);
    }
}


void MyGui::blinkCopied(QWidget* widget, const QRect& absRect, LocKey locKey)
{
    fmMessage.ensure(wiMain)
             .showNearAbs(loc::get(locKey.v), widget, absRect);
}


void MyGui::blinkAtWidget(const QString& text, QWidget* widget)
{
    fmMessage.ensure(wiMain)
             .showNearWidget(text, widget);
}


void MyGui::blinkAtRel(const QString& text, const QWidget* widget, const QRect& relRect)
{
    fmMessage.ensure(wiMain)
             .showNearRel(text, widget, relRect);
}


void MyGui::popupAtAbs(
        QWidget* widget, const QRect& absRect,
        const mywiki::PLink& that, const QString& html)
{
    closePopup(popup);
    mywiki::PHistoryLink lowerLink = std::dynamic_pointer_cast<const mywiki::HistoryLink>(that);
    history.push(lowerLink);
    auto html2 = html;
    if (lowerLink) {
        // Add history link only if the link is really part of history
        // Do not add for e.g. fonts for
        auto backPlace = history.back(lowerLink.get());
        mywiki::appendHistoryLink(html2, backPlace);
    }
    popup.ensure(*this)
         .setText(html2)
         .popupAtAbsBacked(widget, absRect);
}


void MyGui::popupCharAbs(
        QWidget* widget, const QRect& absRect,
        const mywiki::PLink& that, const uc::Cp& cp)
{
    closePopup(popupChar);
    mywiki::PHistoryLink lowerLink = std::dynamic_pointer_cast<const mywiki::HistoryLink>(that);
    history.push(lowerLink);
    auto backPlace = history.back(lowerLink.get());
    popupChar.ensure(*this)
             /// @todo [future] Let it be EMPTY, while these character are not shown
             ///                in Library and articles
             .setCp(cp, uc::GlyphStyleSets::EMPTY, *this, backPlace)
             .popupAtAbsBacked(widget, absRect);
}


void MyGui::goToHistory(QWidget* widget, unsigned index)
{
    goToHistoryRel(widget, QRect{0,0,1,1}, index);
}


void MyGui::goToHistoryRel(QWidget* widget, const QRect& relRect, unsigned index)
{
    if (auto thing = history.extract(index)) {
        thing->go(widget, relRect, thing, *this);
    }
}


void MyGui::goToHistoryAbs(QWidget* widget, const QRect& absRect, unsigned index)
{
    QRect r1 { widget->mapFromGlobal(absRect.topLeft()), absRect.size() };
    goToHistoryRel(widget, r1, index);
}


void MyGui::copyTextAbs(
        QWidget* widget, const QRect& absRect, const QString& text,
        LocKey locKey)
{
    QApplication::clipboard()->setText(text);
    blinkCopied(widget, absRect, locKey);
}

void MyGui::followUrl(const QString& x)
{
    QDesktopServices::openUrl(x);
}


void MyGui::closePopup(RestrictedPtr remainingThing)
{
    if ((remainingThing != &popup) && popup) {
        popup->deselectLink();
        popup->close();
    }
    if ((remainingThing != &popupChar) && popupChar) {
        popupChar->deselectLink();
        popupChar->close();
    }
}


bool MyGui::hasEgypI() const
{
    if (!fHasEgypI) {
        auto metrics = wiMain->fontMetrics();
        fHasEgypI = metrics.inFontUcs4(cp::LATN_SM_GLOTTAL_I);
    }
    return *fHasEgypI;
}


///// PopupGui /////////////////////////////////////////////////////////////////


void PopupGui::popupAtAbs(
        QWidget* widget, const QRect& absRect,
        const mywiki::PLink& that, const QString& html)
{
    if (auto wi = owner.memory.lastWidget) {
        auto rect = owner.memory.lastAbsRect;  // let it be copy
        owner.popupAtAbs(wi, rect, that, html);
        return;
    }
    // otherwise
    owner.popupAtAbs(widget, absRect, that, html);
}

void PopupGui::popupCharAbs(
        QWidget* widget, const QRect& absRect,
        const mywiki::PLink& that, const uc::Cp& cp)
{
    if (auto wi = owner.memory.lastWidget) {
        auto rect = owner.memory.lastAbsRect;  // let it be copy
        owner.popupCharAbs(wi, rect, that, cp);
        return;
    }
    // otherwise
    owner.popupCharAbs(widget, absRect, that, cp);
}

void PopupGui::goToHistory(QWidget* widget, unsigned index)
{
    if (auto wi = owner.memory.lastWidget) {
        auto rect = owner.memory.lastAbsRect;  // let it be copy
        owner.goToHistoryAbs(wi, rect, index);
        return;
    }
    // otherwise
    owner.goToHistory(widget, index);
}


FontSource& PopupGui::fontSource() { return owner.fontSource(); }

void PopupGui::copyTextAbs(
        QWidget* widget, const QRect& absRect, const QString& text,
        LocKey locKey)
    { owner.copyTextAbs(widget, absRect, text, locKey); }

void PopupGui::followUrl(const QString& x)
    { owner.followUrl(x); }

mywiki::InternalLinkWalker& PopupGui::linkWalker()
    { return owner.linkWalker(); }
