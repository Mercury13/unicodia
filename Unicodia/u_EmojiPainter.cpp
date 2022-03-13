#include "u_EmojiPainter.h"

// Libs
#include "i_TempFont.h"
#include "i_ByteSwap.h"
#include "Zippy.hpp"


namespace {

    [[maybe_unused]] uint16_t readIW(std::istream& is)
    {
        Word1 w;
        is.read(w.asChars, 2);
        swapIW(w);
        return w.asWord;
    }

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
    for (unsigned iEntry = 0; iEntry < nEntries; ++iEntry) {
        TapeEntry te;
        te.subtape = readIW(iss);
        te.offset = readID(iss);
        te.length = readID(iss);
        auto length = readIW(iss);
        std::string fname;
        fname.resize(length);
        iss.read(fname.data(), length);
    }
}


QSvgRenderer* EmojiPainter::getSvg(char32_t cp)
{
    ensureTape();
    return nullptr;
}


void EmojiPainter::draw(QPainter* painter, const QRect& rect, char32_t cp)
{
    if (auto rend = getSvg(cp)) {
        /// @todo [urgent] set appropriate size
        rend->render(painter, rect);
    }
}
