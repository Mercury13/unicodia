#include "emoji.h"

// Qt
#include <QSvgRenderer>
#include <QPainter>

// Libs
#include "Fonts/TempFont.h"
#include "i_ByteSwap.h"
#include "u_Strings.h"
#include "Zippy/ZipArchive.hpp"

// Unicode
#include "UcCp.h"

// Painters
#include "global.h"

struct RecolorLib {
    std::string_view fill1;
    std::string_view fill2;
    std::string_view outline1;
    std::string_view hair1;
    std::string_view hairHighlight;  ///< Highlighs on the top of hair made by radial gradient
    std::string_view eyebrows;  ///< Eyebrows, same colour as hair highlight!!
    std::string_view ears1;     ///< used in firefighter emoji
        /// @warning: Leave ears E59600, set nose #E49600
    std::string_view nose1;     ///< used at least in swimmer emoji
    std::string_view mouth1;    ///< used at least in firefighter emoji
    std::string_view eyes1;     ///< used at least in firefighter emoji
    std::string_view handLight; ///< used in new U16 hand emoji
    std::string_view handDark;  ///< used in new U16 hand emoji

    void runOn(QByteArray& bytes) const;
    /// @param [in] race  '1' or '2'
    void runColorSeparatedBiracialOn(char race, QByteArray& bytes) const;
    bool recolorId(QByteArray& bytes, std::string_view id) const;
    void recolorIds(QByteArray& bytes, std::span<const std::string> ids) const;
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

/// As made by SVG Cleaner
#define STOP_COLOR "stop-color=\""

void RecolorLib::runOn(QByteArray& bytes) const
{
    repl(bytes, "#FFB300", fill1);      // used: e.g. runner
    repl(bytes, "#FFCA28", fill2);      // same
    // U15 replaced lots of hands and other body parts → some colours now unused
    repl(bytes, "#EDA600", outline1);   // used: e.g. runner
    repl(bytes, "#543930", hair1);
    repl(bytes, STOP_COLOR "#6D4C41", hairHighlight);
    repl(bytes, "#6D4C41", eyebrows);
    repl(bytes, "#E59600", ears1);
    repl(bytes, "#E49600", nose1);      // artificially made (missing in actual Noto): e.g. swimmer
    repl(bytes, "#795548", mouth1);
    repl(bytes, "#404040", eyes1);
    repl(bytes, "#FAA700", handLight);
    repl(bytes, "#B55E19", handDark);
}

void RecolorLib::runColorSeparatedBiracialOn(char race, QByteArray& bytes) const
{
    // Biracial colours are 1) limited; 2) unreal
    // They are #p0000c for now;
    //   p = person (1/2),  c = colour (2/A/B)
    // Biracial recolouring is not covered with regression tests
    //   and just works somehow
    char buf[] = "#000000";
    static_assert(std::size(buf) == 8);
    buf[1] = race;
    // 2 → fill2
    buf[6] = '2';
    repl(bytes, buf, fill2);
    // A → light hand
    buf[6] = 'A';
    repl(bytes, buf, handLight);
    // B → dark hand
    buf[6] = 'B';
    repl(bytes, buf, handDark);
}

namespace {

    struct TagInfo {
        ptrdiff_t start = 0, end = 0;
        constexpr operator bool() const { return end; }
    };

