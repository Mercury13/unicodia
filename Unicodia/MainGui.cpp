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


MyGui::MyGui(QWidget* aWiMain, FontSource& aFontSource)
    : wiMain(aWiMain), fFontSource(aFontSource) {}


MyGui::~MyGui() = default;


void MyGui::popupLinkActivated(const QString& link)
{
    if (auto wi = qobject_cast<QWidget*>(sender())) {
        emit linkActivated(wi, link);
    }
}


void MyGui::blinkCopied(QWidget* widget, const QRect& absRect)
{
    fmMessage.ensure(wiMain)
             .showAtAbs(loc::get("Common.Copied"), widget, absRect);
}


void MyGui::blinkAtWidget(const QString& text, QWidget* widget)
{
    fmMessage.ensure(wiMain)
             .showAtWidget(text, widget);
}


void MyGui::blinkAtRel(const QString& text, const QWidget* widget, const QRect& relRect)
{
    fmMessage.ensure(wiMain)
             .showAtRel(text, widget, relRect);
}


void MyGui::popupAtAbs(
        QWidget* widget, const QRect& absRect, const QString& html)
{
    popup.ensure(*this)
         .setText(html)
         .popupAtAbsBacked(widget, absRect);
}


void MyGui::copyTextAbs(
        QWidget* widget, const QRect& absRect, const QString& text)
{
    QApplication::clipboard()->setText(text);
    blinkCopied(widget, absRect);
}

void MyGui::followUrl(const QString& x)
{
    QDesktopServices::openUrl(x);
}
