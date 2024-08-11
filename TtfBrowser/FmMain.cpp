#include "FmMain.h"
#include "ui_FmMain.h"

#include "i_OpenSave.h"


///// TreeModel ////////////////////////////////////////////////////////////////


const mf::Obj& TreeModel::objOf(const QModelIndex& index) const
{
    if (auto ptr = index.constInternalPointer()) {
        return *static_cast<const mf::Obj*>(ptr);
    } else {
        return font;
    }
}


QModelIndex TreeModel::index(int row, int, const QModelIndex &parent) const
{
    auto& obj = objOf(parent);
    if (static_cast<size_t>(row) < obj.nChildren()) {
        auto& obj2 = obj.childAt(row);
        return createIndex(row, 0, &obj2);
    } else {
        return {};
    }
}


QModelIndex TreeModel::parent(const QModelIndex &child) const
{
    auto& obj = objOf(child);
    switch (obj.type()) {
    case mf::Type::FONT:    // Has no father, return anything
    case mf::Type::BLOCK:   // Has a father of font (root)
        return {};
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


QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case Qt::DisplayRole: {
            auto& obj = objOf(index);
            return QString::fromStdString(obj.text());
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

    // Models
    ui->treeStructure->setModel(&treeModel);

    // Connect events
    connect(ui->btOpen, &QPushButton::clicked, this, &This::doOpen);
}

FmMain::~FmMain()
{
    delete ui;
}

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
    /// @todo [urgent] reset
    MemFont tmpFont;
    tmpFont.load(QString::fromStdWString(fname));

    treeModel.beginResetModel();
    treeModel.font = std::move(tmpFont);
    treeModel.endResetModel();
}
