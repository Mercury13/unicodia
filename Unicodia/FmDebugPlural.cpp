#include "FmDebugPlural.h"
#include "ui_FmDebugPlural.h"

// Qt
#include <QDialogButtonBox>

// L10n
#include "LocList.h"
#include "LocFmt.h"


///// PluralModel //////////////////////////////////////////////////////////////

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
    default:
        return {};
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
    default:
        return "???";
    }
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