    TagInfo findTag(
            const QByteArray& bytes, QByteArrayView needle, size_t start)
    {
        // Find marker tag
        auto index = bytes.indexOf(needle, start);
        if (index < 0)
            return {};
        // Find tag limits
        auto index1 = bytes.lastIndexOf('<', index);
        if (index1 < 0)
            return {};
        auto index2 = bytes.indexOf('>', index);
        if (index2 < 0)
            return {};
        if (index2 > 0 && bytes[index2 - 1] == '/') {
            // Impair tag?
            return TagInfo { .start = index1, .end = index2 + 1 };
        }
        // Then got pair tag
        auto indName = index1 + 1;
        auto firstSpace = bytes.indexOf(' ', indName);
        if (firstSpace < 0 || firstSpace >= index)
            return {};
        auto nameLen = firstSpace - indName;
        if (nameLen <= 0)
            return {};
        std::string tagName;
        tagName.resize(nameLen);
        std::copy_n(bytes.cbegin() + indName, nameLen, tagName.begin());

        auto closingName = "</" + tagName;
        auto closeStart = bytes.indexOf(closingName, index2 + 1);
        if (closeStart < 0)
            return {};
        auto closeEnd = bytes.indexOf('>', closeStart + closingName.length());
        if (closeEnd < 0)
            return {};

        return { .start = index1, .end = closeEnd + 1 };
    }
}

bool RecolorLib::recolorId(QByteArray& bytes, std::string_view id) const
{
    auto key = str::cat("id=\"", id, '"');
    auto tag = findTag(bytes, key, 0);
    if (!tag)
        return false;

    auto leaveBefore = bytes.sliced(0, tag.start);
    auto change = bytes.sliced(tag.start, tag.end - tag.start);
    auto leaveAfter = bytes.sliced(tag.end);

    runOn(change);
    bytes = leaveBefore + change + leaveAfter;

    return true;
}

void RecolorLib::recolorIds(QByteArray& bytes, std::span<const std::string> ids) const
{
    for (auto& v : ids)
        recolorId(bytes, v);
}


namespace {

    inline QByteArray sliceBetween(
            const QByteArray& bytes, const TagInfo& t1, const TagInfo& t2)
    {
        return bytes.sliced(t1.end, t2.start - t1.end);
    }

    std::vector<std::string> collectIds(const QByteArray& bytes)
    {
        std::vector<std::string> r;
        size_t indStart = 0;
        while (true) {
            // 6 chars
            static constexpr unsigned START_LEN = 6;
            auto index = bytes.indexOf("\"url(#", indStart);
            if (index < 0)
                break;
            auto startData = index + START_LEN;
            static constexpr unsigned END_LEN = 2;
            auto end = bytes.indexOf(")\"", startData);
            if (index < 0)
                break;

            ptrdiff_t length = end - startData;
            if (length > 0) {
                std::string q;
                q.resize(length);
                std::copy_n(bytes.cbegin() + startData, length, q.begin());
                r.push_back(std::move(q));
            }

            indStart = end + END_LEN;
        }
        return r;
    }

}   // anon namespace


bool RecolorInfo::runReverseDelimited(QByteArray& bytes) const
{
    static const char* MARKER = R"("#c01c28")";
    auto tagDefsRace2 = findTag(bytes, MARKER, 0);
    if (!tagDefsRace2)
        return false;
    auto tagRace2Race1 = findTag(bytes, MARKER, tagDefsRace2.end);
    if (!tagRace2Race1)
        return false;
    auto dataHead = bytes.sliced(0, tagDefsRace2.start);
    // [0…index1) is the 2nd race
    auto data1_race2 = sliceBetween(bytes, tagDefsRace2, tagRace2Race1);
    auto idsRace2 = collectIds(data1_race2);
    recolor2->runOn(data1_race2);
    // (index2 …) is the 1st race
    auto data2_race1 = bytes.sliced(tagRace2Race1.end);
    auto idsRace1 = collectIds(data2_race1);
    recolor1->runOn(data2_race1);
    // Run IDs
    recolor1->recolorIds(dataHead, idsRace1);
    recolor2->recolorIds(dataHead, idsRace2);
    // Stick them w/o marker tags
    bytes = dataHead + data1_race2 + data2_race1;
    return true;
}


void RecolorInfo::runColorSeparated(QByteArray& bytes) const
{
    recolor1->runColorSeparatedBiracialOn('1', bytes);
    recolor2->runColorSeparatedBiracialOn('2', bytes);
}


void RecolorInfo::runOn(QByteArray& bytes) const
{
    if (recolor1) {
        if (recolor2) {
            // Choose method of biracial
            if (runReverseDelimited(bytes))
                return;
            runColorSeparated(bytes);
        } else {
            recolor1->runOn(bytes);
        }
    }
}


namespace {

