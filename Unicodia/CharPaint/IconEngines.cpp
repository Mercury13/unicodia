#include "IconEngines.h"

// Qt
#include <QPainter>
#include <QGuiApplication>
#include <QSvgRenderer>
#include <private/qguiapplication_p.h>

// Char paint
#include "routines.h"
#include "Skin.h"

///// Util /////////////////////////////////////////////////////////////////////

namespace util {

    unsigned getMargin(unsigned side, unsigned value) noexcept
    {
        if (value == 0)
            return 0;
        auto r = (side * value) / 14u;  // 2px at 1.75+ = 28px
        if (r == 0)
            r = 1;
        return r << 1;
    }

    constexpr qreal GAMMA = 2.2;
    constexpr qreal INVGAMMA = 1.0 / GAMMA;

    struct CardDimensions {
        qreal thickness;
        QRect rcPixel;
        QRectF rcFrame;
    };

    enum class BrightnessMode { EMPIRICAL, OPTICAL };
    constexpr auto MODE = BrightnessMode::EMPIRICAL;

    //constexpr inline double whitesBrightness(double value, double gamma)
    //    { return std::pow(value, gamma);  }

    constexpr inline double blacksBrightness(double value, double gamma)
        { return 1.0 - std::pow(1.0 - value, gamma);  }
    // 1.0 - value = {value} of black, {1.0 - value} of white
    // pow(prev, gamma) = its optical brightness
    // 1.0 - prev = its optical content of black

    CardDimensions cardDimensions(
            const QRect& rect,
            qreal scale,
            unsigned baseWidth,
            qreal baseThickness)
    {
        // card height is always 14
        unsigned side = std::lround(16.0 * scale - 0.1);  // 0 / 0.5px — sometimes we request a bit smaller icon
        auto margin = util::getMargin(side, 1);
        auto height = side - margin;
        auto cardAspect = baseWidth / 14.0;     // base card height is always 14
        auto width = std::lround(height * cardAspect);
        auto x0 = rect.left() + (rect.width() - width) / 2;
        auto y0 = rect.top() + (rect.height() - height) / 2;

        static constexpr qreal MAX_THICK = 0.9;
        auto actualThickness = baseThickness;
        if (scale > 1) {
            if constexpr (MODE == BrightnessMode::EMPIRICAL) {
                // Simple empirical formula :)
                actualThickness = std::min(baseThickness * sqrt(scale), MAX_THICK);
            } else {
                // This formula is based on optics, but dislike somehow
                auto linearBrightness = blacksBrightness(baseThickness, GAMMA);
                linearBrightness *= scale;
                if (linearBrightness >= 1) {
                    actualThickness = MAX_THICK;
                } else {
                    linearBrightness = blacksBrightness(linearBrightness, INVGAMMA);
                    actualThickness = std::min(linearBrightness, MAX_THICK);
                }
            }
        }

        return {
            .thickness = actualThickness,
            .rcPixel = QRect( x0, y0, width, height ) ,
            .rcFrame { x0 + 0.5, y0 + 0.5, width - 1.0, height - 1.0 },
        };
    }

    /// @pre rect is square too
    void drawHintedSvg(
            QPainter* painter, const QRect& rect,
            QSvgRenderer& texture, const uc::SvgHint& hint)
    {
        QRectF rect1 = rect;
        // Smaller → who knows
        // Equal → no need to hint, both deltas are 0
        if (rect.width() > uc::SvgHint::SIDE && rect.height() > uc::SvgHint::SIDE) {
            if (hint.pos.x != 0) {
                const auto realPos = rect.width() * hint.bakedQX();
                const auto wantedPos = std::lround(rect.width() * hint.balancedQX());
                const auto delta = wantedPos - realPos;
                rect1.moveLeft(rect.left() + delta);
            }
            if (hint.pos.y != 0) {
                const auto realPos = rect.height() * hint.bakedQY();
                const auto wantedPos = std::lround(rect.height() * hint.balancedQY());
                const auto delta = wantedPos - realPos;
                rect1.moveTop(rect.top() + delta);
            }
        }

        texture.render(painter, rect1);
    }

}   // namespace util

///// Veng /////////////////////////////////////////////////////////////////////

QPixmap ie::Veng::pixmap(
        const QSize &size, QIcon::Mode mode, QIcon::State state)
{
    return scaledPixmap(size, mode, state, 1.0);
}

