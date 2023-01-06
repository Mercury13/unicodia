#include "IconEngines.h"

// Qt
#include <QPainter>

// Char paint
#include "routines.h"

///// Veng /////////////////////////////////////////////////////////////////////

QPixmap ie::Veng::pixmap(
        const QSize &size, QIcon::Mode mode, QIcon::State state)
{
    return scaledPixmap(size, mode, state, 1.0);
}

QPixmap ie::Veng::myScaledPixmap(const QSize &bigSize, qreal scale)
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
    QPainter ptr(workingPix);
    // Paint in 100% (in pixels) here
    paint1(&ptr, QRect{ QPoint(0, 0), bigSize }, scale);
    workingPix->setDevicePixelRatio(scale);
    // Paint in dipels here (none currently)
    return *workingPix;
}

QPixmap ie::Veng::scaledPixmap(
        const QSize &size, QIcon::Mode, QIcon::State, qreal scale)
{
    return myScaledPixmap(size, scale);
}


void ie::Veng::paint(
        QPainter *painter, const QRect &rect, QIcon::Mode, QIcon::State)
{
    auto scale = painter->device()->devicePixelRatio();
    QSize sz { lround(rect.width() * scale), lround(rect.height() * scale) };
    auto pix = myScaledPixmap(sz, scale);
    painter->drawPixmap(rect.topLeft(), pix);
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
        drawFolderTile(painter, rect, node, clFg);
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
    // Get rect
    QRect newRect { x0, rect.top(), width, rect.height() };
    QBrush brush(texture);
    painter->fillRect(newRect, brush);
}


///// CoarseImage //////////////////////////////////////////////////////////////


ie::CoarseImage::CoarseImage(const QColor& aBg, const QSize& aMargins, const char* fname)
    : bg(aBg), margins(aMargins)
{
    texture.load(fname);
}


unsigned ie::CoarseImage::getMargin(unsigned side, unsigned value) noexcept
{
    if (value == 0)
        return 0;
    auto r = (side * value) / 24u;
    if (r == 0)
        r = 1;
    return r << 1;
}


void ie::CoarseImage::paint1(QPainter *painter, const QRect &rect, qreal scale)
{
    // Fill BG
    painter->fillRect(rect, bg);

    // Get rect
    unsigned side = std::lround(16.0 * scale - 1.1);  // 1 / 1.5px — sometimes we request a bit smaller icon
    auto mx = getMargin(side, margins.width());
    auto my = getMargin(side, margins.height());
    int times = std::min((side - mx) / texture.width(),
                         (side - my) / texture.height());
    if (times < 1)
        times = 1;
    int ww = texture.width() * times;
    int hh = texture.height() * times;
    int x0 = (rect.width() - ww) >> 1;
    int y0 = (rect.height() - hh) >> 1;

    QRect rcDest { x0, y0, ww, hh };
    painter->setRenderHint(QPainter::SmoothPixmapTransform, false);
    painter->drawPixmap(rcDest, texture);
}
