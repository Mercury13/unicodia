#include "FmMain.h"
#include "ui_FmMain.h"

// C++
#include <cmath>

// STL
#include <map>
#include <ranges>
#include <array>

// Qt
#include "QMessageBox"

// XML
#include "pugixml.hpp"

///// Config ///////////////////////////////////////////////////////////////////


bool FmMain::Config::isPresent() const
{
    return !versioned.isEmpty()
        || !working.isEmpty();
}


bool FmMain::Config::isFull() const
{
    return !versioned.isEmpty()
        && !working.isEmpty();
}


///// DiffModel ////////////////////////////////////////////////////////////////

DiffModel::DiffModel()
{
    /// @todo [urgent] load icons
}

int DiffModel::rowCount(const QModelIndex&) const
    { return lines.size(); }

int DiffModel::columnCount(const QModelIndex&) const
    { return 1; }

QVariant DiffModel::data(const QModelIndex &index, int role) const
{
    size_t i = index.row();
    if (i > lines.size())
        return {};
    auto& line = lines[i];

    switch (role) {
    case Qt::DisplayRole:
        return QString::fromStdWString(line.name);
    case Qt::DecorationRole:
        switch (line.reason) {
        case DiffReason::ADDED:   return icoAdded;
        case DiffReason::DELETED: return icoDeleted;
        case DiffReason::CHANGED: return icoResized;
        case DiffReason::RESIZED: return icoChanged;
        }
        [[fallthrough]];  // should not happen
    default:
        return {};
    }
}


void DiffModel::addVersioned(std::wstring_view name)
{
    QPixmap pix;
    pix.load(QString::fromStdWString(dirVersioned / name));

    auto& v = lines.emplace_back();
    v.name = name;
    v.reason = DiffReason::DELETED;
    v.pix = std::move(pix);
}


void DiffModel::addWorking(std::wstring_view name)
{
    QPixmap pix;
    pix.load(QString::fromStdWString(dirWorking / name));

    auto& v = lines.emplace_back();
    v.name = name;
    v.reason = DiffReason::ADDED;
    v.pix = std::move(pix);
}

void DiffModel::addResized(std::wstring_view name, QPixmap&& pix)
{
    auto& v = lines.emplace_back();
    v.name = name;
    v.reason = DiffReason::RESIZED;
    v.pix = std::move(pix);
}

void DiffModel::addResized(std::wstring_view name)
{
    QPixmap pix;
    pix.load(QString::fromStdWString(dirWorking / name));
    addResized(name, std::move(pix));
}

namespace {

    /// Greater = more colours, cmaller diff visible
    static constexpr int N_COLORS = 32;
    using ArColors = std::array<int, N_COLORS>;

    ArColors makeGammaMap(float gamma)
    {
        auto invGamma = 1.0f / gamma;
        ArColors r {};
        r[0] = 0;
        for (int i = 1; i < N_COLORS; ++i) {
            auto fval = i / 255.0f;
            auto modfval = std::pow(fval, invGamma);
            r[i] = std::round(modfval * 255.0f);
        }
        return r;
    }

    static auto cmap = makeGammaMap(2.6f);

    int colorDiff(int c1, int c2)
    {
        auto dif = abs(c1 - c2);
        return (dif >= N_COLORS) ? 255 : cmap[dif];
    }

}   // anon namespace

void DiffModel::checkChanged(std::wstring_view name,
                             unsigned long long versSize,
                             unsigned long long workSize)
{
    if (versSize != workSize) {
        addResized(name);
        return;
    }

    QImage imVersioned, imWorking;
    imVersioned.load(QString::fromStdWString(dirVersioned / name));
    imWorking.load(QString::fromStdWString(dirWorking / name));

    // Compare pixel size
    auto sz = imVersioned.size();
    if (sz != imWorking.size()) {
        addResized(name, QPixmap::fromImage(std::move(imWorking)));
        return;
    }

    // Compare byte size
    auto lenV = imVersioned.sizeInBytes();
    auto lenW = imWorking.sizeInBytes();
    if (lenV != lenW) {
        addResized(name, QPixmap::fromImage(std::move(imWorking)));
        return;
    }

    // Compare raw data
    auto dataV = imVersioned.constBits();
    auto dataW = imWorking.constBits();
    if (memcmp(dataV, dataW, lenV) == 0)
        return;

    QImage diff(sz, QImage::Format_RGB32);
    for (int y = 0; y < sz.height(); ++y) {
        for (int x = 0; x < sz.width(); ++x) {
            QColor cVers = imVersioned.pixelColor(x, y).toRgb();
            QColor cWork = imWorking.pixelColor(x, y).toRgb();
            auto r = colorDiff(cVers.red(),   cWork.red()  );
            auto g = colorDiff(cVers.green(), cWork.green());
            auto b = colorDiff(cVers.blue(),  cWork.blue()  );
            diff.setPixelColor(x, y, QColor::fromRgb(r, g, b));
        }
    }

    auto& v = lines.emplace_back();
    v.name = name;
    v.reason = DiffReason::CHANGED;
    v.pix = QPixmap::fromImage(std::move(diff));
}


