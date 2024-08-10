#include "FmMain.h"
#include "ui_FmMain.h"


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
    : QMainWindow(parent)
    , ui(new Ui::FmMain)
{
    ui->setupUi(this);
}

FmMain::~FmMain()
{
    delete ui;
}

