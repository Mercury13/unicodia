// My header
#include "routines.h"

// Qt
#include <QPainter>

// Libs
#include "u_Qstrings.h"

// Unicode
#include "UcCp.h"

// Project-local
#include "Skin.h"
#include "emoji.h"

using namespace std::string_view_literals;

EmojiPainter emp;

uc::SvgChecker& svgChecker() { return emp; }

namespace {
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

    // Table for normal abbreviations
    constexpr qreal Q1 = 1.5;
    constexpr qreal Q3 = 2.5;
    constexpr qreal Q4 = 3.0;
    constexpr qreal Q5 = 3.75;
                    //   0   1   2   3   4   5   6   7   8   9
    AbbrTable atAbbr { { Q1, Q3, Q3, Q3, Q4, Q5, Q5, Q5, Q5, Q5 } };

    // Table for tags
    constexpr qreal T3 = 2.1;
                    //   0   1   2   3   4   5   6   7   8   9
    AbbrTable atTags { { Q1, T3, T3, Q3, Q4, Q5, Q5, Q5, Q5, Q5 } };

    enum class SplitMode { NORMAL, FIXED };

    AbbrLines splitAbbr(std::u8string_view abbr, SplitMode mode)
    {
        if (mode == SplitMode::FIXED)
            return { abbr, {}, {} };
        if (auto pSpace = abbr.find(' '); pSpace != std::u8string_view::npos) {
            auto line1 = abbr.substr(0, pSpace), line23 = abbr.substr(pSpace + 1);
            if (auto pSpace2 = line23.find(' '); pSpace2 != std::u8string_view::npos) {
                auto line2 = line23.substr(0, pSpace2);
                auto line3 = line23.substr(pSpace + 1);
                return { line1, line2, line3 };
            } else {
                return { line1, line23, {} };
            }
        }
        switch (abbr.size()) {
        case 4:
        case 6: {
                auto split = abbr.size() / 2;
                return { abbr.substr(0, split), abbr.substr(split), {} };
            }
        case 5: {
                auto split = 3;
                if (isdigit(abbr[2]) && isdigit(abbr[3]))
                    split = 2;
                return { abbr.substr(0, split), abbr.substr(split), {} };
            }
        default:
            return { abbr, {}, {} };
        }
    }

    void drawCustomAbbrText(QPainter* painter, const AbbrLines& sp,
            const QColor& color, QRectF rcFrame, qreal thickness,
            const AbbrTable& table)
    {
        // Draw text
        auto availSize = rcFrame.width();
        auto sz = availSize / sp.sizeQuo(table);
        QFont font { str::toQ(FAM_CONDENSED) };
            font.setStyleStrategy(QFont::PreferAntialias);
            font.setPixelSize(sz);
        painter->setFont(font);
        painter->setBrush(QBrush(color, Qt::SolidPattern));
        rcFrame.setLeft(rcFrame.left() + std::max(thickness, 1.0));
        switch (sp.nLines()) {
        case 1:
            painter->drawText(rcFrame, Qt::AlignCenter, str::toQ(sp.line1));
            break;
        case 2: {
                RcPair p(rcFrame, 2.5);
                painter->drawText(p.rc1, Qt::AlignCenter, str::toQ(sp.line1));
                painter->drawText(p.rc2, Qt::AlignCenter, str::toQ(sp.line2));
            } break;
        default: { // case 3
                RcPair p(rcFrame, 1.8);
                painter->drawText(p.rc1, Qt::AlignCenter, str::toQ(sp.line1));
                painter->drawText(rcFrame, Qt::AlignCenter, str::toQ(sp.line2));
                painter->drawText(p.rc2, Qt::AlignCenter, str::toQ(sp.line3));
            }
        }
    }

}   // anon namespace


///// RcPair ///////////////////////////////////////////////////////////////////


RcPair::RcPair(const QRectF& rcFrame, qreal quo)
        : rc1(rcFrame), rc2(rcFrame)
{
    auto dh = rcFrame.height() / quo;
    rc1.setBottom(rc1.bottom() - dh);
    rc2.setTop(rc2.top() + dh);
}


///// CharTile /////////////////////////////////////////////////////////////////


bool CharTile::isEmoji(const uc::GlyphStyleSets& glyphSets) const
{
    if (text.empty())
        return false;

    auto ch = EmojiPainter::getCp(text);
    if (ch) {
        if (ch.forceGraphic)
            return true;
        auto cp = uc::cpsByCode[ch.cp];
        // As of 2.0.1, glyphSets are unused, but may jump in
        return (cp && cp->drawMethod(emojiDraw, glyphSets) == uc::DrawMethod::SVG_EMOJI);
    } else {
        return (emojiDraw == uc::EmojiDraw::GRAPHIC);
    }
}


///// AbbrLines ////////////////////////////////////////////////////////////////


template <class T, size_t N>
constexpr T& last(T(&x)[N]) { return x[N-1]; }


qreal AbbrLines::sizeQuo(const AbbrTable& tb) const
{
    auto sz = nLines(), len = length();
    switch (sz) {
    case 3: return last(tb.quos);
    case 1:
        if (len == 1
            || (len == 2 && line1[0] >= 0b1100'0000 && line1[0] <= 0b1101'1111)     // 2-byte seq of UTF-8
            || (len == 3 && line1[0] >= 0b1110'0000 && line1[0] <= 0b1110'1111))    // 3-byte seq of UTF-8
            return tb.quos[0];
        [[fallthrough]];
    default:
        return tb.quos[len];
    }
}


///// WiCustomDraw /////////////////////////////////////////////////////////////


void WiCustomDraw::init()
{
    initialSize = minimumSize();
}

namespace {

