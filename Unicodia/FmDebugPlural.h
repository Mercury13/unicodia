#ifndef FMDEBUGPLURAL_H
#define FMDEBUGPLURAL_H

#include <QDialog>
#include <QAbstractTableModel>

namespace Ui {
class FmDebugPlural;
}

class PluralModel : public QAbstractTableModel
{
    using Super = QAbstractTableModel;
public:
    int rowCount(const QModelIndex&) const override { return N_ROWS; }
    int columnCount(const QModelIndex&) const override { return N_COLS; }
    QVariant data(const QModelIndex&, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    void reset();
protected:
    static constexpr int N_ROWS = 102;
    static constexpr int COL_NUM = 0;
    static constexpr int COL_CARD = 1;
    static constexpr int N_COLS = 2;
};

class FmDebugPlural : public QDialog
{
    Q_OBJECT
    using Super = QDialog;
    using This = FmDebugPlural;

public:
    explicit FmDebugPlural(QWidget *parent = nullptr);
    ~FmDebugPlural() override;

    int exec() override;

private:
    Ui::FmDebugPlural *ui;
    PluralModel model;
};

#endif // FMDEBUGPLURAL_H
