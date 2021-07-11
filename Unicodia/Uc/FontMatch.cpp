// My header
#include "FontMatch.h"

// STL
#include <set>

// Project-local
#include "UcData.h"


// Blacklist unused
//namespace {

//    const std::set<std::string_view> blacklistedFonts {
//        "KacstBook"
//    };

//}   // anon namespace


///// Fn ///////////////////////////////////////////////////////////////////////


FontMatch::Fn::Fn(QString aFamily, int aPriority)
    : family(std::move(aFamily)),
      testFont(getFont(family, TEST_POINT_SIZE)),
      metrics(testFont),
      priority(aPriority)
{
    // Blacklist is unused right now, as our “square wheel” somehow works
    //if (blacklistedFonts.contains(family.toStdString())) {
    //    priority = PRIO_BLACKLIST;
    //}

    // Modify font, create metrics
    testFont.setStyleStrategy(static_cast<QFont::StyleStrategy>(
                QFont::PreferOutline | QFont::NoFontMerging | QFont::PreferQuality | QFont::PreferAntialias));

    // Set supported writing systems
    auto wsList = QFontDatabase::writingSystems(family);
    for (auto v : wsList) {
        wsMask |= mask::of(v);
    }
    priority += PRIO_WS * wsList.size();   // Universal fonts will be rated higher

    // Check CJK support
    checkSupport(QFontDatabase::SimplifiedChinese,  Cjkp::SIMP_CHINESE);
    checkSupport(QFontDatabase::TraditionalChinese, Cjkp::TRAD_CHINESE);
    checkSupport(QFontDatabase::Japanese,           Cjkp::JAPANESE);
    checkSupport(QFontDatabase::Korean,             Cjkp::KOREAN);
}


void FontMatch::Fn::checkSupport(
        QFontDatabase::WritingSystem x, Cjkp p)
{
    if (doesSupport(x))
        cjkPriority = std::max(cjkPriority, p);
}


///// FontMatch ////////////////////////////////////////////////////////////////

FontMatch::FontMatch(const QString& myName)
{
    retrieveFonts(myName);
}


QFont FontMatch::getFont(const QString& family, int size)
{
    QFont r { family, size };
    r.setStyleStrategy(static_cast<QFont::StyleStrategy>(
                QFont::PreferOutline | QFont::NoFontMerging | QFont::PreferQuality));
    return r;
}


void FontMatch::retrieveFonts(const QString& myName)
{
    {  auto families = QFontDatabase::families();
        for (auto& v : families) {
            if (QFontDatabase::isSmoothlyScalable(v)) {
                int prio = 0;
                if (v == myName)
                    prio = PRIO_MINE;
                allFonts.emplace_back(new Fn(std::move(v), prio));
            }
        }
    }   // families now entirely bad

    // Sort fonts by priority
    std::stable_sort(allFonts.begin(), allFonts.end(), prioGreater);

    // Check for supported writing systems
    auto wsList = QFontDatabase::writingSystems();
    for (auto v : wsList) {
        allWss[v].isSupported = true;
    }

    // All is always supported
    allWss[QFontDatabase::Any].isSupported = true;
    allWss[QFontDatabase::Any].fonts = allFonts;
}


FontMatch::Ws& FontMatch::loadWs(QFontDatabase::WritingSystem x)
{
    auto& ws = allWss[x];
    if (!ws.isSupported || !ws.fonts.empty())
        return ws;

    if (x == uc::WS_HANI) {
        for (auto& font : allFonts) {
            if (font->doesIntersect(mask::CJK)) {
                ws.fonts.emplace_back(font);
            }
        }
        std::stable_sort(ws.fonts.begin(), ws.fonts.end(), cjkPrioGreater);
    } else {
        auto bit = mask::of(x);
        for (auto& font : allFonts) {
            if (font->doesIntersect(bit)) {
                ws.fonts.emplace_back(font);
            }
        }
    }

    // If somehow no fonts, declare no support
    if (ws.fonts.empty())
        ws.isSupported = false;

    return ws;
}


std::optional<QFont> FontMatch::sysFontFor(
        char32_t cp, QFontDatabase::WritingSystem writingSystem, int size)
{
    auto& ws = loadWs(writingSystem);
    if (!ws.isSupported)
        return std::nullopt;

    for (auto& fn : ws.fonts) {
        if (fn->doesSupport(cp)) {
            return getFont(fn->family, size);
        }
    }
    return std::nullopt;
}
