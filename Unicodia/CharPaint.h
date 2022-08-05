#pragma once

#include <QRect>
#include <QWidget>

#include "UcData.h"
#include "u_EmojiPainter.h"

class QPainter;
class QColor;

extern EmojiPainter emp;


void drawDeprecated(QPainter* painter, const QRect& r);

struct AbbrTable {
    qreal quos[10];     // 0 = 1-character
};

struct AbbrLines {
    std::u8string_view line1, line2, line3;

    unsigned nLines() const { return line2.empty() ? 1 : line3.empty() ? 2 : 3; }
    bool wasSplit() const { return !line2.empty(); }
    unsigned length() const {
        return std::max(std::max(line1.length(), line2.length()), line3.length()); }
    qreal sizeQuo(const AbbrTable& table) const;
};

struct RcPair {
    QRectF rc1, rc2;
    RcPair(const QRectF& rcFrame, qreal quo);
};

void drawCustomAbbrText(QPainter* painter, const AbbrLines& sp,
        const QColor& color, QRectF rcFrame, qreal thickness,
        const AbbrTable& table);

void drawAbbrText(QPainter* painter, std::u8string_view abbreviation,
        const QColor& color, QRectF rcFrame, qreal thickness);

enum class SplitMode { NORMAL, FIXED };

AbbrLines splitAbbr(std::u8string_view abbr, SplitMode mode);

void drawCustomControl(
        QPainter* painter, const QRect& rect, const QColor& color,
        uc::FontPlace place, char32_t subj);

void drawEgyptianHatch(
        QPainter* painter, const QRect& rect,
        const QFont& font, QColor color, char32_t subj);

void drawAbbreviation(
        QPainter* painter, const QRect& rect, std::u8string_view abbreviation,
        const QColor& color);

void drawSpace(
        QPainter* painter, const QRect& rect,
        const QFont& font, QColor color, char32_t subj);

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
    void setEgyptianHatch(const QFont& font, char32_t aSubj);
    void setNormal();
    void init();
protected:
    void paintEvent(QPaintEvent *event);
private:
    QSize initialSize;
    enum class Mode { NONE, SPACE, ABBREVIATION, CUSTOM_CONTROL, EMOJI, EGYPTIAN_HATCH };
    Mode mode = Mode::NONE;
    std::u8string_view abbreviation;
    QFont fontSpace;
    char32_t subj = 0;
    void setSpace1(const QFont& font, char32_t aSubj, Mode aMode);
};
