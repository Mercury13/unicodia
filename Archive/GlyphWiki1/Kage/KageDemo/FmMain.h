#ifndef FMMAIN_H
#define FMMAIN_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class FmMain; }
QT_END_NAMESPACE

class FmMain : public QMainWindow
{
    Q_OBJECT

public:
    FmMain(QWidget *parent = nullptr);
    ~FmMain();

private:
    Ui::FmMain *ui;
};
#endif // FMMAIN_H