QPixmap ie::Veng::myScaledPixmap(const QSize &bigSize, QIcon::Mode mode, qreal scale)
{    
    QPixmap localPix;
    auto* workingPix = cache(scale);
    if (workingPix) {
        if (workingPix->size() == bigSize)
            return *workingPix;      // we rely on pixmap’s data sharing here
        // otherwise mutate existing pixmap
    } else {
        localPix = QPixmap{ bigSize };
        workingPix = &localPix;
    }
    workingPix->setDevicePixelRatio(1.0);
    workingPix->fill(Qt::transparent);
    { QPainter ptr(workingPix);
        // Paint in 100% (in pixels) here
        paint1(&ptr, QRect{ QPoint(0, 0), bigSize }, scale);
        // Paint in dipels here (none currently)
        //workingPix->setDevicePixelRatio(scale);
        //workingPix->setDevicePixelRatio(1.0);
    }

    if (qobject_cast<QGuiApplication*>(QCoreApplication::instance())) {
        if (mode != QIcon::Normal) {
            const QPixmap generated = QGuiApplicationPrivate::instance()->applyQIconStyleHelper(mode, *workingPix);
            if (!generated.isNull()) {
                *workingPix = generated;
            }
        }
    }

    workingPix->setDevicePixelRatio(scale);
    return *workingPix;
}

QPixmap ie::Veng::scaledPixmap(
        const QSize &size, QIcon::Mode mode, QIcon::State, qreal scale)
{
    return myScaledPixmap(size, mode, scale);
}


void ie::Veng::paint(
        QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State)
{
    qreal scale = 1.0;
    if (painter->device())
        scale = painter->device()->devicePixelRatio();
    QSize sz { lround(rect.width() * scale), lround(rect.height() * scale) };
    int dw = 0;
    if (sz.width() > sz.height()) {
        sz.setWidth(sz.height());
        dw = (rect.width() - rect.height()) >> 1;
    }
    auto pix = myScaledPixmap(sz, mode, scale);
    painter->drawPixmap(rect.left() + dw, rect.top(), pix);
}


///// Cp ///////////////////////////////////////////////////////////////////////

ie::Cp::Cp(const PixSource& aSource, uc::EmojiDraw aEmojiDraw, const uc::Cp* aCp)
    : source(aSource), emojiDraw(aEmojiDraw), cp(aCp) {}

void ie::Cp::paint1(QPainter *painter, const QRect &rect, qreal scale)
{
    auto clFg = source.winColor();
    drawSearchChar(painter, rect, cp, clFg, emojiDraw, scale);
}


///// Nonchar //////////////////////////////////////////////////////////////////

void ie::Nonchar::paint1(QPainter *painter, const QRect &rect, qreal)
{
    auto clFg = source.winColor();
    drawCharBorder(painter, rect, clFg);
    QBrush brush (clFg, Qt::DiagCrossPattern);
    painter->fillRect(rect, brush);
}


///// CustomAbbr ///////////////////////////////////////////////////////////////

ie::CustomAbbr::CustomAbbr(const PixSource& aSource, const char8_t* aText)
    : source(aSource), text(aText) {}

void ie::CustomAbbr::paint1(QPainter *painter, const QRect &rect, qreal)
{
    auto clFg = source.winColor();
    drawCharBorder(painter, rect, clFg);
    drawAbbreviation(painter, rect, text, clFg);
}


///// Murky ////////////////////////////////////////////////////////////////////

void ie::Murky::paint1(QPainter *painter, const QRect &rect, qreal)
{
    auto clFg = source.winColor();
    drawMurkyRect(painter, rect, clFg);
}


///// Synth ////////////////////////////////////////////////////////////////////

ie::Synth::Synth(const PixSource& aSource, const uc::SynthIcon& aSi)
    : source(aSource), si(aSi) {}

void ie::Synth::paint1(QPainter *painter, const QRect &rect, qreal scale)
{
    // No continent → draw murky, otherwise use icon colours
    auto clFg = source.winColor();
    if (si.ecContinent == uc::EcContinent::NONE) {
        drawMurkyRect(painter, rect, clFg);
    } else {
        auto cont = si.continent();
        painter->fillRect(rect, cont.icon.bgColor);
        drawCharBorder(painter, rect, clFg);
        clFg = cont.icon.fgColor;
    }
    // Draw icon a bit larger — 120%
    drawChar(painter, rect, lround(120 * scale), si.cp(), clFg,
             TableDraw::CUSTOM, uc::EmojiDraw::CONSERVATIVE);
}

///// Node /////////////////////////////////////////////////////////////////////

ie::Node::Node(const PixSource& aSource, const uc::LibNode& aNode)
    : source(aSource), node(aNode) {}