    inline int emojiHeight(const QRect& r)
    {
        return r.height() * 17 / 20;
    }

}


void WiCustomDraw::paintEvent(QPaintEvent *event)
{
    Super::paintEvent(event);
    switch (mode) {
    case Mode::NONE: break;
    case Mode::ABBREVIATION: {
            QPainter painter(this);
            drawAbbreviation(&painter, geometry(), abbreviation,
                             palette().windowText().color());
        } break;
    case Mode::CUSTOM_CONTROL: {
            QPainter painter(this);
            drawCustomControl(&painter, geometry(), palette().windowText().color(),
                        uc::FontPlace::SAMPLE, subj);
        } break;
    case Mode::SPACE: {
            QPainter painter(this);
            drawSpace(&painter, geometry(), qfont,
                      palette().windowText().color(),
                      subj);
        } break;
    case Mode::EMOJI_CHAR: {
            QPainter painter(this);
            auto r = geometry();
            emp.draw(&painter, r, subj, emojiHeight(r),
                     palette().windowText().color());
        } break;
    case Mode::EMOJI_TEXT: {  // These are not equal: subj vs text
            QPainter painter(this);
            auto r = geometry();
            emp.draw(&painter, r, text, emojiHeight(r),
                     palette().windowText().color());
        } break;
    case Mode::VERTICAL: {
            QPainter painter(this);
            drawVertical(&painter, geometry(), qfont,
                      verticalAngle, palette().windowText().color(),
                      proxy.text);
        } break;
    case Mode::VIRTUAL_VIRAMA: {
            QPainter painter(this);
            drawVirtualVirama(&painter, geometry(), palette().windowText().color(),
                      FSZ_BIG, *uc::cpsByCode[subj]);
        } break;
    case Mode::SAMPLED_CONTROL: {
            QPainter painter(this);
            drawSampledControl(&painter, geometry(), proxy,
                               qfont, palette().windowText().color(),
                               uc::FontPlace::SAMPLE);
        } break;
    case Mode::CHAR_OVER_EMOJI: {
            QPainter painter(this);
            drawCharOverEmoji(&painter, geometry(), FSZ_BIG,
                      palette().windowText().color(), subj);
        } break;
    case Mode::GRAPHIC_SAMPLE: {        
            QPainter painter(this);
            painter.setFont(qfont);
            painter.setPen(palette().windowText().color());
            QRectF r = proxy.styleSheet.applyToGraphicSample(geometry(), qfont.pointSizeF());
            painter.drawText(r, Qt::AlignCenter, proxy.text);
        } break;
    }
}

void WiCustomDraw::setNormal()
{
    mode = Mode::NONE;
    setMinimumSize(initialSize);
}

void WiCustomDraw::setAbbreviation(std::u8string_view x)
{
    setNormal();
    mode = Mode::ABBREVIATION;
    abbreviation = x;
    update();
}


void WiCustomDraw::setCustomControl(char32_t aSubj)
{
    setNormal();
    mode = Mode::CUSTOM_CONTROL;
    subj = aSubj;
    update();
}


void WiCustomDraw::setVirtualVirama(char32_t cp)
{
    setNormal();
    mode = Mode::VIRTUAL_VIRAMA;
    subj = cp;
    update();
}


void WiCustomDraw::setSampledControl(const QFont& font, const uc::SampleProxy& pr)
{
    setNormal();
    mode = Mode::SAMPLED_CONTROL;
    proxy = pr;
    qfont = font;
    update();
}


void WiCustomDraw::setEmoji(char32_t aSubj)
{
    setNormal();
    mode = Mode::EMOJI_CHAR;
    subj = aSubj;
    update();
}


void WiCustomDraw::setEmoji(std::u32string_view aText)
{
    setNormal();
    mode = Mode::EMOJI_TEXT;
    text = aText;
    update();
}


void WiCustomDraw::setVertical(const QFont& font, const uc::SampleProxy& pr, int angle)
{
    verticalAngle = angle;
    proxy = pr;
    qfont = font;
    mode = Mode::VERTICAL;
    QFontMetrics metrics(font);
    auto h = metrics.height();
    setMinimumSize(QSize(h, h * 4 / 5));
    update();
}


void WiCustomDraw::setSpace(const QFont& font, char32_t aSubj)
{
    setSpace1(font, aSubj, Mode::SPACE);
}

void WiCustomDraw::setSpace1(const QFont& font, char32_t aSubj, Mode aMode)
{
    static constexpr auto SPACE_PLUS = 20;

    qfont = font;
    mode = aMode;
    subj = aSubj;

    // Set appropriate size
    auto dim = spaceDimensions(font, aSubj);
    setMinimumSize(QSize(
                std::max(initialSize.width(), dim.width() + SPACE_PLUS),
                std::max(initialSize.height(), dim.height())));
    update();
}


void WiCustomDraw::setCharOverEmoji(char32_t aSubj)
{
    subj = aSubj;
    mode = Mode::CHAR_OVER_EMOJI;
    update();
}


void WiCustomDraw::setGraphicSample(const QFont& font, const uc::SampleProxy& pr)
{
    qfont = font;
    proxy = pr;
    mode = Mode::GRAPHIC_SAMPLE;
    update();
}


///// Standalone functions /////////////////////////////////////////////////////


void drawDeprecated(QPainter* painter, const QRect& r)
{
    static constexpr int SZ = 8;    // we draw lines between pixel centers, actually +1
    static constexpr int OFS = 4;
    const int x1 = r.right() - OFS;
    const int x0 = x1 - SZ;
    const int y0 = r.top() + OFS;
    const int y1 = y0 + SZ;
    painter->setPen(FG_DEPRECATED);
    painter->drawLine(x0, y0, x1, y1);
    painter->drawLine(x0, y1, x1, y0);
}

