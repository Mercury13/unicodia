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
