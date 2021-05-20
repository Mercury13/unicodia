#ifndef FMMAIN_H
#define FMMAIN_H

// C++

// Qt
#include <QMainWindow>
#include <QAbstractTableModel>

// My libs
#include "u_Vector.h"
#include "u_TinyOpt.h"

// Unicode data
#include "UcData.h"

QT_BEGIN_NAMESPACE
namespace Ui { class FmMain; }
QT_END_NAMESPACE


constexpr int NCOLS = 8;
constexpr int FONT_SIZE = 14;

class FmPopup;

class RowCache
{
public:
    RowCache(int anCols);

    size_t nRows() const { return rows.size(); }
    size_t nCols() const { return fnCols; }

    void addCp(const uc::Cp& aCp);

    /// @return  code point if it’s really present
    const uc::Cp* charAt(size_t iRow, unsigned iCol) const;

    /// @return  starting code point of row; or NO_CHAR if bad row
    int startingCpAt(size_t iRow) const;
protected:
    const int fnCols, fColMask, fRowMask;

    struct Row
    {
        unsigned startingCp = 0xFFFFFF;

        const uc::Cp* cps[NCOLS];

        Row() { clear(); }
        Row(int aStartingCp) : startingCp(aStartingCp) { clear(); }

        void clear() { std::fill(std::begin(cps), std::end(cps), nullptr); }
    };

    SafeVector<Row> rows;
    Row& ensureRow(unsigned aStartingCp);
};


class CharsModel : public QAbstractTableModel
{
public:
    QFont fontBasic;

    CharsModel();

    int rowCount(const QModelIndex&) const override;
    int columnCount(const QModelIndex&) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    void initFonts();
    void addCp(const uc::Cp& aCp);
    const uc::Cp* charAt(const QModelIndex& index) const
            { return rows.charAt(index.row(), index.column()); }
private:
    RowCache rows;
};


class FmMain : public QMainWindow
{
    Q_OBJECT
    using This = FmMain;

public:
    FmMain(QWidget *parent = nullptr);
    ~FmMain() override;

private:
    Ui::FmMain *ui;
    CharsModel model;
    std::unique_ptr<FmPopup> popup;

    void showCp(const uc::Cp& cp);
    void linkClicked(std::string_view scheme, std::string_view target,
                     QWidget* widget, TinyOpt<QRect> rect);
    template <class T>
    void showPopupT(const T& x, QWidget* widget, TinyOpt<QRect> rect);
    void showPopup(const uc::BidiClass& x, QWidget* widget, TinyOpt<QRect> rect);
    void showPopup(const uc::Category& x, QWidget* widget, TinyOpt<QRect> rect);
private slots:
    void charChanged(const QModelIndex& current);
    void on_vwInfo_anchorClicked(const QUrl &arg1);
};

#endif // FMMAIN_H