void drawEmojiIcon(QPainter* painter, const QRect& r, char32_t subj)
{
    static constexpr int OFS = 0;
    static constexpr int SZ = 15;
    const int x1 = r.right() - OFS;
    const int x0 = x1 - SZ;
    const int y0 = r.top() + OFS;
    const int y1 = y0 + SZ;
    QRect r1 { QPoint{x0, y0}, QPoint{x1, y1} };
    // Blue — just because every such emoji is old and well-known
    emp.draw(painter, r1, subj, SZ, Qt::blue);
}

void drawAbbrText(QPainter* painter, std::u8string_view abbreviation,
        const QColor& color, QRectF rcFrame, qreal thickness)
{
    // Draw text
    auto lines = splitAbbr(abbreviation, SplitMode::NORMAL);
    drawCustomAbbrText(painter, lines, color, rcFrame, thickness, atAbbr);
}

struct Rc3Matrix {
    int side, side2, side3, x0, y0;
    Rc3Matrix(const QRectF& rcFrame) {
        auto sz = std::min(rcFrame.width(), rcFrame.height());
        side = std::round(sz / 5.5);
        side2 = side * 2;
        side3 = side * 3;
        auto cen = rcFrame.center();
        x0 = std::round(cen.x() - side * 1.5);
        y0 = std::round(cen.y() - side * 1.5);
    }

    //  789
    //  456
    //  123
    QRect rect7() const
        { return QRect( QPoint{ x0, y0 }, QSize { side, side }); }
    QRect rect96() const
        { return QRect( QPoint{ x0 + side2, y0 }, QSize { side, side2 }); }
    QRect rect13() const
        { return QRect( QPoint{ x0, y0 + side2 }, QSize { side3, side }); }
    QRect rect1() const
        { return QRect( QPoint{ x0, y0 + side2 }, QSize { side, side }); }
    QRect rect63() const
        { return QRect( QPoint{ x0 + side2, y0 + side }, QSize { side, side2 }); }
    QRect rect79() const
        { return QRect( QPoint{ x0, y0 }, QSize { side3, side }); }
    QRect rect74() const
        { return QRect( QPoint{ x0, y0 }, QSize { side, side2 }); }
    QRect rect9() const
        { return QRect( QPoint{ x0 + side2, y0 }, QSize { side, side }); }
    QRect rect41() const
        { return QRect( QPoint{ x0, y0 + side }, QSize { side, side2 }); }
    QRect rect3() const
        { return QRect( QPoint{ x0 + side2, y0 + side2 }, QSize { side, side }); }
    QRect rect5() const
        { return QRect( QPoint{ x0 + side, y0 + side }, QSize { side, side }); }
    QRect rect8() const
        { return QRect( QPoint{ x0 + side, y0 }, QSize { side, side }); }
    QRect rect2() const
        { return QRect( QPoint{ x0 + side, y0 + side2 }, QSize { side, side }); }
};

struct Rc6Matrix : public Rc3Matrix {
    int sideTwoThirds;
    int sideOneAndHalf;

    Rc6Matrix(const QRectF& rcFrame)
        : Rc3Matrix(rcFrame),
          sideTwoThirds(side2 / 3),
          sideOneAndHalf(side3 / 2) {}

    QRect topWall() const
        { return QRect( QPoint{ x0, y0 }, QSize { side3, sideTwoThirds } ); }
    QRect leftWall() const
        { return QRect( QPoint{ x0, y0 }, QSize { sideTwoThirds, side3 } ); }
    QRect bottomWall() const
        { return QRect( QPoint{ x0, y0 + side3 - sideTwoThirds }, QSize { side3, sideTwoThirds } ); }
    QRect rightWall() const
        { return QRect( QPoint{ x0 + side3 - sideTwoThirds, y0 }, QSize { sideTwoThirds, side3 } ); }
    QRect topHalf() const
        { return QRect( QPoint{ x0, y0 }, QSize { side3, sideOneAndHalf } ); }
    QRect bottomHalf() const
        { return QRect( QPoint{ x0, y0 + side3 - sideOneAndHalf }, QSize { side3, sideOneAndHalf } ); }
};

struct ControlFrame {
    QRectF r;
    qreal thickness;
};

namespace {

    inline QPointF pround(const QPointF& p)
    {
        return { std::round(p.x()), std::round(p.y()) };
    }

}

QRectF adjustedToPhysicalPixels(
        QPainter* painter, const QRectF& rect, qreal loFrame, BareFrame isBareFrame)
{
    // OK, as scale internally is fixed-point, and it IS taken from some settings,
    //    and it’s just a faster way (no multiplications and 1 division
    //    for devicePicelRatio)
    auto dpr = painter->device()->devicePixelRatio();
    if (dpr == 1.0) {
        // Adjust ORIGINALS OUTER corners to physical pixels
        QPointF corner0 { rect.left() - loFrame, rect.top() - loFrame };
        QPointF corner0round = pround(corner0);
        QPointF corner1 { rect.right() + loFrame, rect.bottom() + loFrame };
        QPointF corner1round = pround(corner1);
        // Set loFrame to 0.5 pixels AFTERWARDS
        if (isBareFrame != BareFrame::NO && loFrame < 0.5)
            loFrame = 0.5;
        return { QPointF { corner0round.x() + loFrame, corner0round.y() + loFrame },
                 QPointF { corner1round.x() - loFrame, corner1round.y() - loFrame } };
    } else {
        // Adjust ORIGINALS OUTER corners to physical pixels
        auto transform = painter->deviceTransform();
        auto corner0 = transform.map(QPointF(rect.left() - loFrame, rect.top() - loFrame));
        QPointF corner0round = pround(corner0);
        auto corner1 = transform.map(QPointF(rect.right() + loFrame, rect.bottom() + loFrame));
        QPointF corner1round = pround(corner1);
        auto inv = transform.inverted();
        QPointF corner0again = inv.map(corner0round);
        QPointF corner1again = inv.map(corner1round);
        // Set loFrame to 0.5 physical pixels AFTERWARDS
        if (isBareFrame != BareFrame::NO) {
            auto thresholdFrame = loFrame * 0.5 / dpr;
            if (loFrame < thresholdFrame)
                loFrame = thresholdFrame;
        }
        return { QPointF { corner0again.x() + loFrame, corner0again.y() + loFrame },
                 QPointF { corner1again.x() - loFrame, corner1again.y() - loFrame } };
    }
}

