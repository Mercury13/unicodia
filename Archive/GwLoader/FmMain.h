#ifndef FMMAIN_H
#define FMMAIN_H

#include <deque>

// Qt
#include <QMainWindow>
#include <QDir>

QT_BEGIN_NAMESPACE
namespace Ui { class FmMain; }
QT_END_NAMESPACE

class QNetworkAccessManager;
class QNetworkReply;

struct Task {
    QString code;

    Task(const QString& aCode) : code(aCode) {}
    QString fname() const { return code + ".svg"; }
};

using PTask = std::shared_ptr<Task>;

class FmMain : public QMainWindow
{
    Q_OBJECT
    using This = FmMain;
public:
    FmMain(QWidget *parent = nullptr);
    ~FmMain();

private:
    Ui::FmMain *ui;


    std::unique_ptr<QNetworkAccessManager> netMan;
    std::deque<PTask> tasks;

    struct Work {
        std::atomic<PTask> task;
        std::atomic<bool> isOn = false;
        QDir path;
        QString urlTemplate;
    } work;

    void prepareTasks();
    void clearConsole();
    void tryEnqueueReply();
    void stopUi();
    void initWork();
    void processReply(const PTask& task, QNetworkReply* reply);
    bool isTaskOk(Task* task);
    void conPrint(const QString& text);

private slots:
    void start();
    void stop();
    void requestFinished(QNetworkReply* reply);
};
#endif // FMMAIN_H
