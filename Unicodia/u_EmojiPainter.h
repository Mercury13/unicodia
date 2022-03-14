#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

namespace Zippy {
    class ZipArchive;
}

class QSvgRenderer;
class QRect;
class QPainter;


class EmojiPainter
{
public:
    EmojiPainter();
    ~EmojiPainter();
    QSvgRenderer* getRenderer(char32_t cp);
    std::string_view getSvg(char32_t cp);
    void draw(QPainter* painter, QRect rect, char32_t cp, int height);
private:
    // Types
    struct TapeEntry {
        unsigned subtape = 0, offset = 0, length = 0;
        constexpr unsigned end() const { return offset + length; }
    };

    // Vars
    std::unique_ptr<Zippy::ZipArchive> arc;
    std::vector<std::string> subtapes;
    std::unordered_map<std::string, TapeEntry> directory;
    std::unordered_map<char32_t, std::unique_ptr<QSvgRenderer>> renderers;

    // Functions
    void ensureTape();
    std::string_view getSubtape(unsigned index);
};
