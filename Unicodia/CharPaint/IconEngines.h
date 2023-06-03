#pragma once

// Qt
#include <QIconEngine>

// Ly lib
#include "u_DumbSp.h"

// Char paint
#include "global.h"

namespace uc {
    struct SynthIcon;
}

class QSvgRenderer;

namespace ie {
    class LazySvg;
}

extern template class dumb::Sp<ie::LazySvg>;


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
        Cp(const PixSource& aSource, uc::EmojiDraw aEmojiDraw, const uc::Cp* aCp,
                    const uc::GlyphStyleSets& aGlyphSets);
        Cp* clone() const override { return new Cp(*this); }
    protected:
        void paint1(QPainter *painter, const QRect &rect, qreal scale) override;
    private:
        const PixSource& source;
        const uc::EmojiDraw emojiDraw;        
        const uc::Cp* const cp;
        const uc::GlyphStyleSets& glyphSets;
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
    };

    // Programmatic drawing of Tai Xuan Jing icons
    class Taixu : public Veng
    {
    public:
        Taixu* clone() const override { return new Taixu(); }
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

    // Programmatic drawing of Playing Cards
    class PlayingCard : public Veng
    {
    public:
        PlayingCard();
        ~PlayingCard();
        PlayingCard* clone() const override { return new PlayingCard(*this); }
        void paint1(QPainter *painter, const QRect &rect, qreal scale) override;
    private:
        dumb::Sp<LazySvg> texture;
    };

    // Programmatic drawing of Mahjong Tiles
    class Mahjong : public Veng
    {
    public:
        Mahjong();
        ~Mahjong();
        Mahjong* clone() const override { return new Mahjong(*this); }
        void paint1(QPainter *painter, const QRect &rect, qreal scale) override;
    private:
        dumb::Sp<LazySvg> texture;
    };

    // Programmatic drawing of Arrows
    class Hint : public Veng
    {
    public:
        Hint(const uc::Block& blk);
        ~Hint();
        Hint* clone() const override { return new Hint(*this); }
        void paint1(QPainter *painter, const QRect &rect, qreal scale) override;
    private:
        dumb::Sp<LazySvg> texture;
        const uc::SynthIcon& icon;
    };

    // Programmatic drawing of format pictures
    class Format : public Veng
    {
    public:
        Format(const uc::Block& blk);
        ~Format();
        Format* clone() const override { return new Format(*this); }
        void paint1(QPainter *painter, const QRect &rect, qreal scale) override;
    private:
        dumb::Sp<LazySvg> texture;
        const uc::SynthIcon& icon;
    };

    // Programmatic drawing of Ideographic descriptions
    class CjkStructure : public Veng
    {
    public:
        CjkStructure* clone() const override { return new CjkStructure; }
        void paint1(QPainter *painter, const QRect &rect, qreal scale) override;
    };

    // Programmatic drawing of tally marks
    class TallyMark : public Veng
    {
    public:
        TallyMark* clone() const override { return new TallyMark; }
        void paint1(QPainter *painter, const QRect &rect, qreal scale) override;
    };

    // Programmatic drawing of enclosed alnum supp
    class ThreeD : public Veng
    {
    public:
        ThreeD();
        ~ThreeD();
        ThreeD* clone() const override { return new ThreeD(*this); }
        void paint1(QPainter *painter, const QRect &rect, qreal scale) override;
    private:
        dumb::Sp<LazySvg> texture;
    };

    // Programmatic drawing of enclosed ideographic supp
    class SqIdeo : public Veng
    {
    public:
        SqIdeo();
        ~SqIdeo();
        SqIdeo* clone() const override { return new SqIdeo(*this); }
        void paint1(QPainter *painter, const QRect &rect, qreal scale) override;
    private:
        dumb::Sp<LazySvg> texture;
    };

    // Programmatic drawing of enclosed alnum ①
    class OneCircle : public Veng
    {
    public:
        OneCircle();
        ~OneCircle();
        OneCircle* clone() const override { return new OneCircle(*this); }
        void paint1(QPainter *painter, const QRect &rect, qreal scale) override;
    private:
        dumb::Sp<LazySvg> texture;
    };
}   // namespace ie
