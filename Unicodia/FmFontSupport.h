#ifndef FMFONTSUPPORT_H
#define FMFONTSUPPORT_H

#include <QDialog>

namespace Ui {
class FmFontSupport;
}

class FmFontSupport : public QDialog
{
    Q_OBJECT

public:
    explicit FmFontSupport(QWidget *parent = nullptr);
    ~FmFontSupport() override;

private:
    Ui::FmFontSupport *ui;
};

#endif // FMFONTSUPPORT_H
