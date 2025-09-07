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

    void checkOwner(QWidget* owner);
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

    constexpr int RIGHT_MARGIN = 8;
    constexpr int LEFT_MARGIN = 1;
    constexpr int BOTTOM_MARGIN = 3;

}   // namespace pop

template <class Me>
class MxPopup
{
public:
    MxPopup(QWidget* aOwner) : fOwner(aOwner) { pop::checkOwner(aOwner); }

    const QRect& lastAbsRect() const { return fLastAbsRect; }
    QWidget* lastWidget() const { return fLastWidget; }

protected:
    QWidget* const fOwner;
    QRect fLastAbsRect;
    QWidget* fLastWidget;

    QWidget* me() { return static_cast<Me*>(this); }
    const QWidget* me() const { return static_cast<const Me*>(this); }

    Me* mee() { return static_cast<Me*>(this); }
    const Me* mee() const { return static_cast<const Me*>(this); }

    void adjustAfterPopup();
    void popupAtY(
            const QRect& hotspotAbsRect,
            const QRect& ownerRect,
            const QRect& screenRect,
            int y) {
        pop::popupAtY(me(), hotspotAbsRect, ownerRect, screenRect, y);
        adjustAfterPopup();
    }

    void myAdjustSize(const QRect& screenRect)
        { pop::myAdjustSize(me(), screenRect); }

    Me& popupAtScreen(QScreen* screen, const QRect& absRect)
    {
        pop::popupAtScreen(me(), fOwner, screen, absRect);
        adjustAfterPopup();
        return *mee();
    }
};


template <class Me>
void MxPopup<Me>::adjustAfterPopup()
{
    mee()->mySetFocus();
    if constexpr (popupMode == PopupMode::ARTIFICIAL) {
        me()->setFocus();
    }
}
