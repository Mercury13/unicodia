#include "MyWiki.h"

// Qt
#include <QWidget>

// Libs
#include "u_Strings.h"
#include "u_Qstrings.h"

// Unicode
#include "UcData.h"

// Wiki
#include "Wiki.h"
#include "Skin.h"

using namespace std::string_view_literals;


///// Gui //////////////////////////////////////////////////////////////////////

void mywiki::Gui::popupAtRel(
        QWidget* widget, const QRect& relRect, const QString& html)
{
    if (!widget) {
        popupAtAbs(widget, {}, html);
    } else {
        popupAtAbs(widget,
                   QRect{ widget->mapToGlobal(relRect.topLeft()), relRect.size() },
                   html);
    }
}


void mywiki::Gui::popupAtWidget(
        QWidget* widget, const QString& html)
{
    popupAtRel(widget, widget->rect(), html);
}


void mywiki::Gui::popupAtRelMaybe(
        QWidget* widget, TinyOpt<QRect> relRect, const QString& html)
{
    if (!widget) {              // no widget
        popupAtAbs(widget, {}, html);
    } else if (!relRect) {      // have widget, no rect
        popupAtWidget(widget, html);
    } else {                    // have widget and rect
        popupAtRel(widget, *relRect, html);
    }
}

namespace {

    template <class Thing>
    class PopLink : public mywiki::Link
    {
    public:
        static_assert(!std::is_pointer<Thing>::value);
        const Thing& thing;
        PopLink(const Thing& aThing) : thing(aThing) {}
        void go(QWidget* widget, TinyOpt<QRect> rect, mywiki::Gui& gui);
    };

    template <class Thing>
    void PopLink<Thing>::go(QWidget* widget, TinyOpt<QRect> rect, mywiki::Gui& gui)
    {
        auto html = mywiki::buildHtml(thing);
        gui.popupAtRelMaybe(widget, rect, html);
    }

    // Some sort of deduction guide
    template <class Thing>
    inline std::unique_ptr<PopLink<Thing>> mu(const Thing& x)
        { return std::make_unique<PopLink<Thing>>(x); }

    class PopFontsLink : public mywiki::Link
    {
    public:
        char32_t cp;
        QFontDatabase::WritingSystem ws;
        PopFontsLink(char32_t aCp, QFontDatabase::WritingSystem aWs) : cp(aCp), ws(aWs) {}
        void go(QWidget* widget, TinyOpt<QRect> rect, mywiki::Gui& gui);
    };

    void PopFontsLink::go(QWidget* widget, TinyOpt<QRect> rect, mywiki::Gui& gui)
    {
        auto html = mywiki::buildFontsHtml(cp, ws, gui);
        gui.popupAtRelMaybe(widget, rect, html);
    }

}   // anon namespace


std::unique_ptr<mywiki::Link> mywiki::parsePopBidiLink(std::string_view target)
{
    if (auto* bidi = uc::findBidiClass(target))
        return mu(*bidi);
    return {};
}


std::unique_ptr<mywiki::Link> mywiki::parsePopCatLink(std::string_view target)
{
    if (auto* cat = uc::findCategory(target))
        return mu(*cat);
    return {};
}


std::unique_ptr<mywiki::Link> mywiki::parsePopScriptLink(std::string_view target)
{
    if (auto* script = uc::findScript(target))
        return mu(*script);
    return {};
}


std::unique_ptr<mywiki::Link> mywiki::parsePopFontsLink(std::string_view target)
{
    auto sv = str::splitSv(target, '/');
    if (sv.size() >= 2) {
        unsigned int cp = 0, ws = 0;
        str::fromChars(sv[0], cp);
        str::fromChars(sv[1], ws);
        if (cp < uc::N_CHARS && ws < QFontDatabase::WritingSystemsCount)
            return std::make_unique<PopFontsLink>(
                        cp, static_cast<QFontDatabase::WritingSystem>(ws));
    }
    return {};
}


