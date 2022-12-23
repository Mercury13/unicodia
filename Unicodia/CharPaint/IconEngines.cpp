#include "IconEngines.h"

// Qt
#include <QPainter>

// Char paint
#include "routines.h"

///// Veng /////////////////////////////////////////////////////////////////////

QPixmap ie::Veng::scaledPixmap(
        const QSize &size, QIcon::Mode mode,
        QIcon::State state, qreal scale)
{
    auto bigW = lround(size.width() * scale);
    auto bigH = lround(size.height() * scale);
    QPixmap pix { bigW, bigH };
    pix.setDevicePixelRatio(scale);
    pix.fill(Qt::transparent);
    QPainter ptr(&pix);
    vpaint(&ptr, QRect{ QPoint(0, 0), size }, mode, state);
    return pix;
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

void ie::Cp::vpaint(QPainter *painter, const QRect &rect, QIcon::Mode, QIcon::State)
{
    auto clFg = source.winColor();
    drawSearchChar(painter, rect, cp, clFg, emojiDraw);
}
