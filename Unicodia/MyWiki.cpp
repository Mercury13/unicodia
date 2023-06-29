#include "MyWiki.h"

// Qt
#include <QWidget>
#include <QApplication>
#include <QTextBlock>

// Libs
#include "u_Strings.h"
#include "u_Qstrings.h"

// Unicode
#include "UcData.h"

// L10n
#include "LocDic.h"
#include "LocList.h"

// Wiki
#include "Wiki.h"
#include "Skin.h"

// Mojibake
#include "mojibake.h"

using namespace std::string_view_literals;

#define NBSP "\u00A0"


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


void mywiki::Gui::copyTextRel(
        QWidget* widget, const TinyOpt<QRect> relRect, const QString& text)
{
    QRect r = relRect
            ? *relRect
            : widget->rect();
    copyTextAbs(widget,
            QRect{ widget->mapToGlobal(r.topLeft()), r.size() },
            text);
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
        static_assert(!std::is_pointer<Thing>::value, "Need object rather than pointer");
        const Thing& thing;
        PopLink(const Thing& aThing) : thing(aThing) {}
        void go(QWidget* widget, TinyOpt<QRect> rect, mywiki::Gui& gui) override;
    };

    template <class Thing>
    void PopLink<Thing>::go(QWidget* widget, TinyOpt<QRect> rect, mywiki::Gui& gui)
    {
        auto html = mywiki::buildHtml(thing);
        gui.popupAtRelMaybe(widget, rect, html);
    }

    // Some sort of deduction guide, MU = Make Unique
    template <class Thing>
    inline std::unique_ptr<PopLink<Thing>> mu(const Thing& x)
        { return std::make_unique<PopLink<Thing>>(x); }

    class PopFontsLink : public mywiki::Link
    {
    public:
        char32_t cp;
        QFontDatabase::WritingSystem ws;
        PopFontsLink(char32_t aCp, QFontDatabase::WritingSystem aWs) : cp(aCp), ws(aWs) {}
        void go(QWidget* widget, TinyOpt<QRect> rect, mywiki::Gui& gui) override;
    };

    void PopFontsLink::go(QWidget* widget, TinyOpt<QRect> rect, mywiki::Gui& gui)
    {
        auto html = mywiki::buildFontsHtml(cp, ws, gui);
        gui.popupAtRelMaybe(widget, rect, html);
    }

    class CopyLink : public mywiki::Link
    {
    public:
        QString text;
        QFontDatabase::WritingSystem ws;
        CopyLink(std::string_view aText) : text(str::toQ(aText)) {}
        mywiki::LinkClass clazz() const override { return mywiki::LinkClass::COPY; }
        void go(QWidget* widget, TinyOpt<QRect> rect, mywiki::Gui& gui) override;
    };

    void CopyLink::go(QWidget* widget, TinyOpt<QRect> rect, mywiki::Gui& gui)
    {
        gui.copyTextRel(widget, rect, text);
    }

    class InetLink : public mywiki::Link
    {
    public:
        QString s;
        InetLink(std::string_view scheme, std::string_view target);
        mywiki::LinkClass clazz() const override { return mywiki::LinkClass::INET; }
        void go(QWidget* widget, TinyOpt<QRect> rect, mywiki::Gui& gui) override;
    };

    InetLink::InetLink(std::string_view scheme, std::string_view target)
    {
        s.reserve(scheme.length() + 1 + target.length());
        str::append(s, scheme);
        s += ':';
        str::append(s, target);
    }

    void InetLink::go(QWidget*, TinyOpt<QRect>, mywiki::Gui& gui)
    {
        gui.followUrl(s);
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

std::unique_ptr<mywiki::Link> mywiki::parsePopTermLink(std::string_view target)
{
    if (auto* term = uc::findTerm(target))
        return mu(*term);
    return {};
}


std::unique_ptr<mywiki::Link> mywiki::parsePopVersionLink(std::string_view target)
{
    if (auto* term = uc::findVersion(target))
        return mu(*term);
    return {};
}


std::unique_ptr<mywiki::Link> mywiki::parsePopIBlockLink(std::string_view target)
{
    uint32_t code = 0;
    str::fromChars(target, code, 16);
    auto block = uc::blockOf(code);
    if (block)
        return mu(*block);
    return {};
}

std::unique_ptr<mywiki::Link> mywiki::parsePopFontsLink(std::string_view target)
{
    auto sv = str::splitSv(target, '/');
    if (sv.size() >= 2) {
        unsigned int cp = 0, ws = 0;
        str::fromChars(sv[0], cp);
        str::fromChars(sv[1], ws);
        if (cp < uc::CAPACITY && ws < QFontDatabase::WritingSystemsCount)
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
    } else if (scheme == "pk"sv) {
        return parsePopIBlockLink(target);
    } else if (scheme == "pf"sv) {
        return parsePopFontsLink(target);
    } else if (scheme == "pt"sv) {
        return parsePopTermLink(target);
    } else if (scheme == "pv"sv) {
        return parsePopVersionLink(target);
    } else if (scheme == "c"sv) {
        return std::make_unique<CopyLink>(target);
    } else if (scheme == "http"sv || scheme == "https"sv) {
        return std::make_unique<InetLink>(scheme, target);
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

    constexpr std::u8string_view BULLET = u8"•\u00A0";

    class Eng : public wiki::Engine
    {
    public:
        QString& s;
        const uc::Font& font;

        Eng(QString& aS, const uc::Font& aFont) : s(aS), font(aFont) {}
        void appendPlain(std::string_view x) override;
        void appendLink(
                const SafeVector<std::string_view>& x,
                bool hasRemainder) override;
        void appendTemplate(
                const SafeVector<std::string_view>& x,
                bool hasRemainder) override;
        void toggleWeight(Flags<wiki::Weight> changed) override;
        void appendBreak(wiki::Strength strength, wiki::Feature feature) override;
        void finish() override;
    protected:
        wiki::HtWeight weight;
        bool isSuppressed = false;
        bool isDiv = false;

        [[nodiscard]] bool prepareRecursion(std::string_view text);
        void runRecursive(std::string_view text);
        void finishRecursion(bool hasRemainder, bool prepareResult);
        void finishDiv();
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

    void Eng::finishDiv()
    {
        if (isDiv) {
            s += "</div>";
            isDiv = false;
        }
    }

    void Eng::appendBreak(wiki::Strength strength, wiki::Feature feature)
    {
        finishDiv();
        switch (strength) {
        case wiki::Strength::BREAK:
            switch (feature) {
            case wiki::Feature::BULLET:
                s += "<br>";
                break;
            case wiki::Feature::NONE:
            case wiki::Feature::INDENT:
                s += "<div>";
                isDiv = true;
                break;
            } break;
        case wiki::Strength::PARAGRAPH:
            s += "<p>";
            break;
        }

        switch (feature) {
        case wiki::Feature::NONE:
            break;
        case wiki::Feature::INDENT:
            s += NBSP NBSP NBSP;
            break;
        case wiki::Feature::BULLET:
            str::append(s, BULLET);
            break;
        }
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

    void Eng::appendLink(const SafeVector<std::string_view>& x, bool hasRemainder)
    {
        auto target = x[0];
        auto text = x[1];
        std::string_view style;
        auto link = mywiki::parseLink(target);
        if (!link) {
            style = "class=missing";
        } else switch (link->clazz()) {
        case mywiki::LinkClass::COPY:
            style = "class=copy";
            break;
        case mywiki::LinkClass::INET:
            style = "class=inet";
            break;
        case mywiki::LinkClass::POPUP:
            style = "class=popup";
            break;
        }

        auto q = prepareRecursion(text);

        s.append("<a ");
        str::append(s, style);
        s.append(" href='");
        str::append(s, target);
        s.append("'>");

        runRecursive(text);

        s.append("</a>");

        finishRecursion(hasRemainder, q);
    }

    constexpr int SIZE_SAMPLE = -1;

    void appendFont(QString& s, const uc::Font& font, std::string_view x, int size)
    {
        bool hasFace = !font.flags.have(uc::Ffg::DESC_STD);
        bool hasSize = (size != 0);
        if (hasFace || hasSize) {
            s += "<font";
            if (hasFace) {
                s += " face='";
                s += font.familiesComma();
                s += '\'';
            }
            if (hasSize) {
                if (size < 0) {
                    size =  font.flags.have(uc::Ffg::DESC_BIGGER) ? 3
                          : font.flags.have(uc::Ffg::DESC_SMALLER) ? 1 : 2;
                }
                s += " size='+";
                s += QChar(size + '0');
                s += '\'';
            }
            s += ">";
            str::append(s, x);
            str::append(s, "</font>");
        } else {
            str::append(s, x);
        }
    }

    void appendFont(QString& s, uc::EcFont fontId, std::string_view x, int size)
    {
        auto& font = uc::fontInfo[static_cast<int>(fontId)];
        appendFont(s, font, x, size);
    }

    template <class Font>
    inline void appendFont(QString& s, Font&& fontId,
                    const SafeVector<std::string_view>& x, int size)
    {
        appendFont(s, std::forward<Font>(fontId), x.safeGetV(1, {}), size);
    }

    void appendSmTable(
            QString& s,
            const uc::Font& font,
            const SafeVector<std::string_view>& x)
    {
        s += "<table cellspacing=0 cellpadding=0><tr valign='middle'><td>&nbsp;&nbsp;&nbsp;";
        auto n = x.size();
        for (size_t i = 1; i < n; ++i) {
            auto v = x[i];
            if (v.starts_with('*')) {
                if (v.starts_with("**")) {  // Glitching sample
                    s += "<td>";
                    auto w = v.substr(2);
                    appendFont(s, font, w, SIZE_SAMPLE);
                } else {    // Normal sample
                    auto w = v.substr(1);
                    appendFont(s, font, w, SIZE_SAMPLE);
                }
            } else {
                if (i > 1)
                    s += "&nbsp;";
                str::append(s, v);
                if (i + 1 < n)
                    s += "&nbsp;";
            }
        }
        s += "</table>";
    }

    // Key: start/end
    constinit const std::u8string_view KEY_START =
            u8"<span style='background-color:palette(midlight);'>\u00A0";
    constinit const std::u8string_view KEY_END = u8"\u00A0</span>";

    void Eng::appendTemplate(const SafeVector<std::string_view>& x, bool)
    {
        auto name = x[0];
        if (loc::currLang) {
            auto& wt = loc::currLang->wikiTemplates;
            if (auto it = wt.find(name); it != wt.end()) {
                str::append(s, it->second);
                return;
            }
        }
        if (name == "sm"sv) {
            appendFont(s, font, x, SIZE_SAMPLE);
        } else if (name == "smb"sv) {
            appendFont(s, uc::EcFont::CJK_NEWHAN, x, 3);
        } else if (name == "smtable"sv) {
            appendSmTable(s, font, x);
        } else if (name == "_"sv) {
            s.append(QChar(0x00A0));
        } else if (name == "%"sv) {
            str::append(s, x.safeGetV(1, {}));
            str::append(s, "<span style='font-size:3pt'>\u00A0</span>%"sv);
        } else if (name == "k"sv) {
            for (size_t i = 1; i < x.size(); ++i) {
                if (i != 1)
                    s += '+';
                str::append(s, KEY_START);
                mywiki::append(s, str::toU8sv(x[i]), font);
                str::append(s, KEY_END);
            }
        } else if (name == "kb"sv) {
            for (size_t i = 1; i < x.size(); ++i) {
                if (i != 1)
                    s += '+';
                str::append(s, KEY_START);
                s += "<b>";
                mywiki::append(s, str::toU8sv(x[i]), font);
                s += "</b>";
                str::append(s, KEY_END);
            }
        } else if (name == "t"sv) {
            str::append(s, u8"<span class='tr'>⌈</span>");
            str::append(s, x.safeGetV(1, {}));
            str::append(s, u8"<span class='tr'>⌋</span>");
        } else if (name == "em"sv) {
            str::append(s, "<font size='+2' face='Segoe UI Emoji,Noto Sans Symbols,Noto Sans Symbols2'>"sv);
            str::append(s, x.safeGetV(1, {}));
            str::append(s, "</font>");
        } else if (name == "fontface"sv) {
            s += font.familiesComma();
        } else if (name == "nchars"sv) {
            s += QString::number(uc::N_CPS);
        } else if (name == "nemoji"sv) {
            s += QString::number(uc::N_EMOJI);
        } else if (name == "version"sv) {
            str::append(s, uc::versionInfo[static_cast<int>(uc::EcVersion::LAST)].locName());
        } else if (name == "funky"sv) {
            appendFont(s, uc::EcFont::FUNKY, x, 0);
        } else if (name == "noto"sv) {
            appendFont(s, uc::EcFont::NOTO, x, 0);
        } else if (name == "DuplCats") {
            uc::fontInfo[static_cast<int>(uc::EcFont::FUNKY)].load(NO_TRIGGER);
            appendFont(s, uc::EcFont::FUNKY, "<span style='font-size:40pt'>&#xE00F;</span>", 0);
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
    inline std::u8string_view locName(const T& x) {
        return x.locName;
    }

    template <Locable T>
    inline std::u8string_view locName(const T& x) {
        return x.loc.name;
    }

    template <class T>
    inline void appendHeader(QString& text, const T& x,
                             std::u8string_view addText = {})
    {
        str::append(text, "<p><nobr><b>");
        str::append(text, locName(x));
        str::append(text, "</b> ("sv);
        str::append(text, loc::get("Prop.Head.NChars").arg(x.nChars));
        if (!addText.empty()) {
            text += " ";
            str::append(text, addText);
        }
        str::append(text, ")</nobr></p>"sv);
    }

    template <class X>
    inline const uc::Font& getFont(const X& obj)
        { return obj.font(); }

    template <class X>
    void appendWiki(QString& text, const X& obj, std::u8string_view x)
    {
        Eng eng(text, getFont(obj));
        wiki::run(eng, x);
    }

}   // anon namespace


void mywiki::appendNoFont(QString& text, std::u8string_view wiki)
{
    Eng eng(text, uc::fontInfo[0]);
    wiki::run(eng, wiki);
}


void mywiki::append(QString& text, std::u8string_view wiki, const uc::Font& font)
{
    Eng eng(text, font);
    wiki::run(eng, wiki);
}

void mywiki::appendVersionValue(QString& text, const uc::Version& version)
{
    str::append(text, "<a class='popup' href='");
    str::append(text, version.link(u8"pv:"));
    str::append(text, "'>");
    str::append(text, version.locName());
    str::append(text, "</a> (");

    // Get text
    char buf[30];
    snprintf(buf, std::size(buf), "Common.Mon.%d",
             static_cast<int>(version.date.month));
    auto& monTemplate = loc::get(buf);
    auto s = monTemplate.arg(version.date.year);

    str::append(text, s);
    str::append(text, ")");
}

void mywiki::appendEmojiValue(QString& text,
        const uc::Version& version, const uc::Version& prevVersion)
{
    if (version.emojiName.empty()) {
        appendVersionValue(text, version);
        return;
    }
    str::append(text, "<a class='popup' href='");
    str::append(text, version.link(u8"pv:"));
    str::append(text, "'>");
    str::append(text, version.emojiName);
    str::append(text, "</a> (");

    // Equiv. Unicode version
    if (!version.unicodeName.empty()) {
        str::append(text, loc::get("Prop.Bullet.EmojiV2").arg(version.unicodeName));
    }

    // Date
    char buf[30];
    snprintf(buf, std::size(buf), "Common.Mon.%d",
             static_cast<int>(version.date.month));
    auto& monTemplate = loc::get(buf);
    auto s = monTemplate.arg(version.date.year);
    str::append(text, s);

    // Prev. Unicode version
    if (version.unicodeName.empty() && !prevVersion.unicodeName.empty()) {
        str::append(text, loc::get("Prop.Bullet.CameAfter").arg(prevVersion.unicodeName));
    }

    str::append(text, ")");
}

namespace {

    void appendSomeBullet(QString& text,
                    std::u8string_view bullet,
                    std::string_view locKey,
                    std::string_view tag1={}, std::string_view tag2={})
    {
        str::append(text, bullet);
        str::append(text, tag1);
        str::append(text, loc::get(locKey));
        str::append(text, tag2);
        text += ": ";
    }

    void appendNonBullet(QString& text, std::string_view locKey,
                      std::string_view tag1={}, std::string_view tag2={})
    {
        appendSomeBullet(text, {}, locKey, tag1, tag2);
    }

    void appendBullet(QString& text, std::string_view locKey,
                      std::string_view tag1={}, std::string_view tag2={})
    {
        appendSomeBullet(text, BULLET, locKey, tag1, tag2);
    }
}


void mywiki::appendVersion(QString& text, std::u8string_view bullet, const uc::Version& version)
{
    appendSomeBullet(text, bullet, "Prop.Bullet.Version");
    appendVersionValue(text, version);
}

QString mywiki::buildHtml(const uc::BidiClass& x)
{
    QString text;
    appendStylesheet(text);
    appendHeader(text, x);

    str::append(text, "<p>");
    str::QSep sp(text, "<br>");

    sp.sep();
    appendBullet(text, "Prop.Bullet.InTech");
    str::append(text, x.tech);

    str::append(text, "</p>");

    str::append(text, "<p>");
    appendNoFont(text, x.loc.description);
    return text;
}


QString mywiki::buildHtml(const uc::Category& x)
{
    QString text;
    appendStylesheet(text);
    appendHeader(text, x);
    str::append(text, "<p>");
    appendNoFont(text, x.loc.description);
    return text;
}

QString mywiki::buildFontsHtml(
        char32_t cp, QFontDatabase::WritingSystem ws,
        Gui& gui)
{
    if (cp >= uc::CAPACITY || ws >= QFontDatabase::WritingSystemsCount)
        return {};
    QString text;
    char buf[50];
    auto format = loc::get("Prop.Os.FontsFor").c_str();
    snprintf(buf, std::size(buf), reinterpret_cast<const char*>(format), (int)cp);
    appendStylesheet(text);
    appendHeader(text, buf);

    auto fonts = gui.allSysFonts(cp, ws, 20);
    str::append(text, "<p>");
    str::QSep sp(text, "<br>");
    for (auto& v : fonts.lines) {
        sp.sep();
        str::append(text, BULLET);
        text += v.name;
    }
    if (fonts.hasMore) {
        sp.sep();
        str::append(text, u8"•\u00A0…");
    }
    return text;
}

namespace {
    uc::DatingLoc myDatingLoc;
}   // anon namespace


void mywiki::translateDatingLoc()
{
    myDatingLoc = uc::DatingLoc {
        .yBc            = loc::get("Dating.Name.YBc"),
        .yBefore        = loc::get("Dating.Name.YBefore"),
        .yApprox        = loc::get("Dating.Name.YApprox"),
        .yApproxBc      = loc::get("Dating.Name.YApproxBc"),
        .decade         = loc::get("Dating.Name.Decade"),
        .centuryNames   = {},
        .century        = loc::get("Dating.Name.Century"),
        .centuryBc      = loc::get("Dating.Name.CenturyBc"),
        .crangeMode     = uc::CrangeMode::SPECIAL_SPECIAL,
        .unknown        = loc::get("Dating.Name.Unknown"),
        .firstInscription = loc::get("Dating.StdComment.FirstInsc"),
        .modernForm     = loc::get("Dating.StdComment.ModernForm"),
        .maybeEarlier   = loc::get("Dating.StdComment.MaybeEarlier"),
    };
    char buf[30];
    for (int i = 1; i < std::ssize(myDatingLoc.centuryNames); ++i) {
        snprintf(buf, std::size(buf), "Dating.Century.%d", i);
        myDatingLoc.centuryNames[i] = loc::get(buf);
    }
}


void mywiki::appendHtml(QString& text, const uc::Script& x, bool isScript)
{
    if (x.ecType != uc::EcScriptType::NONE) {
        str::append(text, "<p>");
        str::QSep sp(text, "<br>");
        appendBullet(text, "Prop.Bullet.Type");
        appendWiki(text, x, loc::get(x.type().locKey));
        if (x.ecDir != uc::EcWritingDir::NOMATTER) {
            sp.sep();
            appendBullet(text, "Prop.Bullet.Dir");
            append(text, loc::get(x.dir().locKey), x.font());
        }
        if (!x.flags.have(uc::Sfg::NO_LANGS)) {
            sp.sep();
            appendBullet(text, "Prop.Bullet.Langs");
            appendWiki(text, x, x.loc.langs);
        }
        if (x.time) {
            sp.sep();
            appendBullet(text, "Prop.Bullet.Appear");
            auto wikiTime = x.time.wikiText(myDatingLoc, x.loc.timeComment);
            appendWiki(text, x, wikiTime);
        }
        if (x.ecLife != uc::EcLangLife::NOMATTER) {
            sp.sep();
            appendBullet(text, "Prop.Bullet.Condition", "<a href='pt:status' class='popup'>", "</a>");
            append(text, loc::get(x.life().locKey), x.font());
        }
        if (isScript) {
            if (x.ecVersion != uc::EcVersion::UNKNOWN) {
                sp.sep();
                appendVersion(text, BULLET, x.version());
            }

            sp.sep();
            appendBullet(text, "Prop.Bullet.Plane");
            if (x.plane == uc::PLANE_BASE) {
                str::append(text, loc::get("Prop.Bullet.base"));
            } else {
                str::append(text, std::to_string(x.plane));
            }
        }

        str::append(text, "</p>");
    }

    str::append(text, "<p>");
    appendWiki(text, x, x.loc.description);
    str::append(text, "</p>");
}


QString mywiki::buildHtml(const uc::Script& x)
{
    QString r;
    std::u8string add;
    if (x.id == "Hira"sv) {
        add = loc::get("Prop.Head.NoHent").arg(u8"href='ps:Hent' class='popup'");
    }
    appendStylesheet(r);
    appendHeader(r, x, add);
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
        auto r = std::to_chars(beg, std::end(cache), value.startingCp, 16);
        return { beg, r.ptr };
    }

    template <class T>
    inline void appendVal(QString& text, const T& value)
        { str::append(text, value.locName); }

    template <Locable T>
    inline void appendVal(QString& text, const T& value)
        { str::append(text, value.loc.name); }

    template<>
    inline void appendVal(QString& text, const uc::BidiClass& value)
        { str::append(text, value.loc.shortName); }

    struct FontLink {
        QString family;
        char32_t cp;
        QFontDatabase::WritingSystem ws;
    };

    template <class T>
    inline void appendValuePopup(
            QString& text, const T& value, std::string_view locKey, const char* scheme)
    {
        StrCache cache, buf;
        appendNonBullet(text, locKey);
        auto vid = idOf(value, cache);
        snprintf(buf, std::size(buf),
                 "<a href='%s:%.*s' class='popup' >",
                scheme, int(vid.size()), vid.data());
        str::append(text, buf);
        appendVal(text, value);
        str::append(text, "</a>");
    }

}


void mywiki::appendCopyable(QString& text, const QString& x, std::string_view clazz)
{
    str::append(text, "<a href='c:"sv);
        text += x;
        str::append(text, "' class='"sv);
        str::append(text, clazz);
        str::append(text, "' >"sv);
    text += x.toHtmlEscaped();
    str::append(text, "</a>"sv);
}


void mywiki::appendCopyable(QString& text, unsigned x, std::string_view clazz)
{
    char c[40];
    snprintf(c, std::size(c), "%u", x);
    appendCopyable(text, c, clazz);
}


void mywiki::appendCopyable2(
        QString& text, const QString& full,
        const QString& shrt, std::string_view clazz)
{
    str::append(text, "<a href='c:"sv);
        text += full;
        str::append(text, "' class='"sv);
        str::append(text, clazz);
        str::append(text, "' >"sv);
    text += shrt.toHtmlEscaped();
    str::append(text, "</a>"sv);
}


void mywiki::appendUtf(QString& text, Want32 want32, str::QSep& sp, char32_t code)
{
    std::u32string_view sv(&code, 1);
    appendUtf(text, want32, sp, sv);
}


void mywiki::appendUtf(QString& text, Want32 want32, str::QSep& sp, std::u32string_view value)
{
    char buf[30];

    // UTF-8
    auto sChar = str::toQ(value);
    auto u8 = sChar.toUtf8();
    QString u8Hex;
    { str::QSep spU8(u8Hex, " ");
        for (unsigned char v : u8) {
            spU8.sep();
            snprintf(buf, std::size(buf), "%02X", static_cast<int>(v));
            str::append(u8Hex, buf);
        }
    }

    sp.sep();
    appendNonBullet(text, "Prop.Bullet.Utf8",
                    "<a href='pt:utf8' class='popup'>", "</a>");
    appendCopyable(text, u8Hex);

    // UTF-16: QString is UTF-16
    QString u16Hex;
    { str::QSep spU16(u16Hex, " ");
        for (auto v : sChar) {
            spU16.sep();
            snprintf(buf, std::size(buf), "%04X", static_cast<int>(v.unicode()));
            str::append(u16Hex, buf);
        }
    }

    sp.sep();
    appendNonBullet(text, "Prop.Bullet.Utf16",
                    "<a href='pt:utf16' class='popup'>", "</a>");
    appendCopyable(text, u16Hex);

    // UTF-32
    if (want32 != Want32::NO) {
        QString u32Hex;
        { str::QSep spU32(u32Hex, " ");
            for (auto v : value) {
                spU32.sep();
                snprintf(buf, std::size(buf), "%04X", static_cast<int>(v));
                str::append(u32Hex, buf);
            }
        }
        sp.sep();
        appendNonBullet(text, "Prop.Bullet.Utf32",
                        "<a href='pt:utf32' class='popup'>", "</a>");
        appendCopyable(text, u32Hex);
    }
}


namespace {

    void appendBlock(QString& text, const uc::Block& block, str::QSep& sp)
    {
        sp.sep();
        appendValuePopup(text, block, "Prop.Bullet.Block", "pk");
    }

    void appendKey(std::u8string& text, std::u8string_view header, char main)
    {
        text += KEY_START;
        text += header;
        if (main) {
            switch (main) {
            case ' ':
                text += loc::get("Prop.Input.Space");
                break;
            default:
                text += main;
            }
        }
        text += KEY_END;
    }

    void appendSgnwVariants(QString& text, const sw::Info& sw)
    {
        if (!sw)
            return;

        if (sw.isSimple()) {
            text += "<h4>";
            str::append(text, loc::get("Prop.Head.SuttonNoFill"));
            text += "</h4>";
            return;
        }

        // Start table
        text += "<table class='swt'>";
        // Draw head
        text += "<tr><th>&nbsp;</th>";
        for (int col = 0; col < sw::N_FILL; ++col) {
            if (sw.hasFill0(col)) {
                text += "<th>";
                if (col == 0) {
                    str::append(text, u8"–");   // short dash
                } else {
                    text += 'F';
                    text += static_cast<char>('1' + col);
                }
                text += "</th>";
            }
        }
        for (int row = 0; row < sw::N_ROT; ++row) {
            if (sw.hasRot0(row)) {
                // Draw vert header
                text += "<tr><th>";
                if (row == 0) {
                    str::append(text, u8"–");   // short dash
                } else {
                    text += 'R';
                    str::append(text, row + 1);
                }
                text += "</th>";
                // Draw cell
                for (int col = 0; col < sw::N_FILL; ++col) {
                    if (sw.hasFill0(col)) {
                        char32_t cp = sw.subj();
                        auto copyable = QString::fromUcs4(&cp, 1);
                        if (col != 0)
                            str::append(copyable, static_cast<char32_t>(0x1DA9A + col));
                        if (row != 0)
                            str::append(copyable, static_cast<char32_t>(0x1DAA0 + row));
                        text += "<td><a href='c:";
                            text += copyable;
                            text += "'>";
                        if (auto bc = sw.baseChar())
                            str::append(text, bc);
                        text += copyable;
                        text += "</a>";
                    }
                }
            }   // if hasRot0
        }
        // Draw chars
        text += "</table>";
    }

    template <auto... Params>
    inline bool isIn(auto what) {
        return ((what == Params) || ...);
    }

}   // anon namespace


void mywiki::appendStylesheet(QString& text, bool hasSignWriting)
{
    str::append(text, "<style>");
    str::append(text, STYLES_WIKI);

    // SignWriting: add more styles
    auto color = QApplication::palette().windowText().color();
    color.setAlpha(20);
    if (hasSignWriting) {
        auto& font = uc::fontInfo[static_cast<int>(uc::EcFont::SIGNWRITING)];
        text += ".swt { border-collapse:collapse; margin:0.8ex 0; } "
                ".swt td { border:1px solid ";
            text += color.name(QColor::HexArgb);
            text += "; padding:0 2px; font-family:";
            text += font.familiesComma();
            text += "; } ";
        text += ".swt a { text-decoration:none; color:palette(window-text); font-size:26pt; } "
                ".swt th { vertical-align:middle; } ";
    }

    str::append(text, "</style>");
}


namespace {

    void appendSubhead(QString& text, std::string_view key)
    {
        text += "<h2>";
        str::append(text, loc::get(key));
        text += "</h2>";
    }

    void appendBlockSubhead(QString& text)
        { appendSubhead(text, "Prop.Head.Block"); }

    void appendScriptSubhead(QString& text)
        { appendSubhead(text, "Prop.Head.Script"); }

    struct DepBuf {
        char d[30] = {0};
        void build(std::u8string_view text);
        std::u8string_view u8() const { return str::toU8sv(d); }
    };

    void DepBuf::build(std::u8string_view text)
    {
        auto chars = mojibake::toS<std::u32string>(text);
        char* p = d;
        char* end = std::end(d);
        for (auto ch : chars) {
            if (p != d && p < end) {
                *(p++) = ' ';
            }
            auto nRem = end - p;
            auto nChars = snprintf(p, nRem, "%04X", static_cast<int>(ch));
            p += nChars;
        }
    }

    void appendDumbSingleCpHtml(QString& text, char32_t v)
    {
        char buf[30];
        snprintf(buf, std::size(buf), "&#%d;", static_cast<int>(v));
        mywiki::appendCopyable(text, buf);
    }

    void appendSingleCpHtml(QString& text, const uc::Cp& cp)
    {
        appendDumbSingleCpHtml(text, cp.subj);
        cp.name.traverseAllT([&text](uc::TextRole role, std::u8string_view s) {
            if (role == uc::TextRole::HTML) {
                text += ' ';
                mywiki::appendCopyable(text, str::toQ(s));
            }
        });
    }

    struct HtInfo {
        std::u8string_view mnemonic {};
    };

    void appendMultiCpHtml(QString& text, std::u32string_view v)
    {
        switch (v.length()) {
        case 0: // No characters — do nothing
            return;
        case 1: // One character — treat as one character
            if (auto cp = uc::cpsByCode[v[0]]) {
                appendSingleCpHtml(text, *cp);
            } else {
                appendDumbSingleCpHtml(text, v[0]);
            }
            return;
        default:;
        }

        // Go!
        char buf[uc::LONGEST_LIB * 30];
        char* p = buf;
        char* end = p + std::size(buf);
        bool haveMnemonic = false;
        HtInfo htData[uc::LONGEST_LIB];

        // Write simple data, check for mnemonics
        for (size_t i = 0; i < v.length(); ++i) {
            int c = v[i];
            auto nWritten = snprintf(p, end - p, "&#%d;", c);
            p += nWritten;
            if (auto cp = uc::cpsByCode[c]) {
                auto mnemonic = cp->name.getText(uc::TextRole::HTML);
                if (!mnemonic.empty()) {
                    haveMnemonic = true;
                    htData[i].mnemonic = mnemonic;
                }
            }
        }
        mywiki::appendCopyable(text, str::toQ(std::string_view{buf, p}));

        // If have any mnemonic → append it!
        if (haveMnemonic) {
            text += ' ';
            p = buf;
            for (size_t i = 0; i < v.length(); ++i) {
                auto& q = htData[i];
                auto remder = end - p;
                if (!q.mnemonic.empty()) {  // Have mnemonic
                    auto nCopy = std::min<ptrdiff_t>(remder, q.mnemonic.length());
                    p = std::copy_n(q.mnemonic.data(), nCopy, p);
                } else {    // Copy HTML decimal
                    auto nWritten = snprintf(p, end - p, "&#%d;", static_cast<int>(v[i]));
                    p += nWritten;
                }
            }
            mywiki::appendCopyable(text, str::toQ(std::string_view{buf, p}));
        }
    }

    enum class CpSerializations { NO, YES };

    void appendCpAltNames(QString& text, const uc::Cp& cp)
    {
        // Alt. names
        bool isInitial = true;
        cp.name.traverseAllT([&text,&isInitial]
                             (uc::TextRole role, std::u8string_view s) {
            switch (role) {
            case uc::TextRole::ALT_NAME:
            case uc::TextRole::ABBREV:
                if (isInitial) {
                    isInitial = false;
                    text += "<p style='" CNAME_ALTNAME "'>";
                } else {
                    text += "; ";
                }
                mywiki::appendCopyable(text, str::toQ(s), "altname");
                break;
            case uc::TextRole::MAIN_NAME:
            case uc::TextRole::HTML:
            case uc::TextRole::DEP_INSTEAD:
            case uc::TextRole::DEP_INSTEAD2:
            case uc::TextRole::CMD_END:
                break;
            }
        });
        if (!isInitial) {
            text += "</b>";
        }
    }

    void appendCpBullets(QString& text, const uc::Cp& cp, CpSerializations serializations)
    {
        str::append(text, "<p>");
        str::QSep sp(text, "<br>");

        // Script
        sp.sep();
        auto& scr = cp.script();
        appendValuePopup(text, scr, "Prop.Bullet.Script", "ps");

        // Unicode version
        sp.sep();
        mywiki::appendVersion(text, {}, cp.version());

        // Character type
        sp.sep();
        appendValuePopup(text, cp.category(), "Prop.Bullet.Type", "pc");

        // Numeric value
        auto& numc = cp.numeric();
        if (numc.isPresent()) {
            sp.sep();
            appendNonBullet(text, numc.type().locKey,
                    "<a href='pt:number' class='popup'>", "</a>");
            QString buf;
            switch (numc.fracType()) {
            case uc::FracType::NONE:        // should not happen
            case uc::FracType::INTEGER:
                str::append(buf, numc.num);
                break;
            case uc::FracType::VULGAR:
                str::append(buf, numc.num);
                str::append(buf, "/");
                str::append(buf, numc.denom);
                break;
            case uc::FracType::DECIMAL: {
                    auto val = static_cast<double>(numc.num) / numc.denom;
                    QString s = QString::number(val);
                    s.replace('.', QChar{loc::active::numfmt.decimalPoint});
                } break;
            }
            if (numc.altInt != 0) {
                buf = str::toQ(loc::get("Prop.Num.Or").arg(str::toU8(buf), numc.altInt));
            }
            text += buf;
        }

        // Bidi writing
        sp.sep();
        appendValuePopup(text, cp.bidiClass(), "Prop.Bullet.Bidi", "pb");

        // Block
        auto& blk = cp.block();
        appendBlock(text, blk, sp);

        auto comps = uc::cpOldComps(cp.subj);
        if (comps) {
            sp.sep();
            appendNonBullet(text, "Prop.Bullet.Computers");
            str::QSep spC(text, ", ");
            while (comps) {
                // Extract and remove bit
                auto bit = comps.smallest();
                comps.remove(bit);
                // Turn bit to index
                auto iBit = std::countr_zero(static_cast<unsigned>(bit));
                // Write what we got
                spC.sep();
                str::append(text, uc::oldCompInfo[iBit].locName());
            }
        }

        // Input
        if (serializations != CpSerializations::NO) {
            auto im = uc::cpInputMethods(cp.subj);
            if (im.hasSmth()) {
                sp.sep();
                appendNonBullet(text, "Prop.Input.Bullet");
                str::QSep sp1(text, ";&nbsp; ");
                if (!im.sometimesKey.empty()) {
                    sp1.sep();
                    mywiki::appendNoFont(
                                text, loc::get("Prop.Input.Sometimes")
                                      .arg(im.sometimesKey));
                }
                if (im.hasAltCode()) {
                    sp1.sep();
                    text += "<a href='pt:altcode' class='popup'>";
                    str::append(text, loc::get("Prop.Input.AltCode"));
                    text += "</a> ";
                    str::QSep sp2(text, ", ");
                    if (im.alt.dosCommon) {
                        sp2.sep();
                        str::append(text, static_cast<int>(im.alt.dosCommon));
                    }
                    if (im.alt.win) {
                        sp2.sep();
                        str::append(text, "0");
                        str::append(text, static_cast<int>(im.alt.win));
                    }
                    if (!im.alt.hasLocaleIndependent()) {
                        if (im.alt.dosEn) {
                            sp2.sep();
                            str::append(text, static_cast<int>(im.alt.dosEn));
                            str::append(text, u8" (en)");
                        }
                        if (im.alt.dosRu) {
                            sp2.sep();
                            str::append(text, static_cast<int>(im.alt.dosRu));
                            str::append(text, u8" (ru)");
                        }
                        if (im.alt.unicode) {
                            sp2.sep();
                            str::append(text, "+");
                            str::appendHex(text, im.alt.unicode);
                        }
                    }
                }
                // Birman test
                if (im.hasBirman()) {
                    sp1.sep();
                    text += "<a href='pt:birman' class='popup'>";
                    str::append(text, loc::get("Prop.Input.Birman"));
                    text += "</a> ";
                    std::u8string sKey;
                        appendKey(sKey, u8"AltGr+", im.birman.key);
                        if (im.birman.letter != 0) {
                            sKey += ' ';
                            appendKey(sKey, {}, im.birman.letter);
                        }
                    if (im.birman.isTwice) {
                        str::append(text, loc::get("Prop.Input.Twice")
                                          .arg(sKey));
                    } else {
                        str::append(text, sKey);
                    }
                }
            }

            // HTML
            sp.sep();
            appendNonBullet(text, "Prop.Bullet.Html");
            appendSingleCpHtml(text, cp);

            mywiki::appendUtf(text, Want32::NO, sp, cp.subj);
        }   // serializations

        text.append("</p>");
    }

}   // anon namespace


QString mywiki::buildHtml(const uc::Cp& cp)
{
    QString text;

    sw::Info sw(cp);

    appendStylesheet(text, sw);
    str::append(text, "<h1>");
    QString name = cp.viewableName();
    appendCopyable(text, name, "bigcopy");
    str::append(text, "</h1>");

    appendCpAltNames(text, cp);

    // Deprecated
    if (cp.isDeprecated()) {
        text += "<h3 class='deph'>";
        static constexpr std::u8string_view HTPROPS = u8"href='pt:deprecated' class='deprecated'";
        DepBuf buf, buf2;
        std::string_view key = "0";
        char cbuf = '0';

        std::u8string_view instead = cp.name.getText(uc::TextRole::DEP_INSTEAD);
        if (!instead.empty()) {
            auto q = instead[0];
            if (q >= uc::INSTEAD_MIN && q <= uc::INSTEAD_MAX) {
                // Special instead
                cbuf = q;
                key = std::string_view{ &cbuf, 1 };
            } else {
                // Characters by code
                key = "Ins";
                buf.build(instead);
                std::u8string_view instead2 = cp.name.getText(uc::TextRole::DEP_INSTEAD2);
                if (!instead2.empty()) {
                    key = "Ins2";
                    buf2.build(instead2);
                }
            }
        }

        auto q = loc::get(str::cat("Prop.Deprec.", key)).arg(HTPROPS, buf.u8(), buf2.u8());
        str::append(text, q);
        text += "</h3>";
    }

    // Default ignorable
    if (cp.isDefaultIgnorable()) {
        text += "<h4><a href='pt:ignorable' class='popup'>";
        str::append(text, loc::get("Prop.Head.DefIgn"));
        text += "</a></h4>";
    }

    // VS16 emoji
    if (cp.isVs16Emoji()) {
        text += "<h4>";
        str::append(text, loc::get("Prop.Head.Emoji16")
                          .arg(u8"href='pt:emoji' class='popup'"));
        text += "</h4>";
    }

    appendSgnwVariants(text, sw);

    {   // Info box
        appendCpBullets(text, cp, CpSerializations::YES);

        auto& blk = cp.block();
        if (blk.startingCp == 0) {
            // Basic Latin:
            // Control → t:control (stored in catInfo)
            // Latn → s:Latn
            // Others → t:ASCII, stored here for simplicity
            if (cp.ecCategory == uc::EcCategory::CONTROL) {
                //  Control char description
                appendSubhead(text, "Prop.Head.Control");
                appendWiki(text, blk, uc::categoryInfo[static_cast<int>(uc::EcCategory::CONTROL)].loc.description);
            } else if (auto& sc = cp.script(); &sc != uc::scriptInfo) {
                // Script description
                appendScriptSubhead(text);
                mywiki::appendHtml(text, sc, false);
            } else {
                // Script description
                appendBlockSubhead(text);
                appendWiki(text, blk, blk.loc.description);
            }
        } else {
            if (blk.hasDescription()) {
                // Block description
                appendBlockSubhead(text);
                appendWiki(text, blk, blk.loc.description);
            } else if (auto& sc = cp.scriptEx(); &sc != uc::scriptInfo){
                // Script description
                appendScriptSubhead(text);
                mywiki::appendHtml(text, sc, false);
            }
        }
    }
    return text;
}   // buildHtml


void mywiki::appendMissingCharInfo(QString& text, char32_t code)
{
    str::append(text, "<p>");
    str::QSep sp(text, "<br>");
    appendBlock(text, *uc::blockOf(code), sp);
    mywiki::appendUtf(text, Want32::NO, sp, code);
}


QString mywiki::buildEmptyCpHtml(char32_t code, const QColor& color)
{
    QString text;
    appendStylesheet(text);
    text += "<h1 style='color:" + color.name() + "'>";
    str::append(text, loc::get("Prop.Head.Empty"));
    text += "</h1>";
    mywiki::appendMissingCharInfo(text, code);
    return text;
}


QString mywiki::buildNonCharHtml(char32_t code)
{
    QString text;
    appendStylesheet(text);
    text += "<h1>";
    str::append(text, loc::get("Prop.Head.NonChar"));
    text += "</h1>";
    mywiki::appendMissingCharInfo(text, code);
    text += "<p>";
    appendWiki(text, *uc::blockOf(code), loc::get("Term.noncharacter.Text"));
    return text;
}


bool mywiki::isEngTermShown(const uc::Term& term)
{
    return loc::active::showEnglishTerms        // language explicitly permits
        && !term.engName.empty()                // and English name present
        && (term.engName != term.loc.name);     // and different from L10n name
}


QString mywiki::buildHtml(const uc::Term& x)
{
    QString text;
    appendStylesheet(text);
    str::append(text, "<p><b>");
    str::append(text, x.loc.name);
    str::append(text, "</b>"sv);
    if (isEngTermShown(x)) {
        str::append(text, u8"\u00A0/ "sv);
        str::append(text, x.engName);
    }

    str::append(text, "<p>");
    appendWiki(text, x, x.loc.description);
    return text;
}


namespace {
    const char8_t* STR_RANGE = u8"%04X…%04X";
}

QString mywiki::buildHtml(const uc::Block& x)
{
    QString text;
    appendStylesheet(text);
    appendHeader(text, x);

    str::append(text, "<p>");
    str::QSep sp(text, "<br>");

    sp.sep();
    appendBullet(text, "Prop.Bullet.EngTech");
    str::append(text, x.name);

    sp.sep();
    appendBullet(text, "Prop.Bullet.Range");
    char buf[30];
    snprintf(buf, std::size(buf), reinterpret_cast<const char*>(STR_RANGE),
                int(x.startingCp), int(x.endingCp));
    str::append(text, buf);

    sp.sep();
    appendBullet(text, "Prop.Bullet.VerAppear");
    mywiki::appendVersionValue(text, x.version());

    auto nNonChars = x.nNonChars();
    if (nNonChars) {
        sp.sep();
        appendBullet(text, "Prop.Bullet.NonChars",
                     "<a href='pt:noncharacter' class='popup'>", "</a>");
        str::append(text, nNonChars);
    }

    sp.sep();
    auto nEmpty = x.nEmptyPlaces();
    if (nEmpty != 0) {
        appendBullet(text, "Prop.Bullet.Empty");
        str::append(text, nEmpty);
    } else {
        appendBullet(text, "Prop.Bullet.VerFilled");
        mywiki::appendVersionValue(text, x.lastVersion());
    }

    if (x.hasDescription()) {
        str::append(text, "<p>");
        appendWiki(text, x, x.loc.description);
        str::append(text, "</p>");
    } else if (x.ecScript != uc::EcScript::NONE) {
        text += "<p><b>";
        str::append(text, loc::get("Prop.Head.Script"));
        text += "</b></p>";
        mywiki::appendHtml(text, x.script(), false);
    }
    return text;
}


void mywiki::hackDocument(QTextDocument* doc)
{
    auto block = doc->firstBlock();
    while (block.isValid()) {
        auto formats = block.textFormats();
        for (auto& fmt : formats) {
            if (fmt.format.isAnchor()) {
                auto href = fmt.format.anchorHref();
                if (href.startsWith("ps:") || href.startsWith("pt:")
                        || href.startsWith("http:") || href.startsWith("https:")) {
                    // Generate format
                    auto newFmt = fmt.format;
                    newFmt.setBackground({});
                    auto color = newFmt.foreground().color();
                    color.setAlpha(64);
                    newFmt.setUnderlineStyle(QTextCharFormat::DashUnderline);
                    newFmt.setUnderlineColor(color);
                    // Mark it
                    QTextCursor cur(doc);
                    auto startPos = block.position() + fmt.start;
                    cur.setPosition(startPos);
                    cur.setPosition(startPos + fmt.length, QTextCursor::KeepAnchor);
                    cur.setCharFormat(newFmt);
                }
            }
        }
        block = block.next();
    }
}


QString mywiki::buildLibFolderHtml(const uc::LibNode& node, const QColor& color)
{
    QString text;
    appendStylesheet(text);
    text += "<h1 style='color:" + color.name() + "'>";
    text += node.viewableTitle(uc::TitleMode::LONG).toHtmlEscaped();
    text += "</h1>";
    return text;
}


const uc::Cp* onlyIndependentCp(std::u32string_view x)
{
    switch (x.length()) {
    case 0: return nullptr;
    case 1:
        if (x[0] < uc::CAPACITY) {
            return uc::cpsByCode[x[0]];
        } else {
            return nullptr;
        }
    default:;
    }
    const uc::Cp* r = nullptr;
    for (auto c : x) {
        if (c < uc::CAPACITY) {
            if (auto cp = uc::cpsByCode[c]) {
                auto& cat = cp->category();
                if (cat.isIndependent()) {
                    if (r) {
                        return nullptr;
                    } else {
                        r = cp;
                    }
                }
            }
        }
    }
    return r;
}


QString mywiki::buildHtml(const uc::LibNode& node)
{
    QString text;
    appendStylesheet(text);
    text += "<h1>";
        auto title = node.viewableTitle(uc::TitleMode::LONG);
    appendCopyable(text, title, "bigcopy");
    text += "</h1>";

    text += "<p>";
    str::QSep sp(text, "<br>");

    if (!node.value.empty()) {
        appendNonBullet(text, "Prop.Bullet.Html");
        appendMultiCpHtml(text, node.value);
    }

    appendUtf(text, Want32::YES, sp, node.value);

    auto ver = uc::EcVersion::FIRST;
    for (auto c : node.value) {
        if (c < uc::CAPACITY) {
            auto cp = uc::cpsByCode[c];
            if (cp) {
                ver = std::max(ver, cp->ecVersion);
            }
        }
    }

    sp.sep();
    appendNonBullet(text, "Prop.Bullet.AllVer");
    appendVersionValue(text, uc::versionInfo[static_cast<int>(ver)]);

    if (node.ecEmojiVersion != uc::EcVersion::NONE) {
        sp.sep();
        appendNonBullet(text, "Prop.Bullet.EmojiVer");
        appendEmojiValue(text, node.emojiVersion(), node.emojiPrevVersion());
    }

    appendSubhead(text, "Prop.Head.Comp");
    text += "<p>";
    str::QSep sp2(text, "<br>");
    char buf[40];
    for (auto c : node.value) {
        sp2.sep();
        snprintf(buf, std::size(buf), "%04X ", static_cast<int>(c));
        text += buf;
        if (c < uc::CAPACITY) {
            auto cp = uc::cpsByCode[c];
            if (cp) {
                appendCopyable(text, cp->viewableName());
            }
        }
    }

    if (auto cp = onlyIndependentCp(node.value)) {
        snprintf(buf, std::size(buf), "<h2>%04X ", static_cast<int>(cp->subj));
        text += buf;
        appendCopyable(text, cp->viewableName());
        text += "</h2>";
        appendCpBullets(text, *cp, CpSerializations::NO);
    }

    return text;
}


QString mywiki::buildHtml(const uc::Version& version)
{
    QString text;
    appendStylesheet(text);
    text += "<p><b>";
    if (!version.unicodeName.empty()) {
        str::append(text, loc::get("Prop.Head.Uc").arg(version.unicodeName));
    } else {
        str::append(text, loc::get("Prop.Head.Em").arg(version.emojiName));
    }
    text += "</b> (";
    // Emoji version
    if (!version.unicodeName.empty() && !version.emojiName.empty()) {
        str::append(text, loc::get("Prop.Head.EqEm").arg(version.emojiName));
    }
    // Date
    char buf[30];
    snprintf(buf, std::size(buf), "Common.Mon.%d",
             static_cast<int>(version.date.month));
    auto& monTemplate = loc::get(buf);
    auto s = monTemplate.arg(version.date.year);
    str::append(text, s);
    text += ")";

    text += "<p>";
    // Transient
    str::QSep sp(text, "<br>");
    if (version.stats.chars.nTransient != 0) {
        sp.sep();
        appendNonBullet(text, "Prop.Bullet.Transient");
        appendCopyable(text, version.stats.chars.nTransient);
    }
    // New
    if (!version.isFirst()) {
        sp.sep();
        appendNonBullet(text, "Prop.Bullet.NewChar");
        appendCopyable(text, version.stats.chars.nNew);
    }
    // Total
    sp.sep();
    mywiki::appendNoFont(text, loc::get("Prop.Bullet.TotalChar"));
    str::append(text, ": ");
    appendCopyable(text, version.stats.chars.nTotal);

    if (version.flags.have(uc::Vfg::TEXT)) {
        str::append(text, "<p>");
        auto key = str::cat("Version.", str::toSv(version.link({})) , ".Text");
        mywiki::appendNoFont(text, loc::get(key));

    }
    return text;
}
