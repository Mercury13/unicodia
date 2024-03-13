// My header
#include "d_Config.h"

// Qt
#include <QCoreApplication>
#include <QStandardPaths>

// XML
#include "pugixml.hpp"

// Misc
#include "u_EcArray.h"
#include "UcData.h"

// L10n
#include "LocList.h"


#ifdef _WIN64
    constexpr std::string_view EQUIV_PLATFORMS = "win64|w64";
    //constexpr std::string_view COMPATIBLE_PLATFORMS = "w32|win32";
#else
    #error Bad platform!
#endif

///// Vars /////////////////////////////////////////////////////////////////////

// progsets
progsets::DirMode progsets::dirMode = progsets::DirMode::INSTALLED;

Version updatever::version;
bool updatever::isDebuggingVersion = false;
std::string updatever::equivPlatforms { EQUIV_PLATFORMS };

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
config::Favs config::favs;

#define APP_NAME "Unicodia"

constexpr std::string_view APP_XML = APP_NAME ".xml";
constexpr std::string_view CONFIG_NAME = "config.xml";

///// Favs /////////////////////////////////////////////////////////////////////

TinySizet config::Favs::add(char32_t code)
{
    // Add only feasible CPs
    if (code > uc::CAPACITY || !uc::cpsByCode[code])
        return TINY_NULL;
    // Already exists — do nothing
    auto [_, wasAdded] = ndx.insert(code);
    if (!wasAdded)
        return TINY_NULL;

    if (!fCodes.empty() && code > fCodes.back()) {
        // A very frequent branch
        auto oldSize = fCodes.size();
        fCodes.emplace_back(code);
        return oldSize;
    } else {
        auto whereInsert = std::lower_bound(fCodes.begin(), fCodes.end(), code);
        auto index = whereInsert - fCodes.begin();
        fCodes.insert(whereInsert, code);
        return index;
    }
}


void config::Favs::clear()
{
    fCodes.clear();
    ndx.clear();
}


TinySizet config::Favs::erase(char32_t code)
{
    // First erase from index
    if (!ndx.erase(code))
        return TINY_NULL;

    auto whereErase = std::equal_range(fCodes.begin(), fCodes.end(), code);
    auto index = whereErase.first - fCodes.begin();
    fCodes.erase(whereErase.first, whereErase.second);
    return index;
}


//// Config ////////////////////////////////////////////////////////////////////

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
        auto hRoot = doc.child("program");
        if (hRoot.attribute("portable").as_bool(false))
            progsets::dirMode = progsets::DirMode::PORTABLE;
        // debug-update
        auto hDebugUpdate = hRoot.child("debug-update");
        if (auto ver = Version::parsePermissive(hDebugUpdate.attribute("version").as_string())) {
            updatever::version = ver;
            updatever::isDebuggingVersion = true;
        }
        std::string_view platforms = hDebugUpdate.attribute("platforms").as_string();
        if (!platforms.empty()) {
            updatever::equivPlatforms = platforms;
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

            auto hFavs = root.child("favs");
            for (auto child : hFavs.children("cp")) {
                if (auto attr = child.attribute("c")) {
                    std::string_view text = attr.as_string();
                    unsigned code = 0xFFFFFF;
                    auto result = std::from_chars(text.data(), text.data() + text.length(), code, 16);
                    if (result.ec == std::errc()) {
                        config::favs.add(code);
                    }
                }
            }
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

    auto hFavs = root.append_child("favs");
    char buf[12];
    for (auto q : config::favs.codes()) {
        auto hFav = hFavs.append_child("cp");
        snprintf(buf, std::size(buf), "%X", (int)q);
        hFav.append_attribute("c") = buf;
    }

    doc.save_file(fname::config.c_str());
}
