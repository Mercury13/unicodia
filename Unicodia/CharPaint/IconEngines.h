#pragma once

// Qt
#include <QIconEngine>

// Char paint
#include "global.h"

namespace ie {

    /// Virtual engine
    class Veng : public QIconEngine
    {
    public:
        QPixmap scaledPixmap(
                const QSize &size, QIcon::Mode mode,
                QIcon::State state, qreal scale) override;
        void paint(QPainter *painter, const QRect &rect, QIcon::Mode, QIcon::State) override;
    protected:
        virtual void vpaint(QPainter *painter, const QRect &rect, QIcon::Mode, QIcon::State) = 0;
    };

    class Cp : public Veng
    {
    public:
        Cp(const PixSource& aSource, uc::EmojiDraw aEmojiDraw, const uc::Cp* aCp);
        Cp* clone() const override { return new Cp(*this); }
    protected:
        void vpaint(QPainter *painter, const QRect &rect, QIcon::Mode, QIcon::State) override;
    private:
        const PixSource& source;
        const uc::EmojiDraw emojiDraw;
        const uc::Cp* const cp;
    };

}   // namespace ie
