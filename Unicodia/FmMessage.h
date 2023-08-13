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
    void moveNear(const QWidget* widget, const QRect& globalRect);
    void moveOver(const QWidget* widget, const QRect& globalRect);
    void show();
    void showNearAbs(const QString& text, const QWidget* widget, const QRect& globalRect);
    void showOverAbs(const QString& text, const QWidget* widget, const QRect& globalRect);
    void showNearRel(const QString& text, const QWidget* widget, const QRect& relRect);
    void showOverRel(const QString& text, const QWidget* widget, const QRect& relRect);
    void showNearWidget(const QString& text, const QWidget* widget);
    void showOverWidget(const QString& text, const QWidget* widget);
protected:
    void paintEvent(QPaintEvent*) override;
private:
    enum class MoveMode { LEAVE, COVER };
    std::unique_ptr<QGridLayout> layout;
    std::unique_ptr<QLabel> label;
    std::unique_ptr<QPropertyAnimation> animation;
    std::unique_ptr<QTimer> timer;
    float fPopupOpacity;

    void setY(const QRect& globalRect, const QRect& screenRect, int y);
    QRect screenRectAt(const QWidget* widget, const QRect& globalRect) const;
private slots:
    void hideAnimation();
    void finalHide();
};
