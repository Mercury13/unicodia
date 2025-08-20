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
#include "u_EcArray.h"
#include "mojibake.h"

// L10n
#include "LocDic.h"
#include "LocManager.h"


loc::LangList loc::allLangs;
loc::Lang* loc::currLang = nullptr;

loc::Lang::Icons loc::active::icons;
loc::Lang::Numfmt loc::active::numfmt;
loc::NumFmtHelp loc::active::numfmtHelp;
loc::Lang::Punctuation loc::active::punctuation;
loc::EngTerms loc::active::engTerms = loc::EngTerms::OFF;


///// CustomRule ///////////////////////////////////////////////////////////////


loc::Plural loc::CustomRule::ofUint(unsigned long long n) const
{
    for (auto& v : lines) {
        long long q = (v.mod == 0) ? n : n % v.mod;
        if (q >= v.min && q <= v.max) {
            return v.outcome;
        }
    }
    return defaultOutcome;
}


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
    active::engTerms = engTerms;
    active::punctuation = punctuation;

    // Active help
    active::numfmtHelp.u8DecimalPoint = mojibake::toS<std::string>(
            std::u16string_view { &active::numfmt.decimalPoint, 1 });
    active::numfmtHelp.u8ThousandPoint = mojibake::toS<std::string>(
            std::u16string_view { &active::numfmt.thousandPoint, 1 });

    // loc::FmtL locale
    loc::activeFmtLocale = currLang;

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

    loc::Plural parseOutcome(std::string_view value)
    {
        for (size_t i = 0; i < loc::Plural_N; ++i) {
            if (loc::pluralNames[i] == value)
                return static_cast<loc::Plural>(i);
        }
        return loc::Plural::OTHER;
    }

    void loadPluralRules(pugi::xml_node h, loc::CustomRule& r)
    {
        r.defaultOutcome = parseOutcome(h.attribute("default-outcome").as_string());
        for (auto hLine : h.children("rule")) {
            auto& line = r.lines.emplace_back();
            line.mod = hLine.attribute("mod").as_uint();
            line.min = hLine.attribute("min").as_uint();
            line.max = hLine.attribute("max").as_uint();
            line.outcome = parseOutcome(hLine.attribute("outcome").as_string());
        }
    }

    char16_t readChar16(pugi::xml_node h, const char* name, char dflt)
    {
        char32_t buf[4];
        // Get attribute
        auto attr = h.attribute(name).as_string();
        if (attr == nullptr || *attr == 0)
            return dflt;
        // Convert
        auto end = mojibake::copyLimS(std::string_view(attr), std::begin(buf), std::end(buf));
        if (end != buf && buf[0] < 65536) {
            return buf[0];
        }
        return dflt;
    }

    constinit const ec::Array<std::string_view, loc::FracPolicy> fracPolicyNames
            { "never", "avoid", "onedig", "prefer", "except1" };

    constinit const ec::Array<std::string_view, loc::MoreLessPhase> moreLessPhaseNames
            { "raw", "unit" };

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

        unsigned et = hLocale.attribute("eng-terms").as_uint(static_cast<int>(loc::EngTerms::NORMAL));
        if (et > static_cast<unsigned>(loc::EngTerms::MAX))
            et = static_cast<unsigned>(loc::EngTerms::NORMAL);
        r.engTerms = static_cast<loc::EngTerms>(et);

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
        int base = 0, greatest = -1;
        for (auto& v : hAlphaSort.children("alp")) {
            if (v.attribute("follow").as_bool(false)) {
                base = greatest + 1;
            }
            auto alph = mojibake::toS<std::u32string>(v.text().as_string());
            for (size_t i = 0; i < alph.length(); ++i) {
                if (auto c = alph[i]; c != '_') {
                    int newCode = base + i;
                    r.sortOrder[c] = newCode;
                    greatest = std::max(greatest, newCode);
                }
            }
        }

        r.alphaFixup.clear();
        auto hAlphaFixup = hLocale.child("alpha-fixup");
        for (auto& v : hAlphaFixup.children("blk")) {
            std::string_view hex = v.attribute("start").as_string(0);
            uint32_t code;
            auto [ptr, ec] = std::from_chars(
                        std::to_address(hex.begin()),
                        std::to_address(hex.end()),
                        code, 16);
            if (ec == std::errc()) {
                auto target = mojibake::toS<std::u32string>(v.attribute("target").as_string());
                r.alphaFixup[code] = std::move(target);
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

        auto hNumFormat = hLocale.child("num-format");
        r.numfmt.decimalPoint  = readChar16(hNumFormat, "dec-point",  loc::Lang::Numfmt::DEFAULT_DECIMAL_POINT);
        r.numfmt.thousandPoint = readChar16(hNumFormat, "thou-point", loc::Lang::Numfmt::DEFAULT_THOUSAND_POINT);
        r.numfmt.thousand.minLength =
                hNumFormat.attribute("thou-min-length").as_uint(
                            loc::Lang::Numfmt::Thousand::DEFAULT_MIN_LENGTH);
        r.numfmt.denseThousand = r.numfmt.thousand;
        r.numfmt.denseThousand.minLength =
                hNumFormat.attribute("thou-min-length-dense").as_uint(
                            r.numfmt.thousand.minLength);

        r.numfmt.imprecise.clear();
        auto hImprecise = hNumFormat.child("imprecise");
        for (auto v : hImprecise.children("fmt")) {
            unsigned char shift = v.attribute("shift").as_int(0);
            unsigned char biggerSubshift = v.attribute("bigger-subshift").as_int(0);
            auto text = v.attribute("text").as_string("{1}");
            auto biggerUnit = v.attribute("bigger-unit").as_string();
            std::string_view sPolicy = v.attribute("frac").as_string();
            auto policy = fracPolicyNames.findDef(sPolicy, loc::FracPolicy::AVOID);
            r.numfmt.imprecise.push_back(loc::ImpreciseInfo {
                    .tmpl = text, .biggerUnit = biggerUnit,
                    .shift = shift, .biggerSubshift = biggerSubshift,
                    .policy = policy });
        }
        // Policy for units should always exist, otherwise add dummy
        if (r.numfmt.imprecise.empty() || r.numfmt.imprecise[0].shift > 0) {
            auto pos = r.numfmt.imprecise.begin();
            r.numfmt.imprecise.insert(pos, loc::ImpreciseInfo {
                    .tmpl = "{1}", .biggerUnit{},
                    .shift = 0, .biggerSubshift = 0,
                    .policy = loc::FracPolicy::NEVER });
        }

        // More and less
        auto hMoreLess = hNumFormat.child("moreless");
        r.numfmt.moreLess.less = hMoreLess.attribute("less").as_string("<{1}");
        r.numfmt.moreLess.more = hMoreLess.attribute("more").as_string(">{1}");
        std::string_view sPhase = hMoreLess.attribute("phase").as_string();
        r.numfmt.moreLess.phase = moreLessPhaseNames.findDef(sPhase, loc::MoreLessPhase::RAW);

        auto hPercent = hNumFormat.child("percent");
        static constexpr bool DEFAULT_INVERSE = loc::Lang::Numfmt::Percent::DEFAULT_INVERSE;
        static constexpr bool DEFAULT_SPACE = loc::Lang::Numfmt::Percent::DEFAULT_SPACE;
        r.numfmt.percent.isInverse = hPercent.attribute("inverse").as_bool(DEFAULT_INVERSE);
        r.numfmt.percent.isSpace = hPercent.attribute("space").as_bool(DEFAULT_SPACE);

        auto hCardinalRules = hLocale.child("cardinal-rules");
        loadPluralRules(hCardinalRules, r.cardRule);

        auto hPeculiarities = hLocale.child("peculiarities");
        r.peculiarities.stillUsesBurmese = hPeculiarities.attribute("still-uses-burmese").as_bool(false);

        auto hPunctuation = hLocale.child("punctuation");
        r.punctuation.keyValueColon = str::toU8sv(
                    hPunctuation.attribute("key-value-colon").as_string("::::::"));
        r.punctuation.uniformComma = str::toU8sv(
                    hPunctuation.attribute("uniform-comma").as_string(",,,,,,"));
        r.punctuation.semicolon = str::toU8sv(
                    hPunctuation.attribute("semicolon").as_string(";;;;;;"));
        r.punctuation.leftParen = str::toU8sv(
                    hPunctuation.attribute("left-paren").as_string("(((((("));
        r.punctuation.rightParen = str::toU8sv(
                    hPunctuation.attribute("right-paren").as_string("))))))"));
        r.punctuation.indentEllip = str::toU8sv(
                    hPunctuation.attribute("indent-ellip").as_string(">>>>>>"));
        r.punctuation.range = str::toU8sv(
                    hPunctuation.attribute("range").as_string("~~~~~~"));
        r.punctuation.yearRange = str::toU8sv(
                    hPunctuation.attribute("year-range").as_string("~~~~~~"));

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

        std::cout << "Loaded lang " << str::toSv(r.name.tech) << '\n';
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
