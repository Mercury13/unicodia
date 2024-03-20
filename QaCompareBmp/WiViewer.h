#ifndef WIVIEWER_H
#define WIVIEWER_H

#include <QWidget>

class WiViewer : public QWidget
{
public:
    using QWidget::QWidget;

    void removePix();
    void setPix(const QPixmap& x);
    void setPix(QPixmap&& x);
    void paintEvent(QPaintEvent* event) override;
private:
    QPixmap pix;
};

#endif // WIVIEWER_H
