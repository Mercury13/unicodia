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
};
