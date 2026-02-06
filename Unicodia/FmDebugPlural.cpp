#include "FmDebugPlural.h"
#include "ui_FmDebugPlural.h"

// Qt
#include <QDialogButtonBox>

// Utils
#include "u_Qstrings.h"

// L10n
#include "LocList.h"
#include "LocFmt.h"


///// PluralModel //////////////////////////////////////////////////////////////


int PluralModel::columnCount(const QModelIndex&) const
    { return N_FIX_COLS + ordChannels.size(); }


QVariant PluralModel::data(const QModelIndex& index, int role) const
{
    if (role != Qt::DisplayRole)
        return {};

    auto num = index.row();
    switch (index.column()) {
    case COL_NUM:
        return num;
    case COL_CARD: {
            auto& cardRule = loc::currLang->cardRule;
            auto outcome = cardRule.ofInt(num);
            if (outcome > loc::Plural::LAST)
                return "Bad???";
            auto name = loc::pluralNames[static_cast<unsigned>(outcome)];
            return QString::fromLatin1(name.data(), name.size());
        }
    default: {
            unsigned i = index.column() - N_FIX_COLS;
            if (i >= ordChannels.size())
                return "???";
            auto text = ordChannels[i]->fmt(num);
            return str::toQ(text);
        }
    }
}


QVariant PluralModel::headerData(int section, Qt::Orientation orientation,
                    int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return {};

    switch (section) {
    case COL_NUM: return "Number";
    case COL_CARD: return "Cardinal";
    default: {
            unsigned i = section - N_FIX_COLS;
            if (i >= ordChannels.size())
                return "???";
            return QString::fromStdString(ordChannels[i]->name);
        }
    }
}


void PluralModel::reset()
{
    beginResetModel();
    ordChannels.clear();
    ordChannels.reserve(loc::currLang->ordChannels.size());
    for (auto& v : loc::currLang->ordChannels) {
        ordChannels.push_back(&v);
    }
    endResetModel();
}


///// FmDebugPlural ////////////////////////////////////////////////////////////


FmDebugPlural::FmDebugPlural(QWidget *parent) :
    Super(parent),
    ui(new Ui::FmDebugPlural)
{
    ui->setupUi(this);
    ui->treePlural->setModel(&model);
    connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &This::close);
}

FmDebugPlural::~FmDebugPlural()
{
    delete ui;
}


int FmDebugPlural::exec()
{
    model.reset();
    auto index0 = model.index(0, 0);
    ui->treePlural->setCurrentIndex(index0);
    ui->treePlural->setFocus();
    return Super::exec();
}
