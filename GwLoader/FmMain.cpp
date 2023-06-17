#include "FmMain.h"
#include "ui_FmMain.h"

// Qt
#include <QRegularExpression>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>

FmMain::FmMain(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::FmMain)
{
    ui->setupUi(this);

    netMan = std::make_unique<QNetworkAccessManager>();

    connect(ui->btStart, &QPushButton::clicked, this, &This::start);
    connect(ui->btStop, &QPushButton::clicked, this, &This::stop);
    connect(netMan.get(), &QNetworkAccessManager::finished, this, &This::requestFinished);

    stopUi();
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
        auto q = p.trimmed();
        if (!q.startsWith('#')) {
            tasks.push_back(std::make_shared<Task>(q));
        }
    }
}


void FmMain::initWork()
{
    work.urlTemplate = ui->edDocTemplate->text();
    QDir currentDir = QDir::current();
    work.path = currentDir.filePath(ui->edDirectory->text());
    work.path.mkpath(work.path.absolutePath());
    work.task.store(nullptr);
}


void FmMain::start()
{
    clearConsole();
    initWork();
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

    if (work.path.exists(task->fname()))
        return false;

    return true;
}


void FmMain::conPrint(const QString& text)
{
    ui->memoConsole->moveCursor(QTextCursor::End);
    ui->memoConsole->appendPlainText(text);
}


void FmMain::tryEnqueueReply()
{
    while (!tasks.empty()) {
        auto task = std::move(tasks.front());
        tasks.pop_front();
        if (isTaskOk(task.get())) {
            work.task = task;
            QString url2 = work.urlTemplate;
            url2.replace('*', task->code.toLower());
            QNetworkRequest rq("http://en.glyphwiki.org/glyph/" + url2);
            netMan->get(rq);
            conPrint(QString("Requesting %1").arg(task->code));
            return;
        } else {
            if (!task) {
                conPrint("Somehow got empty task");
            } else {
                conPrint(QString("Skipped %1").arg(task->code));
            }
        }
    }
    stop();
    stopUi();
}


void FmMain::processReply(const PTask& task, QNetworkReply* reply)
{
    if (!work.isOn || !task)
        return;

    if (!reply->isFinished()) {
        conPrint("Reply is not finished");
    }

    auto vStatusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    int statusCode = vStatusCode.toInt();
    if (statusCode != 200) {
        conPrint(QString("Got code %1 on task %2").arg(statusCode).arg(task->code));
        return;
    }

    auto bytes = reply->readAll();

    QFile file(work.path.filePath(task->fname()));
    file.open(QFile::ReadWrite);
    file.write(bytes);
    conPrint(QString("Wrote %1").arg(task->code));
}


void FmMain::requestFinished(QNetworkReply* reply)
{
    if (auto task = work.task.exchange({})) {
        processReply(task, reply);
    }
    tryEnqueueReply();
    reply->deleteLater();
}
