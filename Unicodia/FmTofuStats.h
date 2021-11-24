#ifndef FMTOFUSTATS_H
#define FMTOFUSTATS_H

// Qt
#include <QDialog>
#include <QAbstractTableModel>

// Libs
#include "u_Vector.h"
#include "UcAutoDefines.h"

namespace Ui {
class FmTofuStats;
}

namespace tofu
{

    struct Counter
    {
        int nOther = 0, nGood = 0, nTofu = 0, firstTofu = -1;
        int nTotal() const { return nOther + nGood + nTofu; }
        void reg(int code, uc::TofuState state);
    };

    struct Named
    {
        QString name;
        Counter data;
    };

    class Model : public QAbstractTableModel
    {
    public:
        int rowCount(const QModelIndex&) const override { return rows.size(); }
        int columnCount(const QModelIndex&) const override { return COL_N; }
        QVariant data(const QModelIndex& index, int role) const override;
        QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

        SafeVector<int> build();
    private:
        enum {
            COL_TOTAL,
            COL_OTHER,
            COL_GOOD,
            COL_TOFU,
            COL_FIRST_TOFU,
            COL_N
        };
        static const char* colNames[COL_N];
        SafeVector<Named> rows;
    };

}   // namespace tofu

class FmTofuStats : public QDialog
{
    Q_OBJECT

public:
    explicit FmTofuStats(QWidget *parent = nullptr);
    ~FmTofuStats();

private:
    Ui::FmTofuStats *ui;
    tofu::Model model;
};

#endif // FMTOFUSTATS_H
