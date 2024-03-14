#include "emoji.h"

// Qt
#include <QSvgRenderer>
#include <QPainter>

// Libs
#include "i_TempFont.h"
#include "i_ByteSwap.h"
#include "u_Strings.h"
#include "Zippy/ZipArchive.hpp"

// Unicode
#include "UcCp.h"

struct RecolorLib {
    std::string_view fill1;
    std::string_view fill2;
    // fill3 unused: just a few hands e.g. crossed fingers
    // fill4 unused
    std::string_view fill5;
    std::string_view outline1;
    std::string_view hair1;
    std::string_view hair2;
    std::string_view ears1;     ///< used in firefighter emoji
    std::string_view mouth1;    ///< used at least in firefighter emoji
    std::string_view eyes1;     ///< used at least in firefighter emoji

    void runOn(QByteArray& bytes) const;
};

namespace {

    void repl(QByteArray& bytes, std::string_view what, std::string_view byWhat)
    {
        if (byWhat.empty())
            return;
        bytes.replace(what.data(), what.size(), byWhat.data(), byWhat.size());
        auto whatLow = str::toLower(what);
        bytes.replace(whatLow.data(), whatLow.size(), byWhat.data(), byWhat.size());
    }

}   // anon namespace

void RecolorLib::runOn(QByteArray& bytes) const
{
    repl(bytes, "#FFB300", fill1);      // used: e.g. runner
    repl(bytes, "#FFCA28", fill2);      // same
    repl(bytes, "#FFB503", fill5);
    repl(bytes, "#FBC02D", fill2);
    repl(bytes, "#EDA600", outline1);
    repl(bytes, "#FA0",    fill1);
    repl(bytes, "#FFD54F", fill2);
    repl(bytes, "#FFC107", fill1);
    repl(bytes, "#D48322", outline1);
    repl(bytes, "#E6A100", outline1);
    repl(bytes, "#F5AC00", fill1);
    repl(bytes, "#543930", hair1);
    repl(bytes, "#6D4C41", hair2);
    repl(bytes, "#E59600", ears1);
    repl(bytes, "#795548", mouth1);
    repl(bytes, "#404040", eyes1);
    // Unicodia emoji
    repl(bytes, "#F6D32D", fill2);
    repl(bytes, "#E5A50A", outline1);
}

void RecolorInfo::runOn(QByteArray& bytes) const
{
    if (recolor)
        recolor->runOn(bytes);
}


namespace {

    RecolorLib allRecolors[5] {

        { // White
            .fill1 = "#FFD29C",
            .fill2 = "#F9DDBD",
            .fill5 = "#F0CB9E",
            .outline1 = "#E6B77E",
            .hair1 = "#312D2D",
            .hair2 = "#454140",
            .ears1 = "#EDC391",
            .mouth1 = "#444444",
            .eyes1 = "#312D2D",
        },
        { // Light
            .fill1 = "#CCA47A",
            .fill2 = "#DEB892",
            .fill5 = "#D6B088",
            .outline1 = "#BA8F63",
            .hair1 = "#AB872F",
            .hair2 = "#BFA055",
            .ears1 = "#C48E6A",
            .mouth1 = "#6D4C41",
            .eyes1 = "#5D4037",
        },
        { // Medium
            .fill1 = "#A47B62",
            .fill2 = "#BA8D68",
            .fill5 = "#AD8264",
            .outline1 = "#91674D",
            .hair1 = "#543930",
            .hair2 = "#6D4C41",
            .ears1 = "#99674F",
            .mouth1 = "#5D4037",
            .eyes1 = "#49362E",
        },
        { // Dark
            .fill1 = "#8D5738",
            .fill2 = "#A56C43",
            .fill5 = "#98603D",
            .outline1 = "#875334",
            .hair1 = "#3C2C23",
            .hair2 = "#554138",
            .ears1 = "#7A4C32",
            .mouth1 = "#473530",
            .eyes1 = "#42312C",
        },
        { // Black
            .fill1 = "#5C4037",
            .fill2 = "#70534A",
            .fill5 = "#63463D",
            .outline1 = "#4A2F27",
            .hair1 = "#232020",
            .hair2 = "#444140",
            .ears1 = "#3C2B24",
            .mouth1 = "#1A1717",
            .eyes1 = "#1A1717",
        },
    };

    ///  Read Intel word
    [[maybe_unused]] uint16_t readIW(std::istream& is)
    {
        Word1 w;
        is.read(w.asChars, 2);
        swapIW(w);
        return w.asWord;
    }

    ///  Read Intel dword
    [[maybe_unused]] uint32_t readID(std::istream& is)
    {
        Dword1 d;
        is.read(d.asChars, 4);
        swapID(d);
        return d.asDword;
    }

}   // anon namespace


