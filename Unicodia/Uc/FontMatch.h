#pragma once

// STL
#include <optional>
#include <memory>

// Qt
#include <QFont>
#include <QFontDatabase>
#include <QFontMetrics>

// My libs
#include "u_Vector.h"

namespace mask {
    using T = uint64_t;
    using Short = uint32_t;

    constexpr T of(QFontDatabase::WritingSystem x)
        { return T(1) << x; }
    constexpr T of(int x) = delete;
    constexpr T CJK = of(QFontDatabase::SimplifiedChinese)
             | of(QFontDatabase::TraditionalChinese)
             | of(QFontDatabase::Japanese)
             | of(QFontDatabase::Korean);

    // Originally we’ve got 33 writing systems, incl. Any.
    // Some day Qt may increase it to 65.
    static_assert(QFontDatabase::WritingSystemsCount <= std::numeric_limits<T>::digits);
}   // namespace mask

enum class FontPrio { BAD, NORMAL, GOOD, MAIN };

struct FontLine {
    QString name;
    FontPrio priority = FontPrio::NORMAL;
};

struct FontList {
    SafeVector<FontLine> lines;
    bool hasMore = false;
};

class FontMatch
{
public:
    FontMatch(const QString& myName);

    /// @return [+] have font [-] draw tofu instead
    std::optional<QFont> sysFontFor(
            char32_t cp, QFontDatabase::WritingSystem writingSystem, int size);
    [[nodiscard]] FontList allSysFonts(
            char32_t cp, QFontDatabase::WritingSystem writingSystem,
            size_t maxCount = std::numeric_limits<size_t>::max());
private:
    static constexpr auto TEST_POINT_SIZE = 50;     // point size for testing font
    static constexpr auto PRIO_MINE = 1'000'000'000;
    static constexpr auto PRIO_WS = 1'000'000;      // priority for writing system support:
                                                    // the wider support, the better font
    static constexpr auto PRIO_BLACKLIST = -2'000'000'000;

    ///  Priority for Hani (Chinese hieroglyphs)
    ///  We care which writing system the font declares, Chinese or Japanese
    enum Cjkp {
        ZERO,           ///< lowest
        KOREAN,
        TRAD_CHINESE,
        JAPANESE,
        SIMP_CHINESE,   ///< highest
        NATIVE          ///< maybe sometimes, when we internationalize, native WS will be above all
                        ///<   (of course, when locale is CJK)
    };

    static QFont getFont(const QString& family, int size);

    class Fn
    {
    public:     // Internal class, do not encapsulate
        QString family;
        QFont testFont;
        QFontMetrics metrics;
        mask::T wsMask = mask::of(QFontDatabase::Any);    // Any=0 is always supported
        int priority = 0;
        Cjkp cjkPriority = Cjkp::ZERO;
        struct Verbal {     // verbal information about font
            FontPrio prio = FontPrio::NORMAL;
        } verbal;

        Fn(QString aFamily, int aPriority);
        constexpr bool doesSupport(QFontDatabase::WritingSystem x)
            { return wsMask & mask::of(x); }
        constexpr bool doesSupport(int x) = delete;
        bool doesSupport(char32_t x) { return metrics.inFontUcs4(x); }
        constexpr bool doesIntersect(mask::T x) { return wsMask & x; }
        constexpr bool doesIntersect(mask::Short x) = delete;
        constexpr bool doesIntersect(QFontDatabase::WritingSystem x) = delete;
        void checkSupport(QFontDatabase::WritingSystem x, Cjkp p);
    };
    using PFn = std::shared_ptr<Fn>;
    using VFn = SafeVector<PFn>;
    struct Ws {
        bool isSupported = false;
        VFn fonts;
    };

    VFn allFonts;
    Fix1d<Ws, QFontDatabase::WritingSystemsCount> allWss;
    /// @todo [match] if it works, implement some cache

    void retrieveFonts(const QString& myName);
    static bool prioGreater(const PFn& x, const PFn& y)
        { return (x->priority > y->priority); }
    static bool cjkPrioGreater(const PFn& x, const PFn& y)
    {
        if (x->cjkPriority != y->cjkPriority)
            return (x->cjkPriority > y->cjkPriority);
        return (x->priority > y->priority);
    }
    Ws& loadWs(QFontDatabase::WritingSystem x);
};
