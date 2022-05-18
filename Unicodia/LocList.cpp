// My header
#include "LocList.h"

// Qt
#include <QApplication>

// XML
#include "pugixml.hpp"

// Libs
#include "u_Strings.h"
#include "u_Qstrings.h"

// L10n
#include "LocDic.h"


loc::LangList loc::allLangs;
loc::Lang* loc::currLang = nullptr;

///// Lang /////////////////////////////////////////////////////////////////////


void loc::Lang::load()
{
    if (currLang == this)
        return;
    if (currLang)
        currLang->unload();
    if (translator)
        QApplication::removeTranslator(translator.get());
    loc::loadIni(loc::dic, fnLang);
    currLang = this;
}


void loc::Lang::unload()
{
    if (currLang == this)
        currLang = nullptr;
    if (translator)
        QApplication::removeTranslator(translator.get());
}


///// Functions ////////////////////////////////////////////////////////////////

namespace {

    constexpr auto MY_OPTS =
        std::filesystem::directory_options::follow_directory_symlink
        | std::filesystem::directory_options::skip_permission_denied;

    bool parseLang(loc::Lang& r, const std::filesystem::path& path)
    {
        // Remove translator
        r.translator.reset();
        r.name.tech = str::toUpper(path.filename().u8string());

        // Check lang.ini
        r.fnLang = path / "lang.ini";
        if (!std::filesystem::exists(r.fnLang))
            return false;

        // Check/parse locale.xml
        auto fnLocale = path / "locale.xml";
        if (!std::filesystem::exists(fnLocale))
            return false;

        pugi::xml_document doc;
        if (!doc.load_file(fnLocale.c_str()))
            return false;

        auto hLocale = doc.child("locale");
        r.name.native = str::toU8(hLocale.attribute("native").as_string());
        r.name.international = str::toU8(hLocale.attribute("international").as_string());
        r.name.isoSmall = str::toU8(hLocale.attribute("iso").as_string());
        r.showEnglishTerms = hLocale.attribute("eng-terms").as_bool(true);

        // Find Qt translator
        std::filesystem::directory_iterator di(path, MY_OPTS);
        for (auto& v : di) {
            if (v.is_regular_file() && v.path().extension() == ".qm") {
                r.translator = std::make_unique<QTranslator>();
                if (!r.translator->load(
                        str::toQ(v.path().stem().u16string()),
                        str::toQ(path.u16string()))) {
                    r.translator.reset();
                }
            }
        }

        std::cout << "Loaded lang " << str::toSv(r.name.tech) << std::endl;
        return true;
    }

}


void loc::collectLangs(const std::filesystem::path& programPath)
{
    auto dir = programPath / "Languages";

    std::unique_ptr<Lang> tempLang;

    std::filesystem::directory_iterator di(dir, MY_OPTS);
    for (const auto& v : di) {
        if (v.is_directory()) {
            //std::cout << v.path().string() << std::endl;
            //std::cout << v.path().filename().string() << std::endl;

            if (!tempLang) {
                tempLang = std::make_unique<loc::Lang>();
            }

            // Parse it
            if (parseLang(*tempLang, v.path())) {
                allLangs.push_back(std::move(tempLang));
            }
        }
    }
}


void loc::loadFirstLang()
{
    if (allLangs.empty())
        return;

    // LAST RESORT — load first available language
    allLangs[0]->load();
}
