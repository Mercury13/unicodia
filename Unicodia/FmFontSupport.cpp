#include "FmFontSupport.h"
#include "ui_FmFontSupport.h"

#include <QDialogButtonBox>
#include "Skin.h"

// Unicode
#include "UcData.h"
#include "UcCp.h"
#include "LocDic.h"

///// FontSupportModel /////////////////////////////////////////////////////////

FontSupportModel::FontSupportModel(FontSource& aFontSource)
    : fontSource(aFontSource)
{}

int FontSupportModel::rowCount(const QModelIndex&) const
{
    return size();
}

QVariant FontSupportModel::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case Qt::DisplayRole: {
            // Check bounds
            if (static_cast<size_t>(index.row()) >= size())
                return {};
            // Go!
            auto& entry = entries[index.row()];
            return entry.name;
        }
    default:
        return {};
    }
}

void FontSupportModel::loadFrom(char32_t c)
{
    // Sample
    sample = QString::fromUcs4(&c, 1);
    if (c < uc::CAPACITY) {
        if (auto pCp = uc::cpsByCode[c]) {
            if (pCp->category().upCat == uc::EcUpCategory::MARK)
                sample = QChar(cp::DOTTED_CIRCLE) + sample;
        }
    }

    // Get fonts
    auto fonts = fontSource.allSysFonts(c,
            QFontDatabase::Any, std::numeric_limits<size_t>::max());

    beginResetModel();
    entries.clear();
    entries.reserve(fonts.lines.size());

    for (auto& v : fonts.lines) {
        entries.emplace_back(std::move(v.name));
    }
    std::ranges::sort(entries,
        [](const Entry& x, const Entry& y) {
            return (x.name < y.name);
        });
    endResetModel();
}

///// FmFontSupport ////////////////////////////////////////////////////////////

FmFontSupport::FmFontSupport(QWidget *parent, FontSource& aFontSource) :
    QDialog(parent),
    ui(new Ui::FmFontSupport),
    model(aFontSource)
{
    ui->setupUi(this);
    ui->listFonts->setModel(&model);
    connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &This::close);
    connect(ui->listFonts->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &This::selectionChanged);
}

FmFontSupport::~FmFontSupport()
{
    delete ui;
}

void FmFontSupport::selectFirstFontIf()
{
    if (model.size() > 0) {
        ui->listFonts->setCurrentIndex(model.index(0));
    } else {
        ui->lbSample->setText({});
    }
    ui->listFonts->setFocus();
}

void FmFontSupport::loadFrom(char32_t c)
{
    char buf[200];
    auto format = loc::get("Prop.Os.FontsFor").c_str();
    snprintf(buf, std::size(buf), reinterpret_cast<const char*>(format), (int)c);
    setWindowTitle(buf);

    model.loadFrom(c);
    selectFirstFontIf();
}

void FmFontSupport::exec(char32_t c)
{
    loadFrom(c);
    Super::exec();
}

void FmFontSupport::selectionChanged(
        const QModelIndex& curr, const QModelIndex& prev)
{
    size_t index = curr.row();
    if (index >= model.size()) {
        ui->lbSample->setText({});
        return;
    }
    auto& entry = model.entries[index];
    QFont fnt(entry.name, (int)Fsz::BIG);
    ui->lbSample->setFont(fnt);
    ui->lbSample->setText(model.sample);
}
