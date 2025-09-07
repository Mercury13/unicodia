#pragma once

// C++
#include <memory>

// Qt
#include <QWidget>
#include <QLabel>

// Project-local
#include "u_Popup.h"


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

        bool operator == (const Selection& x) const { return (start == x.start && length == x.length); }
        bool operator != (const Selection& x) const { return !operator == (x); }
    };
    Selection selection();
    Selection lastSel;
    bool allowClose = false;

    void mousePressEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    bool event(QEvent*) override;

signals:
    void mouseEnter();
    void mouseLeave();

protected slots:
    void onLinkActivated();
};


class QScreen;


template <class Wi, class ... Args>
Wi& ensure(std::unique_ptr<Wi>& x, Args&& ... args)
{
    if (!x) {
        x = std::make_unique<Wi>(std::forward<Args>(args)...);
    }
    return *x;
}


class FmPopup : public QWidget, public MxPopup<FmPopup>
{
    Q_OBJECT
    using Super = QWidget;
    using This = FmPopup;
public:
    FmPopup(QWidget* owner, const char* color);
    ~FmPopup() override = default;
    FmPopup& popupAtAbs(QWidget* widget, const QRect& absRect);
    FmPopup& popupAtAbsBacked(QWidget* widget, const QRect& absRect);
    FmPopup& popup(QWidget* widget, TinyOpt<QRect> rect);
    FmPopup& popup(QWidget* widget);
    FmPopup& setText(const QString& x);

    ClickableLabel* viewport() const { return lbText; }
    void deselectLink();
protected:
    void focusOutEvent(QFocusEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    virtual void myAdjustSize(const QRect& screenRect);
private:
    ClickableLabel* lbText;    
    class QVBoxLayout* layout;

    void popupAtY(
            const QRect& hotspotAbsRect,
            QRect ownerRect,
            const QRect& screenRect,
            int y);
    FmPopup& popupAtScreen(QScreen* screen, const QRect& absRect);
};

extern template class MxPopup<FmPopup>;
