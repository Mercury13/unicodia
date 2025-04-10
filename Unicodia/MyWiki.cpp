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
#include "UcOldInput.h"
#include "UcCp.h"

// Search
#include "Search/request.h"

// L10n
#include "LocDic.h"
#include "LocList.h"

// Wiki
#include "Wiki.h"
#include "Skin.h"

// Mojibake
#include "mojibake.h"

// Functions
#include "CharPaint/emoji.h"

using namespace std::string_view_literals;

#define NBSP "\u00A0"

constexpr const char* EURO_COMMA = ", ";
constexpr const char* EURO_SEMICOLON = "; ";

const mywiki::Context DEFAULT_CONTEXT {
    .font { uc::fontInfo[0] },
    .lang = nullptr,
    .locPrefixDot {},
};

///// Gui //////////////////////////////////////////////////////////////////////

void mywiki::Gui::popupAtRel(
        QWidget* widget, const QRect& relRect, const QString& html)
{
    popupAtAbs(widget,
               QRect{ widget->mapToGlobal(relRect.topLeft()), relRect.size() },
               html);
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
    if (!relRect) {      // have widget, no rect
        popupAtWidget(widget, html);
    } else {                    // have widget and rect
        popupAtRel(widget, *relRect, html);
    }
}

template <>
constexpr unsigned ec::size<uc::EcUpCategory>()
    { return static_cast<unsigned>(uc::EcUpCategory::NN); }

namespace {

    constinit const ec::Array<char, uc::EcUpCategory> upCatNames {
        'C', 'F', 'L', 'M', 'N', 'P', 'S', 'Z'
    };

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

    class GotoCpLink : public mywiki::Link
    {
    public:
        char32_t cp;
        GotoCpLink(char32_t x) : cp(x) {}
        mywiki::LinkClass clazz() const override { return mywiki::LinkClass::INTERNAL; }
        void go(QWidget* widget, TinyOpt<QRect> rect, mywiki::Gui& gui) override;
    };

    void GotoCpLink::go(QWidget* wi, TinyOpt<QRect>, mywiki::Gui& gui)
        { gui.linkWalker().gotoCp(wi, cp); }

    class GotoLibCpLink : public mywiki::Link
    {
    public:
        char32_t cp;
        GotoLibCpLink(char32_t x) : cp(x) {}
        mywiki::LinkClass clazz() const override { return mywiki::LinkClass::INTERNAL; }
        void go(QWidget* widget, TinyOpt<QRect> rect, mywiki::Gui& gui) override;
    };

    void GotoLibCpLink::go(QWidget* wi, TinyOpt<QRect>, mywiki::Gui& gui)
        { gui.linkWalker().gotoLibCp(wi, cp); }

    class BlinkAddCpToFavsLink : public mywiki::Link
    {
    public:
        mywiki::LinkClass clazz() const override { return mywiki::LinkClass::INTERNAL; }
        void go(QWidget*, TinyOpt<QRect>, mywiki::Gui& gui) override;
    };

    void BlinkAddCpToFavsLink::go(QWidget*, TinyOpt<QRect>, mywiki::Gui& gui)
        { gui.linkWalker().blinkAddCpToFavs(); }

    template <class Fields, class Request>
    class RequestLink : public mywiki::Link
    {
    public:
        RequestLink(const Fields& x) : fields(x) {}
        mywiki::LinkClass clazz() const override { return mywiki::LinkClass::SEARCH; }
        void go(QWidget*, TinyOpt<QRect>, mywiki::Gui& gui) override;
    private:
        const Fields fields;
    };

    template <class Fields, class Request>
    void RequestLink<Fields, Request>::go(QWidget*, TinyOpt<QRect>, mywiki::Gui& gui)
    {
        Request rq(fields);
        gui.linkWalker().searchForRequest(rq);
    }

    using CharRequestLink = RequestLink<uc::CharFields, uc::CharFieldRequest>;
    using EmojiRequestLink = RequestLink<uc::EmojiFields, uc::EmojiFieldRequest>;

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

std::unique_ptr<mywiki::Link> mywiki::parsePopOldCompLink(std::string_view target)
{
    if (auto* oldComp = uc::old::findComp(target))
        return mu(*oldComp);
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

std::unique_ptr<mywiki::Link> mywiki::parsePopGlyphStyleLink(std::string_view target)
{
    int index = 0;
    str::fromChars(target, index);
    if (index == 0 || index >= static_cast<int>(uc::EcGlyphStyleChannel::NN))
        return {};
    return mu(uc::glyphStyleChannelInfo[index]);
}

std::unique_ptr<mywiki::Link> mywiki::parseGotoCpLink(std::string_view target)
{
    int code = 0;
    str::fromChars(target, code, 16);
    if (code >= uc::CAPACITY || !uc::cpsByCode[code])
        return {};
    return std::make_unique<GotoCpLink>(code);
}

std::unique_ptr<mywiki::Link> mywiki::parseGotoLibCpLink(std::string_view target)
{
    int code = 0;
    str::fromChars(target, code, 16);
    if (code >= uc::CAPACITY || !uc::cpsByCode[code])
        return {};
    return std::make_unique<GotoLibCpLink>(code);
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

std::unique_ptr<mywiki::Link> mywiki::parseGotoInterfaceLink(std::string_view target)
{
    if (target == "blinkaddcp") {
        return std::make_unique<BlinkAddCpToFavsLink>();
    }
    return {};
}


namespace {

    struct Kv {
        std::string_view key, value;
        operator bool() const noexcept { return !value.empty(); }
    };

    Kv splitIntoKv(std::string_view part)
    {
        auto posEqual = part.find('=');
        if (posEqual == 0 || posEqual == std::string_view::npos)
            return {};
        // Split into code and value
        return { .key = part.substr(0, posEqual),
                 .value = part.substr(posEqual + 1) };
    }

}   // anon namespace


std::unique_ptr<mywiki::Link> mywiki::parseCharRequestLink(std::string_view target)
{
    uc::CharFields fields;
    auto parts = str::splitSv(target, '|');
    if (parts.empty())  // Won’t search for empty thing
        return {};
    unsigned uTmp;
    for (auto part : parts) {
        auto [code, value] = splitIntoKv(part);
        if (value.empty() || code.length() != 1)
            return nullptr;
        switch (code[0]) {
        case 'v':   // version
            if (auto version = uc::findVersion(value)) {
                fields.ecVersion = version->stats.thisEcVersion;
                break;
            }
            return nullptr;
        case 's':   // script
            if (auto script = uc::findScript(value)) {
                fields.ecScript = static_cast<uc::EcScript>(script - uc::scriptInfo);
                break;
            }
            return nullptr;
        case 'c':   // category
            if (auto cat = uc::findCategory(value)) {
                fields.ecCategory = static_cast<uc::EcCategory>(cat - uc::categoryInfo);
                break;
            }
            return nullptr;
        case 'u':   // up-category
            if (value.length() == 1) {
                if (auto v = upCatNames.findDef(value[0], uc::EcUpCategory::NO_VALUE);
                        v != uc::EcUpCategory::NO_VALUE) {
                    fields.ecUpCat = v;
                    break;
                }
            }
            return nullptr;
        case 'b':   // bidi property
            if (auto bdc = uc::findBidiClass(value)) {
                fields.ecBidiClass = static_cast<uc::EcBidiClass>(bdc - uc::bidiClassInfo);
                break;
            }
            return nullptr;
        case 'f':   // flags
            if (auto v = str::fromChars(value, uTmp); v.ec == std::errc{}) {
                fields.fgs.setNumeric(uTmp);
                break;
            }
            return nullptr;
        case 'N':   // number
            if (value == "1"sv) {
                fields.isNumber = 1;
            }
            break;
        case 'C':   // old computers
            if (auto comp = uc::old::findComp(value)) {
                auto index = comp - uc::old::info;
                fields.oldComp = static_cast<uc::OldComp>(1 << index);
                break;
            }
            break;
        default:
            return nullptr;
        }
    }
    return std::make_unique<CharRequestLink>(fields);
}


std::unique_ptr<mywiki::Link> mywiki::parseEmojiRequestLink(std::string_view target)
{
    uc::EmojiFields fields;
    auto parts = str::splitSv(target, '|');
    if (parts.empty())  // Won’t search for empty thing
        return {};
    for (auto part : parts) {
        auto [code, value] = splitIntoKv(part);
        if (value.empty() || code.length() != 1)
            return nullptr;
        switch (code[0]) {
        case 'v':   // version
            if (auto version = uc::findVersion(value)) {
                fields.ecVersion = version->stats.thisEcVersion;
                break;
            }
            return nullptr;
        }
    }
    return std::make_unique<EmojiRequestLink>(fields);
}


constexpr std::string_view SCH_QRY_CHARS = "srq";
constexpr std::string_view SCH_QRY_EMOJI = "sre";


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
    } else if (scheme == "po"sv) {
        return parsePopOldCompLink(target);
    } else if (scheme == "pgs"sv) {
        return parsePopGlyphStyleLink(target);
    } else if (scheme == "gc"sv) {
        return parseGotoCpLink(target);
    } else if (scheme == "gi"sv) {
        return parseGotoInterfaceLink(target);
    } else if (scheme == "glc"sv) {
        return parseGotoLibCpLink(target);
    } else if (scheme == SCH_QRY_CHARS) {
        return parseCharRequestLink(target);
    } else if (scheme == SCH_QRY_EMOJI) {
        return parseEmojiRequestLink(target);
    } else if (scheme == "c"sv) {
        return std::make_unique<CopyLink>(target);
    } else if (scheme == "http"sv || scheme == "https"sv) {
        return std::make_unique<InetLink>(scheme, target);
    }
    return {};
}


std::unique_ptr<mywiki::Link> mywiki::parseLink(std::string_view link)
{
    auto posScheme = link.find(':');
    if (posScheme == std::string_view::npos)
        return {};
    auto scheme = str::trimSv(link.substr(0, posScheme));
    if (scheme.empty())
        return {};
    auto remder = str::trimSv(link.substr(posScheme + 1));
    if (remder.empty())
        return {};
    return parseLink(scheme, remder);
}

void mywiki::go(QWidget* widget, TinyOpt<QRect> rect, Gui& gui, std::string_view link)
{
    auto p = parseLink(link);
    if (p) {
        p->go(widget, rect, gui);
    }
}


const uc::Font& mywiki::DescFont::getDescFont(const uc::Font& font)
{
    auto that = &font;
    while (that->flags.have(uc::Ffg::DESC_AVOID))
        ++that;
    return *that;
}


namespace {

