#ifndef FMMAIN_H
#define FMMAIN_H

// C++
#include <filesystem>

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

    void saveConfig();
    void loadConfig();

private:
    Ui::FmMain *ui;
    std::filesystem::path configPath;

private slots:
    void run();
};
#endif // FMMAIN_H
