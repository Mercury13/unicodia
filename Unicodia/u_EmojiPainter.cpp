#include "u_EmojiPainter.h"

// Qt
#include <QSvgRenderer>

// Libs
#include "i_TempFont.h"
#include "i_ByteSwap.h"
#include "u_Strings.h"
#include "Zippy/ZipArchive.hpp"

struct RecolorLib {
    std::string_view fill1;
    std::string_view fill2;
    std::string_view fill3;
    std::string_view fill4;
    std::string_view fill5;
    std::string_view outline1;

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

}

void RecolorLib::runOn(QByteArray& bytes) const
{
    repl(bytes, "#FFB300", fill1);
    repl(bytes, "#FFCA28", fill2);
    repl(bytes, "#FFC41E", fill3);
    repl(bytes, "#FFBB0D", fill4);
    repl(bytes, "#FFB503", fill5);
    repl(bytes, "#EDA600", outline1);
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
            .fill3 = "#F8DBBA",
            .fill4 = "#F5D5AF",
            .fill5 = "#F0CB9E",
            .outline1 = "#E6B77E",
        },
        { // Light
            .fill1 = "#E0BB95",
            .fill2 = "#DEB892",
            .fill3 = "#D6B088",
            .fill4 = "#D1AA81",
            .fill5 = "#CCA47A",
            .outline1 = "#BA8F63",
        },
        // Medium
        {},
        // Dark
        {},
        // Black
        {},
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


std::string_view EmojiPainter::getSvg(std::u32string_view text)
{
    // Load tape
    ensureTape();

    // Get text
    char fname[80];
    char* p = std::begin(fname);
    const char* end = std::end(fname);
    for (auto v : text) {
        if (v != VS16) {
            if (p != std::begin(fname)) {
                *(p++) = '-';
            }
            auto sz = end - p;
            auto n = snprintf(p, sz, "%x", static_cast<int>(v));
            if (n < 0) {
                throw std::logic_error("[getSvg] Cannot print");
            }
            p += n;
        }
    }
    constexpr std::string_view SUFFIX = ".svg";
    auto remder = end - p;
    if (remder < static_cast<ptrdiff_t>(SUFFIX.size() + 1))
        throw std::logic_error("[getSvg] Buffer overrun");
    strncpy(p, SUFFIX.data(), remder);

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


char32_t EmojiPainter::getCp(std::u32string_view text)
{
    switch (text.length()) {
    case 1:
        return text[0];
    case 2:
        if (text[1] == VS16)
            return text[0];
        [[fallthrough]];
    default:
        return 0;
    }
}


QSvgRenderer* EmojiPainter::getRenderer(std::u32string_view text)
{
    // Check for more performance-y single-char
    if (auto c = getCp(text))
        return getRenderer(c);

    auto it = multiCharRenderers.find(text);
    if (it != multiCharRenderers.end())
        return it->second.get();

    auto recolor = checkForRecolor(text);
    if (!recolor) {
        recolor.baseText = text;
    }

    // No cached renderer!
    auto svg = getSvg(recolor.baseText);
    if (svg.empty())
        return nullptr;

    QByteArray bytes(svg.data(), svg.length());
    recolor.runOn(bytes);

    auto rend = std::make_unique<QSvgRenderer>(bytes);
    rend->setAspectRatioMode(Qt::KeepAspectRatio);
    auto [it2,wasIns] = multiCharRenderers.emplace(std::u32string_view{text}, std::move(rend));
    return it2->second.get();
}


QSvgRenderer* EmojiPainter::getRenderer(char32_t cp)
{
    auto [it,wasIns] = singleCharRenderers.insert({ cp, nullptr });
    if (!wasIns)
        return it->second.get();

    // No cached renderer!
    auto svg = getSvg(cp);
    if (svg.empty())
        return nullptr; // If we go this way, nullptr is cached, and that’s OK

    QByteArray bytes(svg.data(), svg.length());
    auto rend = std::make_unique<QSvgRenderer>(bytes);
    rend->setAspectRatioMode(Qt::KeepAspectRatio);
    it->second = std::move(rend);
    return it->second.get();
}


void EmojiPainter::draw1(QPainter* painter, QRect rect, QSvgRenderer& rend, int height)
{
    if (rect.height() > height) {
        auto delta = (rect.height() - height) / 2;
        rect.moveTop(rect.top() + delta);
        rect.setHeight(height);
    }
    rend.render(painter, rect);
}


void EmojiPainter::draw(
            QPainter* painter, const QRect& rect, char32_t cp, int height)
{
    if (auto* rend = getRenderer(cp)) {
        draw1(painter, rect, *rend, height);
    }
}


void EmojiPainter::draw(
            QPainter* painter, const QRect& rect, std::u32string_view cp, int height)
{
    if (auto* rend = getRenderer(cp)) {
        draw1(painter, rect, *rend, height);
    }
}


namespace {
    inline bool isSkin(char32_t c)
        { return (c >= SKIN1 && c <= SKIN5); }
}   // anon namespace


RecolorInfo EmojiPainter::checkForRecolor(std::u32string_view text)
{
    // 0 or 1 — cannot recolor
    if (text.length() < 2)
        return {};
    auto lastChar = text.back();
    // Last is skin
    if (!isSkin(lastChar))
        return {};
    // No more skins
    auto end1 = text.end() - 1;
    if (std::find_if(text.begin(), end1, isSkin) != end1)
        return {};
    return {
        .baseText { text.begin(), end1 },
        .recolor = &allRecolors[lastChar - SKIN1],
    };
}
