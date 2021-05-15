// My header
#include "FmMain.h"
#include "ui_FmMain.h"

// Qt
#include <QTableView>

// Unicode data
#include "UcDefines.h"

// Misc
#include "u_Strings.h"
#include "u_Qstrings.h"

///// RowCache /////////////////////////////////////////////////////////////////

RowCache::RowCache(int anCols)
    : fnCols(anCols), fColMask(anCols - 1), fRowMask(~fColMask) {}


const uc::Cp* RowCache::charAt(size_t iRow, unsigned iCol) const
{
    // Have row?
    if (iRow >= rows.size() || iCol >= NCOLS)
        return nullptr;
    auto& rw = rows[iRow];

    return rw.cps[iCol];
}


int RowCache::startingCpAt(size_t iRow) const
{
    if (iRow >= rows.size())
        return uc::NO_CHAR;
    return rows[iRow].startingCp;
}


RowCache::Row& RowCache::ensureRow(unsigned aStartingCp)
{
    if (!rows.empty()) {
        Row& bk = rows.back();
        if (bk.startingCp == aStartingCp)
            return bk;
    }
    Row& newBk = rows.emplace_back(aStartingCp);
    return newBk;
}


void RowCache::addCp(const uc::Cp& aCp)
{
    unsigned code = aCp.subj;
    int rowCp = code & fRowMask;
    int iCol = code & fColMask;

    Row& bk = ensureRow(rowCp);
    bk.cps[iCol] = &aCp;
}


///// CharsModel ///////////////////////////////////////////////////////////////


CharsModel::CharsModel() : rows(NCOLS) {}


int CharsModel::rowCount(const QModelIndex&) const
{
    return rows.nRows();
}


int CharsModel::columnCount(const QModelIndex&) const
    { return NCOLS; }


QVariant CharsModel::data(const QModelIndex& index, int role) const
{
    switch (role) {
    case Qt::DisplayRole: {
            auto cp = rows.charAt(index.row(), index.column());
            if (!cp)
                return {};
            return str::toQ(cp->proxy);
        }

    case Qt::FontRole:
        return fontBasic;

    case Qt::TextAlignmentRole:
        return Qt::AlignCenter;
    default:
        return {};
    }
}


namespace {

    template <int Ncols>
    QString horzHeaderOf(int section);

    template <>
    [[maybe_unused]] inline QString horzHeaderOf<8>(int section)
    {
        return QString::number(section, 16) + QString::number(section + 8, 16);
    }

    template <>
    [[maybe_unused]] inline QString horzHeaderOf<16>(int section)
    {
        return QString::number(section, 16);
    }

}   // anon namespace


QVariant CharsModel::headerData(int section, Qt::Orientation orientation,
                    int role) const
{
    switch (role) {
    case Qt::DisplayRole:
        if (orientation == Qt::Horizontal) {
            return horzHeaderOf<NCOLS>(section);
        } else {
            int c = rows.startingCpAt(section);
            if (static_cast<size_t>(section) >= rows.nRows())
                return {};
            char buf[20];
            snprintf(buf, std::size(buf), "%04x", c);
            return buf;
        }
        break;
    default:
        return {};
    }
}


void CharsModel::addCp(const uc::Cp& aCp)
{
    rows.addCp(aCp);
}


void CharsModel::initFonts()
{
    fontBasic = QFont("Cambria", FONT_SIZE);
    fontBasic.setStyleStrategy(
                static_cast<QFont::StyleStrategy>(QFont::PreferAntialias | QFont::PreferQuality));
}


///// FmMain ///////////////////////////////////////////////////////////////////

FmMain::FmMain(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::FmMain)
{
    ui->setupUi(this);

    // Table
    ui->tableChars->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableChars->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableChars->setModel(&model);

    // Fill chars
    auto n = uc::nCps();
    for (unsigned i = 0; i < n; ++i) {
        const auto& cp = uc::cpInfo[i];
        model.addCp(cp);
    }

    // Set fonts
    model.initFonts();
    QFont fontBasicBigger = model.fontBasic;
    fontBasicBigger.setPointSize(50);
    ui->lbTypoEngine->setFont(fontBasicBigger);

    // Connect events
    connect(ui->tableChars->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &This::charChanged);
}

FmMain::~FmMain()
{
    delete ui;
}


void FmMain::showCp(const uc::Cp& cp)
{
    // Code
    char buf[10];
    snprintf(buf, std::size(buf), "U+%04X", static_cast<unsigned>(cp.subj));
    ui->lbCharCode->setText(buf);

    // OS char
    ui->lbTypoEngine->setText(str::toQ(cp.proxy));

    // Text
    // Header
    QString text;
    text.append("<h1>");
    text.append(str::toQ(cp.name.tech));
    text.append("</h1>");

    {   // Info box
        text.append("<p>");
        str::QSep sp(text, "<br>");

        // Unicode version
        text.append(u8"Версия Юникода: ");
        str::append(text, cp.version().name);
        text.append(" (");
        text.append(QString::number(cp.version().year));
        text.append(")");

        // Character type
        sp.sep();
        text.append(u8"Тип: ");
        str::append(text, cp.category().locName);

        // Numeric value
        if (cp.numeric.isPresent()) {
            sp.sep();
            str::append(text, cp.numeric.type().name);
            text.append(": ");
            text.append(QString::number(cp.numeric.num));
            if (cp.numeric.denom != 1) {
                text.append("/");
                text.append(QString::number(cp.numeric.denom));
            }
        }

        text.append("</p>");
    }
    ui->vwInfo->setText(text);
}


void FmMain::charChanged(const QModelIndex& current)
{
    if (auto cp = model.charAt(current))
        showCp(*cp);
}
