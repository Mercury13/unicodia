// My header
#include "routines.h"

// Qt
#include <QPainter>

// Libs
#include "u_Qstrings.h"

// Project-local
#include "Skin.h"

using namespace std::string_view_literals;

EmojiPainter emp;


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


bool CharTile::isEmoji() const
{
    if (text.empty())
        return false;

    auto ch = EmojiPainter::getCp(text);
    if (ch) {
        auto cp = uc::cpsByCode[ch];
        return (cp && cp->drawMethod(emojiDraw) == uc::DrawMethod::SVG_EMOJI);
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
            drawSpace(&painter, geometry(), fontSpace,
                      palette().windowText().color(),
                      subj);
        } break;
    case Mode::EGYPTIAN_HATCH: {
            QPainter painter(this);
            drawEgyptianHatch(&painter, geometry(), fontSpace,
                      palette().windowText().color(),
                      subj);
        } break;
    case Mode::EMOJI_CHAR: {
            QPainter painter(this);
            auto r = geometry();
            emp.draw(&painter, r, subj, emojiHeight(r));
        } break;
    case Mode::EMOJI_TEXT: {
            QPainter painter(this);
            auto r = geometry();
            emp.draw(&painter, r, text, emojiHeight(r));
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


void WiCustomDraw::setEgyptianHatch(const QFont& font, char32_t aSubj)
{
    setSpace1(font, aSubj, Mode::EGYPTIAN_HATCH);
}


void WiCustomDraw::setSpace(const QFont& font, char32_t aSubj)
{
    setSpace1(font, aSubj, Mode::SPACE);
}

void WiCustomDraw::setSpace1(const QFont& font, char32_t aSubj, Mode aMode)
{
    static constexpr auto SPACE_PLUS = 30;

    fontSpace = font;
    mode = aMode;
    subj = aSubj;

    // Set appropriate size
    auto dim = spaceDimensions(font, aSubj);
    setMinimumSize(QSize(
                std::max(initialSize.width(), dim.width() + SPACE_PLUS),
                std::max(initialSize.height(), dim.height())));
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

    QRectF adjustedToPhysicalPixels(const QRectF& rect, qreal scale, qreal frame)
    {
        auto x0 = std::round((rect.left()   - frame) * scale);
        auto y0 = std::round((rect.top()    - frame) * scale);
        auto x1 = std::round((rect.right()  + frame) * scale);
        auto y1 = std::round((rect.bottom() + frame) * scale);
        auto recipScale = 1.0 / scale;
        return { QPointF { x0 * recipScale + frame, y0 * recipScale + frame },
                 QPointF { x1 * recipScale - frame, y1 * recipScale - frame } };
    }

}

ControlFrame drawControlFrame(
        QPainter* painter, const QRect& rect, const QColor& color)
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
    static constexpr qreal Q_THICKNESS = 1.0 / 60.0;
    auto loThickness = availSize * Q_THICKNESS;
    auto hiThickness = loThickness * dpr;  // IDK why dpr, but it scales pen better
    bool isAa = (dpr != 1.0 && hiThickness * dpr > 1);
    painter->setRenderHint(QPainter::Antialiasing, isAa);
    if (isAa) {
        rcFrame = adjustedToPhysicalPixels(rcFrame, dpr, hiThickness * 0.5);
    }
    painter->setPen(QPen(color, hiThickness, Qt::DashLine));
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(rcFrame);

    return { rcFrame, loThickness };
}

void drawFunkySample(
        QPainter* painter, const QRectF& rect, const QColor& color,
        uc::FontPlace place, float quo, char32_t trigger, QChar sample)
{
    auto fn = uc::funkyFont(place, 50, trigger);
    fn.setPixelSize(std::round(rect.width() * 0.75f * quo));
    painter->setFont(fn);
    painter->setBrush(QBrush(color, Qt::SolidPattern));
    painter->drawText(rect, Qt::AlignCenter, sample);
}

void drawTagText(QPainter* painter, std::u8string_view abbreviation,
        const QColor& color, const QRectF& rcFrame, qreal thickness,
        uc::FontPlace place, char32_t trigger)
{
    // Draw text
    AbbrLines lines { abbreviation, u8" ", {} };
    drawCustomAbbrText(painter, lines, color, rcFrame, thickness, atTags);
    drawFunkySample(painter, rcFrame, color, place, 1.2f, trigger, uc::STUB_PUA_TAG);
}

void drawCustomControl(
        QPainter* painter, const QRect& rect, const QColor& color,
        uc::FontPlace place, char32_t subj)
{
    auto [rcFrame, thickness] = drawControlFrame(painter, rect, color);
    // Need this brush for both rects and fonts

    switch (subj) {
    case 0x1039:    // Myanmar virtual virama
    case 0x11D45:   // Masaram Gondi virama
    case 0x11D97:   // Gunjala Gondi virama
        drawFunkySample(painter, rcFrame, color, place, 1.0f, subj, uc::STUB_PUA_VIRAMA);
        break;
    case 0x16F8F:   // Miao tone right
        drawFunkySample(painter, rcFrame, color, place, 1.0f, subj, uc::STUB_PUA_TONE_RIGHT);
        break;
    case 0x16F90:   // Miao tone top right
        drawFunkySample(painter, rcFrame, color, place, 1.0f, subj, uc::STUB_PUA_TONE_TOPRIGHT);
        break;
    case 0x16F91:   // Miao tone above
        drawFunkySample(painter, rcFrame, color, place, 1.0f, subj, uc::STUB_PUA_TONE_ABOVE);
        break;
    case 0x16F92:   // Miao tone below
        drawFunkySample(painter, rcFrame, color, place, 1.0f, subj, uc::STUB_PUA_TONE_BELOW);
        break;
    case 0x1BCA0:   // Shorthand format Letter overlap
        drawFunkySample(painter, rcFrame, color, place, 1.0f, subj, uc::STUB_PUA_OVERLAP);
        break;
    case 0x1BCA1:   // Shorthand format Continuing overlap
        drawFunkySample(painter, rcFrame, color, place, 1.0f, subj, uc::STUB_PUA_CONTINUING_OVERLAP);
        break;
    case 0x13431:   // Egyptian hiero horz joiner
        drawFunkySample(painter, rcFrame, color, place, 1.0f, subj, uc::STUB_PUA_BIG_STAR);
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
        drawFunkySample(painter, rcFrame, color, place, 1.0f, subj, uc::STUB_PUA_ENCLOSURE_START);
        break;
    case 0x1343D:   // Egyptian hieroglyph End enclosure
        drawFunkySample(painter, rcFrame, color, place, 1.0f, subj, uc::STUB_PUA_ENCLOSURE_END);
        break;
    case 0x1343E:   // Egyptian hieroglyph Begin walled enclosure
        drawFunkySample(painter, rcFrame, color, place, 1.0f, subj, uc::STUB_PUA_WALL_START);
        break;
    case 0x1343F:   // Egyptian hieroglyph End walled enclosure
        drawFunkySample(painter, rcFrame, color, place, 1.0f, subj, uc::STUB_PUA_WALL_END);
        break;
    case 0xE0001:   // Language tag
        drawTagText(painter, u8"BEGIN"sv, color, rcFrame, thickness,
                    place, subj);
        break;
    case 0xE0020:   // Tag Space
        drawTagText(painter, u8"SP"sv, color, rcFrame, thickness,
                    place, subj);
        break;
    case 0xE007F:   // Cancel tag
        drawTagText(painter, u8"END"sv, color, rcFrame, thickness,
                    place, subj);
        break;
    default:
        // Tags
        if (subj >= 0xE0000 && subj <= 0xE00FF) {
            char8_t c = subj;
            std::u8string_view line1 { &c, 1 };
            drawTagText(painter, line1, color, rcFrame, thickness,
                        place, subj);
        }
    }
}

void drawAbbreviation(
        QPainter* painter, const QRect& rect, std::u8string_view abbreviation,
        const QColor& color)
{
    auto [rcFrame, thickness] = drawControlFrame(painter, rect, color);
    drawAbbrText(painter, abbreviation, color, rcFrame, thickness);
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

void drawEgyptianHatch(
        QPainter* painter, const QRect& rect,
        const QFont& font, QColor color, char32_t subj)
{
    painter->setFont(font);
    painter->setBrush(color);
    painter->setPen(color);
    painter->drawText(rect,
                      Qt::AlignCenter | Qt::TextSingleLine,
                      uc::STUB_PUA_BIG_CIRCLE);

    QFont font1 = font;
    font1.setStyleStrategy(QFont::NoAntialias);
    painter->setFont(font1);
    color.setAlpha(ALPHA_EGYPTIAN_HATCH);
    painter->setBrush(color);
    painter->setPen(color);
    painter->drawText(rect,
                      Qt::AlignCenter | Qt::TextSingleLine,
                      str::toQ(subj));
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
    auto font = cp.font(uc::MatchLast::NO);
    auto r = font->get(uc::FontPlace::CELL, FSZ_TABLE * sizePc / 100,
                    cp.flags.have(uc::Cfg::NO_AA), cp.subj);
    if (sizePc <= 80 && font->flags.have(uc::Ffg::NOHINT_TINY))
        r.setHintingPreference(QFont::PreferNoHinting);
    return r;
}


std::optional<QFont> fontAt(uc::EmojiDraw emojiMode, const uc::Cp& cp)
{
    auto method = cp.drawMethod(emojiMode);
    return fontAt(method, 100, cp);
}


constexpr int EMOJI_NUM = 4;
constexpr int EMOJI_DEN = 5;


void drawChar(
        QPainter* painter, const QRect& rect, int sizePc, const uc::Cp& cp,
        const QColor& color, TableDraw tableMode, uc::EmojiDraw emojiMode,
        UseMargins useMargins)
{
    auto method = cp.drawMethod(emojiMode);
    switch (method) {
    case uc::DrawMethod::CUSTOM_CONTROL:
        drawCustomControl(painter, rect, color, uc::FontPlace::CELL, cp.subj);
        break;
    case uc::DrawMethod::ABBREVIATION:
        drawAbbreviation(painter, rect, cp.abbrev(), color);
        break;
    case uc::DrawMethod::EGYPTIAN_HATCH:
        drawEgyptianHatch(painter, rect, *fontAt(method, sizePc, cp), color, cp.subj);
        break;
    case uc::DrawMethod::SPACE:
        drawSpace(painter, rect, *fontAt(method, sizePc, cp), color, cp.subj);
        break;
    case uc::DrawMethod::SAMPLE:
        if (tableMode == TableDraw::CUSTOM) {
            // Char
            if (auto font = fontAt(method, sizePc, cp))
                painter->setFont(*font);
            painter->setBrush(color);
            painter->setPen(color);
            painter->drawText(rect,
                              Qt::AlignCenter | Qt::TextSingleLine,
                              textAt(cp));
        } break;
    case uc::DrawMethod::SVG_EMOJI: {
            //auto font = fontAt(*uc::cpsByCode[static_cast<int>('!')]);
            auto h = rect.height();
            if (useMargins != UseMargins::NO) {
                h = h * EMOJI_NUM / EMOJI_DEN;
            }
            emp.draw(painter, rect, cp.subj.ch32(), h);
        } break;
    }
    if (cp.isDeprecated())
        drawDeprecated(painter, rect);
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
        const QColor& color, uc::EmojiDraw emojiMode, qreal scale)
{
    drawCharBorder(painter, rect, color);
    if (cp)
        drawChar(painter, rect, lround(100 * scale), *cp, color, TableDraw::CUSTOM, emojiMode);
}


void drawSearchChars(
        QPainter* painter, const QRect& rect, std::u32string_view text,
        const QColor& color, uc::EmojiDraw emojiMode, qreal scale)
{
    drawCharBorder(painter, rect, color);
    if (auto c1 = EmojiPainter::getCp(text)) {
        // Single-char
        if (auto cp = uc::cpsByCode[c1])
            drawChar(painter, rect, lround(100 * scale), *cp, color, TableDraw::CUSTOM, emojiMode);
    } else {
        // Multi-char
        auto h = rect.height() * EMOJI_NUM / EMOJI_DEN;
        emp.draw(painter, rect, text, h);
    }
}


void drawCharTiles(
        QPainter* painter, const QRect& rect,
        const CharTiles& tiles, const QColor& color,
        qreal scale)
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
        if (auto c1 = EmojiPainter::getCp(tile.text)) {
            // Single-char
            if (auto cp = uc::cpsByCode[c1])
                drawChar(painter, r2, percent, *cp, color, TableDraw::CUSTOM,
                         tile.emojiDraw, UseMargins::NO);
        } else {
            // Multi-char
            emp.draw(painter, r2, tile.text, sz);
        }
    }
}

namespace {

    const uc::LibNode* goToText(const uc::LibNode& x)
    {
        if (x.flags.have(uc::Lfg::NO_TILE))
            return nullptr;
        if (!x.value.empty() && EmojiPainter::hasSkinGender(x.value))
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
        qreal scale)
{
    CharTiles tiles = getCharTiles(node);
    drawCharTiles(painter, bounds, tiles, color, scale);
}