QPixmap DiffModel::diffPix(size_t index) const
{
    if (index >= lines.size())
        return {};
    return lines[index].pix;
}


///// FmMain ///////////////////////////////////////////////////////////////////


FmMain::FmMain(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::FmMain)
{
    ui->setupUi(this);
    ui->splitMain->setSizes({100, 200});

    // Init config path
    std::filesystem::path path = QCoreApplication::applicationFilePath().toStdWString();
    path.remove_filename();
    configPath = path / "config.xml";

    // Tie things
    ui->treeObjects->setModel(&diffModel);
    ui->treeObjects->setIconSize({14, 12});

    // Tie events
    connect(ui->btCompare, &QPushButton::clicked, this, &This::run);
    connect(ui->treeObjects->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &This::diffCurrentChanged);
}

FmMain::~FmMain()
{
    delete ui;
}

FmMain::Config FmMain::config()
{
    return {
        .versioned = ui->edVersioned->text().trimmed(),
        .working = ui->edWorking->text().trimmed(),
    };
}


void FmMain::saveConfig()
{
    auto cfg = config();
    if (cfg.isPresent()) {
        pugi::xml_document doc;
        auto hRoot = doc.append_child("config");

        auto hDirs = hRoot.append_child("dirs");
        hDirs.append_attribute("versioned") = cfg.versioned.toStdString().c_str();
        hDirs.append_attribute("working") = cfg.working.toStdString().c_str();

        doc.save_file(configPath.c_str());
    }
}

void FmMain::loadConfig()
{
    pugi::xml_document doc;
    if (doc.load_file(configPath.c_str())) {
        auto hRoot = doc.child("config");

        auto hDirs = hRoot.child("dirs");
        ui->edVersioned->setText(hDirs.attribute("versioned").as_string());
        ui->edWorking->setText(hDirs.attribute("working").as_string());
    }
}


namespace {

    struct FileInfo {
        unsigned long long size;
    };

    using DirInfo = std::map<std::wstring, FileInfo>;

    DirInfo retrieveFnames(const std::filesystem::path& dir)
    {
        DirInfo r;
        std::error_code errc;
        std::filesystem::directory_iterator it(dir, errc);
        if (errc == std::error_code{}) {
            for (auto& v : it) {
                if (!v.is_directory()
                        && v.path().extension() == ".bmp") {
                    auto& info = r[v.path().filename()];
                    info.size = v.file_size();
                }
            }
        }
        return r;
    }

}   // anon namespace


void FmMain::run()
{
    auto cfg = config();
    if (!cfg.isFull()) {
        QMessageBox::critical(this, "Run", "Please set both directories.");
        return;
    }

    std::filesystem::path dirVersioned = cfg.versioned.toStdWString();
    auto filesVersioned = retrieveFnames(dirVersioned);
    if (filesVersioned.empty()) {
        QMessageBox::critical(this, "Run", "No versioned BMP files.");
        return;
    }

    std::filesystem::path dirWorking = cfg.working.toStdWString();
    auto filesWorking = retrieveFnames(dirWorking);
    if (filesVersioned.empty()) {
        QMessageBox::critical(this, "Run", "No working BMP files.");
        return;
    }

    ui->wiViewer->removePix();
    diffModel.beginResetModel();
    try {
        diffModel.clear();
        diffModel.dirVersioned = dirVersioned;
        diffModel.dirWorking = dirWorking;

        auto pVersioned = filesVersioned.begin();
        auto pWorking = filesWorking.begin();
        while (true) {
            // Synchronous traversal through 2 maps
            if (pVersioned == filesVersioned.end()) {
                // No versioned files left, add working
                for (auto& [key, _] : std::ranges::subrange(pWorking, filesWorking.end()))
                    diffModel.addWorking(key);
                break;
            }
            if (pWorking == filesWorking.end()) {
                // No working files left, remove versioned
                for (auto& [key, _] : std::ranges::subrange(pVersioned, filesVersioned.end()))
                    diffModel.addVersioned(key);
                break;
            }
            if (pWorking->first == pVersioned->first) {
                diffModel.checkChanged(pVersioned->first,
                            pVersioned->second.size, pWorking->second.size);
                ++pVersioned;
                ++pWorking;
            } else if (pVersioned->first <= pWorking->first) {
                diffModel.addVersioned(pVersioned->first);
                ++pVersioned;
            } else {
                diffModel.addWorking(pWorking->first);
                ++pWorking;
            }
        }
    } catch (...) {}
    diffModel.endResetModel();
    if (diffModel.rowCount() == 0) {
        QMessageBox::information(this, "Run", "Directories are equal, you’re cool!");
    } else {
        auto index = diffModel.index(0, 0);
        ui->treeObjects->selectionModel()->select(
                    index, QItemSelectionModel::SelectCurrent);
        diffCurrentChanged(index);
    }
}


void FmMain::diffCurrentChanged(const QModelIndex& index)
{
    auto&& pix = diffModel.diffPix(index.row());
    ui->wiViewer->setPix(pix);
}
