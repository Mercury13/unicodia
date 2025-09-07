#pragma once

// Qt
#include <QWidget>

// My libs
#include <u_TinyOpt.h>

enum class PopupMode : unsigned char {
    NATIVE,         ///< Use native popup window (default)
    ARTIFICIAL      ///< Simulate popup with other means
};
constexpr auto popupMode = PopupMode::NATIVE;

constexpr auto WF_POPUP = (popupMode == PopupMode::ARTIFICIAL)
        ? Qt::FramelessWindowHint | Qt::Tool
        : Qt::Popup;


namespace pop {

    void eatRightMargin(QRect& rect, int mainMargin, int auxMargin);
    void eatBottomMargin(QRect& rect, int margin);
    void popupAtY(
            QWidget* me,
            const QRect& hotspotAbsRect,
            QRect ownerRect,
            const QRect& screenRect,
            int y);
    void myAdjustSize(
            QWidget* me,
            const QRect& screenRect);
    void popupAtScreen(
            QWidget* me, QWidget* owner, QScreen* screen, const QRect& absRect);
    QScreen* findScreen(QWidget* widget, const QRect& absRect);

    constexpr int RIGHT_MARGIN = 8;
    constexpr int LEFT_MARGIN = 1;
    constexpr int BOTTOM_MARGIN = 3;

}   // namespace pop


class WiPopup : public QWidget
{
public:
    WiPopup(QWidget* aOwner);

    const QRect& lastAbsRect() const { return fLastAbsRect; }
    QWidget* lastWidget() const { return fLastWidget; }

    void popupAtAbs(QWidget* widget, const QRect& absRect)
    {
        if (!widget)
            throw std::invalid_argument("[MxPopup.popupAtAbs] Widget should be non-null!");
        fLastAbsRect = absRect;
        fLastWidget = widget;
        auto screen = pop::findScreen(widget, absRect);
        popupAtScreen(screen, absRect);
    }

    void popupAtAbsBacked(QWidget* widget, const QRect& absRect)
    {
        if (widget) {
            popupAtAbs(widget, absRect);
        } else {
            popupAtAbs(fLastWidget, fLastAbsRect);
        }
    }

    void popup(QWidget* widget)
    {
        popupAtAbs(widget, QRect(
                   widget->mapToGlobal(QPoint(0, 0)), widget->size()));
    }

    void popup(QWidget* widget, TinyOpt<QRect> rect)
    {
        if (rect) {
            popupAtAbs(widget, QRect(
                        widget->mapToGlobal(rect->topLeft()), rect->size()));
        } else if (!widget) {
            popupAtAbs(fLastWidget, fLastAbsRect);
        } else {
            popup(widget);
        }
    }

protected:
    QWidget* const fOwner;
    QRect fLastAbsRect;
    QWidget* fLastWidget;

    virtual void mySetFocus() {}

    void adjustAfterPopup();
    void popupAtY(
            const QRect& hotspotAbsRect,
            const QRect& ownerRect,
            const QRect& screenRect,
            int y) {
        pop::popupAtY(this, hotspotAbsRect, ownerRect, screenRect, y);
        adjustAfterPopup();
    }

    void myAdjustSize(const QRect& screenRect)
        { pop::myAdjustSize(this, screenRect); }

    void popupAtScreen(QScreen* screen, const QRect& absRect)
    {
        pop::popupAtScreen(this, fOwner, screen, absRect);
        adjustAfterPopup();
    }
};