std::unique_ptr<mywiki::Link> mywiki::parseLink(
        std::string_view scheme, std::string_view target)
{
    if (scheme == "pb"sv) {
        return parsePopBidiLink(target);
    } else if (scheme == "pc"sv) {
        return parsePopCatLink(target);
    } else if (scheme == "ps"sv) {
        return parsePopScriptLink(target);
        //if (auto* script = uc::findScript(target))
        //    showPopup(*script, widget, rect);
    } else if (scheme == "pk"sv) {
        //int iBlock = 0;
        //fromChars(target, iBlock);
        //showPopup(uc::blocks[iBlock], widget, rect);
    } else if (scheme == "pf"sv) {
        return parsePopFontsLink(target);
    } else if (scheme == "pt"sv) {
        //if (auto* term = uc::findTerm(target))
        //    showPopup(*term, widget, rect);
    }
    return {};
}


std::unique_ptr<mywiki::Link> mywiki::parseLink(std::string_view link)
{
    auto things = str::splitSv(link, ':');
    if (things.size() >= 2) {
        return parseLink(things[0], things[1]);
    }
    return {};
}

void mywiki::go(QWidget* widget, TinyOpt<QRect> rect, Gui& gui, std::string_view link)
{
    auto p = parseLink(link);
    if (p) {
        p->go(widget, rect, gui);
    }
}


namespace {

    class Eng : public wiki::Engine
    {
    public:
        QString& s;
        const uc::Font& font;

        Eng(QString& aS, const uc::Font& aFont) : s(aS), font(aFont) {}
        void appendPlain(std::string_view x) override;
        void appendLink(
                const SafeVector<std::string_view> x,
                bool hasRemainder) override;
        void appendTemplate(
                const SafeVector<std::string_view> x,
                bool hasRemainder) override;
        void toggleWeight(Flags<wiki::Weight> changed) override;
        void finish() override;
    protected:
        wiki::HtWeight weight;
        bool isSuppressed = false;

        [[nodiscard]] bool prepareRecursion(std::string_view text);
        void runRecursive(std::string_view text);
        void finishRecursion(bool hasRemainder, bool prepareResult);
    };

    void Eng::toggleWeight(Flags<wiki::Weight> changed)
    {
        auto tag = weight.toggle(changed);
        str::append(s, tag);
    }

    void Eng::finish()
    {
        if (!isSuppressed) {
            auto tag = weight.finish();
            str::append(s, tag);
        }
    }

    void Eng::appendPlain(std::string_view x)
    {
        str::append(s, x);
    }

    bool Eng::prepareRecursion(std::string_view text)
    {
        if (isSuppressed)
            return true;
        if (weight.flags() & wiki::probeWeights(text)) {
            auto tag = weight.finish();
            str::append(s, tag);
        } else {
            isSuppressed = true;
        }
        return false;
    }

    void Eng::runRecursive(std::string_view text)
    {
        if (!isSuppressed) {
            auto tag = weight.restart();
            str::append(s, tag);
        }
        wiki::run(*this, text);
    }

    void Eng::finishRecursion(bool hasRemainder, bool prepareResult)
    {
        if (!isSuppressed) {
            if (hasRemainder) {
                auto tag = weight.restart();
                str::append(s, tag);
            } else {
                weight.clear();
            }
        }
        isSuppressed = prepareResult;
    }

    void Eng::appendLink(const SafeVector<std::string_view> x, bool hasRemainder)
    {
        auto dest = x[0];
        auto text = x[1];
        std::string_view style;
        if (dest.size() >= 4 && dest[0] == 'p' && dest[2] == ':')
            style = SUBTAG_POPUP;

        auto q = prepareRecursion(text);

        s.append("<a");
        str::append(s, style);
        s.append(" href='");
        str::append(s, dest);
        s.append("'>");

        runRecursive(text);

        s.append("</a>");

        finishRecursion(hasRemainder, q);
    }

