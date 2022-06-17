// My header
#include "LocList.h"

// Qt
#include <QApplication>

// XML
#include "pugixml.hpp"

// Libs
#include "u_Strings.h"
#include "u_Qstrings.h"
#include "mojibake.h"

// L10n
#include "LocDic.h"
#include "LocManager.h"


loc::LangList loc::allLangs;
loc::Lang* loc::currLang = nullptr;

///// Lang /////////////////////////////////////////////////////////////////////


bool loc::Lang::hasMainLang(std::string_view iso) const
{
    return (!triggerLangs.empty() && triggerLangs[0] == iso);
}


bool loc::Lang::hasTriggerLang(std::string_view iso) const
{
    return std::find(triggerLangs.begin(), triggerLangs.end(), iso)
            != triggerLangs.end();
}


void loc::Lang::forceLoad()
{
    if (currLang)
        currLang->unload();
    if (translator)
        QApplication::installTranslator(translator.get());
    loc::loadIni(loc::dic, fnLang);
    currLang = this;
    loc::man.translateMe();
}


void loc::Lang::load()
{
    if (currLang == this)
        return;
    forceLoad();
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
        r.showEnglishTerms = hLocale.attribute("eng-terms").as_bool(true);

        mojibake::simpleCaseFold(r.name.international, r.name.sortKey);

        r.triggerLangs.clear();
        auto hTriggerLangs = hLocale.child("trigger-langs");
        for (auto& v : hTriggerLangs.children("lang")) {
            std::string_view iso = v.attribute("iso").as_string();
            if (!iso.empty()) {
                r.triggerLangs.emplace_back(iso);
            }
        }

        r.sortOrder.clear();
        auto hAlphaSort = hLocale.child("alpha-sort");
        for (auto& v : hAlphaSort.children("alp")) {
            auto alph = mojibake::toS<std::u32string>(v.text().as_string());
            for (size_t i = 0; i < alph.length(); ++i) {
                if (auto c = alph[i]; c != '_')
                    r.sortOrder[c] = i;
            }
        }

        r.wikiTemplates.clear();
        auto hTemplates = hLocale.child("wiki-templates");
        for (auto& v : hTemplates.children("tmpl")) {
            if (std::string_view name = v.attribute("name").as_string(); !name.empty()) {
                auto text = v.text().as_string();
                r.wikiTemplates[std::string{name}] = text;
            }
        }

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


void loc::LangList::collect(const std::filesystem::path& programPath)
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
            if (parseLang(*tempLang, v.path())) {  // NOLINT(bugprone-use-after-move)
                push_back(std::move(tempLang));
            }
        }
    }

    // Sort languages
    std::sort(begin(), end(),
        [](const auto& x, const auto& y) {
            return (x->name.sortKey < y->name.sortKey);
        });
}


loc::Lang* loc::LangList::byIso(std::string_view x)
{
    // Main language
    auto it = std::find_if(begin(), end(),
            [x](auto& p){ return (p->hasMainLang(x)); }
        );
    // Other languages
    if (it != end()) return it->get();
    it = std::find_if(begin(), end(),
            [x](auto& p){ return (p->hasTriggerLang(x)); }
        );
    if (it != end()) return it->get();
    // Did not find
    return nullptr;
}


loc::Lang* loc::LangList::findStarting()
{
    if (empty())
        return nullptr;

    // Find English
    if (auto p = byIso("en"))
        return p;

    // LAST RESORT — first available
    return front().get();
}


void loc::LangList::loadStarting()
{
    if (auto pLang = findStarting()) {
        pLang->load();
    }

}


int loc::LangList::byPtr(const Lang* x)
{
    if (!x)
        return -1;
    auto it = std::find_if(begin(), end(),
            [x](auto& p){ return (p.get() == x); }
        );
    if (it != end()) return it - begin();
    return -1;
}