enum class Thinner { NO, YES };

ControlFrame drawControlFrame(
        QPainter* painter, const QRect& rect, QColor color,
        Thinner thinner)
{
    // Get painter info
    auto dpr = painter->device()->devicePixelRatio();

    const auto availW = rect.width() * (5.0 / 6.0);
    const auto availH = rect.height() * (7.0 / 10.0);
    const auto availSize = std::min(availW, availH);
    // Now we draw availSize×availSize, shrink rect
    auto ofsX = (rect.width() - availSize) / 2;
    auto ofsY = (rect.height() - availSize) / 2;
    QRectF rcFrame { QPointF(rect.left() + ofsX, rect.top() + ofsY),
                     QSizeF(availSize, availSize) };
    // Draw frame
    static constexpr qreal Q_THICKNESS = 1.0 / 88.0;  // The greatest number when on 1× it’s still 1px
    auto loThickness = availW * Q_THICKNESS;
    auto hiThickness = loThickness * dpr;  // IDK why dpr, but it scales pen better
    bool isAa = (dpr > 1)
            ? (hiThickness > 1.0)
            : (hiThickness > 1.3);  // 1.25 — still no anti-alias
    if (dpr > 1 && hiThickness > 1 && hiThickness <= 1.3) {
        //hiThickness = 1.0;  unused for now
        loThickness = 1.0 / dpr;
    }
    painter->setRenderHint(QPainter::Antialiasing, isAa);
    auto rcRetFrame = rcFrame;
    rcFrame = adjustedToPhysicalPixels(painter, rcFrame, loThickness * 0.5, BareFrame::YES);
    if (thinner != Thinner::NO)
        color.setAlphaF(color.alphaF() * 0.25);
    painter->setPen(QPen(color, loThickness, Qt::DashLine));
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(rcFrame);

    return { rcRetFrame, loThickness };
}

void drawFunkySample(
        QPainter* painter, const QRectF& rect, const QColor& color,
        uc::FontPlace place, float quo, QChar sample)
{
    auto fn = uc::funkyFont(place, 50);
    fn.setPixelSize(std::round(rect.width() * 0.75f * quo));
    painter->setFont(fn);
    painter->setBrush(QBrush(color, Qt::SolidPattern));
    painter->drawText(rect, Qt::AlignCenter, sample);
}

void drawTagText(QPainter* painter, std::u8string_view abbreviation,
        const QColor& color, const QRectF& rcFrame, qreal thickness,
        uc::FontPlace place)
{
    // Draw text
    AbbrLines lines { abbreviation, u8" ", {} };
    drawCustomAbbrText(painter, lines, color, rcFrame, thickness, atTags);
    drawFunkySample(painter, rcFrame, color, place, 1.2f, uc::STUB_PUA_TAG);
}

