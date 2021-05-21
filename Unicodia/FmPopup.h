#pragma once

// C++
#include <memory>

// Qt
#include <QWidget>
#include <QLabel>

// My libs
#include <u_TinyOpt.h>


class ClickableLabel: public QLabel
{
    using Super = QLabel;
    Q_OBJECT
public:
    using Super::QLabel;
    ~ClickableLabel() override = default;

signals:
    void clicked();

protected:
    void mouseReleaseEvent(QMouseEvent*) override;
};


class QScreen;


class FmPopup : public QWidget
{
    Q_OBJECT
    using Super = QWidget;
    using This = FmPopup;
public:
    FmPopup(QWidget* owner);
    ~FmPopup() override = default;
    FmPopup& popupAtScreen(QScreen* screen, const QRect& absRect);
    FmPopup& popupAtAbs(QWidget* widget, const QRect& absRect);
    FmPopup& popup(QWidget* widget, TinyOpt<QRect> rect);
    FmPopup& popup(QWidget* widget);
    FmPopup& setText(const QString& x);

    static This& ensure(std::unique_ptr<This>& x, QWidget* owner);
protected:
    void focusOutEvent(QFocusEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
private:
    ClickableLabel* lbText;

    void popupAtY(
            const QRect& absRect, const QRect& screenRect,
            int y);
};
