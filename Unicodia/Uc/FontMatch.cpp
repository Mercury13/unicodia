// My header
#include "FontMatch.h"

// STL
#include <set>
#include <atomic>

// Project-local
#include "UcData.h"


// Blacklist unused
namespace {

    enum class NameOp { EXACT, START };

    struct ListLine {
        std::string_view text;
        NameOp op;
        FontPrio prio;
    };

    /// @todo [portability] These are for Windows
    ListLine fontList[] {
        // WARNING: ASCII list here
        { "DejaVu ",         NameOp::START, FontPrio::BAD  },
        { "Noto ",           NameOp::START, FontPrio::BAD  },
        { "Segoe UI Emoji",  NameOp::EXACT, FontPrio::GOOD },
        { "Segoe UI Symbol", NameOp::EXACT, FontPrio::GOOD },
        { "Webdings",        NameOp::EXACT, FontPrio::BAD  },
        { "Wingdings",       NameOp::EXACT, FontPrio::BAD  },
        { "Wingdings ",      NameOp::START, FontPrio::BAD  },
    };

    //bool isLess(const ListLine& x, std::string_view y)
    //    { return (x.text < y); }

    bool isLess(std::string_view x, const ListLine& y)
        { return (x < y.text); }

    std::atomic<bool> wantCheckFonts = true;

    void checkFontList()
    {
        if (!wantCheckFonts)
            return;
        wantCheckFonts = false;
        std::string_view prev;
        for (auto& v : fontList) {
            if (v.text <= prev)
                throw std::logic_error("Font list is not sorted");
            prev = v.text;
        }
    }

    FontPrio matchPrio(const std::string_view name)
    {
        checkFontList();
        auto it = std::upper_bound(std::begin(fontList), std::end(fontList), name, isLess);
        if (it == std::begin(fontList))
            return FontPrio::NORMAL;    // not found
        --it;       // now we may dereference it
        switch (it->op) {
        case NameOp::START:
            if (name.starts_with(it->text))
                return it->prio;
            break;
        case NameOp::EXACT:
            if (name == it->text)
                return it->prio;
            break;
        }
        return FontPrio::NORMAL;
    }

}   // anon namespace


///// Fn ///////////////////////////////////////////////////////////////////////


FontMatch::Fn::Fn(QString aFamily, int aPriority)
    : family(std::move(aFamily)),
      testFont(getFont(family, TEST_POINT_SIZE)),
      metrics(testFont),
      priority(aPriority)
{
    auto verbalPrio = matchPrio(family.toStdString());
    verbal.prio = verbalPrio;
    priority += PRIO_VERBAL * static_cast<int>(verbalPrio);

    // Modify font, create metrics
    testFont.setStyleStrategy(static_cast<QFont::StyleStrategy>(
                QFont::PreferOutline | QFont::NoFontMerging | QFont::PreferQuality | QFont::PreferAntialias));

    // Set supported writing systems
    auto wsList = QFontDatabase::writingSystems(family);
    for (auto v : wsList) {
        wsMask |= mask::of(v);
    }
    priority += PRIO_WS * wsList.size();   // Universal fonts will be rated lower

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
                if (v == myName) {
                    prio = PRIO_MINE;
                }
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
        const uc::Cp& cp, QFontDatabase::WritingSystem writingSystem, int size)
{
    // Check tofu cache
    if (cp.flags.have(uc::Cfg::DYN_SYSTEM_TOFU))
        return std::nullopt;

    auto& ws = loadWs(writingSystem);
    if (!ws.isSupported)
        return std::nullopt;

    for (auto& fn : ws.fonts) {
        if (fn->doesSupport(cp.subj.ch32())) {
            return getFont(fn->family, size);
        }
    }
    // Cache: got tofu
    cp.flags |= uc::Cfg::DYN_SYSTEM_TOFU;
    return std::nullopt;
}


FontList FontMatch::allSysFonts(
        char32_t cp, QFontDatabase::WritingSystem writingSystem,
        size_t maxCount)
{
    FontList r;
    auto& ws = loadWs(writingSystem);
    if (!ws.isSupported)
        return r;

    for (auto& fn : ws.fonts) {
        if (fn->doesSupport(cp)) {
            if (r.lines.size() >= maxCount) {
                r.hasMore = true;
                break;
            }
            r.lines.emplace_back(fn->family, fn->verbal.prio);
        }
    }
    return r;
}
