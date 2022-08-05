#include "u_EmojiPainter.h"

// Qt
#include <QSvgRenderer>

// Libs
#include "i_TempFont.h"
#include "i_ByteSwap.h"
#include "Zippy.hpp"


namespace {

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
    // Load tape
    ensureTape();

    // Find in directory
    char fname[40];
    snprintf(fname, std::size(fname), "%x.svg", static_cast<int>(cp));
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


QSvgRenderer* EmojiPainter::getRenderer(char32_t cp)
{
    auto it = renderers.find(cp);
    if (it != renderers.end())
        return it->second.get();

    // No cached renderer!
    auto svg = getSvg(cp);
    if (svg.empty())
        return nullptr;

    QByteArray bytes(svg.data(), svg.length());
    auto rend = std::make_unique<QSvgRenderer>(bytes);
    rend->setAspectRatioMode(Qt::KeepAspectRatio);
    auto [it2, wasIns] = renderers.emplace(cp, std::move(rend));
    return it2->second.get();
}


void EmojiPainter::draw(
            QPainter* painter, QRect rect,
            char32_t cp, int height)
{
    if (auto* rend = getRenderer(cp)) {
        if (rect.height() > height) {
            auto delta = (rect.height() - height) / 2;
            rect.moveTop(rect.top() + delta);
            rect.setHeight(height);
        }
        rend->render(painter, rect);
    }
}
