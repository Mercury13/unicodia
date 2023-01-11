#pragma once

// Qt
#include <QIconEngine>

// Char paint
#include "global.h"

namespace uc {
    struct SynthIcon;
}

namespace ie {

    /// Virtual engine
    class Veng : public QIconEngine
    {
    public:
        QPixmap pixmap(
                const QSize &size, QIcon::Mode mode, QIcon::State state) override;
        QPixmap scaledPixmap(
                const QSize &size, QIcon::Mode mode,
                QIcon::State state, qreal scale) override;
        void paint(QPainter *painter, const QRect &rect, QIcon::Mode, QIcon::State) override;
    protected:
        QPixmap myScaledPixmap(const QSize &bigSize, QIcon::Mode mode, qreal scale);
        /// paint at 100% scale
        virtual void paint1(QPainter *painter, const QRect &rect, qreal scale) = 0;
        /// @return [+] a cell in cache  [-] cannot cache
        virtual QPixmap* cache([[maybe_unused]] qreal scale) { return nullptr; }
    };

    class Cp : public Veng
    {
    public:
        Cp(const PixSource& aSource, uc::EmojiDraw aEmojiDraw, const uc::Cp* aCp);
        Cp* clone() const override { return new Cp(*this); }
    protected:
        void paint1(QPainter *painter, const QRect &rect, qreal scale) override;
    private:
        const PixSource& source;
        const uc::EmojiDraw emojiDraw;
        const uc::Cp* const cp;
    };

    class Nonchar : public Veng
    {
    public:
        Nonchar(const PixSource& aSource) : source(aSource) {}
        Nonchar* clone() const override { return new Nonchar(*this); }
    protected:
        void paint1(QPainter *painter, const QRect &rect, qreal scale) override;
    private:
        const PixSource& source;
    };

    class CustomAbbr : public Veng
    {
    public:
        CustomAbbr(const PixSource& aSource, const char8_t* aText);
        CustomAbbr* clone() const override { return new CustomAbbr(*this); }
    protected:
        void paint1(QPainter *painter, const QRect &rect, qreal scale) override;
    private:
        const PixSource& source;
        const char8_t* const text;
    };

    class Murky : public Veng
    {
    public:
        Murky(const PixSource& aSource) : source(aSource) {}
        Murky* clone() const override { return new Murky(*this); }
    protected:
        void paint1(QPainter *painter, const QRect &rect, qreal scale) override;
    private:
        const PixSource& source;
    };

    class Synth : public Veng
    {
    public:
        Synth(const PixSource& aSource, const uc::SynthIcon& aSi);
        Synth* clone() const override { return new Synth(*this); }
    protected:
        void paint1(QPainter *painter, const QRect &rect, qreal scale) override;
    private:
        const PixSource& source;
        const uc::SynthIcon& si;
    };

    class Node : public Veng
    {
    public:
        Node(const PixSource& aSource, const uc::LibNode& aNode);
        Node* clone() const override { return new Node(*this); }
    protected:
        void paint1(QPainter *painter, const QRect &rect, qreal scale) override;
    private:
        const PixSource& source;
        const uc::LibNode& node;
    };

    // Programmatic drawing of Block Elements icon
    class BlockElem : public Veng
    {
    public:
        BlockElem();
        BlockElem* clone() const override { return new BlockElem(*this); }
        void paint1(QPainter *painter, const QRect &rect, qreal scale) override;
    private:
        QPixmap texture;
    };

    // Programmatic drawing of Yijing icon
    class CoarseImage : public Veng
    {
    public:
        CoarseImage(const QColor& aBg, const QSize& aMargins, const char* fname);
        CoarseImage* clone() const override { return new CoarseImage(*this); }
        void paint1(QPainter *painter, const QRect &rect, qreal scale) override;
    private:
        QColor bg;
        QSize margins;
        QPixmap texture;
        static unsigned getMargin(unsigned side, unsigned value) noexcept;
    };

    // Programmatic drawing of Tai Xuan Jing icons
    class Taixu : public Veng
    {
    public:
        Taixu* clone() const override { return new Taixu(*this); }
        void paint1(QPainter *painter, const QRect &rect, qreal scale) override;
    private:
    };

    // Programmatic drawing of Symbols for legacy computing
    class Legacy : public Veng
    {
    public:
        Legacy();
        Legacy* clone() const override { return new Legacy(*this); }
        void paint1(QPainter *painter, const QRect &rect, qreal scale) override;
    private:
        QPixmap texture;
    };

}   // namespace ie