void drawCustomControl(
        QPainter* painter, const QRect& rect, const QColor& color,
        uc::FontPlace place, char32_t subj)
{
    auto [rcFrame, thickness] = drawControlFrame(painter, rect, color, Thinner::NO);
    // Need this brush for both rects and fonts

    switch (subj) {
        // All subjoiners are now drawn through VIRTUAL_VIRAMA
        //drawFunkySample(painter, rcFrame, color, place, 1.0f, uc::STUB_PUA_VIRAMA);
    case 0x303E:    // ideographic variation indicator
        drawFunkySample(painter, rcFrame, color, place, 1.0f, uc::STUB_PUA_CJK_APPROX);
        break;
    case 0x16F8F:   // Miao tone right
        drawFunkySample(painter, rcFrame, color, place, 1.0f, uc::STUB_PUA_TONE_RIGHT);
        break;
    case 0x16F90:   // Miao tone top right
        drawFunkySample(painter, rcFrame, color, place, 1.0f, uc::STUB_PUA_TONE_TOPRIGHT);
        break;
    case 0x16F91:   // Miao tone above
        drawFunkySample(painter, rcFrame, color, place, 1.0f, uc::STUB_PUA_TONE_ABOVE);
        break;
    case 0x16F92:   // Miao tone below
        drawFunkySample(painter, rcFrame, color, place, 1.0f, uc::STUB_PUA_TONE_BELOW);
        break;
    case 0x1BCA0:   // Shorthand format Letter overlap
        drawFunkySample(painter, rcFrame, color, place, 1.0f, uc::STUB_PUA_OVERLAP);
        break;
    case 0x1BCA1:   // Shorthand format Continuing overlap
        drawFunkySample(painter, rcFrame, color, place, 1.0f, uc::STUB_PUA_CONTINUING_OVERLAP);
        break;
    case 0x13431:   // Egyptian hiero horz joiner
        drawFunkySample(painter, rcFrame, color, place, 1.0f, uc::STUB_PUA_BIG_STAR);
        break;
    case 0x13432: { // Egyptian hieroglyph Insert at top start
            Rc3Matrix m(rcFrame);
            painter->fillRect(m.rect7(), color);
            painter->fillRect(m.rect96(), color);
            painter->fillRect(m.rect13(), color);
        } break;
    case 0x13433: { // Egyptian hieroglyph Insert at bottom start
            Rc3Matrix m(rcFrame);
            painter->fillRect(m.rect79(), color);
            painter->fillRect(m.rect63(), color);
            painter->fillRect(m.rect1(), color);
        } break;
    case 0x13434: { // Egyptian hieroglyph Insert at top end
            Rc3Matrix m(rcFrame);
            painter->fillRect(m.rect74(), color);
            painter->fillRect(m.rect9(), color);
            painter->fillRect(m.rect13(), color);
        } break;
    case 0x13435: { // Egyptian hieroglyph Insert at bottom end
            Rc3Matrix m(rcFrame);
            painter->fillRect(m.rect79(), color);
            painter->fillRect(m.rect41(), color);
            painter->fillRect(m.rect3(), color);
        } break;
    case 0x13439: { // Egyptian hieroglyph Insert at middle
            Rc6Matrix m(rcFrame);
            painter->fillRect(m.topWall(), color);
            painter->fillRect(m.leftWall(), color);
            painter->fillRect(m.rightWall(), color);
            painter->fillRect(m.bottomWall(), color);
            painter->fillRect(m.rect5(), color);
        } break;
    case 0x1343A: { // Egyptian hieroglyph Insert at top
            Rc6Matrix m(rcFrame);
            painter->fillRect(m.rect8(), color);
            painter->fillRect(m.leftWall(), color);
            painter->fillRect(m.rightWall(), color);
            painter->fillRect(m.bottomHalf(), color);
        } break;
    case 0x1343B: { // Egyptian hieroglyph Insert at bottom
            Rc6Matrix m(rcFrame);
            painter->fillRect(m.rect2(), color);
            painter->fillRect(m.leftWall(), color);
            painter->fillRect(m.rightWall(), color);
            painter->fillRect(m.topHalf(), color);
        } break;
    case 0x1343C:   // Egyptian hieroglyph Begin enclosure
        drawFunkySample(painter, rcFrame, color, place, 1.0f, uc::STUB_PUA_ENCLOSURE_START);
        break;
    case 0x1343D:   // Egyptian hieroglyph End enclosure
        drawFunkySample(painter, rcFrame, color, place, 1.0f, uc::STUB_PUA_ENCLOSURE_END);
        break;
    case 0x1343E:   // Egyptian hieroglyph Begin walled enclosure
        drawFunkySample(painter, rcFrame, color, place, 1.0f, uc::STUB_PUA_WALL_START);
        break;
    case 0x1343F:   // Egyptian hieroglyph End walled enclosure
        drawFunkySample(painter, rcFrame, color, place, 1.0f, uc::STUB_PUA_WALL_END);
        break;
    case 0xE0001:   // Language tag
        drawTagText(painter, u8"BEGIN"sv, color, rcFrame, thickness, place);
        break;
    case 0xE0020:   // Tag Space
        drawTagText(painter, u8"SP"sv, color, rcFrame, thickness, place);
        break;
    case 0xE007F:   // Cancel tag
        drawTagText(painter, u8"END"sv, color, rcFrame, thickness, place);
        break;
    default:
        // Tags
        if (subj >= 0xE0000 && subj <= 0xE00FF) {
            char8_t c = subj;
            std::u8string_view line1 { &c, 1 };
            drawTagText(painter, line1, color, rcFrame, thickness, place);
        }
    }
}

void drawAbbreviation(
        QPainter* painter, const QRect& rect, std::u8string_view abbreviation,
        const QColor& color)
{
    auto [rcFrame, thickness] = drawControlFrame(painter, rect, color, Thinner::NO);
    drawAbbrText(painter, abbreviation, color, rcFrame, thickness);
}


namespace {

    QRect matchRect(const QRect& x, const uc::StyleSheet& y)
    {
        if (y.topPc == y.botPc)
            return x;
        float size100 = x.height() * 100.0f / (100.0f + y.topPc + y.botPc);
        if (y.topPc > y.botPc) {
            auto r = x;
            r.setTop(r.top() + round(size100 * (y.topPc - y.botPc) / 100.0f ));
            return r;
        } else {
            auto r = x;
            r.setBottom(r.bottom() - round(size100 * (y.botPc - y.topPc) / 100.0f));
            return r;
        }
    }

}   // anon namespace


void drawVirtualVirama(
        QPainter* painter, const QRect& rect,
        const QColor& color, int absSize, const uc::Cp& cp)
{
    // Frame
    auto [rcFrame, thickness] = drawControlFrame(painter, rect, color, Thinner::NO);

    // Dotted circle; sizePc is always 100
    auto* ucfont = cp.font(match::Normal::INST);
    auto qfont = ucfont->get(uc::FontPlace::SAMPLE, absSize, NO_FLAGS, &cp);
    QFontMetrics metrics(qfont);
    static constexpr QChar DC {cp::DOTTED_CIRCLE};
    auto tightRect = metrics.boundingRect(DC);
    auto deltaY = metrics.ascent() - metrics.descent();
    auto width = metrics.horizontalAdvance(DC);
    painter->setFont(qfont);
    painter->setBrush(color);
    // Not rcFrame!!
    auto rcMatch = matchRect(rect, ucfont->styleSheet);
    auto& blk = cp.block();
    if (blk.flags.have(uc::Bfg::VIRAMA_UP)) {
        rcMatch.setBottom(rcMatch.bottom() - tightRect.height() / 2);
    }
    auto cen = QRectF(rcMatch).center();
    auto baseY = cen.y() + deltaY * 0.5;
    // Though we known exact size of char, still draw at rcMatch
    // (there’s sub-pixel difference)
    painter->drawText(rcMatch, Qt::AlignCenter, DC);

    // Limit offset
    auto loY = baseY + tightRect.bottom();
    auto hiY = rcFrame.bottom();
    // Plus
    if (blk.flags.have(uc::Bfg::VIRAMA_INSIDE)) {
        loY = baseY + tightRect.top() - tightRect.height();
        hiY = baseY + tightRect.bottom() + tightRect.height();
    } else {
        cen.setY(cen.y() + width * 0.5f);
        rcFrame.moveCenter(cen);
    }

    auto& font1 = uc::fontInfo[static_cast<int>(uc::EcFont::FUNKY)];
    auto plusSize = absSize;
    if (blk.flags.have(uc::Bfg::VIRAMA_BIGGER))
        plusSize = plusSize * 14 / 10;
    QFont font2 = font1.get(uc::FontPlace::CELL, plusSize, {}, nullptr);
    painter->setFont(font2);
    painter->drawText(QPointF ( cen.x(), (loY + hiY) * 0.5f ), uc::STUB_PUA_PLUS);
}


