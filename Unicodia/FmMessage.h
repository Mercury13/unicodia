#pragma once

// STL
#include <memory>

// Qt
#include <QWidget>

class QLabel;
class QGridLayout;
class QPropertyAnimation;
class QTimer;

class FmMessage : public QWidget
{
    Q_OBJECT
    Q_PROPERTY (float popupOpacity READ popupOpacity WRITE setPopupOpacity)
    using Super = QWidget;
    using This = FmMessage;
public:
    explicit FmMessage(QWidget* parent);
    ~FmMessage() override;
    float popupOpacity() const { return fPopupOpacity; }
    void setPopupOpacity(float x);
public slots:
    void setPopupText(const QString& text);
    void moveTo(const QWidget* widget, const QRect& globalRect);
    void show();
    void showAtAbs(const QString& text, const QWidget* widget, const QRect& globalRect);
    void showAtWidget(const QString& text, const QWidget* widget);
protected:
    void paintEvent(QPaintEvent*) override;
private:
    std::unique_ptr<QGridLayout> layout;
    std::unique_ptr<QLabel> label;
    std::unique_ptr<QPropertyAnimation> animation;
    std::unique_ptr<QTimer> timer;
    float fPopupOpacity;

    void setY(const QRect& globalRect, const QRect& screenRect, int y);
private slots:
    void hideAnimation();
    void finalHide();
};
