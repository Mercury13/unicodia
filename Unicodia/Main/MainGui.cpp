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

// L10n
#include "LocDic.h"


///// FmPopup2 /////////////////////////////////////////////////////////////////


class FmPopup2 : public FmPopup
{
    using Super = FmPopup;
    using This = FmPopup2;
public:
    FmPopup2(MyGui& owner);
    ~FmPopup2() override = default;
};


FmPopup2::FmPopup2(MyGui& owner) : Super(owner.wiMain, CNAME_BG_POPUP)
{
    auto vw = viewport();
    vw->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::TextSelectableByMouse);
    connect(vw, &QLabel::linkActivated, &owner, &MyGui::popupLinkActivated);
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
        QWidget* widget, const QRect& absRect, const QString& html)
{
    popup.ensure(*this)
         .setText(html)
         .popupAtAbsBacked(widget, absRect);
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


void MyGui::closePopup()
{
    if (popup) {
        popup->deselectLink();
        popup->close();
    }
}


///// PopupGui /////////////////////////////////////////////////////////////////


void PopupGui::popupAtAbs(
        QWidget* widget, const QRect& absRect, const QString& html)
{
    if (owner.popup) {
        if (auto wi = owner.popup->lastWidget()) {
            auto rect = owner.popup->lastAbsRect();
            owner.popupAtAbs(wi, rect, html);
            return;
        }
    }
    // otherwise
    owner.popupAtAbs(widget, absRect, html);
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
