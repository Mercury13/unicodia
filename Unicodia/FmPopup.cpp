// My header
#include "FmPopup.h"

// Qt
#include <QLabel>
#include <QApplication>
#include <QScreen>
#include <QLayout>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>


template class MxPopup<FmPopup>;

///// ClickableLabel ///////////////////////////////////////////////////////////


ClickableLabel::ClickableLabel(const QString& text, QWidget* owner)
    : Super(text, owner)
{
    connect(this, &QLabel::linkActivated, this, &This::onLinkActivated);
}


ClickableLabel::Selection ClickableLabel::selection()
{
    Selection sel { selectionStart(), selectedText().length() };
    if (sel.length == 0)
        sel.start = -1;
    return sel;
}


void ClickableLabel::mousePressEvent(QMouseEvent* ev)
{
    allowClose = (ev->button() == Qt::LeftButton && selection().length == 0);
    Super::mousePressEvent(ev);
}


void ClickableLabel::mouseReleaseEvent(QMouseEvent* ev)
{
    if (ev->button() == Qt::RightButton)
        allowClose = false;

    Super::mouseReleaseEvent(ev);

    if (allowClose) {
        auto sel = selection();
        if (sel.length == 0)
            emit clicked();
    }
}


void ClickableLabel::onLinkActivated()
{
    allowClose = false;
}


bool ClickableLabel::event(QEvent* ev)
{
    switch (ev->type()) {
    case QEvent::Enter:
        emit mouseEnter();
        break;
    case QEvent::Leave:
        emit mouseLeave();
        break;
    default: ;
    }
    return Super::event(ev);
}



///// FmPopup //////////////////////////////////////////////////////////////////


FmPopup::FmPopup(QWidget* owner, const char* color)
    : Super(owner, WF_POPUP), MxPopup(owner)
{
    layout = new QVBoxLayout(this);
    setLayout(layout);
    if constexpr (popupMode == PopupMode::ARTIFICIAL) {
        layout->setContentsMargins(10, 10, 10, 10);
    } else {
        layout->setContentsMargins(0, 0, 0, 0);
    }

    lbText = new ClickableLabel("[QLabel]", this);
    lbText->setWordWrap(true);
    layout->addWidget(lbText);

    if constexpr (popupMode == PopupMode::ARTIFICIAL) {
        setAttribute(Qt::WA_TranslucentBackground); //enable window to be transparent
        //setWindowModality(Qt::ApplicationModal);
        QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect();
        effect->setXOffset(2);
        effect->setYOffset(2);
        effect->setBlurRadius(6);
        lbText->setGraphicsEffect(effect);
    }

    char buf[100];
    snprintf(buf, std::size(buf),
             "QLabel { background-color: %s; padding: 10px; border: 1px solid black }",
             color);
    lbText->setStyleSheet(buf);
    connect(lbText, &ClickableLabel::clicked, this, &This::hide);
}


void FmPopup::mySetFocus()
{
    lbText->setFocus();
}


FmPopup& FmPopup::setText(const QString& x)
{
    lbText->setText(x);
    return *this;
}


void FmPopup::focusOutEvent(QFocusEvent* ev)
{
    Super::focusOutEvent(ev);
    hide();
}


void FmPopup::mouseReleaseEvent(QMouseEvent* ev)
{
    Super::mouseReleaseEvent(ev);
    hide();
}


void FmPopup::deselectLink()
{
    lbText->setSelection(0, 0);
}
