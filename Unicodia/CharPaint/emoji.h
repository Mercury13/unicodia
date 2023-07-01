#pragma once

#include <memory>
#include <unordered_map>
#include <vector>
#include <span>

#include <QByteArray>

namespace Zippy {
    class ZipArchive;
}

class QSvgRenderer;
class QRect;
class QPainter;


struct RecolorLib;  // forward


struct RecolorInfo {
    std::u32string baseText;
    const RecolorLib* recolor = nullptr;

    operator bool() const { return recolor; }
    void runOn(QByteArray& bytes) const;
};


struct GetCp {
    char32_t cp = 0;
    bool forceGraphic = false;

    explicit operator bool() const noexcept { return cp; }
    [[nodiscard]] bool isGraphic() const noexcept { return cp && !forceGraphic; }
};


struct SvgThing
{
    QSvgRenderer* renderer;
    bool isHorzFlipped;

    operator bool() const noexcept { return renderer; }
    inline SvgThing horzFlipped() const noexcept;
    inline SvgThing horzFlipped(bool x) const noexcept;
};


inline SvgThing SvgThing::horzFlipped() const noexcept
    { return { .renderer = renderer, .isHorzFlipped = !isHorzFlipped }; }

inline SvgThing SvgThing::horzFlipped(bool x) const noexcept
    { return { .renderer = renderer, .isHorzFlipped = x }; }


class EmojiPainter
{
public:
    EmojiPainter();     // forward decls → need in CPP
    ~EmojiPainter();    // same
    static GetCp getCp(std::u32string_view text);
    SvgThing getRenderer(std::u32string_view text);
    /// @post  horzFlipped is FALSE
    SvgThing getRenderer(char32_t cp);
    std::string_view getSvg(std::u32string_view text);
    std::string_view getSvg(char32_t cp);
    void draw(QPainter* painter, const QRect& rect, char32_t cp, int height);
    void draw(QPainter* painter, const QRect& rect, std::u32string_view cp, int height);
    static bool hasSkinGender(std::u32string_view x);
    static void getFileName(
            std::span<char> rBuf,
            std::u32string_view text,
            std::string_view extension);
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
    void draw1(QPainter* painter, QRect rect, const SvgThing& thing, int height);
    static RecolorInfo checkForRecolor(std::u32string_view text);
};
