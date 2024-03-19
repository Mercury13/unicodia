#include "FmMain.h"
#include "ui_FmMain.h"

// STL
#include <map>

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
        unsigned long size;
    };

    using DirInfo = std::map<std::wstring, FileInfo>;

    DirInfo retrieveFnames(const std::filesystem::path& dir)
    {
        DirInfo r;
        std::filesystem::directory_iterator it(dir);
        for (auto& v : it) {
            if (!v.is_directory()
                    && v.path().extension() == ".bmp") {
                auto& info = r[v.path().filename()];
                info.size = v.file_size();
            }
        }
        return r;
    }

}


void FmMain::run()
{
    auto cfg = config();
    if (!cfg.isFull()) {
        QMessageBox::critical(this, "Run", "Please set both directories.");
        return;
    }

    std::filesystem::path dirVersioned = cfg.versioned.toStdWString();
    auto filesVersioned = retrieveFnames(dirVersioned);

    std::filesystem::path dirWorking = cfg.working.toStdWString();
    auto filesWorking = retrieveFnames(dirWorking);
}