namespace {

    void drawChar1(QPainter* painter, const QRect& rect, int absSize,
                   const QColor& color, const uc::Cp* cp, bool isUpperLayer)
    {
        if (!cp) {
            // Draw on upper layer only
            if (isUpperLayer)
                emp.drawEmojiTofu(painter, rect, color);
            return;
        }
        auto& block = cp->block();
        bool wantDraw = isUpperLayer ^ block.flags.have(uc::Bfg::EMOJI_OVER_CHAR);
        if (!wantDraw)
            return;

        auto font = cp->font(match::Normal::INST);
        auto qfont = font->get(uc::FontPlace::SAMPLE, absSize, NO_FLAGS, cp);
        auto rect2 = font->styleSheet.applyToGraphicSample(rect, qfont.pointSizeF());
        painter->setFont(qfont);
        painter->setPen(color);
        painter->drawText(rect2, Qt::AlignCenter, str::toQ(cp->subj.ch32()));
    }

}   // anon namespace

void drawCharOverEmoji(
        QPainter* painter, const QRect& rect, int absSize,
        const QColor& color, char32_t subj)
{
    // Character
    auto cp = uc::cpsByCode[subj];
    drawChar1(painter, rect, absSize, color, cp, false);

    // Emoji
    auto loSize = std::min(rect.width(), rect.height());
    if (loSize <= 0)  // Maybe this’ll trigger nicer division, [[assume]] is C++23
        return;
    auto emojiSize = loSize * 2 / 5;
    QRect rect1 = rect;
    rect1.setBottomLeft({rect.right() - emojiSize, rect.top() + emojiSize});
    emp.draw(painter, rect1, subj, emojiSize, color);

    drawChar1(painter, rect, absSize, color, cp, true);
}


void drawSampledControl(
        QPainter* painter, const QRect& rect, const uc::SampleProxy& proxy,
        const QFont& font, const QColor& color, uc::FontPlace place)
{
    drawControlFrame(painter, rect, color,
            (place == uc::FontPlace::SAMPLE) ? Thinner::YES : Thinner::NO);
    auto rcMatch = matchRect(rect, proxy.styleSheet);
    painter->setFont(font);
    painter->setPen(color);
    painter->drawText(rcMatch, Qt::AlignCenter, proxy.text);
}


QSize spaceDimensions(const QFont& font, char32_t subj)
{
    QString s = QString::fromUcs4(&subj, 1);
    QFontMetrics metrics(font);
    return {   // Spaces are SPACING → ban 0
               // All known spaces are direction-neutral and thus LTR, but who knows?
        std::max(std::abs(metrics.horizontalAdvance(s)), 1),
        metrics.height() * 4 / 5 };
}

void drawSpace(
        QPainter* painter, const QRect& rect,
        const QFont& font, QColor color, char32_t subj)
{
    painter->setRenderHint(QPainter::Antialiasing, painter->device()->devicePixelRatio() != 1.0);
    // Quotient to convert space height to line thickness
    constexpr auto Q_H2THICK = 1.0 / 25.0;

    auto dim = spaceDimensions(font, subj);
    color.setAlpha(ALPHA_SPACE);
    auto lineW = std::max(
                static_cast<int>(std::round(dim.height() * Q_H2THICK)), 1);
    auto x = rect.left() - lineW + (rect.width() - dim.width()) / 2;
    auto y = rect.top() + (rect.height() - dim.height()) / 2;
    QBrush brush(color);
    painter->fillRect(QRectF(x, y, lineW, dim.height()), brush);
    x += lineW;
    x += dim.width();
    painter->fillRect(QRectF(x, y, lineW, dim.height()), brush);
}


std::optional<QFont> fontAt(
        uc::DrawMethod drawMethod,
        int sizePc,
        const uc::Cp& cp)
{
    if (drawMethod > uc::DrawMethod::LAST_FONT)
        return {};
    auto font = cp.font(match::Normal::INST);
    Flags<uc::FontGetFg> flags;
    if (cp.isNoAa())
        flags |= uc::FontGetFg::NO_AA;
    if (cp.flags.have(uc::Cfg::DYN_SYSTEM_TOFU))
        flags |= uc::FontGetFg::KNOWN_TOFU;
    auto r = font->get(uc::FontPlace::CELL, FSZ_TABLE * sizePc / 100,
                    flags, &cp);
    if (sizePc <= 80 && font->flags.have(uc::Ffg::NOHINT_TINY))
        r.setHintingPreference(QFont::PreferNoHinting);
    return r;
}