void ie::Node::paint1(QPainter *painter, const QRect &rect, qreal scale)
{
    auto clFg = source.winColor();
    drawCharBorder(painter, rect, clFg);
    // draw char
    switch (node.value.length()) {
    case 0:
        drawFolderTile(painter, rect, node, clFg, scale);
        break;
    default:
        drawSearchChars(painter, rect, node.value, clFg, node.emojiDraw(), scale);
        break;
    }
}

///// BlockElem ////////////////////////////////////////////////////////////////

ie::BlockElem::BlockElem()
{
    texture.load(":Misc/blockelem.png");
}

void ie::BlockElem::paint1(QPainter *painter, const QRect &rect, qreal)
{
    // White BG
    painter->fillRect(rect, Qt::white);

    // Get width
    auto width = rect.height() * 3 / 5;
    if (width % 2 != 0)
        ++width;
    width = std::min(width, rect.width());
    // Get starting X
    auto dx = (rect.width() - width) >> 1;
    auto x0 = rect.left() + dx;
    // Top margin: top line of texture is empty, this precise 1
    // Bottom: 0/1 is somehow nicer than 1/2, let it be
    QRect newRect { x0, rect.top(), width, rect.height()};
    QBrush brush(texture);
    painter->fillRect(newRect, brush);
}


///// CoarseImage //////////////////////////////////////////////////////////////


ie::CoarseImage::CoarseImage(const QColor& aBg, const QSize& aMargins, const char* fname)
    : bg(aBg), margins(aMargins)
{
    texture.load(fname);
}


void ie::CoarseImage::paint1(QPainter *painter, const QRect &rect, qreal scale)
{
    // Fill BG
    painter->fillRect(rect, bg);

    // Get rect
    unsigned side = std::lround(16.0 * scale - 0.1);  // 0 / 0.5px — sometimes we request a bit smaller icon
    auto mx = util::getMargin(side, margins.width());
    auto my = util::getMargin(side, margins.height());
    int times = std::min((side - mx) / texture.width(),
                         (side - my) / texture.height());
    if (times < 1)
        times = 1;
    int ww = texture.width() * times;
    int hh = texture.height() * times;
    int x0 = rect.left() + ((rect.width() - ww) >> 1);
    int y0 = rect.top() + ((rect.height() - hh) >> 1);

    QRect rcDest { x0, y0, ww, hh };
    painter->setRenderHint(QPainter::SmoothPixmapTransform, false);
    painter->drawPixmap(rcDest, texture);
}


///// Taixu ////////////////////////////////////////////////////////////////////

void ie::Taixu::paint1(QPainter *painter, const QRect &rect, qreal scale)
{
    painter->fillRect(rect, BG_CJK);

    unsigned side = std::lround(16.0 * scale - 2.1);
    auto hunit = std::max(1u, side / 7u);
    auto hh = hunit * 7;
    auto ww = hh;
    // Need remainder 0 or 2, bigger unit is a SPACE for dotted
    switch (ww % 5) {
    case 1:
    case 3: --ww; break;
    case 4: ww -= 2; break;
    default: ;  // 0,2
    }
    auto wSmallUnit = ww / 5;
    auto wBigUnit = wSmallUnit;
    if (ww % 5 != 0)
        ++wBigUnit;
    int x0 = rect.left() + ((rect.width() - ww) >> 1);
    int y0 = rect.top()  + ((rect.height() - hh) >> 1);
    int x1 = x0 + wSmallUnit;
    int x2 = x1 + wBigUnit;
    int x3 = x2 + wSmallUnit;
    int x4 = x3 + wBigUnit;
    int x5 = x4 + wSmallUnit;
    auto drawL = [&](int x1, int x2, int y) {
        int ystart = y0 + y * hunit;
        QRect r { x1, ystart, x2 - x1, static_cast<int>(hunit) };
        painter->fillRect(r, FG_CJK);
    };
    drawL(x0, x2, 0);                     drawL(x3, x5, 0);
                       drawL(x0, x5, 2);
    drawL(x0, x1, 4);  drawL(x2, x3, 4);  drawL(x4, x5, 4);
    drawL(x0, x1, 6);  drawL(x2, x3, 6);  drawL(x4, x5, 6);
}


///// Legacy ///////////////////////////////////////////////////////////////////

ie::Legacy::Legacy()
{
    texture.load(":Misc/legacy.png");
}