    RecolorLib allRecolors[5] {

        { // White
            .fill1 = "#FFD29C",
            .fill2 = "#F9DDBD",            
            .outline1 = "#E6B77E",
            .hair1 = "#312D2D",
            .hairHighlight = STOP_COLOR "#454140",
            .eyebrows = "#454140",
            .ears1 = "#EDC391",
            .nose1 = "#DBA689",
            .mouth1 = "#444444",
            .eyes1 = "#312D2D",
            .handLight = "#EFB682",
            .handDark = "#B26830",
        },
        { // Light
            .fill1 = "#CCA47A",
            .fill2 = "#E0BB95",
            .outline1 = "#BA8F63",
            .hair1 = "#AB872F",
            .hairHighlight = STOP_COLOR "#BFA055",
            .eyebrows = "#AB872F",
            .ears1 = "#C48E6A",
            .nose1 = "#C48E6A",
            .mouth1 = "#6D4C41",
            .eyes1 = "#5D4037",
            .handLight = "#D19661",
            .handDark = "#8D542E",
        },
        { // Medium
            .fill1 = "#A47B62",
            .fill2 = "#BA8D68",
            .outline1 = "#91674D",
            .hair1 = "#543930",
            .hairHighlight = STOP_COLOR "#6D4C41",
            .eyebrows = "#613E31",
            .ears1 = "#99674F",
            .nose1 = "#99674F",
            .mouth1 = "#5D4037",
            .eyes1 = "#49362E",
            .handLight = "#9A6648",
            .handDark = "#62392B",
        },
        { // Dark
            .fill1 = "#8D5738",
            .fill2 = "#A56C43",
            .outline1 = "#875334",
            .hair1 = "#3C2C23",
            .hairHighlight = STOP_COLOR "#554138",
            .eyebrows = "#42312C",
            .ears1 = "#7A4C32",
            .nose1 = "#875334",
            .mouth1 = "#473530",
            .eyes1 = "#42312C",
            .handLight = "#824624",
            .handDark = "#422319",
        },
        { // Ebony
            .fill1 = "#5C4037",
            .fill2 = "#70534A",
            .outline1 = "#4A2F27",
            .hair1 = "#232020",
            .hairHighlight = STOP_COLOR "#444140",
            .eyebrows = "#1A1717",
            .ears1 = "#3C2B24",
            .nose1 = "#33251F",
            .mouth1 = "#1A1717",
            .eyes1 = "#1A1717",
            .handLight = "#58382B",
            .handDark = "#000000",  // really black
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


namespace {

    constexpr char32_t ARR_FR[] { cp::FLAG_F, cp::FLAG_R };
    constexpr char32_t ARR_ES[] { cp::FLAG_E, cp::FLAG_S };
    constexpr char32_t ARR_IO[] { cp::FLAG_I, cp::FLAG_O };
    constexpr char32_t ARR_NO[] { cp::FLAG_N, cp::FLAG_O };
    constexpr char32_t ARR_US[] { cp::FLAG_U, cp::FLAG_S };

    consteval std::u32string_view toSv(const char32_t (&x)[2])
        { return { x, 2 }; }

}   // anon namespace


std::u32string_view EmojiPainter::followClone(std::u32string_view text)
{
    if (text.length() == 2 && text[0] >= cp::FLAG_A && text[0] <= cp::FLAG_Z) {
        switch (text[0]) {
        case cp::FLAG_B:
            // Bouvet ← Norway
            if (text[1] == cp::FLAG_V) {
                return toSv(ARR_NO);
            }
            break;
        case cp::FLAG_C:
            // Clipperton ← France
            if (text[1] == cp::FLAG_P) {
                return toSv(ARR_FR);
            }
            break;
        case cp::FLAG_D:
            // Giego Garcia ← British Indian Ocean
            if (text[1] == cp::FLAG_G) {
                return toSv(ARR_IO);
            }
            break;
        case cp::FLAG_E:
            // Ceuta & Melilla ← Spain
            if (text[1] == cp::FLAG_A) {
                return toSv(ARR_ES);
            }
            break;
        case cp::FLAG_M:
            // St.Martin ← France
            if (text[1] == cp::FLAG_F) {
                return toSv(ARR_FR);
            }
            break;
        case cp::FLAG_S:
            // Svalbard ← Norway
            if (text[1] == cp::FLAG_J) {
                return toSv(ARR_NO);
            }
            break;
        case cp::FLAG_U:
            // US Outlying ← USA
            if (text[1] == cp::FLAG_M) {
                return toSv(ARR_US);
            }
            break;
        // Similar but inequal:
        // • Indonesia/Monaco: different red
        }
    }
    return text;
}


const EmojiPainter::TapeEntry* EmojiPainter::lookupTape(std::u32string_view text)
{
    // Load tape
    ensureTape();

    text = followClone(text);

    // Get text
    char fname[80];
    getFileName(fname, text, ".svg");

    // Find in directory
    auto it = directory.find(fname);
    if (it == directory.end())
        return nullptr;
    return &it->second;
}


std::string_view EmojiPainter::getSvg(std::u32string_view text)
{
    auto ent = lookupTape(text);
    if (!ent)
        return {};

    // Load subtape
    auto subtape = getSubtape(ent->subtape);
    if (subtape.length() < ent->end())
        throw std::runtime_error("Subtape too short!");
    return subtape.substr(ent->offset, ent->length);
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

    /// @return [+] is flipped
    bool autoFlip(std::u32string_view& text)
    {
        /// @todo [future] We never store flipped emoji, we flip them on-the-fly
        if (text.ends_with(U"\u200D\u27A1\uFE0F")) {
            text = text.substr(0, text.length() - 3);
            return true;
        }
        return false;
    }
}


SvgThing EmojiPainter::getRenderer(std::u32string_view text)
{
    bool isFlipped = autoFlip(text);

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


bool EmojiPainter::canDraw(char32_t cp)
{
    std::u32string_view sv(&cp, 1);
    return lookupTape(sv);
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


void EmojiPainter::drawEmojiTofu(
        QPainter* painter, const QRect& rect, const QColor& color)
{
    // Get painter info
    auto dpr = painter->device()->devicePixelRatio();

    const auto availW = rect.width() * (5.0 / 6.0);
    const auto availH = rect.height() * (3.0 / 5.0);
    const auto availTall = std::min(availW, availH);
    const auto availNarrow = availTall * 2 / 3;
    // Now we draw availNarrow×availTall, shrink rect
    auto ofsX = (rect.width() - availNarrow) / 2;
    auto ofsY = (rect.height() - availTall) / 2;
    QRectF rcFrame { QPointF(rect.left() + ofsX, rect.top() + ofsY),
                     QSizeF(availNarrow, availTall) };
    // Draw frame
    static constexpr qreal Q_THICKNESS = 1.0 / 40.0;
    auto loThickness = availTall * Q_THICKNESS;
    auto hiThickness = loThickness * dpr;  // IDK why dpr, but it scales pen better
    if (hiThickness < 1)
        hiThickness = 1;
    bool isAa = (hiThickness > 1.2);  // 1.2 — still no anti-alias
    painter->setRenderHint(QPainter::Antialiasing, isAa);
    rcFrame = adjustedToPhysicalPixels(painter, rcFrame, loThickness * 0.5, BareFrame::YES);
    painter->setPen(QPen(color, loThickness));
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(rcFrame);
}


void EmojiPainter::draw(
            QPainter* painter, const QRect& rect, char32_t cp, int height,
            const QColor& clTofu)
{
    if (auto rend = getRenderer(cp)) {
        draw1(painter, rect, rend, height);
    } else {
        drawEmojiTofu(painter, rect, clTofu);
    }
}


void EmojiPainter::draw(
            QPainter* painter, const QRect& rect, std::u32string_view cp, int height,
            const QColor& clTofu)
{
    if (auto rend = getRenderer(cp)) {
        draw1(painter, rect, rend, height);
    } else {
        drawEmojiTofu(painter, rect, clTofu);
    }
}


namespace {
    inline bool isSkin(char32_t c)
        { return (c >= cp::SKIN1 && c <= cp::SKIN5); }
}   // anon namespace


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
    decltype(firstPos) secondPos = 0;
    auto secondIt = std::find_if(firstIt + 1, text.end(), isSkin);
    if (secondIt != text.end()) {
        secondPos = secondIt - text.begin();
        auto thirdIt = std::find_if(secondIt + 1, text.end(), isSkin);
        if (thirdIt != text.end())
            return {};
    }

    RecolorInfo ri {
        .baseText = std::u32string{ text },
        .recolor1 = &allRecolors[*firstIt - cp::SKIN1],
    };
    if (secondPos != 0) {
        ri.recolor2 = &allRecolors[*secondIt - cp::SKIN1];
        ri.baseText.erase(secondPos, 1);
    }
    ri.baseText.erase(firstPos, 1);
    return ri;
}
