#ifndef FMPOPUPCHAR_H
#define FMPOPUPCHAR_H

#include <QWidget>

namespace Ui {
class FmPopupChar;
}

class FmPopupChar : public QWidget
{
    Q_OBJECT

public:
    explicit FmPopupChar(QWidget *parent = nullptr);
    ~FmPopupChar() override;

private:
    Ui::FmPopupChar *ui;
};

#endif // FMPOPUPCHAR_H
