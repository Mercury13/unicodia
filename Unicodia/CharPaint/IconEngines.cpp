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

QPixmap ie::Veng::scaledPixmap(
        const QSize &size, QIcon::Mode, QIcon::State, qreal scale)
{
    QSize bigSz { lround(size.width() * scale), lround(size.height() * scale) };
    QPixmap localPix;
    auto workingPix = cache(scale);
    if (workingPix) {
        if (workingPix->size() == bigSz)
            return *workingPix;      // we rely on pixmap’s data sharing here
    } else {
        localPix = QPixmap{ bigSz };
        workingPix = &localPix;
    }
    workingPix->setDevicePixelRatio(1.0);
    workingPix->fill(Qt::transparent);
    QPainter ptr(workingPix);
    // Paint in 100% (in pixels) here
    paint1(&ptr, QRect{ QPoint(0, 0), bigSz }, scale);
    workingPix->setDevicePixelRatio(scale);
    // Paint in dipels here (none currently)
    return *workingPix;
}


void ie::Veng::paint(
        QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state)
{
    auto pix = scaledPixmap(rect.size(), mode, state,
                            painter->device()->devicePixelRatio());
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

void ie::Nonchar::paint1(QPainter *painter, const QRect &rect, qreal scale)
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