EmojiPainter::EmojiPainter() = default;
EmojiPainter::~EmojiPainter() = default;


void EmojiPainter::ensureTape()
{
    if (arc)
        return;

    auto tempName = expandTempFontName("emoji.zip");
    arc = std::make_unique<Zippy::ZipArchive>(tempName.toStdWString());
    auto entry = arc->GetEntry("tape.bin");
    std::string tapeBin = entry.GetDataAsString();

    std::istringstream iss(tapeBin);
    auto nEntries = readID(iss);
    size_t maxSubtape = 0;
    for (unsigned iEntry = 0; iEntry < nEntries; ++iEntry) {
        TapeEntry te;
        te.subtape = readIW(iss);
        te.offset = readID(iss);
        te.length = readID(iss);
        auto length = readIW(iss);
        std::string fname;
        fname.resize(length);
        iss.read(fname.data(), length);
        directory[fname] = te;
        if (te.subtape > maxSubtape)
            maxSubtape = te.subtape;
    }
    subtapes.resize(maxSubtape + 1);
}


std::string_view EmojiPainter::getSubtape(unsigned index)
{
    // Load tape
    ensureTape();

    if (index > subtapes.size())
        return {};

    auto& v = subtapes[index];
    if (!v.empty())
        return v;

    char fname[20];
    snprintf(fname, std::size(fname), "%u.bin", index);

    auto entry = arc->GetEntry(fname);
    v = entry.GetDataAsString();
    return v;
}


std::string_view EmojiPainter::getSvg(char32_t cp)
{
    return getSvg(std::u32string_view{ &cp, 1 });
}


void EmojiPainter::getFileName(
        std::span<char> rBuf,
        std::u32string_view text,
        std::string_view extension)
{
    auto p = rBuf.begin();
    auto end = rBuf.end();
    for (auto v : text) {
        if (v != cp::VS16) {
            if (p != std::begin(rBuf)) {
                *(p++) = '-';
            }
            auto sz = end - p;
            auto n = snprintf(std::to_address(p), sz, "%x", static_cast<int>(v));
            if (n < 0) {
                throw std::logic_error("[getSvg] Cannot print");
            }
            p += n;
        }
    }
    auto remder = end - p;
    if (remder < static_cast<ptrdiff_t>(extension.size() + 1))
        throw std::logic_error("[getSvg] Buffer overrun");
    p = std::copy(extension.begin(), extension.end(), p);
    *p = 0;
}


std::string_view EmojiPainter::getSvg(std::u32string_view text)
{
    // Load tape
    ensureTape();

    // Get text
    char fname[80];
    getFileName(fname, text, ".svg");

    // Find in directory
    auto it = directory.find(fname);
    if (it == directory.end())
        return {};
    auto ent = it->second;

    // Load subtape
    auto subtape = getSubtape(ent.subtape);
    if (subtape.length() < ent.end())
        throw std::runtime_error("Subtape too short!");
    return subtape.substr(ent.offset, ent.length);
}


GetCp EmojiPainter::getCp(std::u32string_view text)
{
    switch (text.length()) {
    case 1:
        return { .cp = text[0], .forceGraphic = false };
    case 2:
        switch (text[1]) {
        case cp::VS15:
            return { .cp = text[0], .forceGraphic = false };
        case cp::VS16:
            return { .cp = text[0], .forceGraphic = true };
        default:;
        }
        [[fallthrough]];
    default:
        return { .cp = 0, .forceGraphic = false };
    }
}


namespace {
    constexpr SvgThing NO_THING { .renderer = nullptr, .isHorzFlipped = false };
}


SvgThing EmojiPainter::getRenderer(std::u32string_view text)
{
    /// @todo [future] We never store flipped emoji, we flip them on-the-fly
    bool isFlipped = false;
    if (text.ends_with(U"\u200D\u27A1\uFE0F")) {
        text = text.substr(0, text.length() - 3);
        isFlipped = true;
    }

    if (text.empty())
        return NO_THING;

    // Check for more performance-y single-char
    if (auto c = getCp(text))
        return getRenderer(c.cp).horzFlipped(isFlipped);

    auto it = multiCharRenderers.find(text);
    if (it != multiCharRenderers.end())
        return { .renderer = it->second.get(), .isHorzFlipped = isFlipped };

    RecolorInfo recolor;

    // No cached renderer!
    auto svg = getSvg(text);
    if (svg.empty()) {
        recolor = checkForRecolor(text);
        if (!recolor)
            return NO_THING;

        svg = getSvg(recolor.baseText);
        if (svg.empty())
            return NO_THING;
    }

    QByteArray bytes(svg.data(), svg.length());
    recolor.runOn(bytes);

    auto rend = std::make_unique<QSvgRenderer>(bytes);
    rend->setAspectRatioMode(Qt::KeepAspectRatio);
    auto [it2,wasIns] = multiCharRenderers.emplace(std::u32string_view{text}, std::move(rend));
    return { .renderer = it2->second.get(), .isHorzFlipped = isFlipped };
}


