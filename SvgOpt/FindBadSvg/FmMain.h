#ifndef FMMAIN_H
#define FMMAIN_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class FmMain;
}
QT_END_NAMESPACE

class FmMain : public QMainWindow
{
    Q_OBJECT
    using This = FmMain;
    using Super = QMainWindow;
public:
    explicit FmMain(QWidget *parent = nullptr);
    ~FmMain() override;

private:
    Ui::FmMain *ui;

private slots:
    void browseForDirectory();
    void go();
};
#endif // FMMAIN_H
