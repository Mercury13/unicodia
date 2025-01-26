#include "IconEngines.h"

// Qt
#include <QPainter>
#include <QApplication>
#include <QStyle>
#include <QStyleOption>
#include <QSvgRenderer>
#include <QFile>

// Utils
#include "u_Qstrings.h"

// Unicode data
#include "UcCp.h"
#include "UcSkin.h"

// Char paint
#include "routines.h"

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

    qreal scaledThickness(qreal base, qreal scale, qreal max)
    {
        if (scale <= 1)
            return base;
        if constexpr (MODE == BrightnessMode::EMPIRICAL) {
            // Simple empirical formula :)
            return std::min(base * sqrt(scale), max);
        } else {
            // This formula is based on optics, but dislike somehow
            auto linearBrightness = blacksBrightness(base, GAMMA);
            linearBrightness *= scale;
            if (linearBrightness >= 1) {
                return max;
            } else {
                linearBrightness = blacksBrightness(linearBrightness, INVGAMMA);
                return std::min(linearBrightness, max);
            }
        }
    }

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

        return {
            .thickness = scaledThickness(baseThickness, scale, MAX_THICK),
            .rcPixel = QRect( x0, y0, width, height ) ,
            .rcFrame = QRectF{ x0 + 0.5, y0 + 0.5, width - 1.0, height - 1.0 },
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

    inline void sprintfCp(char (&buf)[48], char32_t cp)
        { snprintf(buf, std::size(buf), ":/Scripts/%04X.svg", int(cp)); }

    struct FormatInfo {
        QRect frameRect, contentRect;
        unsigned dashSize;
    };

    FormatInfo getFormatInfo(const QRect& rect, qreal scale)
    {
        // #.5 → big indent, small dash
        unsigned dashSize = lround(scale - 0.001);
        unsigned indent = lround(scale + 0.001);
        unsigned contentIndent = scale + 0.79;     // 1.2 round DOWN, 1.25 UP
        unsigned ind1 = indent + dashSize + contentIndent;
        return {            
            .frameRect = rect.marginsRemoved(QMargins(indent, indent, indent, indent)),
            .contentRect = rect.marginsRemoved(QMargins(ind1, ind1, ind1, ind1)),
            .dashSize = dashSize,
        };
    }

    void drawPixelBorder(
            QPainter* painter, const QRect& rect, unsigned thickness,
            const QColor& color)
    {
        auto rt1 = rect.right() + 1 - thickness;
        auto bt1 = rect.bottom() + 1 - thickness;
        // Top
        painter->fillRect(rect.left(), rect.top(), rect.width(), thickness, color);
        // Left
        painter->fillRect(rect.left(), rect.top(), thickness, rect.height(), color);
        // Bottom
        painter->fillRect(rect.left(), bt1, rect.width(), thickness, color);
        // Right
        painter->fillRect(rt1, rect.top(), thickness, rect.height(), color);
    }

    // draw low-resolution format frame
    void drawLoResFormat(
            QPainter* painter, const QRect& rect, unsigned dotSize,
            const QColor& color)
    {
        if (dotSize == 0)
            dotSize = 1;

        unsigned nHorzDots = rect.width() / dotSize;
        unsigned nVertDots = nHorzDots;
        unsigned halfDot = dotSize >> 1;
        unsigned doubleDot = dotSize << 1;
        if ((rect.width() % dotSize) < halfDot
                && (nVertDots & 1u) != 0) {
            --nVertDots;
        }

        QBrush brush { color };

        QRect rcTopLeft(rect.left(), rect.top(), dotSize, dotSize);

        // Draw top
        QRect rc = rcTopLeft;
        for (size_t i = 0; i < nHorzDots; i += 2) {
            painter->fillRect(rc, brush);
            rc.moveLeft(rc.left() + doubleDot);
        }

        // Draw left
        rc = rcTopLeft;
        for (size_t i = 2; i < nVertDots; i += 2) {
            rc.moveTop(rc.top() + doubleDot);
            painter->fillRect(rc, brush);
        }

        // Draw bottom
        rc = rcTopLeft;
        rc.moveBottomRight(rect.bottomRight());
        for (size_t i = 0; i < nHorzDots; i += 2) {
            painter->fillRect(rc, brush);
            rc.moveLeft(rc.left() - doubleDot);
        }

        // Draw right
        rc = rcTopLeft;
        rc.moveBottomRight(rect.bottomRight());
        for (size_t i = 2; i < nVertDots; i += 2) {
            rc.moveTop(rc.top() - doubleDot);
            painter->fillRect(rc, brush);
        }

    }

    // draw low-resolution format frame, bottom-left part
    void drawFormatBottomLeft(
            QPainter* painter, const QRect& rect, unsigned dotSize,
            const QColor& color)
    {
        if (dotSize == 0)
            dotSize = 1;

        unsigned nVertDots = rect.width() / dotSize;
        unsigned halfDot = dotSize >> 1;
        unsigned doubleDot = dotSize << 1;
        if ((rect.width() % dotSize) < halfDot
                && (nVertDots & 1u) != 0) {
            --nVertDots;
        }
        unsigned nHorzDots = nVertDots;

        QBrush brush { color };
        QRect rcTopLeft(rect.left(), rect.top(), dotSize, dotSize);

        // Draw left
        QRect rc = rcTopLeft;
        for (size_t i = 2; i < nVertDots; i += 2) {
            rc.moveTop(rc.top() + doubleDot);
            painter->fillRect(rc, brush);
        }

        // Draw bottom
        rc = rcTopLeft;
        rc.moveBottomRight(rect.bottomRight());
        for (size_t i = 2; i < nHorzDots; i += 2) {
            rc.moveLeft(rc.left() - doubleDot);
            painter->fillRect(rc, brush);
        }
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

    if (mode != QIcon::Normal) {
        QStyleOption option(0);
        option.palette = QGuiApplication::palette();
        const QPixmap generated = QApplication::style()->generatedIconPixmap(mode, *workingPix, &option);
        if (!generated.isNull()) {
            *workingPix = generated;
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

ie::Cp::Cp(
        const PixSource& aSource, uc::EmojiDraw aEmojiDraw, const uc::Cp* aCp,
        const uc::GlyphStyleSets& aGlyphSets)
    : source(aSource), emojiDraw(aEmojiDraw), cp(aCp), glyphSets(aGlyphSets) {}

void ie::Cp::paint1(QPainter *painter, const QRect &rect, qreal scale)
{
    auto clFg = source.winColor();
    drawSearchChar(painter, rect, cp, clFg, emojiDraw, glyphSets, scale);
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


///// IconPalette //////////////////////////////////////////////////////////////

struct IconPalette {
    QColor fg, bg;

    QByteArray repaintFile(const QString& fname);
    static void replaceColor(
            QByteArray& content, std::string_view what, const QColor& byWhat);
};


void IconPalette::replaceColor(
        QByteArray& content, std::string_view what, const QColor& byWhat)
{
    char buf[20];
    auto length = snprintf(buf, std::size(buf), "#%02x%02x%02x", byWhat.red(), byWhat.green(), byWhat.blue());
    content.replace(what.data(), what.size(), buf, length);
}


QByteArray IconPalette::repaintFile(const QString& fname)
{
    QFile file(fname);
    file.open(QIODeviceBase::ReadOnly);
    QByteArray content = file.readAll();
    replaceColor(content, "#c01c28", fg);   // GNOME HIG red 4
    replaceColor(content, "#f9f06b", bg);   // GNOME HIG yellow 1
    return content;
}


///// MyRenderer ///////////////////////////////////////////////////////////////

class MyRenderer : public QSvgRenderer, public dumb::SpTarget
{
public:
    using QSvgRenderer::QSvgRenderer;
};


///// LazySvg //////////////////////////////////////////////////////////////////

class ie::LazySvg : public dumb::SpTarget
{
public:
    LazySvg(const uc::SynthIcon& icon, char32_t startingCp);
    LazySvg(const uc::Block& block)
        : LazySvg(block.synthIcon, block.startingCp) {}
    LazySvg(QString aFname) : fname(std::move(aFname)) {}
    LazySvg(const uc::SynthIcon& icon, QString aFname)
        : fname(std::move(aFname)) { loadPaletteIf(icon); }
    LazySvg(QString aFname, const uc::EcContinent& continent)
        : fname(std::move(aFname)) { loadPalette(continent); }
    void setPalette(const IconPalette& x) { palette = x; }

    /// Loads palette if the icon states “repaint SVG”
    void loadPaletteIf(const uc::SynthIcon& icon);

    void loadPalette(const uc::Continent& continent);
    void loadPalette(const uc::EcContinent& continent)
        { loadPalette(uc::continentInfo[static_cast<unsigned>(continent)]); }

    /// Gets the SVG renderer, probably from cache
    dumb::Sp<MyRenderer> get();
private:
    QString fname;
    dumb::Sp<MyRenderer> x;
    std::optional<IconPalette> palette = std::nullopt;
};

template class dumb::Sp<ie::LazySvg>;


ie::LazySvg::LazySvg(const uc::SynthIcon& icon, char32_t startingCp)
{
    char buf[48];
    util::sprintfCp(buf, startingCp);
    fname = buf;
    loadPaletteIf(icon);
}


void ie::LazySvg::loadPalette(const uc::Continent& continent)
{
    IconPalette pal {
        .fg = continent.icon.fgColor,
        .bg = continent.icon.bgColor,
    };
    setPalette(pal);
}


void ie::LazySvg::loadPaletteIf(const uc::SynthIcon& icon)
{
    if (icon.flags.have(uc::Ifg::PAINT_SVG)) {
        auto& continent = icon.maybeMissingContinent();
        loadPalette(continent);
    }
}


dumb::Sp<MyRenderer> ie::LazySvg::get()
{
    if (auto r1 = x)
        return r1;

    dumb::Sp<MyRenderer> r2;
    if (palette) {
        QByteArray ba = palette->repaintFile(fname);
        r2 = dumb::makeSp<MyRenderer>(ba);
    } else {
        // Dumb load
        r2 = dumb::makeSp<MyRenderer>(fname);
    }
    r2->setAspectRatioMode(Qt::KeepAspectRatio);
    x = r2;
    //std::cout << "Loaded lazy SVG " << fname.toStdString() << std::endl;
    return r2;
}


///// Synth ////////////////////////////////////////////////////////////////////

ie::Synth::Synth(const PixSource& aSource, const uc::SynthIcon& aSi, char32_t aPixStart)
    : source(aSource), si(aSi)
{
    if (aPixStart && aSi.flags.haveAny(uc::Ifg::MISSING | uc::Ifg::SMALL_SVG | uc::Ifg::BIG_SVG)) {
        if (aSi.flags.have(uc::Ifg::BIG_SVG)) { // Big SVG
            char buf[48];
            snprintf(buf, std::size(buf), ":/ScBig/%04X.svg", int(aPixStart));
            texture = dumb::makeSp<LazySvg>(buf);
        } else {    // Small SVG
            /// @todo [future] Only default engine is supported now, check!
            texture = dumb::makeSp<LazySvg>(si, aPixStart);
        }
    }
}

void ie::Synth::paint1(QPainter *painter, const QRect &rect, qreal scale)
{
    // No continent → draw murky, otherwise use icon colours
    auto clFg = source.winColor();
    auto cont = si.maybeMissingContinent();
    if (cont.isInternational) {
        drawMurkyRect(painter, rect, clFg);
    } else {
        painter->fillRect(rect, cont.icon.bgColor);
        drawCharBorder(painter, rect, clFg);
        clFg = cont.icon.fgColor;
    }

    // Texture?
    if (texture) {
        auto smallerSide = std::min(rect.width(), rect.height());
        auto sqSize = smallerSide >> 1;
        if (si.flags.have(uc::Ifg::BIG_SVG)) {
            sqSize = std::round(smallerSide * 0.75);
        } else if (si.flags.have(uc::Ifg::BIGGER)) {
            sqSize = smallerSide * 3 / 5;
        }
        auto dx = (rect.width() - sqSize) >> 1;
        auto dy = (rect.height() - sqSize) >> 1;
        QRectF smallRect(rect.left() + dx, rect.top() + dy, sqSize, sqSize);
        if (si.flags.have(uc::Ifg::BIG_SVG)) {
            smallRect = smallRect.marginsRemoved({0.3, 0.3, 0.3, 0.3});
        }
        texture->get()->render(painter, smallRect);
        return;
    }

    // Shift left/right
    auto rcContent = rect;
    if (si.flags.haveAny(uc::Ifg::SHIFT_LEFT | uc::Ifg::SHIFT_RIGHT)) {
        auto delta = rcContent.width() / 3;
        if (si.flags.haveAny(uc::Ifg::SHIFT_LEFT)) {
            rcContent.setLeft(rcContent.left() - delta);
        } else {
            rcContent.setRight(rcContent.right() + delta);
        }
    }
    if (si.flags.haveAny(uc::Ifg::SHIFT_DOWN | uc::Ifg::SHIFT_UP)) {
        auto delta = rcContent.height() >> 2;
        if (si.flags.haveAny(uc::Ifg::SHIFT_DOWN)) {
            rcContent.setBottom(rcContent.bottom() + delta);
        } else {
            rcContent.setTop(rcContent.top() - delta);
        }
    }
    // Get size
    if (si.flags.have(uc::Ifg::SMALLER))
        scale *= 0.8;
    auto size = lround(120 * scale);  // Draw icon a bit larger — 120%
    // Prepare sample string
    std::u32string_view sample = si.subj.sv();
    char32_t newSample[5];  // max.4
    if (si.flags.have(uc::Ifg::BOTH_DOTTED)) {
        unsigned len = 0;
        for (auto c : sample) {
            newSample[len++] = cp::DOTTED_CIRCLE;
            newSample[len++] = c;
        }
        sample = std::u32string_view{ newSample, len };
    }
    // Rotation
    if (si.flags.haveAny(uc::Ifg::ROTATE_LTR_CW | uc::Ifg::ROTATE_RTL_CCW)) {
        // Draw direction
        int direction = -90;
        if (si.flags.haveAny(uc::Ifg::ROTATE_LTR_CW))
            direction = 90;
        auto firstChar = si.subj.v[0];
        const uc::Cp& firstCp = *uc::cpsByCode[firstChar];
        auto font = fontAt(uc::DrawMethod::SAMPLE, size, firstCp);
        drawVertical(painter, rcContent, *font, direction, clFg, str::toQ(sample));
    } else {
        // Synth icon always draws in default settings
        drawIconChars(painter, rcContent, size, sample, clFg);
    }
}

///// Node /////////////////////////////////////////////////////////////////////

ie::Node::Node(const PixSource& aSource, const uc::LibNode& aNode)
    : source(aSource), node(aNode) {}

void ie::Node::paint1(QPainter *painter, const QRect &rect, qreal scale)
{
    auto clFg = source.winColor();
    // draw char
    switch (node.value.length()) {
    case 0:
        // Nodes always draw in default settings
        drawFolderTile(painter, rect, node, clFg, uc::GlyphStyleSets::EMPTY, scale);
        break;
    default:
        // Nodes always draw in default settings
        drawSearchChars(painter, rect, node.value, clFg, node.emojiDraw(),
                        TableDraw::LIBRARY, uc::GlyphStyleSets::EMPTY, scale);
        break;
    }
}

///// BlockElem ////////////////////////////////////////////////////////////////

ie::BlockElem::BlockElem()
{
    texture.load(":ScCustom/blockelem.png");
}

void ie::BlockElem::paint1(QPainter *painter, const QRect &rect, qreal)
{
    // White BG
    painter->fillRect(rect, BG_INTER);

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


///// BoxDraw //////////////////////////////////////////////////////////////////


namespace {

    template <std::integral T>
    constexpr inline T roundDiv(T x, T y) {
        return (x + (y >> 1)) / y;
    }

}

void ie::BoxDraw::paint1(QPainter *painter, const QRect &rect, qreal)
{
    painter->fillRect(rect, BG_INTER);

    const int pxSide = std::min(rect.width(), rect.height());
    auto thickness = roundDiv(pxSide, BASE_SIZE);
    auto doubleStep = roundDiv(pxSide * 3, BASE_SIZE);
    auto doubleWidth = doubleStep + thickness;
    // Arm is 4px long; from centre was not fine :)
    auto armLength = roundDiv(pxSide * 4, BASE_SIZE);

    auto doubleX = (pxSide - doubleWidth + 1) / 2;
    auto doubleLeft = rect.x() + doubleX;
    painter->fillRect(doubleLeft,              rect.y(), thickness, pxSide, FG_INTER);
    painter->fillRect(doubleLeft + doubleStep, rect.y(), thickness, pxSide, FG_INTER);

    auto armY = (pxSide + 1 - thickness) / 2;
    painter->fillRect(doubleLeft - armLength, rect.y() + armY, armLength, thickness, FG_INTER);
}


///// ControlPic ////////////////////////////////////////////////////////////////


void ie::ControlPic::paint1(QPainter *painter, const QRect &rect, qreal)
{
    painter->fillRect(rect, BG_INTER);
    static constexpr int BASE_SIZE_TEN = BASE_SIZE * 10;
    static constexpr int BASE_SIZE_TEN_LOHALF = (BASE_SIZE_TEN / 2) - 1;

    const int pxSide = std::min(rect.width(), rect.height());
    const int pxBorder = (rect.height() * MRG_SIMPLER + BASE_SIZE_TEN_LOHALF) / BASE_SIZE_TEN;

    auto pxThickness = roundDiv(pxSide, BASE_SIZE);
    auto pxLoSide = roundDiv(pxSide * 9, BASE_SIZE);
    auto pxTotalWidth = pxLoSide + pxThickness;
    auto pxArmHeight = roundDiv(pxSide * 3, BASE_SIZE);
    auto x = rect.x() + (pxSide - pxTotalWidth) / 2;
    auto yLo = pxSide - pxBorder - pxThickness;
    auto yArm = yLo - pxArmHeight;

    painter->fillRect(x, yLo,  pxTotalWidth, pxThickness, FG_INTER);
    painter->fillRect(x,            yArm, pxThickness, pxArmHeight, FG_INTER);
    painter->fillRect(x + pxLoSide, yArm, pxThickness, pxArmHeight, FG_INTER);
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

ie::Legacy::Legacy(const char* fname)
{
    texture.load(fname);
}


void ie::Legacy::paint1(QPainter *painter, const QRect &rect, qreal scale)
{
    // Fill BG
    painter->fillRect(rect, BG_INTER);

    static constexpr unsigned MIN_WIDTH = 11;

    // Get rect
    unsigned side = std::lround(16.0 * scale - 0.1);  // 0 / 0.5px — sometimes we request a bit smaller icon
    int times = (side - 1) / MIN_WIDTH;
    if (times < 1)
        times = 1;
    int ww = texture.width() * times;
    int hh = texture.height() * times;
    int x0 = 0;
    if (rect.width() > ww) {  // Those pics are always wide, not tall
        x0 = rect.left() + ((rect.width() - ww) >> 1);
        if (x0 < 1)
            x0 = 1;
    }
    int y0 = rect.top() + ((rect.height() - hh) >> 1);

    QRect rcDest { x0, y0, ww, hh };
    painter->setRenderHint(QPainter::SmoothPixmapTransform, false);
    painter->drawPixmap(rcDest, texture);
}


///// PlayingCard //////////////////////////////////////////////////////////////

ie::PlayingCard::PlayingCard()
    : texture(dumb::makeSp<LazySvg>(":ScCustom/playcard.svg")) {}

ie::PlayingCard::~PlayingCard() = default;


void ie::PlayingCard::paint1(QPainter *painter, const QRect &rect, qreal scale)
{
    painter->fillRect(rect, BG_INTER);

    // Get dimensions
    // (point’s dimensions are baked into SVG)
    auto dim = util::cardDimensions(rect, scale, 10, 0.3);
    auto indexSize = std::lround(dim.rcPixel.height() * (1.0 / 11.0));     // 1 at 1×, 2 at 1.25×
    auto radius = dim.rcPixel.height() * 0.1;

    // Frame
    painter->setBrush(Qt::white);  // Card’s face is always white, regardless of colour
    /// @todo [dark] Playing card: what is the pen? And what will be the dark BG?
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
    texture->get()->render(painter, dim.rcPixel);
}


///// Mahjong //////////////////////////////////////////////////////////////////

ie::Mahjong::Mahjong()
    : texture(dumb::makeSp<LazySvg>(":ScCustom/mahjong.svg")) {}

ie::Mahjong::~Mahjong() = default;


void ie::Mahjong::paint1(QPainter *painter, const QRect &rect, qreal scale)
{
    painter->fillRect(rect, BG_INTER);

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
    texture->get()->render(painter, dim.rcPixel);
}


///// Hint /////////////////////////////////////////////////////////////////////

ie::Hint::Hint(const uc::Block& blk)
    : icon(blk.synthIcon)
{
    texture = dumb::makeSp<LazySvg>(blk);
}

// -warn: complains about =default
ie::Hint::~Hint() {}


void ie::Hint::paint1(QPainter *painter, const QRect &rect, qreal)
{
    painter->fillRect(rect, icon.maybeMissingContinent().icon.bgColor);
    util::drawHintedSvg(painter, rect, *texture->get(), icon.svgHint);
}


///// Format ///////////////////////////////////////////////////////////////////


ie::Format::Format(const uc::Block& blk)
    : icon(blk.synthIcon)
{
    texture = dumb::makeSp<LazySvg>(blk);
}

ie::Format::~Format() = default;


void ie::Format::paint1(QPainter *painter, const QRect &rect, qreal scale)
{
    auto& continent = icon.maybeMissingContinent().icon;
    painter->fillRect(rect, continent.bgColor);

    auto fi = util::getFormatInfo(rect, scale);
    util::drawLoResFormat(painter, fi.frameRect, fi.dashSize, continent.frameColor);

    util::drawHintedSvg(painter, rect, *texture->get(), icon.svgHint);
}

///// CjkStructure /////////////////////////////////////////////////////////////

void ie::CjkStructure::paint1(QPainter *painter, const QRect &rect, qreal scale)
{
    painter->fillRect(rect, BG_CJK);

    auto fi = util::getFormatInfo(rect, scale);
    util::drawLoResFormat(painter, fi.frameRect, fi.dashSize, FRAME_CJK);

    auto loSz = ((rect.width() - 2*fi.dashSize) * 2 / 3) + fi.dashSize;
    QRect rc2(0, 0, loSz, loSz);
    rc2.moveTopRight(fi.frameRect.topRight());
    util::drawFormatBottomLeft(painter, rc2, fi.dashSize, FRAME_CJK);
}


///// TallyMark ////////////////////////////////////////////////////////////////

void ie::TallyMark::paint1(QPainter *painter, const QRect &rect, qreal)
{
    painter->fillRect(rect, BG_INTER);

    // Sticks
    static constexpr auto N_STICKS = 4;
    // Equalize margins above and below
    auto stickStep = rect.width() * 3 / (4 * N_STICKS);
    auto stickThickness = stickStep >> 1;
    if (stickThickness <= 0)
        stickThickness = 1;
    auto stickTotalWidth = stickStep * (N_STICKS - 1) + stickThickness;
    unsigned stickHeight = stickStep * 4.5;
    if ((rect.height() % 2) != static_cast<int>(stickHeight % 2))
        ++stickHeight;
    auto stickY = rect.top() + (rect.height() - stickHeight) / 2;
    auto stickX0 = rect.left() + (rect.width() - stickTotalWidth) / 2;
    auto x = stickX0;
    for (int i = 0; i < N_STICKS; ++i) {
        painter->fillRect(x, stickY, stickThickness, stickHeight, FG_INTER);
        x += stickStep;
    }

    // Slash
    static constexpr auto SLASH_RELY = 0.8;
    static constexpr auto SLASH_RELY2 = 1.0 - SLASH_RELY;
    static constexpr auto MINUS_FOR_GAMMA = 0.3;    // reduce due to monitor’s gamma
    auto slashHangout = (stickStep * 0.4) + 0.5;    // 0.5 is for monitor’s gamma
    auto slashX1 = stickX0 - slashHangout;
    auto slashX2 = stickX0 + stickTotalWidth + slashHangout;
    auto slashY1 = stickY + stickHeight * SLASH_RELY;
    auto slashY2 = stickY + stickHeight * SLASH_RELY2;
    painter->setRenderHint(QPainter::Antialiasing);
    QPen pen(FG_INTER, stickThickness - MINUS_FOR_GAMMA);
    painter->setPen(pen);
    painter->drawLine(QPointF{slashX1, slashY1}, QPointF{slashX2, slashY2});
}


///// ThreeD ///////////////////////////////////////////////////////////////////


ie::ThreeD::ThreeD()
    : texture(dumb::makeSp<LazySvg>(":ScCustom/3D.svg", uc::EcContinent::NONE)) {}

ie::ThreeD::~ThreeD() = default;

void ie::ThreeD::paint1(QPainter *painter, const QRect &rect, qreal scale)
{
    painter->fillRect(rect, BG_INTER);

    auto fi = util::getFormatInfo(rect, scale);
    util::drawPixelBorder(painter, fi.frameRect, fi.dashSize, FG_INTER);

    util::drawHintedSvg(painter, fi.contentRect, *texture->get(), {});
}


///// SqIdeo ///////////////////////////////////////////////////////////////////

ie::SqIdeo::SqIdeo()
    : texture(dumb::makeSp<LazySvg>(":ScCustom/sqideo.svg", uc::EcContinent::CJK)) {}

ie::SqIdeo::~SqIdeo() {}

void ie::SqIdeo::paint1(QPainter *painter, const QRect &rect, qreal scale)
{
    painter->fillRect(rect, BG_CJK);

    auto fi = util::getFormatInfo(rect, scale);
    painter->fillRect(fi.frameRect, FRAME_CJK);

    util::drawHintedSvg(painter, rect, *texture->get(), {});
}


///// OneCircle ////////////////////////////////////////////////////////////////

ie::OneCircle::OneCircle()
    : texture(dumb::makeSp<LazySvg>(":ScCustom/1circ.svg", uc::EcContinent::NONE)) {}

ie::OneCircle::~OneCircle() = default;

void ie::OneCircle::paint1(QPainter *painter, const QRect &rect, qreal scale)
{
    // Should be 2 at 1.5×
    static constexpr int BASE_SIZE_HIHALF = (BASE_SIZE / 2) + 1;

    painter->fillRect(rect, BG_INTER);

    // Frame rect
    unsigned margin = (rect.width() + BASE_SIZE_HIHALF) / BASE_SIZE;
    QRect rcFrame = rect.marginsRemoved(QMargins(margin, margin, margin, margin));

    // Thickness
    auto thick = 0.7 * scale;
    auto th2 = thick * 0.5;
    QRectF rcEllipse = rcFrame;
    rcEllipse = rcEllipse.marginsRemoved({th2, th2, th2, th2});

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(FG_INTER, thick));
    painter->setBrush(Qt::NoBrush);
    painter->drawEllipse(rcEllipse);

    texture->get()->render(painter, rect);
}


///// Margin ///////////////////////////////////////////////////////////////////


ie::Margin::Margin(const uc::SynthIcon& synthIcon,
                   std::string_view aName, int aValue,
                   Flags<Mfg> aFlags)
    : texture(dumb::makeSp<LazySvg>(synthIcon, str::toQ(aName))),
      bgColor(synthIcon.maybeMissingContinent().icon.bgColor),
      value(aValue), flags(aFlags) {}

ie::Margin::~Margin() = default;

void ie::Margin::paint1(QPainter *painter, const QRect &rect, qreal)
{
    static constexpr int BASE_SIZE_TEN = BASE_SIZE * 10;
    static constexpr int BASE_SIZE_TEN_LOHALF = (BASE_SIZE_TEN / 2) - 1;

    // Background
    painter->fillRect(rect, bgColor);

    // SVG
    unsigned margin = (rect.width() * value + BASE_SIZE_TEN_LOHALF) / BASE_SIZE_TEN;
    QRect rcContent = rect.marginsRemoved(QMargins(margin, margin, margin, margin));
    if (flags.have(Mfg::IMBA_UP)) {
        auto shift = (rect.width() + 15) / 32;  // 15: 0.5 = down
        rcContent.moveTop(rcContent.top() + shift);
    }
    texture->get()->render(painter, rcContent);
}


///// Tall /////////////////////////////////////////////////////////////////////


ie::Tall::Tall(const uc::SynthIcon& synthIcon, std::string_view aName,
               unsigned char aHintX, uc::ImbaX aImbaX,
               bool aHintPixelCenter)
    : texture(dumb::makeSp<LazySvg>(synthIcon, str::toQ(aName))),
      bgColor(synthIcon.maybeMissingContinent().icon.bgColor),
      hintX(aHintX),
      imbaX(static_cast<signed char>(aImbaX)),
      hintPixelCenter(aHintPixelCenter) {}

ie::Tall::Tall(const uc::Block& block)
    : bgColor(block.synthIcon.maybeMissingContinent().icon.bgColor),
      hintX(block.synthIcon.svgHint.pos.x),
      imbaX(block.synthIcon.svgHint.imba.x),
      hintPixelCenter(block.synthIcon.flags.have(uc::Ifg::HINT_PX_CENTER))
{
    char buf[48];
    snprintf(buf, std::size(buf), ":/ScTall/%04X.svg", int(block.startingCp));
    texture = dumb::makeSp<LazySvg>(block.synthIcon, buf);
}

ie::Tall::~Tall() = default;

void ie::Tall::paint1(QPainter *painter, const QRect &rect, qreal)
{
    static constexpr int BASE_SIZE_TEN = BASE_SIZE * 10;
    static constexpr int BASE_SIZE_TEN_LOHALF = (BASE_SIZE_TEN / 2) - 1;
    // Image height in device-independent pixels
    static constexpr int HEIGHT_DIP = 14;

    // Background
    painter->fillRect(rect, bgColor);

    // Width?
    auto rend = texture->get();
    auto width = rend->defaultSize().width();

    // Get base sizeand position
    unsigned scaledBorder = (rect.height() * MRG_SIMPLER + BASE_SIZE_TEN_LOHALF) / BASE_SIZE_TEN;
    auto y = scaledBorder;
    auto scaledH = rect.height() - (scaledBorder << 1);
    if (scaledH <= 0)   // strange??
        return;
    auto realScale = static_cast<double>(scaledH) / HEIGHT_DIP;
    auto scaledW = width * realScale;
    auto x = (rect.width() - scaledW) / 2;

    // Hint by moving left/right
    if (hintX != uc::NO_HINT.v) {
        auto hintActual = x + (hintX - imbaX * 0.1) * realScale;
        double myHintX = hintX;
        double hintWanted;
        if (hintPixelCenter) {
            hintWanted = std::round(hintActual - 0.5) + 0.5;
            myHintX += 0.5;
        } else {
            hintWanted = std::round(hintActual);
        }
        x = hintWanted - myHintX * realScale;
    }

    QRectF r(rect.left() + x, rect.top() + y, scaledW, scaledH);
    rend->render(painter, r);
}


///// ByLong ///////////////////////////////////////////////////////////////////


ie::ByLong::ByLong(const uc::SynthIcon& synthIcon, std::string_view aName)
    : texture(dumb::makeSp<LazySvg>(synthIcon, str::toQ(aName))),
      bgColor(synthIcon.maybeMissingContinent().icon.bgColor) {}

ie::ByLong::ByLong(const uc::Block& block)
    : bgColor(block.synthIcon.maybeMissingContinent().icon.bgColor)
{
    char buf[48];
    snprintf(buf, std::size(buf), ":/ScLong/%04X.svg", int(block.startingCp));
    texture = dumb::makeSp<LazySvg>(block.synthIcon, buf);
}

ie::ByLong::~ByLong() = default;

void ie::ByLong::paint1(QPainter *painter, const QRect &rect, qreal scale)
{
    // Background
    painter->fillRect(rect, bgColor);

    // Get principal values in devide-independent pixels
    const auto rend = texture->get();
    const auto dipWidth = rend->defaultSize().width();
    const auto dipHeight = rend->defaultSize().height();
    auto dipLoSide = dipWidth;
    auto dipHiSide = dipHeight;
    /// [+] wide  [-] tall
    const bool isWide = (dipWidth > dipHeight);
    if (isWide)
        std::swap(dipLoSide, dipHiSide);

    const auto dip2xMargin = BASE_SIZE - dipHiSide;

    // We think that the rectangle is square here
    const int pxSide = std::min(rect.width(), rect.height());
    // Let it le this way: 1× will surely yield 1px,
    // hypothetical 1.1× will probably shrink smaller side even more
    //   and will yield >1px,
    // and 1.25× is ju-ust < 1px  (1/1.25 = 0.8)
    const double pxAllowed2xMargin = scale * dip2xMargin * 0.78;
    const double pxAllowedHiSide = pxSide - pxAllowed2xMargin;
    const double pxCorrespLoSide = pxAllowedHiSide * dipLoSide / dipHiSide;
    const int pxLoSide = pxCorrespLoSide;     // rounded down
    if (pxLoSide <= 0)  // strange values?
       return;
    const double pxHiSide = double(pxLoSide * dipHiSide) / dipLoSide;
    const double pxHiMargin = (pxSide - pxHiSide) * 0.5;
    const int pxLoMargin = (pxSide - pxLoSide) / 2;
    const auto r = isWide   // Wide: hi = X, lo = Y.     Tall: lo = X, hi = Y
        ? QRectF( rect.left() + pxHiMargin, rect.top() + pxLoMargin, pxHiSide, pxLoSide )
        : QRectF( rect.left() + pxLoMargin, rect.top() + pxHiMargin, pxLoSide, pxHiSide );
    rend->render(painter, r);
}


///// Small ////////////////////////////////////////////////////////////////////

ie::Small::Small(const uc::SynthIcon& synthIcon, std::string_view name,
                 unsigned char aHintX, uc::ImbaX aImbaX)
    : texture(dumb::makeSp<LazySvg>(synthIcon, str::toQ(name))),
      bgColor(synthIcon.maybeMissingContinent().icon.bgColor),
      hintX(aHintX),
      imbaX(static_cast<unsigned char>(aImbaX)) {}

ie::Small::Small(const uc::Block& block)
    : bgColor(block.synthIcon.maybeMissingContinent().icon.bgColor),
      hintX(block.synthIcon.svgHint.pos.x),
      imbaX(block.synthIcon.svgHint.imba.x)
{
    char buf[48];
    snprintf(buf, std::size(buf), ":/ScSmall/%04X.svg", int(block.startingCp));
    texture = dumb::makeSp<LazySvg>(block.synthIcon, buf);
}

ie::Small::~Small() = default;

void ie::Small::paint1(QPainter *painter, const QRect &rect, qreal)
{
    static constexpr int BASE_SIZE_TEN = BASE_SIZE * 10;
    static constexpr int BASE_SIZE_TEN_LOHALF = (BASE_SIZE_TEN / 2) - 1;
    static constexpr int REMDER_TEN = 138;  // In 1× will round to exact dipHeight

    // Background
    painter->fillRect(rect, bgColor);

    const auto rend = texture->get();
    const auto dipWidth = rend->defaultSize().width();
    const auto dipHeight = rend->defaultSize().height();
    const int pxSide = std::min(rect.width(), rect.height());

    // Get Y coords
    int pxBorder = (rect.height() * MRG_SIMPLER + BASE_SIZE_TEN_LOHALF) / BASE_SIZE_TEN;
    int pxRemder = pxSide - pxBorder * 2;
    int pxHeight = roundDiv(dipHeight * 10 * pxRemder, REMDER_TEN);
    int pxY = pxSide - pxBorder - pxHeight;
    double realScale = double(pxHeight) / dipHeight;

    // Get X coord somehow
    double pxWidth = double(pxHeight * dipWidth) / dipHeight;
    double pxX = (rect.width() - pxWidth) * 0.5;

    // Hint X to pixels
    auto hintActual = pxX + (hintX - imbaX * 0.1) * realScale;
    double myHintX = hintX;
    double hintWanted = std::round(hintActual);
    pxX = hintWanted - myHintX * realScale;

    QRectF r(rect.left() + pxX, rect.top() + pxY, pxWidth, pxHeight);
    rend->render(painter, r);
}
