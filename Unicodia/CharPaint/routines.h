#ifndef CHARPAINT_H_
#define CHARPAINT_H_    // need normal include guard — so needs custom widget

#include <QRect>
#include <QWidget>

// Libs
#include "u_TinyOpt.h"

#include "UcData.h"
#include "global.h"

class QPainter;
class QColor;


void drawDeprecated(QPainter* painter, const QRect& r);

struct RcPair {
    QRectF rc1, rc2;
    RcPair(const QRectF& rcFrame, qreal quo);
};

void drawSample(QPainter* painter, QRect rect, int sizePt, int sizePc,
                const uc::Cp& cp, const QColor& color, uc::EmojiDraw emojiMode,
                const uc::GlyphStyleSets& glyphSets, float offset);

void drawSample(QPainter* painter, QRect rect, TinyOpt<QFont> font, const uc::Cp& cp,
                const QColor& color, uc::EmojiDraw emojiMode,
                const uc::GlyphStyleSets& glyphSets, float offset);

void drawAbbrText(QPainter* painter, std::u8string_view abbreviation,
        const QColor& color, QRectF rcFrame, qreal thickness);

void drawCustomControl(
        QPainter* painter, const QRect& rect, const QColor& color,
        uc::FontPlace place, char32_t subj);

void drawAbbreviation(
        QPainter* painter, const QRect& rect, std::u8string_view abbreviation,
        const QColor& color);

void drawSpace(
        QPainter* painter, const QRect& rect,
        const QFont& font,
        QColor color,  ///< @warning actually changed
        char32_t subj);

void drawVirtualVirama(
        QPainter* painter, const QRect& rect,
        const QColor& color, int absSize, const uc::Cp& cp);

void drawSampledControl(
        QPainter* painter, const QRect& rect, const uc::SampleProxy& proxy,
        const QFont& font, const QColor& color, uc::FontPlace place);

uc::SvgChecker& svgChecker();

constexpr int ROT_CW = 90;
constexpr int ROT_CCW = -90;

void drawVertical(
        QPainter* painter, const QRect& rect,
        const QFont& font, int rotation, QColor color, const QString& subj);

enum class TableDraw {
    INTERNAL,   // Normal chars are drawn internally
    CUSTOM,     // Normal chars are drawn in custom way
    LIBRARY,    // Custom + turn off drawing text+emoji
    SEARCH = CUSTOM,  // Search uses custom way
};

std::optional<QFont> fontAt(
        uc::EmojiDraw emojiMode, const uc::Cp& cp,
        int sizePt, const uc::GlyphStyleSets& glyphSets);
std::optional<QFont> fontAt(
        uc::DrawMethod drawMethod, int sizePt,
        int sizePc, const uc::Cp& cp);

/// Draws border of search/emoji
void drawCharBorder(QPainter* painter, const QRect& rect, const QColor& color);

/// Draws murky rect with border of unallocated / reserved international
void drawMurkyRect(QPainter* painter, const QRect& rect, const QColor& color);

enum class UseMargins { NO, YES };

void drawChar(
        QPainter* painter, const QRect& rect, int sizePt, int sizePc,
        const uc::Cp& cp, const QColor& color, TableDraw tableMode,
        uc::EmojiDraw emojiMode, const uc::GlyphStyleSets& glyphSets,
        UseMargins useMargins = UseMargins::YES);

void drawIconChars(
        QPainter* painter, const QRect& rect, int sizePc, std::u32string_view s,
        const QColor& color);

/// Draws existing character (not reserved icon!)
void drawSearchChar(
        QPainter* painter, const QRect& rect, const uc::Cp* cp,
        const QColor& color, uc::EmojiDraw emojiMode,
        const uc::GlyphStyleSets& glyphSets, qreal scale);

/// @param tableDraw   CUSTOM or LIBRARY only
void drawSearchChars(
        QPainter* painter, const QRect& rect, std::u32string_view text,
        const QColor& color, uc::EmojiDraw emojiMode, TableDraw tableDraw,
        const uc::GlyphStyleSets& glyphSets, qreal scale);
void drawEmojiDirect(
        QPainter* painter, const QRect& rect, std::u32string_view text,
        const QColor& clTofu);

void drawCharTiles(
        QPainter* painter, const QRect& rect,
        const CharTiles& tiles, const QColor& color,
        const uc::GlyphStyleSets& glyphSets, qreal scale);
CharTiles getCharTiles(const uc::LibNode& node);
void drawFolderTile(
        QPainter* painter, const QRect& bounds,
        const uc::LibNode& node, const QColor& color,
        const uc::GlyphStyleSets& glyphSets, qreal scale);

void drawCharOverEmoji(
        QPainter* painter, const QRect& rect, int absSize,
        const QColor& color, char32_t subj);

QSize spaceDimensions(const QFont& font, char32_t subj);

class WiCustomDraw : public QWidget
{
    using Super = QWidget;
public:
    using Super::Super;
    void setAbbreviation(std::u8string_view x);
    void setSpace(const QFont& font, char32_t aSubj);
    void setCustomControl(char32_t aSubj);
    void setEmoji(char32_t aSubj);
    void setEmoji(std::u32string_view aText);
    void setVertical(const QFont& font, const uc::SampleProxy& pr, int angle);
    void setVirtualVirama(char32_t cp);
    void setSampledControl(const QFont& font, const uc::SampleProxy& pr);
    void setCharOverEmoji(char32_t aSubj);
    void setGraphicSample(const QFont& font, const uc::SampleProxy& pr);
    void setNormal();
    void init();
protected:
    void paintEvent(QPaintEvent *event);
private:
    QSize initialSize;
    enum class Mode {
        NONE, SPACE, ABBREVIATION, CUSTOM_CONTROL, EMOJI_CHAR,
        EMOJI_TEXT, VERTICAL, VIRTUAL_VIRAMA, SAMPLED_CONTROL,
        CHAR_OVER_EMOJI, GRAPHIC_SAMPLE };
    Mode mode = Mode::NONE;
    std::u8string_view abbreviation;
    QFont qfont;
    char32_t subj = 0;
    std::u32string_view text;
    int verticalAngle = 90;
    uc::SampleProxy proxy;
    void setSpace1(const QFont& font, char32_t aSubj, Mode aMode);
};

#endif