std::optional<QFont> fontAt(
        uc::EmojiDraw emojiMode, const uc::Cp& cp,
        const uc::GlyphStyleSets& glyphSets)
{
    auto method = cp.drawMethod(emojiMode, glyphSets);
    return fontAt(method, 100, cp);
}


constexpr int EMOJI_NUM = 4;
constexpr int EMOJI_DEN = 5;


void drawSample(QPainter* painter, QRect rect, int sizePc, const uc::Cp& cp,
                const QColor& color, uc::EmojiDraw emojiMode,
                const uc::GlyphStyleSets& glyphSets, float offset)
{
    auto font = fontAt(uc::DrawMethod::SAMPLE, sizePc, cp);
    if (font)
        painter->setFont(*font);
    painter->setBrush(color);
    painter->setPen(color);
    if (offset != 0 && font) {
        if (offset < 0) {
            rect.setTop(rect.top() + std::round(offset * font->pointSize()));
        } else {
            rect.setBottom(rect.bottom() + std::round(offset * font->pointSize()));
        }
    }
    painter->drawText(rect,
                      Qt::AlignCenter | Qt::TextSingleLine | Qt::TextIncludeTrailingSpaces,
                      cp.sampleProxy(uc::ProxyType::EXTENDED, emojiMode, glyphSets).text);
}

namespace {

    void drawMultiSample(QPainter* painter, QRect rect, int sizePc, std::u32string_view s,
                    const QColor& color)
    {
        if (s.empty())
            return;
        auto c = s[0];
        if (c == cp::DOTTED_CIRCLE && s.length() > 1) {
            c = s[1];
        }
        auto cp = uc::cpsByCode[c];
        auto font = fontAt(uc::DrawMethod::SAMPLE, sizePc, *cp);
        if (font)
            painter->setFont(*font);
        painter->setBrush(color);
        painter->setPen(color);
        painter->drawText(rect,
                          Qt::AlignCenter | Qt::TextSingleLine | Qt::TextIncludeTrailingSpaces,
                          str::toQ(s));
    }

}


void drawChar(
        QPainter* painter, const QRect& rect, int sizePc, const uc::Cp& cp,
        const QColor& color, TableDraw tableMode, uc::EmojiDraw emojiMode,
        const uc::GlyphStyleSets& glyphSets, UseMargins useMargins)
{
    auto method = cp.drawMethod(emojiMode, glyphSets);
    switch (method) {
    case uc::DrawMethod::VIRTUAL_VIRAMA:
        drawVirtualVirama(painter, rect, color, FSZ_TABLE, cp);
        break;
    case uc::DrawMethod::CUSTOM_CONTROL:
        drawCustomControl(painter, rect, color, uc::FontPlace::CELL, cp.subj);
        break;
    case uc::DrawMethod::ABBREVIATION:
        drawAbbreviation(painter, rect, cp.abbrev(), color);
        break;
    case uc::DrawMethod::SPACE:
        drawSpace(painter, rect, *fontAt(method, sizePc, cp), color, cp.subj);
        break;
    case uc::DrawMethod::VERTICAL_CW:
    case uc::DrawMethod::VERTICAL_CCW: {
            auto angle = (method == uc::DrawMethod::VERTICAL_CW) ? ROT_CW : ROT_CCW;
            auto proxy = cp.sampleProxy(uc::ProxyType::EXTENDED,
                        uc::EmojiDraw::MOSTLY_TEXT, uc::GlyphStyleSets::EMPTY);
            drawVertical(painter, rect, *fontAt(method, sizePc, cp), angle, color,
                         proxy.text);
        } break;
    case uc::DrawMethod::MARCHEN:
        drawSample(painter, rect, sizePc, cp, color, emojiMode, glyphSets, -0.3);
        break;
    case uc::DrawMethod::SAMPLED_CONTROL:
        drawControlFrame(painter, rect, color, Thinner::NO);
        drawSample(painter, rect, sizePc, cp, color, emojiMode, glyphSets, 0);
        break;
    case uc::DrawMethod::SAMPLE:
        if (tableMode != TableDraw::INTERNAL) {
            drawSample(painter, rect, sizePc, cp, color, emojiMode, glyphSets, 0);
        }
        if (cp.isVs16Emoji() && tableMode != TableDraw::LIBRARY) {
            drawEmojiIcon(painter, rect, cp.subj);
        }
        break;
    case uc::DrawMethod::SVG_EMOJI: {
            //auto font = fontAt(*uc::cpsByCode[static_cast<int>('!')]);
            auto h = rect.height();
            if (useMargins != UseMargins::NO) {
                h = h * EMOJI_NUM / EMOJI_DEN;
            }
            emp.draw(painter, rect, cp.subj.ch32(), h, color);
        } break;
    }
    if (cp.isDeprecated())
        drawDeprecated(painter, rect);
}


void drawIconChars(
        QPainter* painter, const QRect& rect, int sizePc, std::u32string_view s,
        const QColor& color)
{
    switch (s.length()) {
    case 0: break;
    case 1:
        drawChar(painter, rect, sizePc, *uc::cpsByCode[s[0]], color, TableDraw::CUSTOM,
                uc::EmojiDraw::GRAPHIC, uc::GlyphStyleSets::EMPTY);
        break;
    default:
        drawMultiSample(painter, rect, sizePc, s, color);
    }
}


void drawCharBorder(QPainter* painter, const QRect& rect, const QColor& color)
{
    // Create transparent color
    QColor clTrans(color);
    clTrans.setAlpha(ALPHA_BORDER);

    // Draw bounds
    auto bounds1 = rect;
    bounds1.adjust(0, 0, -1, -1);
    painter->setBrush(Qt::NoBrush);
    painter->setPen(clTrans);
    painter->drawRect(bounds1);
}


