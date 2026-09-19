#ifndef FMMAIN_H
#define FMMAIN_H

#include <QMainWindow>
#include <QAbstractTableModel>

#include "work.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class FmMain;
}
QT_END_NAMESPACE

class LockableModel : public QAbstractTableModel
{
public:
    friend class ResetLock;
protected:
    using QAbstractTableModel::beginResetModel;
    using QAbstractTableModel::endResetModel;
};

class ResetLock
{
public:
    ResetLock(LockableModel& aModel);
    ~ResetLock();
private:
    LockableModel& model;
};

class BadModel : public LockableModel
{
public:
    int rowCount(const QModelIndex&) const override { return files.size(); }
    int columnCount(const QModelIndex&) const override { return 1; }
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    void take(std::vector<BadFile>&& x) { files = std::move(x); }
private:
    std::vector<BadFile> files;
};

class GoodModel : public LockableModel
{
public:
    int rowCount(const QModelIndex&) const override { return files.size(); }
    int columnCount(const QModelIndex&) const override { return 1; }
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    void take(std::vector<std::wstring>&& x) { files = std::move(x); }
private:
    std::vector<std::wstring> files;
};

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
    BadModel badModel;
    GoodModel goodModel;

private slots:
    void browseForDirectory();
    void go();
};
#endif // FMMAIN_H
