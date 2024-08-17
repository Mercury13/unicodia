#include "FmMain.h"
#include "ui_FmMain.h"

#include "i_OpenSave.h"


///// TreeModel ////////////////////////////////////////////////////////////////


const mfs::Obj& TreeModel::objOf(const QModelIndex& index) const
{
    if (auto ptr = index.constInternalPointer()) {
        return *static_cast<const mfs::Obj*>(ptr);
    } else {
        return structure;
    }
}


QModelIndex TreeModel::indexOf(int row, const mfs::Obj* obj) const noexcept
{
    return createIndex(row, 0, obj);
}


QModelIndex TreeModel::index(int row, int, const QModelIndex &parent) const
{
    auto& obj = objOf(parent);
    if (static_cast<size_t>(row) < obj.nChildren()) {
        auto& obj2 = obj.childAt(row);
        return indexOf(row, obj2);
    } else {
        return {};
    }
}


QModelIndex TreeModel::parent(const QModelIndex &child) const
{
    auto& obj = objOf(child);
    switch (obj.type()) {
    case mfs::Type::FONT:    // Has no father, return anything
    case mfs::Type::BLOCK:   // Has a father of font (root)
        return {};
    case mfs::Type::CMAP:
        return indexOf(structure.cmapIndex(), structure.cmapBlock());
    }
    __builtin_unreachable();
}


int TreeModel::rowCount(const QModelIndex &parent) const
{
    auto& obj = objOf(parent);
    return obj.nChildren();
}


int TreeModel::columnCount(const QModelIndex &) const
    { return 1; }


namespace {

    void transformConstantLength(QString& text)
    {
        if (text.startsWith(' ')) {
            for (auto &q : text) {
                if (q == ' ') {
                    q = L'␣';
                } else {
                    break;
                }
            }
        }
        if (text.endsWith(' ')) {
            for (auto it = text.rbegin(); it != text.rend(); ++it) {
                auto& q = *it;
                if (q == ' ') {
                    q = L'␣';
                } else {
                    break;
                }
            }
        }
    }

}   // anon namespace


QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case Qt::DisplayRole: {
            auto& obj = objOf(index);
            auto text = obj.text();
            auto shownText = QString::fromStdString(text.value);
            if (text.isConstantLength) {
                transformConstantLength(shownText);
            }
            return shownText;
        }
    default:
        return {};
    }
}


///// FmMain ///////////////////////////////////////////////////////////////////

FmMain::FmMain(QWidget *parent)
    : Super(parent), ui(new Ui::FmMain)
{
    ui->setupUi(this);

    // UI
    ui->splitMain->setStretchFactor(0, 4);
    ui->splitMain->setStretchFactor(1, 8);

    // Models
    ui->treeStructure->setModel(&treeModel);

    // Connect events
    connect(ui->btOpen, &QPushButton::clicked, this, &This::doOpen);
    connect(ui->treeStructure->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &This::selCurrentChanged);

    selCurrentUpdate();
}

FmMain::~FmMain()
{
    delete ui;
}


namespace {

    void selectAny(QTreeView* view)
    {
        auto* model = view->model();
        QModelIndex idxRoot {};
        auto nRows = model->rowCount();
        if (nRows == 0) {
            view->setCurrentIndex(idxRoot);
        } else {
            auto firstIndex = model->index(0, 0, idxRoot);
            view->setCurrentIndex(firstIndex);
        }
    }

}   // anon namespace

void FmMain::doOpen()
{
    filedlg::Filters filters {
        { L"Font files", L"*.ttf *.otf" },
        { L"All files", L"*" }
    };
    auto fname = filedlg::open(
            this, L"Open font", filters, L"", filedlg::AddToRecent::NO,
            filedlg::CheckForAccess::YES);
    if (fname.empty())
        return;

    MemFont tmpFont;
    tmpFont.load(QString::fromStdWString(fname));

    treeModel.beginResetModel();
        ui->hxView->clear();
        treeModel.font = std::move(tmpFont);
        treeModel.structure.loadFrom(treeModel.font);

    treeModel.endResetModel();

    selectAny(ui->treeStructure);
    ui->treeStructure->setFocus();

    QString text;
    treeModel.font.traverseCps([&text](char32_t cp, unsigned glyph) {
        char buf[40];
        snprintf(buf, std::size(buf), "%04X→%u\n", (unsigned)cp, glyph);
        text += buf;
    });
    ui->memoMapping->setPlainText(text);
}


namespace {

    QString toReadableHex(size_t x)
    {
        char s[60];
        unsigned long long ull = x;
        snprintf(s, std::size(s), "%lld = $%llX", ull, ull);
        return s;
    }

}   // anon namespace


///
/// \param [in] nw   new index
///
void FmMain::selCurrentChanged(const QModelIndex& nw)
{
    if (nw.isValid()) {
        auto& obj = treeModel.objOf(nw);
        // Data span
        if (auto dspan = obj.dataSpan()) {
            ui->lbStartValue->setText(toReadableHex(dspan->fileOffset));
            ui->lbLenValue->setText(toReadableHex(dspan->size));
            ui->lbEndValue->setText(toReadableHex(dspan->fileOffset + dspan->size));
            auto entireFile = treeModel.font.data();
            auto data = obj.body(entireFile);
            auto dataArray = new QHexView::DataStorageArray(
                        QByteArray::fromRawData(data.buffer(), data.size()));
            ui->hxView->setData(dataArray);
        } else {
            removeDataSpan();
        }
        // Cmap info
        if (auto cm = obj.cmapInfo()) {
            ui->wiCmap->show();
            mfs::Cbuf buf;
            ui->lbPlatformValue->setText(mfs::toPlatformLongString(
                            cm->platformId, buf));
            ui->lbEncodingValue->setText(mfs::toEncodingLongString(
                            cm->platformId, cm->encodingId, buf));
            ui->lbFormatValue->setText(mfs::toTableFormatLongString(
                            cm->formatId, buf));
        } else {
            ui->wiCmap->hide();
        }
    } else {
        removeDataSpan();
        ui->wiCmap->hide();
    }
}


void FmMain::removeDataSpan()
{
    ui->lbStartValue->setText("-");
    ui->lbEndValue->setText("-");
    ui->lbLenValue->setText("-");
    ui->hxView->clear();
}


void FmMain::selCurrentUpdate()
{
    selCurrentChanged(ui->treeStructure->selectionModel()->currentIndex());
}