    void Eng::appendTemplate(const SafeVector<std::string_view> x, bool)
    {
        auto name = x[0];
        if (name == "sm"sv) {
            bool useCustom = !font.flags.have(uc::Ffg::DESC_STD);
            if (useCustom)
                font.load();
            auto fontSize =
                    font.flags.have(uc::Ffg::DESC_BIGGER) ? '3'
                  : font.flags.have(uc::Ffg::DESC_SMALLER) ? '1' : '2';
            str::append(s, "<font size='+"sv);
            str::append(s, fontSize);
            str::append(s, '\'');
            if (useCustom) {
                str::append(s, " face='"sv);
                str::append(s, font.family);
                str::append(s, '\'');
            }
            str::append(s, '>');
            str::append(s, x.safeGetV(1, {}));
            str::append(s, "</font>");
        } else if (name == "_"sv) {
            s.append(QChar(0x00A0));
        } else if (name == "-"sv) {
            str::append(s, "<span style='font-size:4pt'>\u00A0</span>—<span style='font-size:4pt'> </span>"sv);
        } else if (name == ",-"sv) {
            str::append(s, ",—<span style='font-size:4pt'> </span>"sv);
        } else if (name == "%"sv) {
            str::append(s, x.safeGetV(1, {}));
            str::append(s, "<span style='font-size:3pt'>\u00A0</span>%"sv);
        } else if (name == "em"sv) {
            str::append(s, "<font size='+2' face='Segoe UI Emoji,Noto Sans Symbols,Noto Sans Symbols2'>"sv);
            str::append(s, x.safeGetV(1, {}));
            str::append(s, "</font>");
        } else if (name == "№"sv) {
            str::append(s, u8"№<span style='font-size:3pt'>\u00A0</span>"sv);
            str::append(s, x.safeGetV(1, {}));
        } else {
            wiki::appendHtml(s, x[0]);
        }
    }

    inline void appendHeader(QString& text, std::string_view x)
    {
        str::append(text, "<p><nobr><b>");
        str::append(text, x);
        str::append(text, "</b></nobr></p>");
    }

    template <int N>
    inline void appendHeader(QString& text, char (&x)[N])
    {
        size_t len = strnlen(x, N);
        appendHeader(text, std::string_view{ x, len });
    }

    template <class T>
    inline void appendHeader(QString& text, const T& x)
    {
        str::append(text, "<p><nobr><b>");
        str::append(text, x.locName);
        str::append(text, "</b> ("sv);
        str::append(text, x.nChars);
        str::append(text, " шт.)</nobr></p>");
    }


    template <class X>
    void appendWiki(QString& text, const X& obj, std::u8string_view x)
    {
        Eng eng(text, obj.font());
        wiki::run(eng, x);
    }

}   // anon namespace


void mywiki::appendNoFont(QString& text, std::u8string_view wiki)
{
    Eng eng(text, uc::fontInfo[0]);
    wiki::run(eng, wiki);
}


void mywiki::append(QString& text, const std::u8string_view wiki, const uc::Font& font)
{
    Eng eng(text, font);
    wiki::run(eng, wiki);
}

void mywiki::appendVersionValue(QString& text, const uc::Version& version)
{
    str::append(text, version.name);
    str::append(text, " (");
    str::append(text, version.year);
    str::append(text, ")");
}

void mywiki::appendVersion(QString& text, std::u8string_view prefix, const uc::Version& version)
{
    str::append(text, prefix);
    str::append(text, u8"Версия Юникода: ");
    appendVersionValue(text, version);
}

QString mywiki::buildHtml(const uc::BidiClass& x)
{
    QString text;
    appendHeader(text, x);

    str::append(text, "<p>");
    str::QSep sp(text, "<br>");

    sp.sep();
    str::append(text, u8"• В техдокументации: ");
    str::append(text, x.locId);

    str::append(text, "</p>");

    str::append(text, "<p>");
    appendNoFont(text, x.locDescription);
    return text;
}


QString mywiki::buildHtml(const uc::Category& x)
{
    QString text;
    appendHeader(text, x);
    str::append(text, "<p>");
    appendNoFont(text, x.locDescription);
    return text;
}


QString mywiki::buildFontsHtml(
        const char32_t cp, QFontDatabase::WritingSystem ws,
        Gui& gui)
{
    /// @todo [urgent] What to do with fonts?
    if (cp >= uc::N_CHARS || ws >= QFontDatabase::WritingSystemsCount)
        return {};
    QString text;
    char buf[50];
    auto format = u8"Шрифты для U+%04X";
    snprintf(buf, std::size(buf), reinterpret_cast<const char*>(format), (int)cp);
    appendHeader(text, buf);

    auto fonts = gui.allSysFonts(cp, ws, 20);
    str::append(text, "<p>");
    str::QSep sp(text, "<br>");
    for (auto& v : fonts.lines) {
        sp.sep();
        str::append(text, u8"•\u00A0");
        text += v.name;
    }
    if (fonts.hasMore) {
        sp.sep();
        str::append(text, u8"•\u00A0…");
    }
    return text;
}


