// My header
#include "LocList.h"

// C++
#include <charconv>

// Qt
#ifdef __GNUC__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wattributes"
#endif
    #include <QLocale>
#ifdef __GNUC__
    #pragma GCC diagnostic pop
#endif

#include <QApplication>
#include <QTranslator>

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

loc::Lang::Icons loc::active::icons;
loc::Lang::Numfmt loc::active::numfmt;
bool loc::active::showEnglishTerms = false;

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

    // Active
    active::icons = icons;
    active::numfmt = numfmt;
    active::showEnglishTerms = showEnglishTerms;

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

        r.stamp = hLocale.attribute("trigger-stamp").as_int(0);

        r.triggerLangs.clear();
        auto hTriggerLangs = hLocale.child("trigger-langs");
        for (auto& v : hTriggerLangs.children("lang")) {
            std::string_view iso = v.attribute("iso").as_string();
            if (!iso.empty()) {
                r.triggerLangs.emplace_back(iso);
            }
        }
        // Fallback: dir = Gibberish → let it be "!Gibberish",
        // but not empty
        if (r.triggerLangs.empty())
            r.triggerLangs.emplace_back(str::toSv(u8"!" + r.name.tech));

        r.sortOrder.clear();
        auto hAlphaSort = hLocale.child("alpha-sort");
        for (auto& v : hAlphaSort.children("alp")) {
            auto alph = mojibake::toS<std::u32string>(v.text().as_string());
            for (size_t i = 0; i < alph.length(); ++i) {
                if (auto c = alph[i]; c != '_')
                    r.sortOrder[c] = i;
            }
        }

        r.ellipsis.blocks.clear();
        auto hEllipsis = hLocale.child("ellipsis");
        r.ellipsis.text = str::toU8(hEllipsis.attribute("text").as_string("..."));
        for (auto& v : hEllipsis.children("blk")) {
            std::string_view hex = v.attribute("start").as_string(0);
            uint32_t code;
            auto [ptr, ec] = std::from_chars(
                        std::to_address(hex.begin()),
                        std::to_address(hex.end()),
                        code, 16);
            if (ec == std::errc()) {
                r.ellipsis.blocks.push_back(code);
            }
        }
        std::sort(r.ellipsis.blocks.begin(), r.ellipsis.blocks.end());

        r.wikiTemplates.clear();
        auto hTemplates = hLocale.child("wiki-templates");
        for (auto& v : hTemplates.children("tmpl")) {
            if (std::string_view name = v.attribute("name").as_string(); !name.empty()) {
                auto text = v.text().as_string();
                r.wikiTemplates[std::string{name}] = text;
            }
        }

        auto hIcons =  hLocale.child("icons");
        r.icons.sortAZ = hIcons.attribute("sort-az").as_string();

        r.numfmt.decimalPoint = '.';
        auto hNumFormat = hLocale.child("num-format");
        auto dp = mojibake::toS<std::u32string>(
                    hNumFormat.attribute("dec-point").as_string());
        if (!dp.empty() && dp[0] < 65536) {
            r.numfmt.decimalPoint = dp[0];
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


loc::Lang* loc::LangList::byIso(std::string_view x, int lastStamp)
{
    // Main language
    auto it = std::find_if(begin(), end(),
            [x, lastStamp](auto& p) {
                return (p->stamp > lastStamp && p->hasMainLang(x));
            });
    // Other languages
    if (it != end()) return it->get();
    it = std::find_if(begin(), end(),
            [x, lastStamp](auto& p){
                return (p->stamp > lastStamp && p->hasTriggerLang(x));
            });
    if (it != end()) return it->get();
    // Did not find
    return nullptr;
}


loc::Lang* loc::LangList::findStarting(
        std::string_view lastLang, int lastStamp)
{
    if (empty())
        return nullptr;

    // Find system language
    auto sysLoc = QLocale::system();
    auto locName = sysLoc.name().toLower().toStdString();
    if (auto p = locName.find('_'); p != std::string::npos) {
        locName = locName.substr(0, p);
    }

    // Override stamp if such language is present is system
    if (auto p = byIso(locName, lastStamp))
        return p;

    // Find by locale
    if (auto p = byIso(lastLang))
        return p;

    // Find by locale
    if (auto p = byIso(locName))
        return p;

    // Find English
    if (auto p = byIso("en"))
        return p;

    // LAST RESORT — first available
    return front().get();
}


void loc::LangList::loadStarting(
        std::string_view lastLang, int lastStamp)
{
    if (auto pLang = findStarting(lastLang, lastStamp)) {
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


int loc::LangList::lastStamp() const
{
    int r = 0;
    for (auto& v : *this) {
        r = std::max(r, v->stamp);
    }
    return r;
}
