#include "FmMain.h"
#include "ui_FmMain.h"

// Qt
#include <QRegularExpression>
#include <QNetworkAccessManager>
#include <QNetworkReply>

FmMain::FmMain(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::FmMain)
{
    ui->setupUi(this);

    netMan = std::make_unique<QNetworkAccessManager>();

    connect(ui->btStart, &QPushButton::clicked, this, &This::start);
    connect(ui->btStop, &QPushButton::clicked, this, &This::stop);
    connect(netMan.get(), &QNetworkAccessManager::finished, this, &This::requestFinished);

    stop();
}

FmMain::~FmMain()
{
    delete ui;
}


void FmMain::clearConsole()
{
    ui->memoConsole->clear();
}


void FmMain::prepareTasks()
{
    auto text = ui->memoGlyphs->toPlainText();
    auto lines = text.split(QRegularExpression("\\s"), Qt::SkipEmptyParts);

    tasks.clear();
    for (auto& p : lines) {
        tasks.push_back(std::make_shared<Task>(p));
    }
}


void FmMain::start()
{
    clearConsole();
    prepareTasks();
    ui->btStart->setEnabled(false);
    ui->btStop->setEnabled(true);
    work.isOn = true;
    tryEnqueueReply();
}


void FmMain::stop()
{
    work.isOn = false;
    work.task.load({});
    tasks.clear();
}


void FmMain::stopUi()
{
    ui->btStart->setEnabled(true);
    ui->btStop->setEnabled(false);
}


bool FmMain::isTaskOk(Task* task)
{
    if (!task)
        return false;

    /// @todo [urgent] Check for file existence
    return true;
}


void FmMain::tryEnqueueReply()
{
    while (!tasks.empty()) {
        auto task = std::move(tasks.front());
        tasks.pop_front();
        if (isTaskOk(task.get())) {
            work.task = std::move(task);
            return;
        } else {
            /// @todo [urgent] print to console
        }
    }
    stop();
    stopUi();
}


void FmMain::processReply(const PTask& task, QNetworkReply* reply)
{
    if (!work.isOn)
        return;
}


void FmMain::requestFinished(QNetworkReply* reply)
{
    if (auto task = work.task.exchange({})) {
        processReply(task, reply);
    }
    tryEnqueueReply();
}
