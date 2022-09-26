#ifndef FMMAIN_H
#define FMMAIN_H

#include <deque>

// Qt
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class FmMain; }
QT_END_NAMESPACE

class QNetworkAccessManager;
class QNetworkReply;

struct Task {
    QString code;

    Task(const QString& aCode) : code(aCode) {}
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
    } work;

    void prepareTasks();
    void clearConsole();
    void tryEnqueueReply();
    void stopUi();
    void processReply(const PTask& task, QNetworkReply* reply);
    bool isTaskOk(Task* task);

private slots:
    void start();
    void stop();
    void requestFinished(QNetworkReply* reply);
};
#endif // FMMAIN_H
