#ifndef FMMAIN_H
#define FMMAIN_H

#include <QMainWindow>
#include <QAbstractItemModel>

#include "Fonts/MemFont.h"

QT_BEGIN_NAMESPACE
namespace Ui { class FmMain; }
QT_END_NAMESPACE

class TreeModel : public QAbstractItemModel
{
public:
    MemFont font;

    const mf::Obj& objOf(const QModelIndex& index) const;

    // QAbstractItemModel
    QModelIndex index(int row, int column, const QModelIndex &parent = {}) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
};

class FmMain : public QMainWindow
{
    Q_OBJECT

public:
    FmMain(QWidget *parent = nullptr);
    ~FmMain() override;

private:
    Ui::FmMain *ui;

    TreeModel model;
};
#endif // FMMAIN_H
