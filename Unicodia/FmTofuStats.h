#ifndef FMTOFUSTATS_H
#define FMTOFUSTATS_H

// Qt
#include <QDialog>
#include <QAbstractTableModel>
#include <QPlainTextEdit>

// Libs
#include "u_Vector.h"
#include "UcAutoDefines.h"

namespace Ui {
class FmTofuStats;
}

namespace tofu
{
    struct Memo : public QPlainTextEdit
    {
    private:
        using Super = QPlainTextEdit;
    public:
        using QPlainTextEdit::QPlainTextEdit;
    protected:
        void inputMethodEvent(QInputMethodEvent *) override;
        void keyPressEvent(QKeyEvent *e) override;
        bool canInsertFromMimeData(const QMimeData *) const override { return false; }
        void insertFromMimeData(const QMimeData *) override {}
    };

    struct Counter
    {
        int nOther = 0, nGood = 0, nTofu = 0, firstTofu = -1;
        [[nodiscard]] int nTotal() const { return nOther + nGood + nTofu; }
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

        SafeVector<int> build(uc::SvgChecker& svgChecker);
    private:
        enum : unsigned char {
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
    using Super = QDialog;
    using This = FmTofuStats;
public:
    explicit FmTofuStats(QWidget *parent, uc::SvgChecker& svgChecker);
    ~FmTofuStats() override;
    int exec() override;
private:
    Ui::FmTofuStats *ui;
    tofu::Model model;

    void buildModel(uc::SvgChecker& svgChecker);
};

#endif // FMTOFUSTATS_H