SvgThing EmojiPainter::getRenderer(char32_t cp)
{
    auto [it,wasIns] = singleCharRenderers.insert({ cp, nullptr });
    if (!wasIns)
        return { .renderer = it->second.get(), .isHorzFlipped = false };

    // No cached renderer!
    auto svg = getSvg(cp);
    if (svg.empty())
        return NO_THING; // If we go this way, nullptr is cached, and that’s OK

    QByteArray bytes(svg.data(), svg.length());
    auto rend = std::make_unique<QSvgRenderer>(bytes);
    rend->setAspectRatioMode(Qt::KeepAspectRatio);
    it->second = std::move(rend);
    return { .renderer = it->second.get(), .isHorzFlipped = false };
}


void EmojiPainter::draw1(QPainter* painter, QRect rect, const SvgThing& thing, int height)
{
    if (!thing)
        return;
    if (rect.height() > height) {
        auto delta = (rect.height() - height) / 2;
        rect.moveTop(rect.top() + delta);
        rect.setHeight(height);
    }
    if (thing.isHorzFlipped) {
        auto oldTransform = painter->transform();
        //  What to do:
        //        |
        //     111|000     222
        //     111|000     222
        //  If we draw at 000, SVG will actually be drawn at 111,
        //    and we need to translate x+w pixels aka right
        //  Peculiarity of Qt’s right() → counts pixel centres
        //    rather than grid nodes → use right+1
        painter->scale(-1, 1);
        painter->translate(-rect.right() - 1, 0);
        rect.moveLeft(0);
        thing.renderer->render(painter, rect);
        painter->setTransform(oldTransform);
    } else {
        thing.renderer->render(painter, rect);
    }
}


void EmojiPainter::draw(
            QPainter* painter, const QRect& rect, char32_t cp, int height)
{
    if (auto rend = getRenderer(cp)) {
        draw1(painter, rect, rend, height);
    }
}


void EmojiPainter::draw(
            QPainter* painter, const QRect& rect, std::u32string_view cp, int height)
{
    if (auto rend = getRenderer(cp)) {
        draw1(painter, rect, rend, height);
    }
}


namespace {
    inline bool isSkin(char32_t c)
        { return (c >= cp::SKIN1 && c <= cp::SKIN5); }
}   // anon namespace


bool EmojiPainter::hasSkinGender(std::u32string_view x)
{
    static constinit char32_t ALL_CHARS_AR[] {
        // Allowed when used alone
        cp::SKIN1, cp::SKIN2, cp::SKIN3, cp::SKIN4, cp::SKIN5,   // 5
        cp::MALE, cp::FEMALE, cp::MAN, cp::WOMAN,            // 9
        cp::RIGHT_ARROW,                         // 10
        // The rest
        cp::MAN_AND_WOMAN, cp::TWO_MEN, cp::TWO_WOMEN, cp::SANTA_CLAUS, cp::MRS_CLAUS };
    constexpr size_t OFS_1 = 10;
    static constinit std::u32string_view ALL_CHARS { ALL_CHARS_AR, std::size(ALL_CHARS_AR) };
    static constinit std::u32string_view SPEC_CHARS { ALL_CHARS_AR + OFS_1, std::end(ALL_CHARS_AR) };

    switch (x.size()) {
    case 0:
        return false;
    case 1:
        return (x.find_first_of(SPEC_CHARS) != std::u32string::npos);
    case 2: {
            auto sv = (x[1] == cp::VS16) ? SPEC_CHARS : ALL_CHARS;
            return (x.find_first_of(sv) != std::u32string::npos);
        }
    default:
        return (x.find_first_of(ALL_CHARS) != std::u32string::npos);
    }
}


RecolorInfo EmojiPainter::checkForRecolor(std::u32string_view text)
{
    // Debug
//    if (text.find(0x1F692) != std::u32string_view::npos) {
//        std::cout << "Recolor fireman" << std::endl;
//    }
    // 0 or 1 — cannot recolor
    if (text.length() < 2)
        return {};

    // Has skin tone
    auto firstIt = std::find_if(text.begin(), text.end(), isSkin);
    if (firstIt == text.end())
        return {};
    auto firstPos = firstIt - text.begin();

    // Has no more skin tones
    if (auto nextIt = std::find_if(firstIt + 1, text.end(), isSkin);
            nextIt != text.end())
        return {};

    RecolorInfo ri {
        .baseText = std::u32string{ text },
        .recolor = &allRecolors[*firstIt - cp::SKIN1],
    };
    ri.baseText.erase(firstPos, 1);
    return ri;
}
