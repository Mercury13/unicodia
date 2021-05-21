// My header
#include "FmPopup.h"

// Qt
#include <QLabel>
#include <QApplication>
#include <QDesktopWidget>
#include <QScreen>
#include <QLayout>

namespace {
    constexpr auto WF_POPUP = Qt::FramelessWindowHint | Qt::Popup | Qt::WindowStaysOnTopHint;
}

void ClickableLabel::mouseReleaseEvent(QMouseEvent* ev)
{
    Super::mouseReleaseEvent(ev);
    emit clicked();
}


FmPopup::FmPopup(QWidget* owner) : Super(owner, WF_POPUP)
{
    auto vl = new QVBoxLayout(this);
    setLayout(vl);
    vl->setMargin(1);

    lbText = new ClickableLabel("[QLabel]", this);
    lbText->setWordWrap(true);
    vl->addWidget(lbText);

    setStyleSheet("QWidget { background-color: black }");
    lbText->setStyleSheet("QLabel { background-color: LightYellow; padding: 10px; }");
    connect(lbText, &ClickableLabel::clicked, this, &This::hide);
}

FmPopup& FmPopup::ensure(
        std::unique_ptr<This>& x, QWidget* owner)
{
    if (!x) {
        x = std::make_unique<This>(owner);
    }
    return *x;
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
    auto screen = QApplication::screenAt(absRect.topLeft());
    if (!screen)
        screen = widget->screen();
    popupAtScreen(screen, absRect);
    return *this;
}


FmPopup& FmPopup::popup(QWidget* widget, TinyOpt<QRect> rect)
{
    if (rect) {
        return popupAtAbs(widget, QRect(
                    widget->mapToGlobal(rect->topLeft()), rect->size()));
    } else {
        return popup(widget);
    }
}


FmPopup& FmPopup::popup(QWidget* widget)
{
    return popupAtAbs(widget, QRect(
                widget->mapToGlobal({ 0, 0 }), widget->size()));
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
