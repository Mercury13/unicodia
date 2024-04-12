#pragma once

// Qt
#include <QColor>
#include <QSize>
#include <QRectF>

// Uc
#include "UcAutoDefines.h"

class PixSource // interface
{
public:
    virtual int pixSize() const = 0;
    virtual QColor winColor() const = 0;
    virtual ~PixSource() = default;

    QSize pixQsize() const
    {
        auto sz = pixSize();
        return { sz, sz };
    }
};

struct CharTile {
    std::u32string_view text {};
    uc::EmojiDraw emojiDraw = uc::EmojiDraw::GRAPHIC;
    bool isEmoji(const uc::GlyphStyleSets& glyphSets) const;
};
using CharTiles = std::array<CharTile, 4>;

QRectF adjustedToPhysicalPixels(const QRectF& rect, qreal scale, qreal frame);
