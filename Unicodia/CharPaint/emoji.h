#pragma once

#include <memory>
#include <unordered_map>
#include <vector>
#include <span>

#include <QByteArray>

#include "UcFlags.h"

namespace Zippy {
    class ZipArchive;
}

class QSvgRenderer;
class QRect;
class QPainter;
class QColor;


struct RecolorLib;  // forward


struct RecolorInfo {
    std::u32string baseText;
    const RecolorLib* recolor1 = nullptr;
    const RecolorLib* recolor2 = nullptr;

    operator bool() const { return recolor1; }
    void runOn(QByteArray& bytes) const;
private:
    /// Method of biracial recolouring: reverse-delimited
    /// 1. SECOND race
    /// 2. Some shape filled with #c01c28
    /// 2. FIRST race
    /// @return  [+] method found [-] method not found
    bool runReverseDelimited(QByteArray& bytes) const;
    /// Method of biracial recolouring: 1st and 2nd have own colours
    /// No identification, the last method
    void runColorSeparated(QByteArray& bytes) const;
};


struct GetCp {
    char32_t cp = 0;
    bool forceGraphic = false;

    explicit operator bool() const noexcept { return cp; }
    /// @return [+] may be text  [-] surely graphic
    [[nodiscard]] bool mayBeText() const noexcept { return cp && !forceGraphic; }
    [[nodiscard]] bool mayBeText(uc::EmojiDraw emojiDraw)
        { return (cp && ((emojiDraw == uc::EmojiDraw::FORCE_TEXT) || !forceGraphic)); }
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


class EmojiPainter : public uc::SvgChecker
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
    void draw(QPainter* painter, const QRect& rect, char32_t cp, int height, const QColor& clTofu);
    void draw(QPainter* painter, const QRect& rect, std::u32string_view cp, int height, const QColor& clTofu);
    static void drawEmojiTofu(QPainter* painter, const QRect& rect, const QColor& color);
    static void getFileName(
            std::span<char> rBuf,
            std::u32string_view text,
            std::string_view extension);

    /// A very fast function that refers just to contents
    /// @return [+] can draw [-] tofu
    bool canDraw(char32_t cp) override;
private:
    // Types
    struct TapeEntry {
        unsigned subtape = 0, offset = 0, length = 0;
        constexpr unsigned end() const { return offset + length; }
    };

    struct Hash : public std::hash<std::u32string_view> {
        using is_transparent = void;
    };

    // Vars
    std::unique_ptr<Zippy::ZipArchive> arc;
    std::vector<std::string> subtapes;
    std::unordered_map<std::string, TapeEntry> directory;
    std::unordered_map<char32_t, std::unique_ptr<QSvgRenderer>> singleCharRenderers;
    std::unordered_map<std::u32string, std::unique_ptr<QSvgRenderer>, Hash, std::equal_to<>> multiCharRenderers;

    // Functions
    void ensureTape();
    const TapeEntry* lookupTape(std::u32string_view text);
    static std::u32string_view followClone(std::u32string_view text);
    std::string_view getSubtape(unsigned index);    
    void draw1(QPainter* painter, QRect rect, const SvgThing& thing, int height);
    static RecolorInfo checkForRecolor(std::u32string_view text);
};