void drawMurkyRect(QPainter* painter, const QRect& rect, const QColor& color)
{
    // Create transparent color
    QColor clTrans(color);
    clTrans.setAlpha(ALPHA_INTERNATIONAL);
    painter->fillRect(rect, clTrans);
    drawCharBorder(painter, rect, color);
}


void drawSearchChar(
        QPainter* painter, const QRect& rect, const uc::Cp* cp,
        const QColor& color, uc::EmojiDraw emojiMode,
        const uc::GlyphStyleSets& glyphSets, qreal scale)
{
    drawCharBorder(painter, rect, color);
    if (cp)
        drawChar(painter, rect, lround(100 * scale), *cp, color, TableDraw::SEARCH, emojiMode, glyphSets);
}


void drawSearchChars(
        QPainter* painter, const QRect& rect, std::u32string_view text,
        const QColor& color, uc::EmojiDraw emojiMode, TableDraw tableDraw,
        const uc::GlyphStyleSets& glyphSets, qreal scale)
{
    if (emojiMode == uc::EmojiDraw::FORCE_TEXT) {
        std::cout << "Force text" << std::endl;
    }

    drawCharBorder(painter, rect, color);
    if (text.empty())
        return;

    auto c1 = EmojiPainter::getCp(text);
    /// @todo [future] This is just check whether it’s a single character
    ///    (all multi-chars are emoji)
    if (c1.mayBeText(emojiMode)) {
        if (auto cp = uc::cpsByCode[c1.cp])
            drawChar(painter, rect, lround(100 * scale), *cp, color, tableDraw, emojiMode, glyphSets);
    } else {
        // Graphic
        auto h = rect.height() * EMOJI_NUM / EMOJI_DEN;
        emp.draw(painter, rect, text, h, color);
    }
}


void drawEmojiDirect(
        QPainter* painter, const QRect& rect, std::u32string_view text,
        const QColor& clTofu)
{
    emp.draw(painter, rect, text, rect.height(), clTofu);
}


void drawCharTiles(
        QPainter* painter, const QRect& rect,
        const CharTiles& tiles, const QColor& color,
        const uc::GlyphStyleSets& glyphSets, qreal scale)
{
    static constexpr int SIZE_PC = 70;  /// size percentage
    static constexpr int SIDE_GAP = 2;
    static constexpr int INNER_GAP = 1;
    static constexpr int SUBDIV = 2;
    const auto percent = llround(SIZE_PC * scale);
    drawCharBorder(painter, rect, color);
    QMargins m;  m += SIDE_GAP;
    QRect r1 = rect.marginsRemoved(m);
    auto sz = std::min(r1.width(), r1.height());
    // adjust to square
    r1.setLeft(r1.left() + (r1.width() - sz) / 2);
    r1.setTop(r1.top() + (r1.height() - sz) / 2);
    sz -= INNER_GAP;
    sz /= SUBDIV;
    auto step = sz + INNER_GAP;
    for (unsigned iTile = 0; iTile < std::size(tiles); ++iTile) {
        auto& tile = tiles[iTile];
        if (tile.text.empty())
            break;
        int iX = iTile % SUBDIV;
        int iY = iTile / SUBDIV;
        QRect r2 { r1.left() + iX * step,
                   r1.top() + iY * step,
                   sz, sz };
        /// @todo [future] This is just check whether it’s a single character
        ///    (all multi-chars are emoji)
        if (auto c1 = EmojiPainter::getCp(tile.text); c1.mayBeText()) {
            // Single-char
            if (auto cp = uc::cpsByCode[c1.cp])
                drawChar(painter, r2, percent, *cp, color, TableDraw::LIBRARY,
                         tile.emojiDraw, glyphSets, UseMargins::NO);
        } else {
            // Multi-char
            emp.draw(painter, r2, tile.text, sz, color);
        }
    }
}

namespace {

    const uc::LibNode* goToText(const uc::LibNode& x)
    {
        if (x.flags.have(uc::Lfg::NO_TILE))
            return nullptr;
        const uc::LibNode* p = &x;
        while (true) {
            if (!p->value.empty())
                return p;
            if (p->nChildren == 0)
                return nullptr;
            p = &uc::libNodes[p->iFirstChild];
        }
    }

}   // anon namespace


CharTiles getCharTiles(const uc::LibNode& node)
{
    CharTiles tiles;
    if (node.value.empty()) {
        size_t iTile = 0;
        for (int iChild = 0; iChild < node.nChildren; ++iChild) {
            if (auto textNode = goToText(uc::libNodes[node.iFirstChild + iChild])) {
                auto& tile = tiles[iTile];
                tile.text = textNode->value;
                tile.emojiDraw = textNode->emojiDraw();
                if (++iTile >= std::size(tiles))
                    break;
            }
        }
    }
    return tiles;
}


void drawFolderTile(
        QPainter* painter, const QRect& bounds,
        const uc::LibNode& node, const QColor& color,
        const uc::GlyphStyleSets& glyphSets, qreal scale)
{
    CharTiles tiles = getCharTiles(node);
    drawCharTiles(painter, bounds, tiles, color, glyphSets, scale);
}


void drawVertical(
        QPainter* painter, const QRect& rect,
        const QFont& font, int rotation, QColor color, const QString& subj)
{
    auto trans = painter->transform();
    painter->rotate(rotation);
    auto newTrans = painter->transform().inverted();
    auto newRect = newTrans.mapRect(rect);
    painter->setFont(font);
    painter->setPen(color);
    static constexpr auto flags = Qt::AlignCenter | Qt::TextSingleLine | Qt::TextIncludeTrailingSpaces;
    painter->drawText(newRect, flags, subj);
    painter->setTransform(trans);
}