void ie::Legacy::paint1(QPainter *painter, const QRect &rect, qreal scale)
{
    // Fill BG
    painter->fillRect(rect, Qt::white);

    static constexpr unsigned MIN_WIDTH = 11;

    // Get rect
    unsigned side = std::lround(16.0 * scale - 0.1);  // 0 / 0.5px — sometimes we request a bit smaller icon
    int times = (side - 1) / MIN_WIDTH;
    if (times < 1)
        times = 1;
    int ww = texture.width() * times;
    int hh = texture.height() * times;
    int x0 = rect.left() + ((rect.width() - ww) >> 1);
    if (x0 < 1)
        x0 = 1;
    int y0 = rect.top() + ((rect.height() - hh) >> 1);

    QRect rcDest { x0, y0, ww, hh };
    painter->setRenderHint(QPainter::SmoothPixmapTransform, false);
    painter->drawPixmap(rcDest, texture);
}


///// PlayingCard //////////////////////////////////////////////////////////////

ie::PlayingCard::PlayingCard()
{    
    texture = std::make_shared<QSvgRenderer>(QString{":Misc/playcard.svg"});
    texture->setAspectRatioMode(Qt::KeepAspectRatio);
}

// -warn: complains about =default
ie::PlayingCard::~PlayingCard() {}


void ie::PlayingCard::paint1(QPainter *painter, const QRect &rect, qreal scale)
{
    painter->fillRect(rect, Qt::white);

    // Get dimensions
    // (point’s dimensions are baked into SVG)
    auto dim = util::cardDimensions(rect, scale, 10, 0.3);
    auto indexSize = std::lround(dim.rcPixel.height() * (1.0 / 11.0));     // 1 at 1×, 2 at 1.25×
    auto radius = dim.rcPixel.height() * 0.1;

    // Frame
    painter->setBrush(Qt::NoBrush);
    painter->setPen(QPen{Qt::black, dim.thickness});
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->drawRoundedRect(dim.rcFrame, radius, radius);

    // Top index
    static constexpr QColor HEART_RED { 0xD4, 0x00, 0x00 };
    auto indexDy = lround(dim.rcPixel.height() * (1.0 / 9.0));
    auto xi1 = dim.rcPixel.left() + 1;
    auto yi1 = dim.rcPixel.top() + indexDy;
    painter->setRenderHint(QPainter::Antialiasing, false);
    QRect rcTopIndex( xi1, yi1, indexSize, indexSize );
    painter->fillRect(rcTopIndex, HEART_RED);

    // Bottom index
    auto xi2 = dim.rcPixel.right();
    auto yi2 = dim.rcPixel.bottom() + 1 - indexDy;
    QRect rcBottomIndex( xi2 - indexSize, yi2 - indexSize, indexSize, indexSize );
    painter->fillRect(rcBottomIndex, HEART_RED);

    // Point
    painter->setRenderHint(QPainter::Antialiasing, true);
    texture->render(painter, dim.rcPixel);
}


///// Mahjong //////////////////////////////////////////////////////////////////

ie::Mahjong::Mahjong()
{
    texture = std::make_shared<QSvgRenderer>(QString{":Misc/mahjong.svg"});
    texture->setAspectRatioMode(Qt::KeepAspectRatio);
}

// -warn: complains about =default
ie::Mahjong::~Mahjong() {}


void ie::Mahjong::paint1(QPainter *painter, const QRect &rect, qreal scale)
{
    painter->fillRect(rect, Qt::white);

    // Get dimensions
    auto dim = util::cardDimensions(rect, scale, 10, 0.25);

    static constexpr QColor CL_BG { 0xF6, 0xF2, 0xEE }; // ≈old F2F2F2, but a bit warmer
    static constexpr QColor CL_FRAME { 0x30, 0, 0 };    // It’ll be brown

    // Background
    painter->setRenderHint(QPainter::Antialiasing, false);
    painter->fillRect(dim.rcPixel, CL_BG);

    // Frame
    painter->setBrush(Qt::NoBrush);
    painter->setPen(QPen{CL_FRAME, dim.thickness});
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->drawRect(dim.rcFrame);

    // Flower
    texture->render(painter, dim.rcPixel);
}


///// Hint /////////////////////////////////////////////////////////////////////

ie::Hint::Hint(const uc::Block& blk)
    : icon(blk.synthIcon)
{
    char buf[48];
    snprintf(buf, std::size(buf), ":/Scripts/%04X.svg", blk.startingCp);
    texture = std::make_shared<QSvgRenderer>(QString{buf});
    texture->setAspectRatioMode(Qt::KeepAspectRatio);
}

// -warn: complains about =default
ie::Hint::~Hint() {}


void ie::Hint::paint1(QPainter *painter, const QRect &rect, qreal)
{
    painter->fillRect(rect, icon.continent().icon.bgColor);
    util::drawHintedSvg(painter, rect, *texture, icon.svgHint);
}
