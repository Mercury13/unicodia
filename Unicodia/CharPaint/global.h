#pragma once

// Qt
#include <QColor>
#include <QSize>

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
    bool isEmoji() const;
};
using CharTiles = std::array<CharTile, 4>;

