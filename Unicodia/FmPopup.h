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


template <class Wi, class ... Args>
Wi& ensure(std::unique_ptr<Wi>& x, Args&& ... args)
{
    if (!x) {
        x = std::make_unique<Wi>(std::forward<Args>(args)...);
    }
    return *x;
}


class FmPopup : public WiPopup
{
    Q_OBJECT
    using Super = WiPopup;
    using This = FmPopup;
public:
    FmPopup(QWidget* owner, pop::ClickMemory& memory, const char* color);
    ~FmPopup() override = default;
    FmPopup& setText(const QString& x);

    ClickableLabel* viewport() const { return lbText; }
    void deselectLink();
    void mySetFocus() override;
private:
    ClickableLabel* lbText;    
    class QVBoxLayout* layout;
};
