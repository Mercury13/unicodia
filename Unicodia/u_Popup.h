#pragma once

// Qt
#include <QWidget>
#include <QLabel>

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


class ClickableLabel: public QLabel
{
    using Super = QLabel;
    using This = ClickableLabel;
    Q_OBJECT
public:
    using Super::QLabel;
    ClickableLabel(const QString& text, QWidget* owner);
    ~ClickableLabel() override = default;

signals:
    void clicked();

protected:
    struct Selection {
        int start = -1;
        qsizetype length = 0;

        // Let it be this way: C++17 instead of 20, probably for older MinGW
        bool operator == (const Selection& x) const { return (start == x.start && length == x.length); }
        bool operator != (const Selection& x) const { return !operator == (x); }
    };
    Selection lastSel;
    bool allowClose = false;

    Selection selection();
    void mousePressEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    bool event(QEvent*) override;

signals:
    void mouseEnter();
    void mouseLeave();

protected slots:
    void onLinkActivated();
};


class WiAdjust : public QWidget
{
private:
    using Super = QWidget;
public:
    using Super::Super;
    virtual void adjustSize()
        { Super::adjustSize(); }
};


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
            WiAdjust* me,
            const QRect& screenRect);
    void popupAtScreen(
            WiAdjust* me, QWidget* owner, QScreen* screen, const QRect& absRect);
    QScreen* findScreen(QWidget* widget, const QRect& absRect);

    constexpr int RIGHT_MARGIN = 8;
    constexpr int LEFT_MARGIN = 1;
    constexpr int BOTTOM_MARGIN = 3;

    struct ClickMemory {
        QWidget* lastWidget;
        QRect lastAbsRect;
    };

}   // namespace pop


class WiPopup : public WiAdjust
{
private:
    using Super = WiAdjust;
public:
    WiPopup(QWidget* aOwner, pop::ClickMemory& memory);

    pop::ClickMemory& memory() const { return fMemory; }

    void popupAtAbs(QWidget* widget, const QRect& absRect);
    void popupAtAbsBacked(QWidget* widget, const QRect& absRect);
    void popup(QWidget* widget);
    void popup(QWidget* widget, TinyOpt<QRect> rect);

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
