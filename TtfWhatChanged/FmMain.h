#ifndef FMMAIN_H
#define FMMAIN_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class FmMain; }
QT_END_NAMESPACE

class QLineEdit;

class FmMain : public QMainWindow
{
    Q_OBJECT
    using Super = QMainWindow;
    using This = FmMain;
public:
    FmMain(QWidget *parent = nullptr);
    ~FmMain() override;

private:
    Ui::FmMain *ui;

    void browseFor(QLineEdit* ed);

private slots:
    void browseForOld();
    void browseForNew();
    void go();
};
#endif // FMMAIN_H