void mywiki::appendHtml(QString& text, const uc::Script& x, bool isScript)
{
    if (x.ecType != uc::EcScriptType::NONE) {
        str::append(text, "<p>");
        str::QSep sp(text, "<br>");
        str::append(text, u8"• Тип: ");
        appendWiki(text, x, x.type().locName);
        if (x.ecDir != uc::EcWritingDir::NOMATTER) {
            sp.sep();
            str::append(text, u8"• Направление: ");
            str::append(text, x.dir().locName);
        }
        if (!x.locLangs.empty()) {
            sp.sep();
            str::append(text, u8"• Языки: ");
            str::append(text, x.locLangs);
        }
        if (!x.locTime.empty()) {
            sp.sep();
            str::append(text, u8"• Появилась: ");
            str::append(text, x.locTime);
        }
        if (x.ecLife != uc::EcLangLife::NOMATTER) {
            sp.sep();
            str::append(text, u8"• Состояние: ");
            str::append(text, x.life().locName);
        }
        if (isScript) {
            if (x.ecVersion != uc::EcVersion::UNKNOWN) {
                sp.sep();
                appendVersion(text, u8"• "sv, x.version());
            }

            sp.sep();
            str::append(text, u8"• Плоскость: ");
            if (x.plane == uc::PLANE_BASE) {
                str::append(text, u8"базовая");
            } else {
                str::append(text, std::to_string(x.plane));
            }
        }

        str::append(text, "</p>");
    }

    str::append(text, "<p>");
    appendWiki(text, x, x.locDescription);
    str::append(text, "</p>");
}


QString mywiki::buildHtml(const uc::Script& x)
{
    QString r;
    appendHeader(r, x);
    appendHtml(r, x, true);
    return r;
}


namespace {

    using StrCache = char[300];

    template <class T>
    std::string_view idOf(const T& value, StrCache&) { return value.id; }

    template <>
    std::string_view idOf(const uc::Block& value, StrCache& cache)
    {
        auto beg = std::begin(cache);
        auto r = std::to_chars(beg, std::end(cache), value.index());
        return { beg, r.ptr };
    }

    template <class T>
    inline void appendVal(QString& text, const T& value)
        { str::append(text, value.locName); }

    template<>
    inline void appendVal(QString& text, const uc::BidiClass& value)
        { str::append(text, value.locShortName); }

    struct FontLink {
        QString family;
        char32_t cp;
        QFontDatabase::WritingSystem ws;
    };

    template <>
    std::string_view idOf(const FontLink& value, StrCache& cache)
    {
        snprintf(cache, std::size(cache), "%d/%d", (int)value.cp, (int)value.ws);
        return cache;
    }

    template<>
    inline void appendVal(QString& text, const FontLink& value)
        { text += value.family.toHtmlEscaped(); }

    template <class T, class Name1>
    inline void appendValuePopup(
            QString& text, const T& value, Name1 name, const char* scheme)
    {
        StrCache cache, buf;
        str::append(text, name);
        auto vid = idOf(value, cache);
        snprintf(buf, std::size(buf),
                 ": <a href='%s:%.*s'" SUBTAG_POPUP ">",
                scheme, int(vid.size()), vid.data());
        str::append(text, buf);
        appendVal(text, value);
        str::append(text, "</a>");
    }

}


void mywiki::appendUtf(QString& text, str::QSep& sp, char32_t code)
{
    char buf[30];

    // UTF-8
    sp.sep();
    auto sChar = str::toQ(code);
    str::append(text, u8"<a href='pt:utf8'" SUBTAG_POPUP ">UTF-8</a>:");
    auto u8 = sChar.toUtf8();
    for (unsigned char v : u8) {
        snprintf(buf, 10, " %02X", static_cast<int>(v));
        str::append(text, buf);
    }

    // UTF-16: QString us UTF-16
    sp.sep();
    str::append(text, u8"<a href='pt:utf16'" SUBTAG_POPUP ">UTF-16</a>:");
    for (auto v : sChar) {
        snprintf(buf, std::size(buf), " %04X", static_cast<int>(v.unicode()));
        str::append(text, buf);
    }
}


