#ifndef FMSORTORDER_H
#define FMSORTORDER_H

#include <QWidget>

namespace Ui {
class FmSortOrder;
}

class FmSortOrder : public QWidget
{
    Q_OBJECT

public:
    explicit FmSortOrder(QWidget *parent = nullptr);
    ~FmSortOrder();

    FmSortOrder& popupAtScreen(QScreen* screen, const QRect& absRect);
private:
    Ui::FmSortOrder *ui;
    void popupAtY(
            const QRect& hotspotAbsRect,
            const QRect& screenRect,
            int y);
};

#endif // FMSORTORDER_H
