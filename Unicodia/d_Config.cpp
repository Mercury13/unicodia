// My header
#include "d_Config.h"

// Qt
#include <QCoreApplication>
#include <QStandardPaths>

// XML
#include "pugixml.hpp"

// Misc
#include "u_EcArray.h"

// L10n
#include "LocList.h"


#ifdef _WIN64
    constexpr std::string_view COINCIDING_PLATFORMS = "win64|w64";
    //constexpr std::string_view COMPATIBLE_PLATFORMS = "w32|win32";
#else
    #error Bad platform!
#endif

///// Vars /////////////////////////////////////////////////////////////////////

// progsets
progsets::DirMode progsets::dirMode = progsets::DirMode::INSTALLED;

Version updatever::version;
bool updatever::isDebuggingVersion = false;
std::string updatever::coincidingPlatforms { COINCIDING_PLATFORMS };

// fname
std::filesystem::path fname::config;
std::filesystem::path fname::progsets;

// path
std::filesystem::path path::exeBundled;
std::filesystem::path path::exeAdmined;
std::filesystem::path path::config;

// config
bool config::window::isMaximized;
std::string config::lang::wanted;
int config::lang::savedStamp = 0;

#define APP_NAME "Unicodia"

constexpr std::string_view APP_XML = APP_NAME ".xml";
constexpr std::string_view CONFIG_NAME = "config.xml";

namespace {

    constinit const ec::Array<std::string_view, BlockOrder> orderNames {
        "alpha", "continent", "code", "tech"
    };

    void loadProgSets()
    {
        static std::string myCoincidingPlatforms;

        progsets::dirMode = progsets::DirMode::INSTALLED;

        if (fname::progsets.empty() || !std::filesystem::exists(fname::progsets))
            return;

        pugi::xml_document doc;
        doc.load_file(fname::progsets.c_str());
        auto root = doc.child("program");
        if (root.attribute("portable").as_bool(false))
            progsets::dirMode = progsets::DirMode::PORTABLE;
        if (auto ver = Version::parsePermissive(root.attribute("debugVersion").as_string())) {
            updatever::version = ver;
            updatever::isDebuggingVersion = true;
        }
    }

    void loadConfig([[maybe_unused]] QRect& winRect, BlockOrder& blockOrder)
    {
        if (fname::config.empty() || !std::filesystem::exists(fname::config))
            return;

        pugi::xml_document doc;
        if (auto res = doc.load_file(fname::config.c_str())) {
            auto root = doc.child("config");

            // Do not save window position right now
//            auto oldW = winRect.width();
//            auto oldH = winRect.height();
//            auto tagWin = root.child("window");
//                winRect.setLeft(tagWin.attribute("x").as_int(winRect.left()));
//                winRect.setTop(tagWin.attribute("y").as_int(winRect.top()));
//                winRect.setWidth(tagWin.attribute("w").as_int(oldW));
//                winRect.setHeight(tagWin.attribute("h").as_int(oldH));
//                config::window::isMaximized = tagWin.attribute("max").as_bool();

            auto hLang = root.child("lang");
            config::lang::wanted = hLang.attribute("v").as_string();
            config::lang::savedStamp = hLang.attribute("stamp").as_int();

            auto hView = root.child("view");
            auto sortName = hView.attribute("sort").as_string();
            blockOrder = orderNames.findDef(sortName, blockOrder);
        }
    }

}   // anon namespace

void config::init(QRect& winRect, BlockOrder& blockOrder)
{
    path::exeBundled = QCoreApplication::applicationFilePath().toStdWString();
#ifdef _WIN32
    path::exeBundled.remove_filename();
    path::exeAdmined = path::exeBundled;
    fname::progsets = path::exeAdmined / APP_XML;
#else
    #error Unknown OS
#endif

    loadProgSets();

    switch (progsets::dirMode) {
    case progsets::DirMode::INSTALLED: {
            std::filesystem::path localDir {
                QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdWString() };
            path::config = localDir;
        } break;
    case progsets::DirMode::PORTABLE:
        path::config = path::exeAdmined;
        break;
    }
    fname::config = path::config / CONFIG_NAME;
    loadConfig(winRect, blockOrder);
}


void config::save(
        [[maybe_unused]] const QRect& winRect,
        [[maybe_unused]] bool isMaximized,
        BlockOrder blockOrder)
{
    std::filesystem::create_directories(path::config);
    pugi::xml_document doc;

    auto root = doc.append_child("config");

    auto hLang = root.append_child("lang");
        if (loc::currLang) {
            hLang.append_attribute("v") = loc::currLang->mainLang().c_str();
            hLang.append_attribute("stamp") = loc::allLangs.lastStamp();
        }

//    auto hWin = root.append_child("window");
//        hWin.append_attribute("x") = winRect.left();
//        hWin.append_attribute("y") = winRect.top();
//        hWin.append_attribute("w") = winRect.width();
//        hWin.append_attribute("h") = winRect.height();
//        hWin.append_attribute("max") = isMaximized;

    auto hView = root.append_child("view");
    hView.append_attribute("sort") = orderNames[blockOrder].data();   // OK, const s_v have trailing 0

    doc.save_file(fname::config.c_str());
}
