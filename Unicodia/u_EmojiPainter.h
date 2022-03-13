#pragma once

#include <memory>
#include <unordered_map>

// Qt
#include <QSvgRenderer>

namespace Zippy {
    class ZipArchive;
}

class EmojiPainter
{
public:
    EmojiPainter();
    ~EmojiPainter();
    void draw(QPainter* painter, const QRect& rect, char32_t cp);
private:
    struct TapeEntry {
        unsigned subtape = 0, offset = 0, length = 0;
    };
    void ensureTape();
    QSvgRenderer* getSvg(char32_t cp);
    std::unique_ptr<Zippy::ZipArchive> arc;
    std::unordered_map<int, std::string> subtapes;
    std::unordered_map<std::string, TapeEntry> dir;
    std::unordered_map<char32_t, QSvgRenderer> renderers;
};
