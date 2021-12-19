#include "FmTofuStats.h"
#include "ui_FmTofuStats.h"

#include "UcData.h"
#include "u_Qstrings.h"


///// tofu::Counter //////////////////////////////////////////////////////////////


void tofu::Counter::reg(int code, uc::TofuState state)
{
    switch (state) {
    case uc::TofuState::NO_FONT:
        ++nOther;
        break;
    case uc::TofuState::PRESENT:
        ++nGood;
        break;
    case uc::TofuState::TOFU:
        ++nTofu;
        if (firstTofu < 0)
            firstTofu = code;
        break;
    }
}


///// tofu::Model //////////////////////////////////////////////////////////////


QVariant tofu::Model::data(const QModelIndex& index, int role) const
{
    unsigned iRow = index.row();
    if (iRow >= rows.size())
        return {};
    auto& rw = rows[iRow];
    switch (role) {
    case Qt::DisplayRole:
        switch (index.column()) {
        case COL_TOTAL: return rw.data.nTotal();
        case COL_OTHER: return rw.data.nOther;
        case COL_GOOD: return rw.data.nGood;
        case COL_TOFU: return rw.data.nTofu;
        case COL_FIRST_TOFU: {
                if (rw.data.firstTofu < 0)
                    return {};
                char buf[10];
                snprintf(buf, std::size(buf), "%04X", rw.data.firstTofu);
                return QString(buf);
            }
        }
        [[fallthrough]];
    default:
        return {};
    }
}


const char* tofu::Model::colNames[COL_N] {
    "Total", "Custom-drawn", "Good", "Tofu", "1st tofu"
};


QVariant tofu::Model::headerData(
        int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            if (section < COL_N) {
                return colNames[section];
            }
        } else {
            if (static_cast<unsigned>(section) < rows.size()) {
                return rows[section].name;
            }
        }
    }
    return {};
}


namespace {

    class VersionCounter
    {
    public:
        void reg(int code, uc::EcVersion version, uc::TofuState state);
        void drop(SafeVector<tofu::Named>& r, const char* name);
    private:
        tofu::Counter total;
        tofu::Counter byVer[static_cast<int>(uc::EcVersion::NN)];
    };

    void VersionCounter::reg(int code, uc::EcVersion version, uc::TofuState state)
    {
        total.reg(code, state);
        byVer[static_cast<int>(version)].reg(code, state);
    }

    void VersionCounter::drop(SafeVector<tofu::Named>& r, const char* name)
    {
        r.emplace_back(name, total);
        for (size_t iVer = 0; iVer < std::size(byVer); ++iVer) {
            auto& ctr = byVer[iVer];
            if (ctr.nTofu != 0) {
                r.emplace_back("-- v" + str::toQ(uc::versionInfo[iVer].name), ctr);
            }
        }
    }

}   // anon namespace


SafeVector<int> tofu::Model::build()
{
    beginResetModel();
    rows.clear();

    SafeVector<int> allTofu;

    Counter all;
    VersionCounter cjk, rest;
    Counter byPlane[uc::N_PLANES];
    Counter byBlock[uc::N_BLOCKS];

    for (size_t i = 0; i < uc::N_CPS; ++i) {
        auto& cp = uc::cpInfo[i];
        auto code = cp.subj.val();
        auto tofuInfo = cp.tofuInfo();

        all.reg(code, tofuInfo.state);
        byPlane[cp.plane()].reg(code, tofuInfo.state);
        auto iBlock = tofuInfo.block->index();
        byBlock[iBlock].reg(code, tofuInfo.state);

        if (tofuInfo.place == uc::TofuPlace::CJK) {
            cjk.reg(code, cp.ecVersion, tofuInfo.state);
        } else {
            rest.reg(code, cp.ecVersion, tofuInfo.state);
        }

        if (tofuInfo.state == uc::TofuState::TOFU)
            allTofu.push_back(code);
    }

    rows.emplace_back("All", all);
    cjk.drop(rows, "CJK");
    rest.drop(rows, "Rest");

    for (int iPlane = 0; iPlane < uc::N_PLANES; ++iPlane) {
        auto& ctr = byPlane[iPlane];
        if (ctr.nTotal() > 0) {
            char buf[30];
            snprintf(buf, std::size(buf), "Plane %d", iPlane);
            rows.emplace_back(buf, ctr);
        }
    }

    for (int iBlock = 0; iBlock < uc::N_BLOCKS; ++iBlock) {
        auto& ctr = byBlock[iBlock];
        if (ctr.nTofu > 0) {
            rows.emplace_back(str::toQ(uc::blocks[iBlock].name), ctr);
        }
    }

    return allTofu;
    endResetModel();
}


///// FmTofuStats //////////////////////////////////////////////////////////////


FmTofuStats::FmTofuStats(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FmTofuStats)
{
    ui->setupUi(this);
    ui->tableStats->setModel(&model);
    buildModel();
}


FmTofuStats::~FmTofuStats()
{
    delete ui;
}


void FmTofuStats::buildModel()
{
    auto r = model.build();
    QString s;
    s.reserve(r.size() * 5);
    for (int code : r) {
        char buf[10];
        snprintf(buf, std::size(buf), "%04X\n", code);
        s.append(buf);
    }
    ui->memoTofuList->setPlainText(s);
    ui->tableStats->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableStats->resizeColumnsToContents();
}
