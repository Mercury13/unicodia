#pragma once

// Qt
#include <QIconEngine>

// My lib
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

    /// All low-res icons are 16×16 dip (dipels, device-independent pixels).
    /// As we want to do something with device pixels rather than
    ///    dumbly anti-alias, we’ve made some special engines
    /// (pixels do matter even on 1.5× = 24×24 px!)
    constexpr int BASE_SIZE = 16;

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

    /// Drawing of code point
    /// Type: hi-res
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

    /// Drawing of non-character
    /// Type: hi-res
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

    /// Drawing of abbreviated char
    /// Type: hi-res
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

    /// Drawing of empty murky cell
    /// Type: hi-res
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

    /// Drawing of synthesized icon
    /// Type: hi-res
    class Synth : public Veng
    {
    public:
        Synth(const PixSource& aSource, const uc::SynthIcon& aSi, char32_t aBlkStart);
        Synth* clone() const override { return new Synth(*this); }
    protected:
        void paint1(QPainter *painter, const QRect &rect, qreal scale) override;
    private:
        const PixSource& source;
        const uc::SynthIcon& si;
        dumb::Sp<LazySvg> texture;
    };

    /// Drawing of Library node
    /// Type: hi-res
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

    /// Programmatic drawing of Block Elements icon
    /// Type: lo-res
    class BlockElem : public Veng
    {
    public:
        BlockElem();
        BlockElem* clone() const override { return new BlockElem(*this); }
        void paint1(QPainter *painter, const QRect &rect, qreal scale) override;
    private:
        QPixmap texture;
    };

    /// Programmatic drawing of Yijing icon
    /// Type: lo-res
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

    /// Programmatic drawing of Tai Xuan Jing icons
    /// Type: lo-res
    class Taixu : public Veng
    {
    public:
        Taixu* clone() const override { return new Taixu(); }
        void paint1(QPainter *painter, const QRect &rect, qreal scale) override;
    private:
    };

    /// Programmatic drawing of Symbols for legacy computing
    /// Type: lo-res
    class Legacy : public Veng
    {
    public:
        Legacy(const char* fname);
        Legacy* clone() const override { return new Legacy(*this); }
        void paint1(QPainter *painter, const QRect &rect, qreal scale) override;
    private:
        QPixmap texture;
    };

    /// Programmatic drawing of Playing Cards
    /// Type: lo-res
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

    /// Programmatic drawing of Mahjong Tiles
    /// Type: lo-res
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

    /// Programmatic drawing of Arrows
    /// Type: lo-res
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

    /// Programmatic drawing of format pictures
    /// Type: lo-res
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

    /// Programmatic drawing of Ideographic descriptions
    /// Type: lo-res
    class CjkStructure : public Veng
    {
    public:
        CjkStructure* clone() const override { return new CjkStructure; }
        void paint1(QPainter *painter, const QRect &rect, qreal scale) override;
    };

    /// Programmatic drawing of tally marks
    /// Type: lo-res
    class TallyMark : public Veng
    {
    public:
        TallyMark* clone() const override { return new TallyMark; }
        void paint1(QPainter *painter, const QRect &rect, qreal scale) override;
    };

    /// Programmatic drawing of enclosed alnum supp
    /// Type: lo-res
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

    /// Programmatic drawing of enclosed ideographic supp
    /// Type: lo-res
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

    /// Programmatic drawing of enclosed alnum ①
    /// Type: lo-res
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

    enum class Mfg : unsigned char {
        IMBA_UP = 1,  /// [+] imbalance 0.5px−ε upwards (no fine-grained like ±0.4px here)
    };

    enum class HalfPixelDown : unsigned char { NO, YES };

    /// For more detailed chars: 1px at 1.25×, 2px at 1.5×
    constexpr int MRG_DETAILED = 12;

    /// For simpler chars: 2px even at 1.25×
    constexpr int MRG_SIMPLER = 13;

    /// Drawing with constant margin around SVG (Kawi)
    /// Type: lo-res
    class Margin : public Veng
    {
    public:
        /// @param [in] synthIcon        destination palette (and whether to repaint at all)
        /// @param [in] aName            file name
        /// @param [in] aValue           1 = 0.1dip (device-independent pixel)
        Margin(const uc::SynthIcon& synthIcon, std::string_view aName,
               int aValue, Flags<Mfg> aFlags);
        ~Margin();
        Margin* clone() const override { return new Margin(*this); }
        void paint1(QPainter *painter, const QRect &rect, qreal scale) override;
    private:
        dumb::Sp<LazySvg> texture;
        QColor bgColor;
        int value;
        Flags<Mfg> flags;
    };

    /// Drawing small square SVG in the lower part of the cell (Control pictures)
    ///    SVG is specified with gap from bottom and base size,
    ///    both in 0.1dip increment
    /// Type: lo-res
    class SvgBelow : public Veng
    {
    public:
        /// @param [in] synthIcon   destination palette (and whether to repaint at all)
        /// @param [in] aName       file name
        /// @param [in] aBorder     border in 0.1 dip
        /// @param [in] aSide       side in 0.1 dip
        SvgBelow(const uc::SynthIcon& synthIcon, std::string_view aName,
               int aBorder, int aSide);
        ~SvgBelow();
        SvgBelow* clone() const override { return new SvgBelow(*this); }
        void paint1(QPainter *painter, const QRect &rect, qreal scale) override;
    private:
        dumb::Sp<LazySvg> texture;
        QColor bgColor;
        int border, side;
    };

}   // namespace ie
