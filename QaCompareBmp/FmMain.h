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
    ~FmMain() override;

    struct Config {
        QString versioned;
        QString working;

        /// @return [+] can save to file
        bool isPresent() const;

        /// @return [+] can work
        bool isFull() const;
    };

    Config config();

    bool hasConfig();
    void saveConfig();
    void loadConfig();

private:
    Ui::FmMain *ui;
};
#endif // FMMAIN_H