QString mywiki::buildHtml(
        const uc::Cp& cp, const uc::Block* hint,
        const std::optional<QFont>& font, QFontDatabase::WritingSystem ws)
{
    QString text;
    str::append(text, "<h1>");
    str::append(text, cp.name.tech());
    str::append(text, "</h1>");

    // Deprecated
    if (cp.isDeprecated()) {
        str::append(text, u8"<h3><a href='pt:deprecated'" SUBTAG_DEPRECATED ">Запрещённый символ</a></h3>"sv);
    }

    {   // Info box
        str::append(text, "<p>");
        str::QSep sp(text, "<br>");

        // Script
        sp.sep();
        auto& scr = cp.script();
        appendValuePopup(text, scr, u8"Письменность", "ps");

        // Unicode version
        sp.sep();
        mywiki::appendVersion(text, {}, cp.version());

        // Character type
        sp.sep();
        appendValuePopup(text, cp.category(), u8"Тип", "pc");

        // Numeric value
        auto& numc = cp.numeric();
        if (numc.isPresent()) {
            sp.sep();
            str::append(text, "<a href='pt:number'" SUBTAG_POPUP ">");
            str::append(text, numc.type().locName);
            str::append(text, "</a>");
            str::append(text, ": ");
            str::append(text, numc.num);
            if (numc.denom != 1) {
                str::append(text, "/");
                str::append(text, numc.denom);
            }
        }

        // Bidi writing
        sp.sep();
        appendValuePopup(text, cp.bidiClass(), u8"В двунаправленном письме", "pb");

        // Block
        sp.sep();
        appendValuePopup(text, *hint, u8"Блок", "pk");

        auto comps = uc::cpOldComps(cp.subj);
        if (comps) {
            sp.sep();
            str::append(text, u8"Компьютеры: ");
            str::QSep spC(text, ", ");
            while (comps) {
                // Extract and remove bit
                auto bit = comps.smallest();
                comps.remove(bit);
                // Turn bit to index
                auto iBit = std::countr_zero(static_cast<unsigned>(bit));
                // Write what we got
                spC.sep();
                str::append(text, uc::oldCompNames[iBit]);
            }
        }

        // Font
        if (font) {
            sp.sep();
            FontLink lnk { font->family(), cp.subj, ws };
            appendValuePopup(text, lnk, u8"Системный шрифт", "pf");
        }

        // HTML
        sp.sep();
        str::append(text, u8"HTML: ");
        char buf[30];
        snprintf(buf, std::size(buf), "&amp;#%d;", static_cast<int>(cp.subj));
        str::append(text, buf);

        appendUtf(text, sp, cp.subj);

        text.append("</p>");

        if (cp.ecCategory == uc::EcCategory::CONTROL) {
            //  Control char description
            str::append(text, u8"<h2>Об управляющих символах</h2>");
            appendWiki(text, *hint,
                       uc::categoryInfo[static_cast<int>(uc::EcCategory::CONTROL)].locDescription);
        } else if (!hint->locDescription.empty()) {
            // Block description
            str::append(text, u8"<h2>О блоке</h2>");
            appendWiki(text, *hint, hint->locDescription);
        } else if (auto sc = cp.scriptEx(hint); &sc != uc::scriptInfo){
            // Script description
            str::append(text, u8"<h2>О письменности</h2>");
            mywiki::appendHtml(text, sc, false);
        }
    }
    return text;
}


void mywiki::appendMissingCharInfo(QString& text, char32_t code)
{
    str::append(text, "<p>");
    str::QSep sp(text, "<br>");
    mywiki::appendUtf(text, sp, code);
}


QString mywiki::buildNonCharHtml(char32_t code, const uc::Block* hint)
{
    QString text;
    str::append(text, u8"<h1>Зарезервирован как отсутствующий</h1>"sv);
    mywiki::appendMissingCharInfo(text, code);
    appendWiki(text, *hint, uc::TX_NOCHAR);
    return text;
}