    constexpr std::u8string_view BULLET = u8"•\u00A0";

    struct TextLang {
        std::string_view key {};
        std::string_view preComment {};

        static const TextLang DFLT;
    };
    const TextLang TextLang::DFLT;

    enum class Nspkf : unsigned char {
        ITALIC_PARENS   = 1<<0,  // italic style + parentheses
        LOCATION        = 1<<1,  // location
        DFLT = ITALIC_PARENS | LOCATION
    };

    class Eng : public wiki::Engine
    {
    public:
        QString& s;
        mywiki::Context context;
        bool hasNSpeakers = false;

        Eng(QString& aS, const mywiki::Context& aContext) : s(aS), context(aContext) {}
        void appendPlain(std::string_view x) override;
        void appendLink(
                const SafeVector<std::string_view>& x,
                bool hasRemainder, bool needRecurse) override;
        void appendTemplate(
                Buf1d<const std::string_view> x,
                bool hasRemainder) override;
        void toggleWeight(Flags<wiki::Weight> changed) override;
        void appendBreak(wiki::Strength strength, wiki::Feature feature, unsigned indentSize) override;
        std::string_view defaultLinkTextSv(std::string_view target) override;
        void finish() override;
        void appendNSpeakers(const TextLang& x, Flags<Nspkf> fgs);
        void appendNSpeakers(Buf1d<const std::string_view> x, Flags<Nspkf> fgs);
    protected:
        std::u8string linkText;
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
            auto tag = weight.finishingTag();
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
            auto tag = weight.finishingTag();
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

    void Eng::appendBreak(
            wiki::Strength strength, wiki::Feature feature,
            unsigned indentSize)
    {
        finishDiv();
        bool isStart = false;
        switch (strength) {
        case wiki::Strength::BREAK:
            switch (feature) {
            case wiki::Feature::BULLET:
                if (!isStart) {
                    s += "<br>";
                } break;
            case wiki::Feature::NONE:
            case wiki::Feature::INDENT:
                s += "<div>";
                isDiv = true;
                break;
            } break;
        case wiki::Strength::START:
        case wiki::Strength::PARAGRAPH:
            s += "<p>";
            break;
        }

        switch (feature) {
        case wiki::Feature::NONE:
            break;
        case wiki::Feature::INDENT:
            for (unsigned i = 0; i < indentSize; ++i) {
                s += NBSP NBSP NBSP;
            }
            break;
        case wiki::Feature::BULLET:
            str::append(s, BULLET);
            break;
        }
    }

    void Eng::runRecursive(std::string_view text)
    {
        if (!isSuppressed) {
            auto tag = weight.restartingTag();
            str::append(s, tag);
        }
        wiki::run(*this, text, wiki::Mode::SPAN);
    }

    void Eng::finishRecursion(bool hasRemainder, bool prepareResult)
    {
        if (!isSuppressed) {
            if (hasRemainder) {
                auto tag = weight.restartingTag();
                str::append(s, tag);
            } else {
                weight.clear();
            }
        }
        isSuppressed = prepareResult;
    }

    std::string_view Eng::defaultLinkTextSv(std::string_view target)
    {
        if (target.starts_with("po:")) {
            auto key = target.substr(3);
            if (auto info = uc::old::findComp(key)) {
                linkText = info->locName();
                return str::toSv(linkText);
            }
        }
        return {};
    }

    void startLink(QString& s, std::string_view style, std::string_view target)
    {
        s.append("<a ");
        str::append(s, style);
        s.append(" href='");
        str::append(s, target);
        s.append("'>");
    }

    void finishLink(QString& s)
    {
        s.append("</a>");
    }

    void Eng::appendLink(const SafeVector<std::string_view>& x,
                         bool hasRemainder,
                         bool needRecurse)
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
        case mywiki::LinkClass::INTERNAL:
            style = "class=inet";
            break;
        case mywiki::LinkClass::POPUP:
            style = "class=popup";
            break;
        }

        if (needRecurse) {
            auto q = prepareRecursion(text);
            startLink(s, style, target);
            runRecursive(text);
            finishLink(s);
            finishRecursion(hasRemainder, q);
        } else {
            startLink(s, style, target);
            str::append(s, text);
            finishLink(s);
        }
    }

    constexpr int SIZE_SAMPLE = -1;

