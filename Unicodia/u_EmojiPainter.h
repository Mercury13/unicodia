#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include <QByteArray>

namespace Zippy {
    class ZipArchive;
}

constexpr char32_t VS16 = 0xFE0F;
constexpr char32_t SKIN1 = 0x1F3FB;
constexpr char32_t SKIN2 = 0x1F3FC;
constexpr char32_t SKIN3 = 0x1F3FD;
constexpr char32_t SKIN4 = 0x1F3FE;
constexpr char32_t SKIN5 = 0x1F3FF;
constexpr char32_t MALE = 0x2642;
constexpr char32_t FEMALE = 0x2640;
constexpr char32_t MAN = 0x1F468;
constexpr char32_t WOMAN = 0x1F469;
constexpr char32_t MAN_AND_WOMAN = 0x1F46B;
constexpr char32_t TWO_MEN = 0x1F46C;
constexpr char32_t TWO_WOMEN = 0x1F46D;
constexpr char32_t SANTA_CLAUS = 0x1F385;
constexpr char32_t MRS_CLAUS = 0x1F936;

class QSvgRenderer;
class QRect;
class QPainter;


struct RecolorLib;  // forward


struct RecolorInfo {
    std::u32string_view baseText;
    const RecolorLib* recolor = nullptr;

    operator bool() const { return recolor; }
    void runOn(QByteArray& bytes) const;
};


class EmojiPainter
{
public:
    EmojiPainter();     // forward decls → need in CPP
    ~EmojiPainter();    // same
    static char32_t getCp(std::u32string_view text);
    QSvgRenderer* getRenderer(std::u32string_view text);
    QSvgRenderer* getRenderer(char32_t cp);
    std::string_view getSvg(std::u32string_view text);
    std::string_view getSvg(char32_t cp);
    void draw(QPainter* painter, const QRect& rect, char32_t cp, int height);
    void draw(QPainter* painter, const QRect& rect, std::u32string_view cp, int height);
    static bool hasSkinGender(std::u32string_view x);
private:
    // Types
    struct TapeEntry {
        unsigned subtape = 0, offset = 0, length = 0;
        constexpr unsigned end() const { return offset + length; }
    };

    struct Hash {
        using is_transparent = void;
        [[no_unique_address]] std::hash<std::u32string> h1;
        [[no_unique_address]] std::hash<std::u32string_view> h2;

        [[nodiscard]] size_t operator()(const std::u32string &txt) const
            { return h1(txt); }
        [[nodiscard]] size_t operator()(std::u32string_view txt) const
            { return h2(txt); }
    };

    // Vars
    std::unique_ptr<Zippy::ZipArchive> arc;
    std::vector<std::string> subtapes;
    std::unordered_map<std::string, TapeEntry> directory;
    std::unordered_map<char32_t, std::unique_ptr<QSvgRenderer>> singleCharRenderers;
    std::unordered_map<std::u32string, std::unique_ptr<QSvgRenderer>, Hash, std::equal_to<>> multiCharRenderers;

    // Functions
    void ensureTape();
    std::string_view getSubtape(unsigned index);    
    void draw1(QPainter* painter, QRect rect, QSvgRenderer& rend, int height);
    static RecolorInfo checkForRecolor(std::u32string_view text);
};
