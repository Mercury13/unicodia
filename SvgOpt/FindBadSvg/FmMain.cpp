#include "FmMain.h"
#include "ui_FmMain.h"

#include "i_OpenSave.h"

#include "work.h"

///// ResetLock ////////////////////////////////////////////////////////////////

ResetLock::ResetLock(LockableModel& aModel) : model(aModel)
{
    model.beginResetModel();
}

ResetLock::~ResetLock()
{
    model.endResetModel();
}


///// QSep /////////////////////////////////////////////////////////////////////

namespace {

    class QSep
    {
    public:
        QString& target;

        QSep(QString& aTarget, QString aSep)
            : target(aTarget), sp(std::move(aSep)), len(target.length()) {}
        void sep();
        void sepAnd(const QString& s);
        void sepAnd(const char* s);
    private:
        QString sp;
        qsizetype len;
    };

    void QSep::sep()
    {
        if (target.length() != len) {
            target += sp;
        }
    }

    void QSep::sepAnd(const QString& s)
    {
        sep();
        target += s;
    }

    void QSep::sepAnd(const char* s)
    {
        sep();
        target += s;
    }

}   // anon namespace


///// BadModel /////////////////////////////////////////////////////////////////

QVariant BadModel::data(const QModelIndex &index, int role) const
{
    if (static_cast<size_t>(index.row()) >= files.size())
        return {};
    auto& row = files[index.row()];
    switch (role) {
    case Qt::DisplayRole: {
            QString r = QString::fromStdWString(row.name) + ": ";
            if (row.isValid) {
                QSep sp(r, ", ");
                if (row.discrep.hasClipPath) {
                    sp.sepAnd("clipPath");
                }
                if (row.discrep.hasRgbaColor) {
                    sp.sepAnd("rgba colour");
                }
                if (row.discrep.hasGradientXlink) {
                    sp.sepAnd("gradient Xlink");
                }
            } else {
                r += "UNREADABLE";
            }
            return r;
        }
    default:
        return {};
    }
}


///// GoodModel ////////////////////////////////////////////////////////////////

QVariant GoodModel::data(const QModelIndex &index, int role) const
{
    if (static_cast<size_t>(index.row()) >= files.size())
        return {};
    auto& row = files[index.row()];
    switch (role) {
    case Qt::DisplayRole:
        return QString::fromStdWString(row);
    default:
        return {};
    }
}


///// FmMain ///////////////////////////////////////////////////////////////////

FmMain::FmMain(QWidget *parent)
    : Super(parent)
    , ui(new Ui::FmMain)
{
    ui->setupUi(this);
    ui->listBad->setModel(&badModel);
    ui->listGood->setModel(&goodModel);
    connect(ui->btBrowse, &QPushButton::clicked, this, &This::browseForDirectory);
    connect(ui->btGo, &QPushButton::clicked, this, &This::go);
}

FmMain::~FmMain()
{
    delete ui;
}

void FmMain::browseForDirectory()
{
    filedlg::Filter filter { L"SVG files", L"*.svg" };
    if (!filedlg::browseLineEdit(
            this, L"Choose directory", filter, L".svg",
            filedlg::BrowseMode::DIRECTORY, ui->edDirectory))
        return;
}

void FmMain::go()
{
    ResetLock _1(badModel), _2(goodModel);

    std::filesystem::path dir = ui->edDirectory->text().toStdWString();
    auto r = processDir(dir);
    badModel.take(std::move(r.bad));
    goodModel.take(std::move(r.good));
}