    template <class Ch>
    void appendNormalFont(QString& s, const uc::Font& font, std::basic_string_view<Ch> x, int size)
    {
        if (x.empty())
            return;
        s += "<font face='";
        s += font.familiesComma();
        s += '\'';
        if (size != 0) {
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
    }

    const uc::Font* getExtendedFont(const uc::Font* font, char32_t c)
    {
        for (; font->flags.have(uc::Ffg::FALL_TO_NEXT); ++font) {
            if (font->flags.have(uc::Ffg::DESC_AVOID))
                continue;
            if (font->doesSupportChar(c))
                return font;
        }
        return font;
    }

    void appendExtendedFont(QString& s, const uc::Font& font, std::string_view x, int size)
    {
        if (x.empty())
            return;
        auto cps = mojibake::toQ<std::u32string>(x);
        // Shouldn’t really
        if (cps.empty())
            return;
        size_t iWritten = 0;
        const uc::Font* workingFont = nullptr;
        for (size_t i = 0; i < cps.length(); ++i) {
            auto newFont = getExtendedFont(&font, cps[i]);
            if (newFont != workingFont) {
                std::u32string_view part = std::u32string_view{cps}.substr(iWritten, i - iWritten);
                appendNormalFont(s, *workingFont, part, size);
                iWritten = i;
                workingFont = newFont;
            }
        }
        if (workingFont) {  // Should always really
            std::u32string_view tail = std::u32string_view{cps}.substr(iWritten);
            appendNormalFont(s, *workingFont, tail, size);
        }
    }

    void appendFont(QString& s, const uc::Font& font, std::string_view x, int size)
    {
        if (font.flags.have(uc::Ffg::DESC_EXTENDED)) {
            // Extended font
            appendExtendedFont(s, font, x, size);
        } else {
            appendNormalFont(s, font, x, size);
        }
    }

    void appendFont(QString& s, uc::EcFont fontId, std::string_view x, int size)
    {
        auto& font = uc::fontInfo[static_cast<int>(fontId)];
        appendFont(s, font, x, size);
    }

    template <class Font>
    inline void appendFont(QString& s, Font&& fontId,
                    Buf1d<const std::string_view> x, int size)
    {
        appendFont(s, std::forward<Font>(fontId), x.safeGetV(1, {}), size);
    }

    void appendSmTable(
            QString& s,
            Buf1d<const std::string_view> x,
            const mywiki::Context& context)
    {
        // How smtable works:
        // • ∞ params
        // • nbsp between params unless both are samples
        // • *xxx = sample: draw in sample font
        // • **xxx = glitching sample: start new cell, draw in sample font
        // EXAMPLE: {{smtable|ka|*KA|+ i|**_I|= ki|*KI}}
        //   ka [KA] + i|[_I] = ki [KI]
        //   [xxx] = font, a|b = new cell
        s += "<table cellspacing=0 cellpadding=0><tr valign='middle'><td>&nbsp;&nbsp;&nbsp;";
        auto n = x.size();
        for (size_t i = 1; i < n; ++i) {
            auto v = x[i];
            if (v.starts_with('*')) {
                if (v.starts_with("**")) {  // Glitching sample
                    s += "<td>";
                    auto w = v.substr(2);
                    appendFont(s, *context.font, w, SIZE_SAMPLE);
                } else {    // Normal sample
                    auto w = v.substr(1);
                    appendFont(s, *context.font, w, SIZE_SAMPLE);
                }
            } else {
                if (i > 1)
                    s += "&nbsp;";
                auto u8 = str::toU8(v);
                str::replace(u8, u8' ', u8"&nbsp;");
                mywiki::append(s, u8, context, wiki::Mode::SPAN);
                if (i + 1 < n)
                    s += "&nbsp;";
            }
        }
        s += "</table>";
    }

    bool shouldWriteLocations(const uc::Lang& lang)
    {
        if (lang.locations.areEmpty())
            return false;
        if (lang.flags.have(uc::Langfg::BURMESE))
            return loc::currLang->peculiarities.stillUsesBurmese;
        return true;
    }

    QString formatNumOnly(unsigned mantissa, const uc::NumOrderInfo& nii)
    {
        static constexpr bool NO_TRUNCATE = false;

        QString s = QString::number(mantissa);
        if (nii.nDigs > 0) {
            s = s.rightJustified(nii.nDigs + 1, '0', NO_TRUNCATE);
            int whereIns = s.length() - nii.nDigs;
            s.insert(whereIns, loc::currLang->numfmt.decimalPoint);
        }
        return s;
    }

    template <class... Args>
    void wrapWith(QString& s, std::string_view locKey, Args&&... args)
    {
        s = loc::get(locKey).argQ(
                str::toU8sv(s.toStdString()),
                std::forward<Args>(args)...);
    }

    // In Tahoma: 1px normal inter-char, 2px too bad, 3px some-what-OK, 4px normal space
    constexpr std::u8string_view NNBSP_RAW = u8"\u202F"sv;
    // As Qt heavily modifies HTML, that’s for HTML versions (it does not increase inter-line)
    constexpr std::u8string_view SMALL_NBSP_HT = u8"<sub>&nbsp;</sub>"sv;  // HTML view is still funky

    /// @brief  Number’s subformat
    enum class Subf : unsigned char {
        STANDALONE,     ///< standalone number, key: 1'234
        DENSE,          ///< number in dense text, the key is 1234
        HEX             ///< hex number, 89'ABCD
    };

    std::u8string finishFormattingNum(std::string_view x,
                Subf subformat, mywiki::NumPlace place)
    {
        // Retrieve thousand separator
        std::u8string_view thouSep;
        char8_t data[4];
        if (loc::active::numfmt.thousandPoint == ' ') {
            if (place == mywiki::NumPlace::HTML) {
                thouSep = SMALL_NBSP_HT;
            } else {
                thouSep = NNBSP_RAW;
            }
        } else {
            std::u16string_view sv { &loc::active::numfmt.thousandPoint, 1 };
            auto end = mojibake::copyLimS(sv, data, std::end(data));
            thouSep = std::u8string_view{ data, end };
        }
        // Retrieve other features: min. length and period
        unsigned minLength = loc::active::numfmt.thousand.minLength;
        unsigned period = loc::Lang::Numfmt::Thousand::DEFAULT_PERIOD;
        switch (subformat) {
        case Subf::STANDALONE: break;
        case Subf::DENSE: minLength = loc::active::numfmt.denseThousand.minLength; break;
        case Subf::HEX: minLength = 6; period = 4; break;
        }

        const char* src = x.data();
        const char* end = x.data() + x.size();
        std::u8string dest;
        dest.reserve(end - src);
        // Minus
        if (src != end && !std::isalnum(*src)) {
            dest += *(src++);
        }
        // The rest
        auto length = end - src;
        if (length < minLength || length <= period) {
            // For fixed RVO: dest will be in place
            while (src < end)
                dest += *(src++);
            return dest;
        }
        // Formatting
        while (src < end) {
            dest += *(src++);
            --length;
            if (length != 0 && length % period == 0) {
                dest += thouSep;
            }
        }
        return dest;
    }

    template <std::integral T>
    std::u8string formatNum(T x, Subf subformat, mywiki::NumPlace place)
    {
        char tmp[30];
        auto q = std::to_chars(std::begin(tmp), std::end(tmp), x);
        const char* end = q.ptr;
        if (!end || end <= tmp)
            throw std::logic_error("Strange end");
        std::string_view sv { tmp, end };
        return finishFormattingNum(sv, subformat, place);
    }

    template <std::integral T>
    void appendNum(QString& r, T x,
                   Subf subformat,
                   mywiki::NumPlace place)
    {
        auto q = formatNum(x, subformat, place);
        r.append(str::toSv(q).data());
    }

    void Eng::appendNSpeakers(const TextLang& x, Flags<Nspkf> fgs)
    {
        char locBuf[40];
        auto lang = context.lang;   // primary language
        std::string_view locPrefixDot = context.locPrefixDot;
        if (x.key.empty()) {
            hasNSpeakers = true;
        } else {
            lang = nullptr;
            locPrefixDot = {};
            // Key points to script?
            if (x.key.size() == 4 && (lat::isUpper(x.key[0]))) {
                if (auto sc = uc::findScript(x.key)) {
                    if (sc->mainLang) {
                        lang = &sc->mainLang;
                        sc->printfLocKey(locBuf, "");
                        locPrefixDot = locBuf;
                    }
                }
            } else {
                // No place for L10n kinks for side-languages
                lang = uc::findSideLang(x.key);
            }
        }
        if (fgs.have(Nspkf::ITALIC_PARENS)) {
            s += "<i>";
            str::append(s, loc::active::punctuation.leftParen);
        }
        if (lang) {
            bool wasWritten = false;
            // Pre-comment from L10n
            if (!x.preComment.empty()) {
                mywiki::append(s, str::toU8sv(x.preComment), context, wiki::Mode::SPAN);
                wasWritten = true;
            }
            // Pre-comment from data
            if (lang->flags.have(uc::Langfg::CUSTOM_PRENOTE)) {
                if (wasWritten)
                    str::append(s, loc::active::punctuation.uniformComma);
                if (locPrefixDot.empty()) {
                    s += "[NO L10N PREFIX]";
                } else {
                    auto locKey = str::cat(locPrefixDot, "LangNote");
                    s += loc::get(locKey);
                }
                wasWritten = true;
            }
            // Language info
            if (lang->hasValue()) {
                // Locations
                if (fgs.have(Nspkf::LOCATION) && shouldWriteLocations(*lang)) {
                    if (wasWritten) {
                        str::append(s, loc::active::punctuation.semicolon);
                    }
                    for (auto& v : lang->locations) {
                        auto locKey = str::cat("Prop.LangLoc.", v.locSubKey);
                        s += loc::get(locKey);
                        str::append(s, loc::active::punctuation.uniformComma);
                    }
                } else {
                    if (wasWritten) {
                        str::append(s, loc::active::punctuation.uniformComma);
                    }
                }
                // # of speakers
                QString sNum;
                auto& nii = uc::numOrderInfo[lang->numOrder];
                sNum = formatNumOnly(lang->mantissa, nii);
                if (lang->hiMantissa) {
                    sNum += "–";  // en dash
                    sNum += formatNumOnly(lang->hiMantissa, nii);
                }
                wrapWith(sNum, nii.locKey);
                if (lang->flags.have(uc::Langfg::GREATER_THAN)) {
                    sNum = "&gt;" + sNum;
                } else if (lang->flags.have(uc::Langfg::LESS_THAN)) {
                    sNum = "&lt;" + sNum;
                }
                if (lang->year != 0) {
                    std::u8string sYear = str::toU8(std::to_string(lang->year));
                    if (lang->flags.have(uc::Langfg::DECADE))
                        sYear = loc::get("Prop.Lang.Decade").arg(sYear);
                    const char* key = "Prop.Lang.Qty";
                    if (lang->flags.have(uc::Langfg::AS_NATIVE)) {
                        key = "Prop.Lang.QtyNative";
                    } else if (lang->flags.have(uc::Langfg::IN_INDIA)) {
                        key = "Prop.Lang.QtyIndia";
                    }
                    wrapWith(sNum, key, sYear);
                }
                s += sNum;
            }
        } else {
            s += "[NO LANGUAGE!!!]";
        }
        // End
        if (fgs.have(Nspkf::ITALIC_PARENS)) {
            str::append(s, loc::active::punctuation.rightParen);
            s += "</i>";
        }
    }

    void Eng::appendNSpeakers(Buf1d<const std::string_view> x, Flags<Nspkf> fgs)
    {
        TextLang textLang {
            // 0 = template name
            .key        = x.safeGetV(1, {}),
            .preComment = x.safeGetV(2, {}),
        };
        appendNSpeakers(textLang, fgs);
    }

    // Key: start/end
    constinit const std::u8string_view KEY_START =
            u8"<span style='background-color:palette(midlight);'>\u00A0";
    constinit const std::u8string_view KEY_END = u8"\u00A0</span>";

    void Eng::appendTemplate(Buf1d<const std::string_view> x, bool)
    {
        auto name = x[0];
        if (name.empty())
            return;
        if (loc::currLang) {
            auto& wt = loc::currLang->wikiTemplates;
            if (auto it = wt.find(name); it != wt.end()) {
                str::append(s, it->second);
                return;
            }
        }
        switch (name[0]) {
        case '%':
            if (name == "%"sv) {
                str::append(s, x.safeGetV(1, {}));
                str::append(s, "<span style='font-size:3pt'>\u00A0</span>%"sv);
            } else {
                goto dflt;
            }
            break;

        case '_':
            if (name == "_"sv) {
                    s.append(QChar(0x00A0));
            } else {
                goto dflt;
            }
            break;

        case 'D':
            if (name == "DuplCats") {
                uc::fontInfo[static_cast<int>(uc::EcFont::FUNKY)].load(NO_TRIGGER);
                appendFont(s, uc::EcFont::FUNKY, "<span style='font-size:40pt'>&#xE00F;</span>", 0);
            } else {
                goto dflt;
            }
            break;

        case 'e':
            if (name == "em"sv) {
                str::append(s, "<font size='+2' face='Segoe UI Emoji,Noto Sans Symbols,Noto Sans Symbols2'>"sv);
                str::append(s, x.safeGetV(1, {}));
                str::append(s, "</font>");
            } else {
                goto dflt;
            }
            break;

        case 'f':
            if (name == "fontface"sv) {
                    s += context.font->familiesComma();
            } else if (name == "funky"sv) {
                appendFont(s, uc::EcFont::FUNKY, x, 0);
            } else {
                goto dflt;
            }
            break;

        case 'h':
            if (name == "h"sv) {
                auto formatted = finishFormattingNum(
                        x.safeGetV(1, {}), Subf::HEX, mywiki::NumPlace::HTML);
                str::append(s, formatted);
            } else {
                goto dflt;
            }
            break;

        case 'k':
            if (name == "k"sv) {
                for (size_t i = 1; i < x.size(); ++i) {
                    if (i != 1)
                        s += '+';
                    str::append(s, KEY_START);
                    mywiki::append(s, str::toU8sv(x[i]), context, wiki::Mode::SPAN);
                    str::append(s, KEY_END);
                }
            } else if (name == "kb"sv) {
                for (size_t i = 1; i < x.size(); ++i) {
                    if (i != 1)
                        s += '+';
                    str::append(s, KEY_START);
                    s += "<b>";
                    mywiki::append(s, str::toU8sv(x[i]), context, wiki::Mode::SPAN);
                    s += "</b>";
                    str::append(s, KEY_END);
                }
            } else {
                goto dflt;
            }
            break;

        case 'n':
            if (name == "n"sv) {
                auto formatted = finishFormattingNum(
                        x.safeGetV(1, {}), Subf::DENSE, mywiki::NumPlace::HTML);
                str::append(s, formatted);
            } else if (name == "nspk"sv) {
                appendNSpeakers(x, Nspkf::DFLT);
            } else if (name == "nspk1"sv) {
                appendNSpeakers(x, NO_FLAGS);
            } else if (name == "nchars"sv) {
                appendNum(s, uc::N_CPS, Subf::DENSE, mywiki::NumPlace::HTML);
            } else if (name == "nemoji"sv) {
                auto nEmoji = uc::versionInfo[static_cast<int>(uc::EcVersion::LAST)].stats.emoji.nTotal;
                appendNum(s, nEmoji, Subf::DENSE, mywiki::NumPlace::HTML);
            } else if (name == "noto"sv) {
                appendFont(s, uc::EcFont::NOTO, x, 0);
            } else {
                goto dflt;
            }
            break;

        case 's':
            if (name == "sm"sv) {
                appendFont(s, context.font, x, SIZE_SAMPLE);
            } else if (name == "smb"sv) {
                /// @todo [future] Plane 2 in text?
                appendFont(s, uc::EcFont::CJK, x, 3);
            } else if (name == "smfunky"sv) {
                appendFont(s, uc::EcFont::FUNKY, x, SIZE_SAMPLE);
            } else if (name == "smtable"sv) {
                appendSmTable(s, x, context);
            } else {
                goto dflt;
            }
            break;

        case 't':
            if (name == "t"sv) {
                str::append(s, u8"<span class='tr'>⌈</span>");
                str::append(s, x.safeGetV(1, {}));
                str::append(s, u8"<span class='tr'>⌋</span>");
            } else {
                goto dflt;
            }
            break;

        case 'v':
            if (name == "version"sv) {
                str::append(s, uc::versionInfo[static_cast<int>(uc::EcVersion::LAST)].locName());
            } else {
                goto dflt;
            }
            break;

        case 'w':
            if (name == "warn") {
                s += "<b style='"  STYLE_MISRENDER "'>";
                str::append(s, x.safeGetV(1, {}));
                s += "</b>";
            } else {
                goto dflt;
            }
            break;

        default:
        dflt:
            if (name == "GrekCoptUni"sv
             || name == "ArabPres1"sv
             || name == "ArabPres2"sv) {
                mywiki::append(s, loc::get(str::cat("Snip.", name)), context, wiki::Mode::SPAN);
            } else {
                wiki::appendHtml(s, x[0]);
            }
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

    void appendQuery(QString& text, std::string_view schema, std::string_view params)
    {
        auto& font = uc::fontInfo[(int)uc::EcFont::FUNKY];
        auto names = font.familiesComma().toStdString();
        text += loc::Fmt(
                "&nbsp;&nbsp;<a href='{1}:{2}' class='query' style='font-family: {3};'>&#{4};</a>")
                       (schema)(params)(names)
                       ((int)uc::STUB_PUA_ZOOM.unicode()).q();
    }

    template <class... T>
    struct SpecText {
        std::string_view id {};
        std::tuple<T...> params;

        std::string_view idOr(std::string_view x) const
            { return id.empty() ? x : id; }
    };

    template <class T, class... U>
    inline void appendHeader(QString& text, const T& x,
                             std::string_view qry = {},
                             const SpecText<U...>& specText = {})
    {
        str::append(text, "<p><nobr>"sv);
        std::u8string boldHead = str::cat(u8"<b>"sv, locName(x), u8"</b>"sv);
        auto fmt = formatNum(x.nChars, Subf::DENSE, mywiki::NumPlace::HTML);
        loc::PreformN pref{ std::u8string_view{fmt}, x.nChars };
        text += loc::get(specText.idOr("Prop.Head.NChars2")).argQ(
                    boldHead, pref, specText.params);
        if (!qry.empty()) {
            appendQuery(text, SCH_QRY_CHARS, qry);
        }
        str::append(text, "</nobr></p>"sv);
    }

    template <class X>
    inline const uc::Font& getFont(const X& obj)
        { return obj.font(); }

    template <>
    inline const uc::Font& getFont<uc::Block>(const uc::Block& obj)
    {
        switch (obj.startingCp) {
        case 0x2150:
            return uc::fontInfo[static_cast<int>(uc::EcFont::NUMBER_FORMS)];
        default:
            return obj.font();
        }
    }

    template <class X>
    void appendWiki(QString& text, const X& obj, std::u8string_view x, wiki::Mode mode)
    {
        mywiki::Context context {
            .font { getFont(obj) },
            .lang = nullptr,
            .locPrefixDot {},
        };
        Eng eng(text, context);
        wiki::run(eng, x, mode);
    }

}   // anon namespace


void mywiki::appendNoFont(QString& text, std::u8string_view wiki, wiki::Mode mode)
{
    mywiki::Context context {
        .font { uc::fontInfo[0] },
        .lang = nullptr,
        .locPrefixDot {},
    };
    Eng eng(text, context);
    wiki::run(eng, wiki, mode);
}


mywiki::AppendWiki mywiki::append(
        QString& text, std::u8string_view wiki, const Context& context,
        wiki::Mode mode)
{
    Eng eng(text, context);
    wiki::run(eng, wiki, mode);
    return {
        .hasNSpeakers = eng.hasNSpeakers,
    };
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

void mywiki::appendVersionValue(QString& text, uc::EcVersion version)
{
    appendVersionValue(text, uc::versionInfo[static_cast<int>(version)]);
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
        auto key = version.otherEmojiDate ? "Prop.Bullet.EmojiVA" : "Prop.Bullet.EmojiV2";
        str::append(text, loc::get(key).arg(version.unicodeName));
    }

    // Date
    char buf[30];
    auto& date = version.emojiDate();
    snprintf(buf, std::size(buf), "Common.Mon.%d", static_cast<int>(date.month));
    auto& monTemplate = loc::get(buf);
    auto s = monTemplate.arg(date.year);
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
        str::append(text, loc::active::punctuation.keyValueColon);
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
    appendHeader(text, x, str::cat("b=", x.id));

    str::append(text, "<p>");
    str::QSep sp(text, "<br>");

    sp.sep();
    appendBullet(text, "Prop.Bullet.InTech");
    str::append(text, x.tech);

    str::append(text, "</p>");

    appendNoFont(text, x.loc.description, wiki::Mode::ARTICLE);
    return text;
}


namespace {

    std::string catQuery(const uc::Category& x)
        { return str::cat("c="sv, x.id); }

    inline std::string catQuery(const uc::EcCategory& x)
        { return catQuery(uc::categoryInfo[static_cast<int>(x)]); }

}   // anon namespace


QString mywiki::buildHtml(const uc::Category& x)
{
    QString text;
    appendStylesheet(text);
    appendHeader(text, x, catQuery(x));
    appendNoFont(text, x.loc.description, wiki::Mode::ARTICLE);
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

    auto fonts = gui.fontSource().allSysFonts(cp, ws, 20);
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
    char buf[40];
    x.printfLocKey(buf, "");
    mywiki::Context context {
        .font = x.font(),
        .lang = x.mainLang ? &x.mainLang : nullptr,
        .locPrefixDot = buf,
    };
    if (x.ecType != uc::EcScriptType::NONE) {
        str::append(text, "<p>");
        str::QSep sp(text, "<br>");
        appendBullet(text, "Prop.Bullet.Type");
        appendWiki(text, x, loc::get(x.type().locKey), wiki::Mode::SPAN);
        if (x.ecDir != uc::EcWritingDir::NOMATTER) {
            sp.sep();
            appendBullet(text, "Prop.Bullet.Dir");
            append(text, loc::get(x.dir().locKey), context, wiki::Mode::SPAN);
        }
        if (!x.flags.have(uc::Sfg::NO_LANGS)) {
            sp.sep();
            appendBullet(text, "Prop.Bullet.Langs");
            auto info = append(text, x.loc.langs, context, wiki::Mode::SPAN);
            if (!info.hasNSpeakers && context.lang
                    && !context.lang->flags.have(uc::Langfg::NO_AUTO)) {
                text += ' ';
                Eng eng(text, context);
                eng.appendNSpeakers(TextLang::DFLT, Nspkf::DFLT);
            }
        }
        if (x.time) {
            sp.sep();
            appendBullet(text, "Prop.Bullet.Appear");
            auto wikiTime = x.time.wikiText(myDatingLoc, x.loc.timeComment);
            append(text, wikiTime, context, wiki::Mode::SPAN);
        }
        if (x.ecLife != uc::EcLangLife::NOMATTER) {
            sp.sep();
            appendBullet(text, "Prop.Bullet.Condition", "<a href='pt:status' class='popup'>", "</a>");
            append(text, loc::get(x.life().locKey), context, wiki::Mode::SPAN);
        }
        if (isScript) {
            if (x.ecVersion != uc::EcVersion::TOO_HIGH) {
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

    append(text, x.loc.description, context, wiki::Mode::ARTICLE);
}


QString mywiki::buildHtml(const uc::Script& x)
{
    QString r;
    SpecText<std::u8string_view> spec;
    if (x.id == "Hira"sv) {
        spec.id = "Prop.Head.NoHent3";
        spec.params = std::make_tuple(u8"href='ps:Hent' class='popup'"sv);
    }
    appendStylesheet(r);
    appendHeader(r, x, str::cat("s="sv, x.id), spec);
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


template <class Str>
void mywiki::appendCopyable(Str& text, unsigned x, std::string_view clazz)
{
    char c[40];
    snprintf(c, std::size(c), "%u", x);
    auto q = formatNum(x, Subf::STANDALONE, mywiki::NumPlace::HTML);
    appendCopyableHt(text, c, q.c_str(), clazz);
}


void mywiki::appendCopyableHt(
        QString& text, std::string_view toCopy,
        std::u8string_view toView, std::string_view clazz)
{
    str::append(text, "<a href='c:"sv);
        str::append(text, toCopy);
        str::append(text, "' class='"sv);
        str::append(text, clazz);
        str::append(text, "' >"sv);
    str::append(text, toView);
    str::append(text, "</a>"sv);
}


void mywiki::appendCopyableHt(
        std::u8string& text, std::string_view toCopy,
        std::u8string_view toView, std::string_view clazz)
{
    str::append(text,
        u8"<a href='c:"sv, str::toU8sv(toCopy), u8"' class='"sv,
            str::toU8sv(clazz), u8"' >"sv,
            toView, u8"</a>"sv);
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


QString mywiki::toString(const uc::Numeric& numc, mywiki::NumPlace place)
{
    QString buf;
    switch (numc.fracType()) {
    case uc::FracType::NONE:        // should not happen
    case uc::FracType::INTEGER:
        appendNum(buf, numc.num, Subf::STANDALONE, place);
        break;
    case uc::FracType::VULGAR:
        appendNum(buf, numc.num, Subf::STANDALONE, place);
        str::append(buf, '/');
        appendNum(buf, numc.denom, Subf::STANDALONE, place);
        break;
    case uc::FracType::DECIMAL: {
            auto val = static_cast<double>(numc.num) / numc.denom;
            buf = QString::number(val);
            buf.replace('.', QChar{loc::active::numfmt.decimalPoint});
        } break;
    }
    if (numc.altInt != 0) {
        auto q = formatNum(numc.altInt, Subf::STANDALONE, place);
        buf = str::toQ(loc::get("Prop.Num.Or").arg(str::toU8(buf), q));
    }
    return buf;
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
        cp.traverseTextsT(uc::AutoName::NO,     // Need HTML only
            [&text](uc::TextRole role, std::u8string_view s) {
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
                auto mnemonic = cp->getText(uc::TextRole::HTML);
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

    enum class CpSerializations : unsigned char { NO, YES };

    void appendCpAltNames(QString& text, const uc::Cp& cp)
    {
        // Alt. names
        bool isInitial = true;
        cp.traverseTextsT(uc::AutoName::NO,
            [&text,&isInitial](uc::TextRole role, std::u8string_view s) {
                switch (role) {
                case uc::TextRole::ALT_NAME:
                case uc::TextRole::ABBREV:
                case uc::TextRole::EMOJI_NAME:
                /// @todo [future] Need “description in Wikipedia”, “description in Unicode”…
                case uc::TextRole::EGYP_EWP:
                case uc::TextRole::EGYP_UC:
                    if (isInitial) {
                        isInitial = false;
                        text += "<p style='" CNAME_ALTNAME "'>";
                    } else {
                        text += EURO_SEMICOLON;
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

    void appendMisrender(QString& text, std::u32string_view value, uc::Lfgs mask)
    {
        if (value.empty())
            return;

        char buf[40];
        const char* key = buf;

        // Get L10n key
        switch (mask.numeric()) {
        case uc::MISRENDER_FAMILY.numeric():
            key = "Lib.Misr.Family";
            break;
        case uc::MISRENDER_IO.numeric():
            key = "Lib.Misr.IO";
            break;
        default: {
                auto end = buf + std::size(buf);
                static constinit const std::string_view HEAD = "Lib.Misr.U";
                auto p = std::copy(HEAD.begin(), HEAD.end(), buf);
                for (auto v : value) {
                    ptrdiff_t remder = end - p;
                    if (remder > 1) {
                        auto nPrinted = snprintf(p, remder, "+%04X", static_cast<int>(v));
                        p += nPrinted;
                    }
                }
            }
        }

        auto& desc = loc::get(key);
        auto paragraph = loc::get("Lib.Misr.Head").arg(desc);

        mywiki::appendNoFont(text, paragraph, wiki::Mode::ARTICLE);
    }

    enum class CpPlace : unsigned char { CP, LIB  };

    void appendHaniRadical(QString& text, const uc::Cp::ScriptSpecific& sspec)
    {
        text += "<a href='pk:2F00' class='popup'>";
        text += loc::get("Prop.Kx.Bullet");
        text += "</a>";
        str::append(text, loc::active::punctuation.keyValueColon);
        char16_t sRad[] = u"?";
        sRad[0] = cp::KANGXI_DELTA + sspec.haniRadical();
        auto& font = uc::fontInfo[static_cast<int>(uc::EcFont::CJK_UHAN)];
        auto fontFace = font.familiesComma();
        auto sFullRad = loc::Fmt(u8"<font size='+2' face='{2}'><b>{1}</b></font>")
                       (mojibake::toQ<std::u8string>(sRad))
                       (str::toU8sv(fontFace.toStdString())).str();
        text += loc::get("Prop.Kx.Data").argQ(
                    sFullRad,
                    (sspec.haniPlusStrokes() >= 0) ? u8"+" : u8"−",
                    std::abs(sspec.haniPlusStrokes()));
    }

    /// @param [in] serializations  [+] write UTF-8, HTML etc
    ///
    void appendCpBullets(QString& text, const uc::Cp& cp,
                         CpSerializations serializations, CpPlace place)
    {
        str::append(text, "<p>");
        str::QSep sp(text, "<br>");

        // Script
        sp.sep();
        auto& scr = cp.script();
        appendValuePopup(text, scr, "Prop.Bullet.Script", "ps");

        // Kangxi
        if (cp.scriptSpecific) {
            switch (cp.ecScript) {
            case uc::EcScript::Hani:
                sp.sep();
                appendHaniRadical(text, cp.scriptSpecific);
                break;
            default: ;
            }
        }

        // Unicode version
        sp.sep();
        mywiki::appendVersion(text, {}, cp.version());

        // Character type
        sp.sep();
        appendValuePopup(text, cp.category(), "Prop.Bullet.Type", "pc");

        // Numeric value        
        if (auto& numc = cp.numeric(); numc.isPresent()) {
            sp.sep();
            appendNonBullet(text, numc.type().locKey,
                    "<a href='pt:number' class='popup'>", "</a>");
            text += mywiki::toString(numc, mywiki::NumPlace::HTML);
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
            static constexpr std::string_view KEY = "Prop.Bullet.Computers";
            if (cp.block().startingCp == 0x1FB00 && place == CpPlace::CP) {
                appendNonBullet(text, KEY);
            } else {
                appendNonBullet(text, KEY, "<a href='pk:1FB00' class='popup'>", "</a>");
            }
            str::QSep spC(text, loc::active::punctuation.uniformComma);
            while (comps) {
                // Extract and remove bit
                auto bit = comps.smallest();
                comps.remove(bit);
                // Turn bit to index
                auto iBit = std::countr_zero(Flags<uc::OldComp>::toUnsignedStorage(bit));
                // Write what we got
                spC.sep();
                auto& oci = uc::old::info[iBit];
                text += "<a class='popup' href='po:";
                    str::append(text, oci.key);
                    text += "'>";
                str::append(text, oci.locName());
                text += "</a>";
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
                                      .arg(im.sometimesKey),
                                wiki::Mode::SPAN);
                }
                if (im.hasAltCode()) {
                    sp1.sep();
                    text += "<a href='pt:altcode' class='popup'>";
                    str::append(text, loc::get("Prop.Input.AltCode"));
                    text += "</a> ";
                    str::QSep sp2(text, EURO_COMMA);    // alt codes
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
                        if (im.alt.dosEn
                                && im.alt.dosRu == im.alt.dosEn
                                && im.alt.dosEl == im.alt.dosEn) {
                            sp2.sep();
                            str::append(text, static_cast<int>(im.alt.dosEn));
                            str::append(text, u8" (en/ru/el)");
                        } else {
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
                            if (im.alt.dosEl) {
                                sp2.sep();
                                str::append(text, static_cast<int>(im.alt.dosEl));
                                str::append(text, u8" (el)");
                            }
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

        std::u8string_view instead = cp.getText(uc::TextRole::DEP_INSTEAD);
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
                std::u8string_view instead2 = cp.getText(uc::TextRole::DEP_INSTEAD2);
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

    // Misrender
    if (cp.flags.have(uc::Cfg::G_MISRENDER)) {
        char32_t s[1] { cp.subj };
        appendMisrender(text, { s, 1 }, uc::MISRENDER_SIMPLE);
    }

    appendSgnwVariants(text, sw);

    {   // Info box
        appendCpBullets(text, cp, CpSerializations::YES, CpPlace::CP);

        auto& blk = cp.block();
        if (blk.startingCp == 0) {
            // Basic Latin:
            // Control → t:control (stored in catInfo)
            // Latn → s:Latn
            // Others → t:ASCII, stored here for simplicity
            if (cp.ecCategory == uc::EcCategory::CONTROL) {
                //  Control char description
                appendSubhead(text, "Prop.Head.Control");
                appendWiki(text, blk,
                           uc::categoryInfo[static_cast<int>(uc::EcCategory::CONTROL)].loc.description,
                           wiki::Mode::ARTICLE);
            } else if (auto& sc = cp.script(); &sc != uc::scriptInfo) {
                // Script description
                appendScriptSubhead(text);
                mywiki::appendHtml(text, sc, false);
            } else {
                // Script description
                appendBlockSubhead(text);
                appendWiki(text, blk, blk.loc.description, wiki::Mode::ARTICLE);
            }
        } else {
            if (blk.hasDescription()) {
                // Block description
                appendBlockSubhead(text);
                appendWiki(text, blk, blk.loc.description, wiki::Mode::ARTICLE);
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


QString mywiki::buildVacantCpHtml(char32_t code, const QColor& color)
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
    appendWiki(text, *uc::blockOf(code), loc::get("Term.noncharacter.Text"), wiki::Mode::ARTICLE);
    return text;
}


bool mywiki::isEngTermShown(const uc::Term& term)
{
    switch (loc::active::engTerms) {
    case loc::EngTerms::OFF:
        return false;
    case loc::EngTerms::NORMAL:
        if (term.fgs.have(uc::Tfg::SORT_KEY))
            return false;
        [[fallthrough]];
    case loc::EngTerms::SORT_KEY:
        return !term.engName.empty()            // and English name present
            && (term.engName != term.loc.name); // and different from L10n name
    }
    __builtin_unreachable();
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

    // We always search by 1 object
    if (x.search.ecCategory != uc::EcCategory::NO_VALUE) {
        appendQuery(text, SCH_QRY_CHARS, catQuery(x.search.ecCategory));
    } else if (x.search.ecUpCat != uc::EcUpCategory::NO_VALUE) {
        appendQuery(text, SCH_QRY_CHARS, str::cat("u=", upCatNames[x.search.ecUpCat]));
    } else if (x.search.fgs) {
        char s[20];
        snprintf(s, std::size(s), "f=%u", static_cast<unsigned>(x.search.fgs.numeric()));
        appendQuery(text, SCH_QRY_CHARS, s);
    } else if (x.search.isNumber) {
        appendQuery(text, SCH_QRY_CHARS, "N=1");
    }

    appendWiki(text, x, x.loc.description, wiki::Mode::ARTICLE);
    return text;
}


namespace {
    const char8_t* STR_RANGE = u8"%04X…%04X";
    const char8_t* STR_RESIZE = u8" %04X → %04X";
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
    appendCopyable(text, str::toQ(x.name));

    sp.sep();
    appendBullet(text, "Prop.Bullet.Range");
    char buf[30];
    snprintf(buf, std::size(buf), reinterpret_cast<const char*>(STR_RANGE),
                int(x.startingCp), int(x.endingCp));
    appendCopyable(text, buf);

    sp.sep();
    appendBullet(text, "Prop.Bullet.VerAppear");
    mywiki::appendVersionValue(text, x.version());

    // When extended/shrunk
    x.resizeHistoryT([&](uc::EcVersion when, char32_t bef, char32_t aft) {
        sp.sep();
        appendBullet(text, (bef < aft)
                            ? "Prop.Bullet.Extend" : "Prop.Bullet.Shrunk");
        mywiki::appendVersionValue(text, when);
        snprintf(buf, std::size(buf), reinterpret_cast<const char*>(STR_RESIZE),
                    int(x.history.oldEndingCp), int(x.endingCp));
        str::append(text, buf);
    });

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
        appendWiki(text, x, x.loc.description, wiki::Mode::ARTICLE);
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


namespace {

    ///
    /// @brief appendNodesTextIf
    ///    Appends node’s text if it’s present
    ///
    void appendNodesTextIf(QString& text, const uc::LibNode& node)
    {
        if (node.flags.have(uc::Lfg::HAS_TEXT)) {
            char buf[40];
            snprintf(buf, std::size(buf), "Lib.Text.%s",
                     reinterpret_cast<const char*>(node.text.data()));
            mywiki::appendNoFont(text, loc::get(buf), wiki::Mode::ARTICLE);
        }
    }

    ///
    /// @return  the only independent codepoint, by category().isIndependent()
    /// @warning  Unallocated codepoints (incl. surrogate and too high) are skipped
    ///
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
                    if (cat.isIndependent != uc::Independent::NO) {
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

    cou::TwoLetters countryLetters(std::u32string_view x)
    {
        static constexpr int DELTA = cp::FLAG_A - 'A';

        if (x.length() == 2
                && x[0] >= cp::FLAG_A && x[0] <= cp::FLAG_Z
                && x[1] >= cp::FLAG_A && x[1] <= cp::FLAG_Z) {
            return cou::TwoLetters(x[0] - DELTA, x[1] - DELTA);
        }
        return {};
    }

}   // anon namespace


QString mywiki::buildLibFolderHtml(const uc::LibNode& node, const QColor& color)
{
    QString text;
    appendStylesheet(text);
    text += "<h1 style='color:" + color.name() + "'>";
    text += node.viewableTitle(uc::TitleMode::LONG).toHtmlEscaped();
    text += "</h1>";

    appendNodesTextIf(text, node);

    return text;
}


QString mywiki::buildHtml(const uc::LibNode& node, const uc::LibNode& parent)
{
    QString text;
    appendStylesheet(text);
    text += "<h1>";
        auto title = node.viewableTitle(uc::TitleMode::LONG);
    appendCopyable(text, title, "bigcopy");
    text += "</h1>";

    if (auto getcp = EmojiPainter::getCp(node.value)) {
        if (auto pCp = uc::cpsByCode[getcp.cp]) {
            auto title1 = str::toU8(title);
            bool isInitial = true;
            pCp->traverseTextsT(uc::AutoName::NO,  // Main name should not be template
                [&text, &isInitial, &title1](uc::TextRole role, std::u8string_view s) {
                    switch (role) {
                    case uc::TextRole::MAIN_NAME:
                    case uc::TextRole::ALT_NAME:
                    case uc::TextRole::ABBREV:
                        if (s != title1) {
                            if (isInitial) {
                                isInitial = false;
                                text += "<p style='" CNAME_ALTNAME "'>";
                            } else {
                                text += EURO_SEMICOLON;  // names/abbreviations are euro
                            }
                            mywiki::appendCopyable(text, str::toQ(s), "altname");
                        } break;
                    case uc::TextRole::HTML:
                    case uc::TextRole::DEP_INSTEAD:
                    case uc::TextRole::DEP_INSTEAD2:
                    case uc::TextRole::CMD_END:
                    case uc::TextRole::EGYP_EWP:
                    case uc::TextRole::EGYP_UC:
                    case uc::TextRole::EMOJI_NAME:  // Equal to mine
                        break;
                    }
                });
        }
    }

    if (auto q = node.flags & uc::MISRENDER_MASK) {
        appendMisrender(text, node.value, q);
    }

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

    if (node.ecEmojiVersion != uc::EcVersion::NOT_EMOJI) {
        sp.sep();
        appendNonBullet(text, "Prop.Bullet.EmojiVer");
        appendEmojiValue(text, node.emojiVersion(), node.emojiPrevVersion());
    }

    appendNodesTextIf(text, parent);

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
        appendCpBullets(text, *cp, CpSerializations::NO, CpPlace::LIB);
    } else if (auto letters = countryLetters(node.value)) {
        if (auto country = cou::find(letters)) {
            appendSubhead(text, "Lib.Cinfo.Info");
            str::QSep sp(text, "<br>");
            // Status
            appendNonBullet(text, "Lib.Cinfo.Sta");
            mywiki::appendNoFont(text, loc::get(cou::typeKeys[country->type]), wiki::Mode::SPAN);
            // Location
            sp.sep();
            appendNonBullet(text, "Lib.Cinfo.Loc");
            mywiki::appendNoFont(text, loc::get(cou::locKeys[country->location]), wiki::Mode::SPAN);
            // Population
            if (country->popul != cou::Popul::NOT_APPLICABLE) {
                sp.sep();
                appendNonBullet(text, "Lib.Cinfo.Pop");
                mywiki::appendNoFont(text, loc::get(cou::popKeys[country->popul]), wiki::Mode::SPAN);
            }
        }
    }

    return text;
}


namespace {

    void appendValue(str::QSep& sp, unsigned depth, const char* locKey,
                     const uc::EmojiCounter& value,
                     std::string_view schema, std::u8string_view unicodeLink)
    {
        sp.sep();
        // Indent
        if (depth != 0) {
            auto q = str::toQ(loc::active::punctuation.indentEllip);
            for (unsigned i = 0; i < depth; ++i)
                sp.target() += q;
        }
        mywiki::appendNoFont(sp.target(), loc::get(locKey), wiki::Mode::SPAN);
        str::append(sp.target(), loc::active::punctuation.keyValueColon);
        if (value.nSkintone == 0) {
            mywiki::appendCopyable(sp.target(), value.nNormal);
        } else {
            std::u8string fmtNormal, fmtSkintone;
            mywiki::appendCopyable(fmtNormal, value.nNormal);
            mywiki::appendCopyable(fmtSkintone, value.nSkintone);
            auto q = loc::get("Version.Bullet.Plus").arg(
                         loc::PreformN( std::u8string_view{fmtNormal},   value.nNormal ),
                         loc::PreformN{ std::u8string_view{fmtSkintone}, value.nSkintone });
            sp.target() += "<nobr>";
            str::append(sp.target(), q);
            sp.target() += "</nobr>";
        }
        // Query
        if (value.nTotal() != 0 && !schema.empty() && !unicodeLink.empty()) {
            auto params = str::cat("v=", str::toSv(unicodeLink));
            appendQuery(sp.target(), schema, params);
        }
    }

    bool appendValueIf(str::QSep& sp, unsigned depth, const char* locKey,
                       const uc::EmojiCounter& value)
    {
        bool r = (value.nTotal() != 0);
        if (r) {
            appendValue(sp, depth, locKey, value, {}, {});
        }
        return r;
    }

    void appendUnicodeName(
            str::QSep& sp, std::u8string_view name,
            const uc::CoarseDate& date,
            std::u8string_view altName)
    {
        sp.sep();
        sp.target() += "<b>";
        str::append(sp.target(), name);
        sp.target() += "</b> (";
        if (!altName.empty() && name.find(altName) == std::string_view::npos) {
            str::append(sp.target(), loc::get("Prop.Head.EqEm").arg(altName));
        }
        // Date
        char buf[30];
        snprintf(buf, std::size(buf), "Common.Mon.%d",
                 static_cast<int>(date.month));
        auto& monTemplate = loc::get(buf);
        auto s = monTemplate.arg(date.year);
        str::append(sp.target(), s);
        sp.target() += ")";
    }

    void appendBlockLink(QString& text, const uc::Block& blk)
    {
        char buf[30];
        snprintf(buf, std::size(buf), "pk:%04X", static_cast<unsigned>(blk.startingCp));
        text += "<a class='popup' href='";
        text += buf;
        text += "'>";
        str::append(text, blk.loc.name);
        text += "</a>";
    }

    bool chLess(char32_t a, char32_t b) { return (a < b); }
    bool chGreater(char32_t a, char32_t b) { return (a > b); }
    using EvCmp = bool (*)(char32_t, char32_t);

    void appendChangedBlocks(QString& text, const uc::Version& version,
                             bool& wasStarted,
                             std::string_view locKey, EvCmp cmp)
    {
        if (!wasStarted) {
            text += "<p>";
            wasStarted = true;
        } else {
            text += "<br>";
        }
        text += loc::get(locKey);
        str::append(text, loc::active::punctuation.keyValueColon);
        str::QSep sp(text, loc::active::punctuation.uniformComma);
        for (auto& blk : uc::allBlocks()) {
            // Think that resize history is heavy (not really a problem now)
            if (version.stats.thisEcVersion > blk.ecVersion) {
                blk.resizeHistoryT([&](uc::EcVersion v, char32_t bef, char32_t aft) {
                    if (v == version.stats.thisEcVersion && cmp(bef, aft)) {
                        sp.sep();
                        appendBlockLink(text, blk);
                    }
                });
            }
        }
    }

}   // anon namespace

QString mywiki::buildHtml(const uc::Version& version)
{
    QString text;
    appendStylesheet(text);
    text += "<p>";
    { str::QSep sp(text, "<br>");
        if (version.otherEmojiDate) {
            appendUnicodeName(sp, version.locLongName(), version.date, {});
            appendUnicodeName(sp,
                    loc::get("Prop.Head.Em").arg(version.emojiName),
                             version.otherEmojiDate, {});
        } else {
            appendUnicodeName(sp, version.locLongName(), version.date,
                              version.emojiName);
        }
    }

    {
        text += "<p>";
        // Transient
        str::QSep sp(text, "<br>");
        appendValueIf(sp, 0, "Version.Bullet.Transient", version.stats.chars.nTransient);
        auto link = version.link();
        static constexpr std::string_view NO_SCHEMA {};
        static constexpr std::u8string_view NO_LINK {};
        // New
        if (!version.isFirst()) {
            appendValue(sp, 0, "Version.Bullet.NewChar", version.stats.chars.nw.nTotal(), SCH_QRY_CHARS, link);
            appendValueIf(sp, 1, "Version.Bullet.NewCjk", version.stats.chars.nw.nHani);
            appendValueIf(sp, 1, "Version.Bullet.NewNew", version.stats.chars.nw.nNewScripts);
            appendValueIf(sp, 1, "Version.Bullet.NewEx", version.stats.chars.nw.nExistingScripts);
            appendValueIf(sp, 1, "Version.Bullet.NewFmt", version.stats.chars.nw.nFormat);
            appendValueIf(sp, 1, "Version.Bullet.NewSym", version.stats.chars.nw.nSymbols);
        }
        // Total
        appendValue(sp, 0, "Version.Bullet.TotalChar", version.stats.chars.nTotal,
                    SCH_QRY_CHARS,
                    version.isFirst() ? static_cast<std::u8string_view>(link) : NO_LINK);

        if (version.stats.emoji.nTotal != 0) {
            // New emoji
            auto tot = version.stats.emoji.nw.nTotal();
            appendValue(sp, 0, "Version.Bullet.NewEm", tot, SCH_QRY_EMOJI, link);
            if (tot != 0) {
                // Single-char
                auto singleKey = version.stats.emoji.nw.singleChar.areVs16Present
                        ? "Version.Bullet.NewEm1" : "Version.Bullet.NewEm1a";
                if (appendValueIf(sp, 1, singleKey, version.stats.emoji.nw.singleChar.nTotal())) {
                    if (version.stats.emoji.nw.singleChar.nOldUnicode > 0) {
                        appendValueIf(sp, 2, "Version.Bullet.NewEm1This", version.stats.emoji.nw.singleChar.nThisUnicode);
                        appendValueIf(sp, 2, "Version.Bullet.NewEm1Prev", version.stats.emoji.nw.singleChar.nOldUnicode);
                    }
                }
                // Non-ZWJ
                if (appendValueIf(sp, 1, "Version.Bullet.NewEmNot", version.stats.emoji.nw.other.nTotal())) {
                    appendValueIf(sp, 2, "Version.Bullet.NewNotSkin", version.stats.emoji.nw.other.nSingleSkintone);
                    appendValueIf(sp, 2, "Version.Bullet.NewNotFlag", version.stats.emoji.nw.other.nNationalFlags);
                    appendValueIf(sp, 2, "Version.Bullet.NewNotSubdiv", version.stats.emoji.nw.other.nSubdivisionFlags);
                    appendValueIf(sp, 2, "Version.Bullet.NewNotKey", version.stats.emoji.nw.other.nKeycaps);
                }
                // ZWJ
                if (appendValueIf(sp, 1, "Version.Bullet.NewEmZwj", version.stats.emoji.nw.zwj.nTotal())) {
                    appendValueIf(sp, 2, "Version.Bullet.NewZwjMulti", version.stats.emoji.nw.zwj.nMultiracial);
                    appendValueIf(sp, 2, "Version.Bullet.NewZwjRight", version.stats.emoji.nw.zwj.right);
                    appendValueIf(sp, 2, "Version.Bullet.NewZwjFam", version.stats.emoji.nw.zwj.nFamilies);
                    appendValueIf(sp, 2, "Version.Bullet.NewZwjGender", version.stats.emoji.nw.zwj.gender);
                    appendValueIf(sp, 2, "Version.Bullet.NewZwjActivity", version.stats.emoji.nw.zwj.activity);
                    appendValueIf(sp, 2, "Version.Bullet.NewZwjApp", version.stats.emoji.nw.zwj.appearance);
                    appendValueIf(sp, 2, "Version.Bullet.NewZwjColor", version.stats.emoji.nw.zwj.color);
                    appendValueIf(sp, 2, "Version.Bullet.NewZwjOther", version.stats.emoji.nw.zwj.other);
                }
            }
            // Total emoji
            appendValue(sp, 0, "Version.Bullet.TotalEm", version.stats.emoji.nTotal, NO_SCHEMA, NO_LINK);
        }
        if (!version.isFirst()) {
            appendValue(sp, 0, "Version.Bullet.NewSc", version.stats.scripts.nNew, NO_SCHEMA, NO_LINK);
        }
        appendValue(sp, 0, "Version.Bullet.TotalSc", version.stats.scripts.nTotal, NO_SCHEMA, NO_LINK);
    }

    // Text
    if (version.flags.have(uc::Vfg::TEXT)) {
        auto key = str::cat("Version.", str::toSv(version.link({})) , ".Text");
        mywiki::appendNoFont(text, loc::get(key), wiki::Mode::ARTICLE);
    }

    if (!version.isFirst() && version.stats.blocks.nNew != 0) {        
        text += "<p><b>";
        str::append(text, loc::get("Prop.Head.NewBlk"));
        text += "</b><p>";
        constexpr auto NO_BREAK = std::numeric_limits<unsigned>::max();
        unsigned columnBreak = NO_BREAK;
        if (version.stats.blocks.nNew > 15) {
            columnBreak = (version.stats.blocks.nNew + 1) / 2;
        }
        if (columnBreak != NO_BREAK) {
            text += "<table border=0><tr><td>";
        }
        str::QSep sp(text, "<br>");
        unsigned i = 0;
        for (auto& blk : uc::allBlocks()) {
            if (version.stats.thisEcVersion == blk.ecVersion) {
                if (i == columnBreak) {
                    text += "<td>&nbsp;&nbsp;&nbsp;&nbsp;<td>";
                } else {
                    sp.sep();
                }
                appendBlockLink(text, blk);
                ++i;
            }
        }
        if (columnBreak != NO_BREAK) {
            text += "</table>";
        }
    }

    bool wasStarted = false;
    if (version.stats.blocks.wereExtended) {
        appendChangedBlocks(text, version, wasStarted, "Prop.Ver.Ext", chLess);
    }
    if (version.stats.blocks.wereShrunk) {
        appendChangedBlocks(text, version, wasStarted, "Prop.Ver.Shr", chGreater);
    }

    return text;
}


QString mywiki::buildHtml(const uc::GlyphStyleChannel& channel)
{
    char buf[40];

    QString text;
    appendStylesheet(text);

    // Style names are always null-terminated
    snprintf(buf, sizeof(buf), "GlyphVar.%s.Name", channel.name.data());
    text += "<p><nobr><b>";
    mywiki::appendNoFont(text, loc::get(buf), wiki::Mode::SPAN);
    text += "</b></nobr>";

    snprintf(buf, sizeof(buf), "GlyphVar.%s.Text", channel.name.data());
    mywiki::appendNoFont(text, loc::get(buf), wiki::Mode::ARTICLE);

    snprintf(buf, sizeof(buf), "GlyphVar.%s.Name", channel.name.data());
    text += "<p><b>";
    str::append(text, loc::get("Prop.Head.AffBlk"));
    text += "</b>";

    text += "<p>";
    str::QSep sp(text, "<br>");
    for (auto& blk : uc::allBlocks()) {
        if (blk.ecStyleChannel == channel.value) {
            sp.sep();
            snprintf(buf, std::size(buf), "pk:%04X", static_cast<unsigned>(blk.startingCp));
            text += "<a class='popup' href='";
            text += buf;
            text += "'>";
            str::append(text, blk.loc.name);
            text += "</a>";
        }
    }

    return text;
}


template<>
void wiki::append(QString& s, const char* start, const char* end)
{
    s.append(QByteArray::fromRawData(start, end - start));
}


QString mywiki::buildEmptyFavsHtml()
{
    QString text;
    appendStylesheet(text);
    text += "<h3>";
    mywiki::appendNoFont(text, loc::get("Main.NoFavs"), wiki::Mode::SPAN);
    text += "</h3>";
    return text;
}


namespace {

    void addNoteIf(QString& text, const uc::old::Info& info,
                   uc::old::Ocfg trigger, const char* subKey)
    {
        if (info.flags.have(trigger)) {
            char buf[40];
            text += " <i>";
            snprintf(buf, std::size(buf), "OldComp.%s.%s", info.key.data(), subKey);
            mywiki::append(text, loc::get(buf), DEFAULT_CONTEXT, wiki::Mode::SPAN);
            text += "</i>";
        }
    }

}   // anon namespace


QString mywiki::buildHtml(const uc::old::Info& info)
{
    QString text;
    appendStylesheet(text);

    // Name
    str::append(text, "<p><b>");
    str::append(text, info.locLongName());
    str::append(text, "</b>"sv);

    // Year
    text += "<p>";
    str::QSep sp(text, "<br>");
    if (info.year != 0) {
        sp.sep();
        appendNonBullet(text, "OldComp.PropName.Year");
        text += QString::number(info.year);
        addNoteIf(text, info, uc::old::Ocfg::NOTE_YEAR, "YearNote");
    }

    char buf[40];

    // Country
    sp.sep();
    appendNonBullet(text, "OldComp.PropName.Country");
    snprintf(buf, std::size(buf), "OldComp.Prop.Country.%s",
             uc::old::countryInfo[info.country].key);
    text += loc::get(buf);

    // Type
    sp.sep();
    appendNonBullet(text, "OldComp.PropName.Type");
    snprintf(buf, std::size(buf), "OldComp.Prop.Type.%s",
             uc::old::typeInfo[info.type].key);
    mywiki::append(text, loc::get(buf), DEFAULT_CONTEXT, wiki::Mode::SPAN);

    // CPU
    if (info.cpuDataWidth > 0) {
        sp.sep();
        mywiki::append(text,
                loc::get("OldComp.PropName.Cpu").arg(info.cpuDataWidth),
                DEFAULT_CONTEXT, wiki::Mode::SPAN);
    }

    // Memory
    if (info.mem.lo > 0) {
        sp.sep();
        appendNonBullet(text, "OldComp.PropName.Memory");
        if (info.mem.hi == 0) {
            text += loc::get("OldComp.Prop.Mem.1").argQ(info.mem.lo);
        } else {
            text += loc::get("OldComp.Prop.Mem.2").argQ(info.mem.lo, info.mem.hi);
        }
        addNoteIf(text, info, uc::old::Ocfg::NOTE_MEMORY, "MemNote");
    }

    // Colour
    if (info.color != uc::old::Color::NOMATTER) {
        sp.sep();
        appendNonBullet(text, "OldComp.PropName.Color");
        snprintf(buf, std::size(buf), "OldComp.Prop.Color.%s",
                 uc::old::colorInfo[info.color].key);
        text += loc::get(buf);
    }

    // Graphics
    if (info.graphics != uc::old::Graphics::NOMATTER) {
        sp.sep();
        appendNonBullet(text, "OldComp.PropName.Graphics");
        snprintf(buf, std::size(buf), "OldComp.Prop.Graphics.%s",
                 uc::old::graphicsInfo[info.graphics].key);
        text += loc::get(buf);
    }

    // Chars
    { sp.sep();
        auto chars = info.charTypes;
        appendNonBullet(text, "OldComp.PropName.Chars");
        str::QSep spC(text, loc::active::punctuation.uniformComma);
        while (chars) {
            auto bit = chars.smallest();
            chars.remove(bit);
            // Turn bit to index
            auto iBit = std::countr_zero(Flags<uc::old::CharType>::toUnsignedStorage(bit));
            // Write what we got
            spC.sep();
            auto& tyi = uc::old::charTypeInfo[iBit];
            snprintf(buf, std::size(buf), "OldComp.Prop.Chars.%s", tyi.key);
            str::append(text, loc::get(buf));
        }
        appendQuery(text, SCH_QRY_CHARS, str::cat("C=", info.key));
    }

    if (info.flags.have(uc::old::Ocfg::UNENCODED)) {
        sp.sep();
        appendNonBullet(text, "OldComp.PropName.Unenc");
        snprintf(buf, std::size(buf), "OldComp.%s.Unenc", info.key.data());
        mywiki::append(text, loc::get(buf), DEFAULT_CONTEXT, wiki::Mode::SPAN);
    }

    // Sales
    if (info.sales != uc::old::Sales::NOMATTER) {
        sp.sep();
        appendNonBullet(text, "OldComp.PropName.Sales");
        snprintf(buf, std::size(buf), "OldComp.Prop.Sales.%s",
                 uc::old::salesInfo[info.sales].key);
        text += loc::get(buf);
        addNoteIf(text, info, uc::old::Ocfg::NOTE_SALES, "SalesNote");
    }

    // Supported since
    { sp.sep();
        appendNonBullet(text, "OldComp.PropName.Supp");
        appendVersionValue(text, info.versionSupportedSince());
    }

    snprintf(buf, std::size(buf), "OldComp.%s.Text", info.key.data());
    mywiki::append(text, loc::get(buf), DEFAULT_CONTEXT, wiki::Mode::ARTICLE);

    return text;
}
