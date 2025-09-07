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

    struct ClickMemory {
        QWidget* lastWidget;
        QRect lastAbsRect;
    };

}   // namespace pop


class WiPopup : public QWidget
{
private:
    using Super = QWidget;
public:
    WiPopup(QWidget* aOwner, pop::ClickMemory& memory);

    pop::ClickMemory& memory() const { return fMemory; }

    void popupAtAbs(QWidget* widget, const QRect& absRect)
    {
        if (!widget)
            throw std::invalid_argument("[MxPopup.popupAtAbs] Widget should be non-null!");
        fMemory.lastAbsRect = absRect;
        fMemory.lastWidget = widget;
        auto screen = pop::findScreen(widget, absRect);
        popupAtScreen(screen, absRect);
    }

    void popupAtAbsBacked(QWidget* widget, const QRect& absRect)
    {
        if (widget) {
            popupAtAbs(widget, absRect);
        } else {
            popupAtAbs(fMemory.lastWidget, fMemory.lastAbsRect);
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
            popupAtAbs(fMemory.lastWidget, fMemory.lastAbsRect);
        } else {
            popup(widget);
        }
    }

protected:
    QWidget* const fOwner;
    pop::ClickMemory& fMemory;

    void focusOutEvent(QFocusEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;

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
