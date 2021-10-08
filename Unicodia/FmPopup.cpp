// My header
#include "FmPopup.h"

// Qt
#include <QLabel>
#include <QApplication>
#include <QScreen>
#include <QLayout>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>

namespace {
    enum class PopupMode { NATIVE, ARTIFICIAL };
    constexpr auto popupMode = PopupMode::NATIVE;

    constexpr auto WF_POPUP = (popupMode == PopupMode::ARTIFICIAL)
            ? Qt::FramelessWindowHint | Qt::Tool
            : Qt::Popup;
}


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


FmPopup::FmPopup(QWidget* owner, const char* color) : Super(owner, WF_POPUP)
{
    auto vl = new QVBoxLayout(this);
    setLayout(vl);
    if constexpr (popupMode == PopupMode::ARTIFICIAL) {
        vl->setContentsMargins(10, 10, 10, 10);
    } else {
        vl->setContentsMargins(1, 1, 1, 1);
    }

    lbText = new ClickableLabel("[QLabel]", this);
    lbText->setWordWrap(true);
    vl->addWidget(lbText);

    if constexpr (popupMode == PopupMode::ARTIFICIAL) {
        setAttribute(Qt::WA_TranslucentBackground); //enable window to be transparent
        //setWindowModality(Qt::ApplicationModal);
        QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect();
        effect->setXOffset(2);
        effect->setYOffset(2);
        effect->setBlurRadius(6);
        lbText->setGraphicsEffect(effect);
    }

    setStyleSheet("QWidget { background-color: black }");
    char buf[100];
    snprintf(buf, std::size(buf),
             "QLabel { background-color: %s; padding: 10px; }",
             color);
    lbText->setStyleSheet(buf);
    connect(lbText, &ClickableLabel::clicked, this, &This::hide);
}


FmPopup& FmPopup::setText(const QString& x)
{
    lbText->setText(x);
    return *this;
}


void FmPopup::popupAtY(const QRect& absRect, const QRect& screenRect, int y)
{
    auto myW = width();
    auto x = std::min(absRect.left(), screenRect.right() - myW);
    x = std::max(x, 0);
    move(x, y);
    show();

    lbText->setFocus();
    if (popupMode == PopupMode::ARTIFICIAL) {
        setFocus();
    }
}


FmPopup& FmPopup::popupAtScreen(QScreen* screen, const QRect& absRect)
{
    adjustSize();
    auto screenRect = screen->availableGeometry();
    auto myH = height();
    if (auto bottomRemainder = screenRect.bottom() - absRect.bottom();
            bottomRemainder >= myH) {
        // Try on the bottom
        popupAtY(absRect, screenRect, absRect.bottom());
    } else if (auto topRemainder = absRect.top() - screenRect.top();
            topRemainder >= myH) {
        // Try on the top
        popupAtY(absRect, screenRect, absRect.top() - myH);
    } else {
        // Try everywhere
        popupAtY(absRect, screenRect, screenRect.bottom() - myH);
    }
    return *this;
}

FmPopup& FmPopup::popupAtAbs(QWidget* widget, const QRect& absRect)
{
    if (!widget)
        throw std::invalid_argument("[FmPopup.popupAtAbs] Widget should be non-null!");
    fLastAbsRect = absRect;
    fLastWidget = widget;
    auto screen = QApplication::screenAt(absRect.center());
    if (!screen) {
        screen = widget->screen();
        if (!screen)
            screen = QApplication::primaryScreen();
    }
    popupAtScreen(screen, absRect);
    return *this;
}


FmPopup& FmPopup::popupAtAbsBacked(QWidget* widget, const QRect& absRect)
{
    if (widget) {
        return popupAtAbs(widget, absRect);
    } else {
        return popupAtAbs(fLastWidget, fLastAbsRect);
    }
}


FmPopup& FmPopup::popup(QWidget* widget, TinyOpt<QRect> rect)
{
    if (rect) {
        return popupAtAbs(widget, QRect(
                    widget->mapToGlobal(rect->topLeft()), rect->size()));
    } else if (!widget) {
        return popupAtAbs(fLastWidget, fLastAbsRect);
    } else {
        return popup(widget);
    }
}


FmPopup& FmPopup::popup(QWidget* widget)
{
    return popupAtAbs(widget, QRect(
                widget->mapToGlobal(QPoint(0, 0)), widget->size()));
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
