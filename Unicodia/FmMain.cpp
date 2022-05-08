// My header
#include "FmMain.h"
#include "ui_FmMain.h"

// C++
#include <iostream>
#include <cmath>
#include <bit>
#include <filesystem>

// Qt
#include <QTableView>
#include <QTextFrame>
#include <QClipboard>
#include <QPainter>
#include <QShortcut>
#include <QPaintEngine>
#include <QMessageBox>
#include <QFile>
#include <QDesktopServices>
#include <QUrl>
#include <QTimer>
#include <QSvgRenderer>

// Misc
#include "u_Strings.h"
#include "u_Qstrings.h"

// Project-local
#include "Skin.h"
#include "Wiki.h"
#include "MyWiki.h"
#include "u_EmojiPainter.h"

// Forms
#include "FmPopup.h"
#include "FmMessage.h"
#include "FmTofuStats.h"

// L10n
#include "LocDic.h"

template class LruCache<char32_t, QPixmap>;

using namespace std::string_view_literals;

namespace {
    // No need custom drawing — solves nothing
    constexpr TableDraw TABLE_DRAW = TableDraw::INTERNAL;
    const char16_t U16_TOFU[] { 0xD807, 0xDEE0, 0 };     // Makasar Ka
    EmojiPainter emp;
}

///// FmPopup2 /////////////////////////////////////////////////////////////////


FmPopup2::FmPopup2(FmMain* owner) : Super(owner, CNAME_BG_POPUP)
{
    auto vw = viewport();
    vw->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::TextSelectableByMouse);
    connect(vw, &QLabel::linkActivated, owner, &FmMain::popupLinkActivated);
}


///// RowCache /////////////////////////////////////////////////////////////////

RowCache::RowCache(int anCols)
    : fnCols(anCols), fColMask(anCols - 1), fRowMask(~fColMask) {}


MaybeChar RowCache::charAt(size_t iRow, unsigned iCol) const
{
    // Have row?
    if (iRow >= rows.size() || iCol >= NCOLS)
        return { 0, nullptr };
    auto& rw = rows[iRow];
    auto start = rw.startingCp;

    return { start + iCol, rw.cps[iCol] };
}


int RowCache::startingCpAt(size_t iRow) const
{
    if (iRow >= rows.size())
        return uc::NO_CHAR;
    return rows[iRow].startingCp;
}


RowCache::Row& RowCache::ensureRow(unsigned aStartingCp)
{
    if (!rows.empty()) {
        Row& bk = rows.back();
        if (bk.startingCp == aStartingCp)
            return bk;
    }
    Row& newBk = rows.emplace_back(aStartingCp);
    return newBk;
}


void RowCache::addCp(const uc::Cp& aCp)
{
    unsigned code = aCp.subj;
    int rowCp = code & fRowMask;
    int iCol = code & fColMask;

    Row& bk = ensureRow(rowCp);
    bk.cps[iCol] = &aCp;
}


bool RowCache::isLessRC(const Row& x, char32_t y)
{
    return x.startingCp < y;
}


CacheCoords RowCache::findCode(char32_t code) const
{
    auto it = std::lower_bound(rows.begin(), rows.end(), code, isLessRC);
    if (it == rows.end() || it->startingCp > code)
        --it;
    size_t rw = it - rows.begin();
    unsigned dif = code - it->startingCp;
    if (dif < static_cast<unsigned>(fnCols)) {
        return { rw, dif };
    } else {
        return { rw, 0 };
    }
}


///// BlocksModel //////////////////////////////////////////////////////////////


int BlocksModel::rowCount(const QModelIndex&) const { return uc::N_BLOCKS; }

int BlocksModel::columnCount(const QModelIndex&) const { return 1; }

QVariant BlocksModel::data(const QModelIndex& index, int role) const
{
    #define GET_BLOCK \
            size_t i = index.row();         \
            if (i >= uc::N_BLOCKS)          \
                return {};                  \
            auto& block = uc::blocks[i];

    switch (role) {
    case Qt::DisplayRole: {
            GET_BLOCK
            return str::toQ(block.loc.name);
        }
    case Qt::DecorationRole: {
            GET_BLOCK
            if (!block.icon) {
                char buf[48];
                snprintf(buf, std::size(buf), ":/Scripts/%04X.png", static_cast<int>(block.startingCp));
                block.icon = new QPixmap();
                block.icon->load(buf);
            }
            return *block.icon;
        }
    default:
        return {};
    }

    #undef GET_BLOCK
}


///// CharsModel ///////////////////////////////////////////////////////////////


CharsModel::CharsModel(QWidget* aOwner) :
    owner(aOwner),
    match(str::toQ(FAM_DEFAULT)),
    rows(NCOLS)
{
    tcache.connectSignals(this);
}


CharsModel::~CharsModel() = default;


int CharsModel::rowCount(const QModelIndex&) const
{
    return rows.nRows();
}


int CharsModel::columnCount(const QModelIndex&) const
    { return NCOLS; }


std::optional<QFont> CharsModel::fontAt(const QModelIndex& index) const
{
    if (auto cp = charAt(index))
        return fontAt(*cp);
    return {};
}


std::optional<QFont> CharsModel::fontAt(const uc::Cp& cp)
{
    static constexpr int DUMMY_DPI = 96;
    if (cp.drawMethod(DUMMY_DPI) > uc::DrawMethod::LAST_FONT)
        return {};
    auto font = cp.font(uc::MatchLast::NO);
    return font->get(uc::FontPlace::CELL, FSZ_TABLE,
                    cp.flags.have(uc::Cfg::NO_AA), cp.subj);
}


QColor CharsModel::fgAt(const QModelIndex& index, TableColors tcl) const
{
    auto cp = charAt(index);
    if (!cp)
        return {};
    return fgAt(*cp, tcl);
}


QColor CharsModel::fgAt(const uc::Cp& cp, TableColors tcl) const
{
    if (tcl != TableColors::NO) {
        if (isCjkCollapsed) {
            auto block = uc::blockOf(cp.subj);
            if (block->flags.have(uc::Bfg::COLLAPSIBLE))
                return TX_CJK;
        }
    }
    return {};
}


QString CharsModel::textAt(const QModelIndex& index, int aDpi) const
{
    auto cp = charAt(index);
    if (!cp)
        return {};
    return textAt(*cp, aDpi);
}


QString CharsModel::textAt(const uc::Cp& cp, int dpi)
    { return cp.sampleProxy(dpi).text; }


QVariant CharsModel::data(const QModelIndex& index, int role) const
{
    switch (role) {
    case Qt::DisplayRole:
        if constexpr (TABLE_DRAW == TableDraw::INTERNAL) {
            if (auto q = textAt(index, uc::DPI_ALL_CHARS); !q.isEmpty())
                return q;
        }
        return {};

    case Qt::FontRole:
        if constexpr (TABLE_DRAW == TableDraw::INTERNAL) {
            if (auto q = fontAt(index))
                return *q;
        }
        return {};

    case Qt::ForegroundRole:
        if constexpr (TABLE_DRAW == TableDraw::INTERNAL) {
            if (auto c = fgAt(index, TableColors::YES); c.isValid())
                return c;
        }
        if (isCjkCollapsed) {

        }
        return {};

    case Qt::BackgroundRole: {
            auto cp = charAt(index);
            if (cp) {
                if (isCjkCollapsed) {
                    auto block = uc::blockOf(cp->subj);
                    if (block->flags.have(uc::Bfg::COLLAPSIBLE))
                        return BG_CJK;
                }
            } else {
                if (uc::isNonChar(cp.code)) {
                    return QBrush(owner->palette().windowText().color(), Qt::DiagCrossPattern);
                }
                return owner->palette().button().color();
            }
            return {};
        }

    case Qt::TextAlignmentRole:
        return Qt::AlignCenter;
    default:
        return {};
    }
}


namespace {

    template <int Ncols>
    QString horzHeaderOf(int section);

    template <>
    [[maybe_unused]] inline QString horzHeaderOf<8>(int section)
    {
        return QString::number(section, 16) + QString::number(section + 8, 16);
    }

    template <>
    [[maybe_unused]] inline QString horzHeaderOf<16>(int section)
    {
        return QString::number(section, 16);
    }

}   // anon namespace


QVariant CharsModel::headerData(int section, Qt::Orientation orientation,
                    int role) const
{
    switch (role) {
    case Qt::DisplayRole:
        if (orientation == Qt::Horizontal) {
            return horzHeaderOf<NCOLS>(section);
        } else {
            int c = rows.startingCpAt(section);
            if (static_cast<size_t>(section) >= rows.nRows())
                return {};
            char buf[20];
            snprintf(buf, std::size(buf), "%04x", c);
            return buf;
        }
        break;
    default:
        return {};
    }
}


void CharsModel::addCp(const uc::Cp& aCp)
{
    rows.addCp(aCp);
}


QModelIndex CharsModel::indexOf(char32_t code)
{
    auto coords = rows.findCode(code);
    return index(coords.row, coords.col);
}

void CharsModel::build()
{
    beginResetModel();
    rows.clear();
    for (auto& cp : uc::cpInfo) {
        if (isCharCollapsed(cp.subj))
            continue;
        addCp(cp);
    }
    endResetModel();
}


namespace {

    enum class SplitMode { NORMAL, FIXED };

    struct AbbrTable {
        qreal quos[10];     // 0 = 1-character
    };

    struct AbbrLines {
        std::u8string_view line1, line2, line3;

        unsigned nLines() const { return line2.empty() ? 1 : line3.empty() ? 2 : 3; }
        bool wasSplit() const { return !line2.empty(); }
        unsigned length() const {
            return std::max(std::max(line1.length(), line2.length()), line3.length()); }
        qreal sizeQuo(const AbbrTable& table) const;
    };

    template <class T, size_t N>
    constexpr T& last(T(&x)[N]) { return x[N-1]; }

    qreal AbbrLines::sizeQuo(const AbbrTable& tb) const
    {
        auto sz = nLines(), len = length();
        switch (sz) {
        case 3: return last(tb.quos);
        case 1:
            if (len == 1
                || (len == 2 && line1[0] >= 0b1100'0000 && line1[0] <= 0b1101'1111)     // 2-byte seq of UTF-8
                || (len == 3 && line1[0] >= 0b1110'0000 && line1[0] <= 0b1110'1111))    // 3-byte seq of UTF-8
                return tb.quos[0];
            [[fallthrough]];
        default:
            return tb.quos[len];
        }
    }

    // Table for normal abbreviations
    constexpr qreal Q1 = 1.5;
    constexpr qreal Q3 = 2.5;
    constexpr qreal Q4 = 3.0;
    constexpr qreal Q5 = 3.75;
                    //   0   1   2   3   4   5   6   7   8   9
    AbbrTable atAbbr { { Q1, Q3, Q3, Q3, Q4, Q5, Q5, Q5, Q5, Q5 } };

    // Table for tags
    constexpr qreal T3 = 2.1;
                    //   0   1   2   3   4   5   6   7   8   9
    AbbrTable atTags { { Q1, T3, T3, Q3, Q4, Q5, Q5, Q5, Q5, Q5 } };

    AbbrLines splitAbbr(std::u8string_view abbr, SplitMode mode)
    {
        if (mode == SplitMode::FIXED)
            return { abbr, {}, {} };
        if (auto pSpace = abbr.find(' '); pSpace != std::u8string_view::npos) {
            auto line1 = abbr.substr(0, pSpace), line23 = abbr.substr(pSpace + 1);
            if (auto pSpace2 = line23.find(' '); pSpace2 != std::u8string_view::npos) {
                auto line2 = line23.substr(0, pSpace2);
                auto line3 = line23.substr(pSpace + 1);
                return { line1, line2, line3 };
            } else {
                return { line1, line23, {} };
            }
        }
        switch (abbr.size()) {
        case 4:
        case 6: {
                auto split = abbr.size() / 2;
                return { abbr.substr(0, split), abbr.substr(split), {} };
            }
        case 5: {
                auto split = 3;
                if (isdigit(abbr[2]) && isdigit(abbr[3]))
                    split = 2;
                return { abbr.substr(0, split), abbr.substr(split), {} };
            }
        default:
            return { abbr, {}, {} };
        }
    }

    struct RcPair {
        QRectF rc1, rc2;

        RcPair(const QRectF& rcFrame, qreal quo) : rc1(rcFrame), rc2(rcFrame) {
            auto dh = rcFrame.height() / quo;
            rc1.setBottom(rc1.bottom() - dh);
            rc2.setTop(rc2.top() + dh);
        }
    };

    struct Rc3Matrix {
        int side, side2, side3, x0, y0;
        Rc3Matrix(const QRectF& rcFrame) {
            auto sz = std::min(rcFrame.width(), rcFrame.height());
            side = std::round(sz / 5.5);
            side2 = side * 2;
            side3 = side * 3;
            auto cen = rcFrame.center();
            x0 = std::round(cen.x() - side * 1.5);
            y0 = std::round(cen.y() - side * 1.5);
        }

        //  789
        //  456
        //  123
        QRect rect7() const
            { return QRect( QPoint{ x0, y0 }, QSize { side, side }); }
        QRect rect96() const
            { return QRect( QPoint{ x0 + side2, y0 }, QSize { side, side2 }); }
        QRect rect13() const
            { return QRect( QPoint{ x0, y0 + side2 }, QSize { side3, side }); }
        QRect rect1() const
            { return QRect( QPoint{ x0, y0 + side2 }, QSize { side, side }); }
        QRect rect63() const
            { return QRect( QPoint{ x0 + side2, y0 + side }, QSize { side, side2 }); }
        QRect rect79() const
            { return QRect( QPoint{ x0, y0 }, QSize { side3, side }); }
        QRect rect74() const
            { return QRect( QPoint{ x0, y0 }, QSize { side, side2 }); }
        QRect rect9() const
            { return QRect( QPoint{ x0 + side2, y0 }, QSize { side, side }); }
        QRect rect41() const
            { return QRect( QPoint{ x0, y0 + side }, QSize { side, side2 }); }
        QRect rect3() const
            { return QRect( QPoint{ x0 + side2, y0 + side2 }, QSize { side, side }); }
    };

    void drawCustomAbbrText(QPainter* painter, const AbbrLines& sp,
            const QColor& color, QRectF rcFrame, qreal thickness,
            const AbbrTable& table)
    {
        // Draw text
        auto availSize = rcFrame.width();
        auto sz = availSize / sp.sizeQuo(table);
        QFont font { str::toQ(FAM_CONDENSED) };
            font.setStyleStrategy(QFont::PreferAntialias);
            font.setPixelSize(sz);
        painter->setFont(font);
        painter->setBrush(QBrush(color, Qt::SolidPattern));
        rcFrame.setLeft(rcFrame.left() + std::max(thickness, 1.0));
        switch (sp.nLines()) {
        case 1:
            painter->drawText(rcFrame, Qt::AlignCenter, str::toQ(sp.line1));
            break;
        case 2: {
                RcPair p(rcFrame, 2.5);
                painter->drawText(p.rc1, Qt::AlignCenter, str::toQ(sp.line1));
                painter->drawText(p.rc2, Qt::AlignCenter, str::toQ(sp.line2));
            } break;
        default: { // case 3
                RcPair p(rcFrame, 1.8);
                painter->drawText(p.rc1, Qt::AlignCenter, str::toQ(sp.line1));
                painter->drawText(rcFrame, Qt::AlignCenter, str::toQ(sp.line2));
                painter->drawText(p.rc2, Qt::AlignCenter, str::toQ(sp.line3));
            }
        }
    }

    void drawAbbrText(QPainter* painter, std::u8string_view abbreviation,
            const QColor& color, QRectF rcFrame, qreal thickness)
    {
        // Draw text
        auto lines = splitAbbr(abbreviation, SplitMode::NORMAL);
        drawCustomAbbrText(painter, lines, color, rcFrame, thickness, atAbbr);
    }

    struct ControlFrame {
        QRectF r;
        qreal thickness;
    };

    ControlFrame drawControlFrame(
            QPainter* painter, const QRect& rect, const QColor& color)
    {
        const auto availW = rect.width() * 5 / 6;
        const auto availH = rect.height() * 7 / 10;
        const auto availSize = std::min(availW, availH);
        // Now we draw availSize×availSize, shrink rect
        auto ofsX = (rect.width() - availSize) / 2;
        auto ofsY = (rect.height() - availSize) / 2;
        auto rcFrame = QRectF(QPoint(rect.left() + ofsX, rect.top() + ofsY),
                              QSize(availSize, availSize));
        // Draw frame
        auto thickness = availSize / 60.0f;
        painter->setPen(QPen(color, thickness, Qt::DashLine));
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(rcFrame);

        return { rcFrame, thickness };
    }

    void drawFunkySample(
            QPainter* painter, const QRectF& rect, const QColor& color,
            uc::FontPlace place, float quo, char32_t trigger, QChar sample)
    {
        auto fn = uc::funkyFont(place, 50, trigger);
        fn.setPixelSize(std::round(rect.width() * 0.75f * quo));
        painter->setFont(fn);
        painter->setBrush(QBrush(color, Qt::SolidPattern));
        painter->drawText(rect, Qt::AlignCenter, sample);
    }

    void drawTagText(QPainter* painter, std::u8string_view abbreviation,
            const QColor& color, const QRectF& rcFrame, qreal thickness,
            uc::FontPlace place, char32_t trigger)
    {
        // Draw text
        AbbrLines lines { abbreviation, u8" ", {} };
        drawCustomAbbrText(painter, lines, color, rcFrame, thickness, atTags);
        drawFunkySample(painter, rcFrame, color, place, 1.2f, trigger, uc::STUB_PUA_TAG);
    }

    void drawCustomControl(
            QPainter* painter, const QRect& rect, const QColor& color,
            uc::FontPlace place, char32_t subj)
    {
        auto [rcFrame, thickness] = drawControlFrame(painter, rect, color);
        // Need this brush for both rects and fonts

        switch (subj) {
        case 0x11D45:   // Masaram Gondi virama
        case 0x11D97:   // Gunjala Gondi virama
            drawFunkySample(painter, rcFrame, color, place, 1.0f, subj, uc::STUB_PUA_VIRAMA);
            break;
        case 0x16F8F:   // Miao tone right
            drawFunkySample(painter, rcFrame, color, place, 1.0f, subj, uc::STUB_PUA_TONE_RIGHT);
            break;
        case 0x16F90:   // Miao tone top right
            drawFunkySample(painter, rcFrame, color, place, 1.0f, subj, uc::STUB_PUA_TONE_TOPRIGHT);
            break;
        case 0x16F91:   // Miao tone above
            drawFunkySample(painter, rcFrame, color, place, 1.0f, subj, uc::STUB_PUA_TONE_ABOVE);
            break;
        case 0x16F92:   // Miao tone below
            drawFunkySample(painter, rcFrame, color, place, 1.0f, subj, uc::STUB_PUA_TONE_BELOW);
            break;
        case 0x1BCA0:   // Shorthand format Letter overlap
            drawFunkySample(painter, rcFrame, color, place, 1.0f, subj, uc::STUB_PUA_OVERLAP);
            break;
        case 0x1BCA1:   // Shorthand format Continuing overlap
            drawFunkySample(painter, rcFrame, color, place, 1.0f, subj, uc::STUB_PUA_CONTINUING_OVERLAP);
            break;
        case 0x13431:   // Egyptian hiero horz joiner
            drawFunkySample(painter, rcFrame, color, place, 1.0f, subj, uc::STUB_PUA_BIG_STAR);
            break;
        case 0x13432: {
                Rc3Matrix m(rcFrame);
                painter->fillRect(m.rect7(), color);
                painter->fillRect(m.rect96(), color);
                painter->fillRect(m.rect13(), color);
            } break;
        case 0x13433: {
                Rc3Matrix m(rcFrame);
                painter->fillRect(m.rect79(), color);
                painter->fillRect(m.rect63(), color);
                painter->fillRect(m.rect1(), color);
            } break;
        case 0x13434: {
                Rc3Matrix m(rcFrame);
                painter->fillRect(m.rect74(), color);
                painter->fillRect(m.rect9(), color);
                painter->fillRect(m.rect13(), color);
            } break;
        case 0x13435: {
                Rc3Matrix m(rcFrame);
                painter->fillRect(m.rect79(), color);
                painter->fillRect(m.rect41(), color);
                painter->fillRect(m.rect3(), color);
            } break;
        case 0xE0001:
                // 00A0 = NBSP
            drawTagText(painter, u8"BEGIN"sv, color, rcFrame, thickness,
                        place, subj);
            break;
        case 0xE0020:
            drawTagText(painter, u8"SP"sv, color, rcFrame, thickness,
                        place, subj);
            break;
        case 0xE007F:
            drawTagText(painter, u8"END"sv, color, rcFrame, thickness,
                        place, subj);
            break;
        default:
            // Tags
            if (subj >= 0xE0000 && subj <= 0xE00FF) {
                char8_t c = subj;
                std::u8string_view line1 { &c, 1 };
                drawTagText(painter, line1, color, rcFrame, thickness,
                            place, subj);
            }
        }
    }

    void drawAbbreviation(
            QPainter* painter, const QRect& rect, std::u8string_view abbreviation,
            const QColor& color)
    {
        auto [rcFrame, thickness] = drawControlFrame(painter, rect, color);
        drawAbbrText(painter, abbreviation, color, rcFrame, thickness);
    }

    QSize spaceDimensions(const QFont& font, char32_t subj)
    {
        QString s = QString::fromUcs4(&subj, 1);
        QFontMetrics metrics(font);
        return {   // Spaces are SPACING → ban 0
                   // All known spaces are direction-neutral and thus LTR, but who knows?
            std::max(std::abs(metrics.horizontalAdvance(s)), 1),
            metrics.height() * 4 / 5 };
    }

    void drawSpace(
            QPainter* painter, const QRect& rect,
            const QFont& font, QColor color, char32_t subj)
    {
        // Quotient to convert space height to line thickness
        constexpr auto Q_H2THICK = 1.0 / 25.0;

        auto dim = spaceDimensions(font, subj);
        color.setAlpha(ALPHA_SPACE);
        auto lineW = std::max(
                    static_cast<int>(std::round(dim.height() * Q_H2THICK)), 1);
        auto x = rect.left() - lineW + (rect.width() - dim.width()) / 2;
        auto y = rect.top() + (rect.height() - dim.height()) / 2;
        QBrush brush(color);
        painter->fillRect(x, y, lineW, dim.height(), brush);
        x += lineW;
        x += dim.width();
        painter->fillRect(x, y, lineW, dim.height(), brush);
    }

    void drawDeprecated(QPainter* painter, const QRect& r)
    {
        static constexpr int SZ = 8;    // we draw lines between pixel centers, actually +1
        static constexpr int OFS = 4;
        const int x1 = r.right() - OFS;
        const int x0 = x1 - SZ;
        const int y0 = r.top() + OFS;
        const int y1 = y0 + SZ;
        painter->setPen(FG_DEPRECATED);
        painter->drawLine(x0, y0, x1, y1);
        painter->drawLine(x0, y1, x1, y0);
    }

}   // anon namespace


void CharsModel::drawChar(QPainter* painter, const QRect& rect,
            const uc::Cp& cp, const QColor& color, TableDraw mode, int dpi)
{
    switch (cp.drawMethod(dpi)) {
    case uc::DrawMethod::CUSTOM_CONTROL:
        drawCustomControl(painter, rect, color, uc::FontPlace::CELL, cp.subj);
        break;
    case uc::DrawMethod::ABBREVIATION:
        drawAbbreviation(painter, rect, cp.abbrev(), color);
        break;
    case uc::DrawMethod::SPACE:
        drawSpace(painter, rect, *fontAt(cp), color, cp.subj);
        break;
    case uc::DrawMethod::CUSTOM_AA: {
            auto font = *fontAt(cp);
            font.setStyleStrategy(fst::CUSTOM_AA);
            painter->setFont(font);
            painter->setBrush(color);
            painter->setPen(color);
            auto text = textAt(cp);
            painter->drawText(rect, Qt::AlignCenter | Qt::TextSingleLine, text);
        } break;
    case uc::DrawMethod::SAMPLE:
        if (mode == TableDraw::CUSTOM) {
            // Char
            painter->setFont(*fontAt(cp));
            painter->setBrush(color);
            painter->setPen(color);
            painter->drawText(rect,
                              Qt::AlignCenter | Qt::TextSingleLine,
                              textAt(cp));
        } break;
    case uc::DrawMethod::SVG_EMOJI: {
            auto font = fontAt(*uc::cpsByCode[static_cast<int>('!')]);
            QFontMetrics m(*font);
            auto h = rect.height() * 8 / 10;
            emp.draw(painter, rect, cp.subj.ch32(), h);
        } break;
    }
    if (cp.isDeprecated())
        drawDeprecated(painter, rect);
}

void CharsModel::drawChar(QPainter* painter, const QRect& rect,
            const QModelIndex& index, const QColor& color, int dpi) const
{
    auto ch = charAt(index);
    if (ch) {
        auto color1 = fgAt(*ch, TableColors::YES);
        if (!color1.isValid())
            color1 = color;
        drawChar(painter, rect, *ch, color1, TABLE_DRAW, dpi);
    }
}


void CharsModel::initStyleOption(QStyleOptionViewItem *option,
                     const QModelIndex &index) const
{
    SuperD::initStyleOption(option, index);
    if (option->state & (QStyle::State_HasFocus | QStyle::State_Selected)) {
        option->state.setFlag(QStyle::State_Selected, false);
        option->state.setFlag(QStyle::State_HasFocus, false);
        if (option->backgroundBrush.style() != Qt::NoBrush) {
            auto clBg = option->backgroundBrush.color();
            if (clBg.isValid()) {
                clBg.setAlpha(clBg.alpha() / 2);
                option->backgroundBrush.setColor(clBg);
            }
        }
    }
    if (!hasText)
        option->text.clear();
}


void CharsModel::paintItem1(
        QPainter* painter,
        const QStyleOptionViewItem& option,
        const QModelIndex& index,
        const QColor& color) const
{
    auto ch = charAt(index);
    auto dpi = painter->device()->physicalDpiX();
    hasText = !(ch && ch->drawMethod(dpi) == uc::DrawMethod::CUSTOM_AA);
    SuperD::paint(painter, option, index);
    drawChar(painter, option.rect, index, color, dpi);
}

void CharsModel::paintItem(
        QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (option.state.testFlag(QStyle::State_HasFocus)) {
        // It’d be nice to draw some nice focus using Windows skin, but cannot
        //Super::paint(painter, option, index);
        // Draw it as a button
        QStyleOptionButton sob;
            sob.state = QStyle::State_HasFocus | QStyle::State_MouseOver | QStyle::State_Selected
                        | QStyle::State_Active | QStyle::State_Enabled;
            sob.rect = option.rect;
        owner->style()->drawControl(QStyle::CE_PushButton, &sob, painter, option.widget);
        paintItem1(painter, option, index, owner->palette().buttonText().color());
    } else if (option.state.testFlag(QStyle::State_Selected)) {
        // Selected, not focused? Initial style is bad
        auto opt2 = option;
        opt2.state.setFlag(QStyle::State_Selected, false);
        owner->style()->drawPrimitive(QStyle::PE_FrameMenu, &opt2, painter, option.widget);
        paintItem1(painter, option, index, owner->palette().windowText().color());
    } else {
        paintItem1(painter, option, index, owner->palette().windowText().color());
    }
}


void CharsModel::paint(QPainter *painter, const QStyleOptionViewItem &option,
           const QModelIndex &index) const
{
    tcache.paint(painter, option, index, *this);
}


bool CharsModel::isCharCollapsed(char32_t code) const
{
    if (isCjkCollapsed) {
        auto blk = uc::blockOf(code);
        return (blk->flags.have(uc::Bfg::COLLAPSIBLE)
                && (code ^ static_cast<uint32_t>(blk->firstAllocated->subj)) >= NCOLS);
    } else {
        return false;
    }
}


///// SearchModel //////////////////////////////////////////////////////////////


void SearchModel::clear()
{
    beginResetModel();
    v.clear();
    endResetModel();
}


const uc::SearchLine& SearchModel::lineAt(size_t index) const
{
    if (index >= v.size())
        return uc::SearchLine::STUB;
    return v[index];
}


void SearchModel::set(SafeVector<uc::SearchLine>&& x)
{
    beginResetModel();
    v = std::move(x);
    endResetModel();
}


int SearchModel::pixSize() const
{
    auto& font = sample->font();
    QFontMetrics metrics{font};
    return (metrics.ascent() + metrics.descent()) * 3;
}


QVariant SearchModel::data(const QModelIndex& index, int role) const
{
    auto& line = lineAt(index.row());
    char buf[30];

    switch (role) {
    case Qt::DisplayRole: {
            uc::sprintUPLUS(buf, line.code);
            QString s = buf;
            if (line.prio.high == uc::HIPRIO_DEC) {
                // Found by decimal
                s += " = ";
                s += QString::number(line.code);
                s += "₁₀";
            } if (!line.triggerName.empty()) {
                // Triggered alt. name
                s += ": ";
                s += str::toQ(line.triggerName);
            };
            s += '\n';
            switch (line.type) {
            case uc::CpType::EXISTING:
                return s + line.cp->viewableName();
            case uc::CpType::RESERVED:
                return s + loc::get("Search.Empty") + ": "
                         + str::toQ(uc::blockOf(line.code)->loc.name);
            default:
                return s + loc::get(uc::cpTypeKeys[static_cast<int>(line.type)]);
            }
        }

    case Qt::DecorationRole:
        return cache.getT(line.code,
            [cp = line.cp, type = line.type, this](QPixmap& pix) {
                auto size = pixSize();
                if (pix.size() != QSize{size, size}) {
                    pix = QPixmap{size, size};
                }
                pix.fill(Qt::transparent);

                // Create painter
                QColor clFg = sample->palette().windowText().color();
                QPainter painter(&pix);
                auto bounds = pix.rect();

                // Create transparent color
                QColor clTrans(clFg);
                clTrans.setAlpha(30);

                // Draw bounds
                auto bounds1 = bounds;
                bounds1.adjust(0, 0, -1, -1);
                painter.setBrush(Qt::NoBrush);
                painter.setPen(clTrans);
                painter.drawRect(bounds1);

                /// @todo [future] default DPI here, DPI is unused right now
                enum { DPI_STUB = 96 };

                switch (type) {
                case uc::CpType::NONCHARACTER: {
                        painter.drawRect(bounds1);
                        QBrush brush (clFg, Qt::DiagCrossPattern);
                        painter.fillRect(bounds, brush);
                    } break;
                case uc::CpType::PRIVATE_USE:
                    painter.drawRect(bounds1);
                    drawAbbreviation(&painter, bounds, u8"PUA", clFg);
                    break;
                case uc::CpType::SURROGATE:
                    painter.drawRect(bounds1);
                    drawAbbreviation(&painter, bounds, u8"SUR", clFg);
                    break;
                case uc::CpType::RESERVED: {
                        auto& si = cp->block().synthIcon;
                        auto clBg = clTrans;
                        // No continent → draw murky, otherwise use icon colours
                        if (si.ecContinent != uc::EcContinent::NONE) {
                            auto cont = si.continent();
                            clBg = cont.icon.bgColor;
                            clFg = cont.icon.fgColor;
                        }
                        painter.fillRect(bounds, clBg);
                        painter.drawRect(bounds1);
                        CharsModel::drawChar(&painter, bounds, si.cp(),
                                             clFg, TableDraw::CUSTOM, DPI_STUB);
                    } break;
                case uc::CpType::NN:
                case uc::CpType::UNALLOCATED:
                    // Just draw murky w/o foreground
                    painter.drawRect(bounds1);
                    painter.fillRect(bounds, clTrans);
                    break;
                case uc::CpType::EXISTING:
                    painter.drawRect(bounds1);
                    // OK w/o size, as 39 ≈ 40
                    if (cp)
                        CharsModel::drawChar(&painter, bounds, *cp, clFg, TableDraw::CUSTOM, DPI_STUB);
                }
            });
    default:
        return {};
    }
}


///// WiCustomDraw /////////////////////////////////////////////////////////////


void WiCustomDraw::init()
{
    initialSize = minimumSize();
}


void WiCustomDraw::paintEvent(QPaintEvent *event)
{
    Super::paintEvent(event);
    switch (mode) {
    case Mode::NONE: break;
    case Mode::ABBREVIATION: {
            QPainter painter(this);
            drawAbbreviation(&painter, geometry(), abbreviation,
                             palette().windowText().color());
        } break;
    case Mode::CUSTOM_CONTROL: {
            QPainter painter(this);
            drawCustomControl(&painter, geometry(), palette().windowText().color(),
                        uc::FontPlace::SAMPLE, subj);
        } break;
    case Mode::SPACE: {
            QPainter painter(this);
            drawSpace(&painter, geometry(), fontSpace,
                      palette().windowText().color(),
                      subj);
        } break;
    case Mode::EMOJI: {
            QPainter painter(this);
            auto r = geometry();
            emp.draw(&painter, geometry(), subj, r.height() * 17 / 20);
        } break;
    }
}

void WiCustomDraw::setNormal()
{
    mode = Mode::NONE;
    setMinimumSize(initialSize);
}

void WiCustomDraw::setAbbreviation(std::u8string_view x)
{
    setNormal();
    mode = Mode::ABBREVIATION;
    abbreviation = x;
    update();
}


void WiCustomDraw::setCustomControl(char32_t aSubj)
{
    setNormal();
    mode = Mode::CUSTOM_CONTROL;
    subj = aSubj;
    update();
}


void WiCustomDraw::setEmoji(char32_t aSubj)
{
    setNormal();
    mode = Mode::EMOJI;
    subj = aSubj;
    update();
}


void WiCustomDraw::setSpace(const QFont& font, char32_t aSubj)
{
    static constexpr auto SPACE_PLUS = 30;

    fontSpace = font;
    mode = Mode::SPACE;
    subj = aSubj;

    // Set appropriate size
    auto dim = spaceDimensions(font, aSubj);
    setMinimumSize(QSize(
                std::max(initialSize.width(), dim.width() + SPACE_PLUS),
                std::max(initialSize.height(), dim.height())));
}


///// FmMain ///////////////////////////////////////////////////////////////////


FmMain::FmMain(QWidget *parent)
    : Super(parent),
      ui(new Ui::FmMain),
      model(this),
      searchModel(this),
      fontBig(str::toQ(FAM_DEFAULT), FSZ_BIG),
      fontTofu(str::toQ(FAM_TOFU), FSZ_BIG)
{
    ui->setupUi(this);

    // Tabs to 0
    ui->tabsMain->setCurrentIndex(0);

    // Collapse bar
    ui->wiCollapse->hide();
    ui->wiCollapse->setStyleSheet(
                "#wiCollapse { background-color: " + BG_CJK.name() + "; }"   );
    connect(ui->btCollapse, &QPushButton::clicked,
            this, &This::cjkExpandCollapse);
    cjkReflectCollapseState();

    // Top bar
    QPalette pal = ui->wiCharBar->palette();
    const QColor& color = pal.color(QPalette::Normal, QPalette::Button);
    ui->wiCharBar->setStyleSheet("#wiCharBar { background-color: " + color.name() + " }");
    ui->pageInfo->setStyleSheet("#pageInfo { background-color: " + color.name() + " }");
    ui->pageSearch->setStyleSheet("#pageSearch { background-color: " + color.name() + " }");

    { // Copy ex
        auto font = ui->btCopyEx->font();
        QFontMetrics metrics(font);
        auto sz = metrics.horizontalAdvance("+000000");
        ui->btCopyEx->setFixedWidth(sz);
    }

    // Fill chars
    model.build();

    // Combobox
    ui->comboBlock->setModel(&blocksModel);

    // Table
    ui->tableChars->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableChars->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableChars->setItemDelegate(&model);
    ui->tableChars->setModel(&model);

    // Divider
    auto w = width();
    /// @todo [sizes] Not really a cool size
    QList<int> sizes { w * 45 / 100, w * 55 / 100 };
    ui->splitBlocks->setStretchFactor(0, 0);
    ui->splitBlocks->setStretchFactor(1, 1);
    ui->splitBlocks->setSizes(sizes);

    // Connect events
    connect(ui->tableChars->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &This::charChanged);

    // Sample
    ui->pageSampleCustom->init();

    // OS style    
    fontTofu.setStyleStrategy(fst::TOFU);
    auto& font = uc::fontInfo[0];
    ui->lbOs->setFont(font.get(uc::FontPlace::SAMPLE, FSZ_BIG, false, NO_TRIGGER));

    // Copy
        // Ctrl+C
    auto shcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_C), ui->tableChars,
                nullptr, nullptr, Qt::WidgetWithChildrenShortcut);
    connect(shcut, &QShortcut::activated, this, &This::copyCurrentChar);
        // Ctrl+Ins
    shcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Insert), ui->tableChars,
                nullptr, nullptr, Qt::WidgetWithChildrenShortcut);
    connect(shcut, &QShortcut::activated, this, &This::copyCurrentChar);
        // Button
    connect(ui->btCopy, &QPushButton::clicked, this, &This::copyCurrentChar);
        // 2click
    ui->tableChars->viewport()->installEventFilter(this);

    // Copy sample: Ctrl+Shift+C
    shcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_C), ui->tableChars,
                nullptr, nullptr, Qt::WidgetWithChildrenShortcut);
    connect(shcut, &QShortcut::activated, this, &This::copyCurrentSample);
    shcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_Insert), ui->tableChars,
                nullptr, nullptr, Qt::WidgetWithChildrenShortcut);
    connect(shcut, &QShortcut::activated, this, &This::copyCurrentSample);
    connect(ui->btCopyEx, &QPushButton::clicked, this, &This::copyCurrentSample);

    // Tofu stats
    shcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_T), this);
    connect(shcut, &QShortcut::activated, this, &This::showTofuStats);

    // Clicked
    connect(ui->vwInfo, &QTextBrowser::anchorClicked, this, &This::anchorClicked);
    connect(ui->lbCharCode, &QLabel::linkActivated, this, &This::labelLinkActivated);
    connect(ui->lbOsTitle, &QLabel::linkActivated, this, &This::labelLinkActivated);

    // Search
    ui->stackSearch->setCurrentWidget(ui->pageInfo);
    connect(ui->btCloseSearch, &QPushButton::clicked, this, &This::closeSearch);
    connect(ui->tableChars, &CharsTable::focusIn, this, &This::closeSearch);    
    ui->listSearch->setUniformItemSizes(true);
    ui->listSearch->setModel(&searchModel);
    connect(ui->edSearch, &SearchEdit::searchPressed, this, &This::startSearch);
    connect(ui->edSearch, &SearchEdit::focusIn, this, &This::focusSearch);
    connect(ui->listSearch, &SearchList::enterPressed, this, &This::searchEnterPressed);

    finishTranslation();

    // Set focus defered
        // Windows timer is low-priority, even after paint
    timerSetFocus = std::make_unique<QTimer>(this);
    timerSetFocus->setSingleShot(true);
    timerSetFocus->setInterval(5);
    connect(timerSetFocus.get(), &QTimer::timeout, this, &This::slotSetFocusDefered);

    // Select index
    ui->tableChars->setFocus();
    auto index = model.index(0, 0);
    ui->tableChars->selectionModel()->select(index, QItemSelectionModel::SelectCurrent);
    charChanged(index);
}


void FmMain::finishTranslation()
{
    initTerms();
    initAbout();
}



void FmMain::initTerms()
{
    QString text;
    mywiki::appendStylesheet(text);

    const size_t n = uc::nTerms();
    auto lastCat = uc::EcTermCat::NN;
    for (size_t i = 0; i < n; ++i) {
        const uc::Term& term = uc::terms[i];
        if (term.cat != lastCat) {
            lastCat = term.cat;
            auto& cat = uc::termCats[static_cast<int>(lastCat)];
            str::append(text, "<h1>");
            str::append(text, cat.locName);
            str::append(text, "</h1>");
        }
        str::append(text, "<p><a href='pt:");
        str::append(text, term.key);
        str::append(text, "' class='popup'><b>");
        str::append(text, term.loc.name);
        str::append(text, "</b></a>");
        /// @todo [L10n] There are locales that NEVER borrow English names
        if (!term.engName.empty() && term.engName != term.loc.name) {
            str::append(text, "&nbsp;/ ");
            str::append(text, term.engName);
        }
    }

    ui->vwTerms->setText(text);
    connect(ui->vwTerms, &QTextBrowser::anchorClicked, this, &This::anchorClicked);
}


namespace {

//    QString qEpaulets(
//            std::string_view locKey,
//            const char* left, const char* right)
//        { return left + loc::get(locKey).q() + right; }

    QString qPopupLink(
            std::string_view locKey, const char* target)
    {
        return QString("<a href='") + target + "' style='" STYLE_POPUP "'>"
                + loc::get(locKey) + "</a>";
    }

}   // anon namespace


void FmMain::initAbout()
{
    // Get version
    auto version = QApplication::applicationVersion();
        // Count “.” chars
    int nDots = 0;
    for (auto c : version)
        if (c == '.')
            ++nDots;
    // Remove '.0' if there will be dots remaining
    while (nDots > 1 && version.endsWith(".0")) {
        version.resize(version.length() - 2);
        --nDots;
    }

    // lbVersion
    ui->lbAboutVersion->setText(str::toQ(
                loc::get("About.Version")
                .arg(str::toU8sv(version.toStdString()),
                     str::toU8sv(uc::versionInfo[static_cast<int>(uc::EcVersion::LAST)].name))));

    // vwVersion
    QFile f(":/Texts/about.htm");
    f.open(QIODevice::ReadOnly);
    QString s = f.readAll();
    while (true) {
        auto pos = s.indexOf("{#");
        if (pos < 0)
            break;
        auto posKey = pos + 2;
        auto pos1 = s.indexOf('}', posKey);
        if (pos1 < 0)
            break;
        auto lenKey = pos1 - posKey;
        auto key = "About." + s.mid(posKey, lenKey).toStdString();
        s.remove(pos, pos1 + 1 - pos);
        s.insert(pos, loc::get(key));
    }

    s = "<style>a { text-decoration: none; color: " CNAME_LINK_OUTSIDE "; }</style>" + s;
    ui->vwAbout->setText(s);

    // lbTofuStats
    ui->lbTofuStats->setText(qPopupLink("About.TofuStats", "ac:tofustats"));
    connect(ui->lbTofuStats, &QLabel::linkActivated, this, &This::showTofuStats);
}


bool FmMain::eventFilter(QObject *obj, QEvent *event)
{
    switch (event->type()) {
    case QEvent::MouseButtonDblClick: {
            auto mouseEvent = static_cast<QMouseEvent*>(event);
            if (obj == ui->tableChars->viewport()
                    && mouseEvent->button() == Qt::LeftButton) {
                copyCurrentChar();
                return true;
            }
        }
    default:;
    }
    return Super::eventFilter(obj, event);
}


FmMain::~FmMain()
{
    delete ui;
}


void FmMain::showCopied(QWidget* widget, const QRect& absRect)
{
    fmMessage.ensure(this)
             .showAtAbs("Скопировано", widget, absRect);
}


void FmMain::showCopied(QAbstractItemView* table)
{
    auto widget = qobject_cast<QWidget*>(sender());
    QPoint corner { 0, 0 };
    QSize size { 0, 0 };
    if (widget) {
        // corner is (0,0)
        size = widget->size();
    } else {
        auto selIndex = table->currentIndex();
        auto selRect = table->visualRect(selIndex);
        widget = table->viewport();
        // Geometry is in PARENT’s coord system
        // Rect is (0,0) W×H
        auto visibleGeo = widget->rect();
        selRect = selRect.intersected(visibleGeo);
        if (selRect.isEmpty()) {
            corner = visibleGeo.center();
            // Size is still 0
        } else {
            corner = selRect.topLeft();
            size = selRect.size();
        }
    }

    corner = widget->mapToGlobal(corner);
    showCopied(widget, QRect{ corner, size});
}


void FmMain::copyCurrentThing(CurrThing thing)
{
    auto ch = model.charAt(ui->tableChars->currentIndex());
    QString q = str::toQ(ch.code);
    if (thing == CurrThing::SAMPLE && ch) {
        if (ch->category().upCat == uc::UpCategory::MARK) {
            q = uc::STUB_CIRCLE + q;
        } else if (ch->isVs16Emoji()) {
            q += QChar(0xFE0F);
        }
    }
    QApplication::clipboard()->setText(q);
    showCopied(ui->tableChars);
}


void FmMain::copyCurrentChar()
{
    copyCurrentThing(CurrThing::CHAR);
}


void FmMain::copyCurrentSample()
{
    copyCurrentThing(CurrThing::SAMPLE);
}


void FmMain::drawSampleWithQt(const uc::Cp& ch)
{
    ui->pageSampleCustom->setNormal();

    // Font
    auto font = ch.font(uc::MatchLast::NO);
    ui->lbSample->setFont(font->get(uc::FontPlace::SAMPLE, FSZ_BIG, false, ch.subj));

    // Sample char
    ui->stackSample->setCurrentWidget(ui->pageSampleQt);
    auto proxy = ch.sampleProxy(uc::DPI_ALL_CHARS);
    // Color
    if (ch.isTrueSpace()) {
        auto c = palette().text().color();
        c.setAlpha(ALPHA_SPACE);
        QString css = "color: " + c.name(QColor::HexArgb) + ';';
        ui->lbSample->setStyleSheet(css);
    } else {
        ui->lbSample->setStyleSheet(str::toQ(proxy.styleSheet));
    }
    ui->lbSample->setText(proxy.text);
}


template<>
void wiki::append(QString& s, const char* start, const char* end)
{
    s.append(QByteArray::fromRawData(start, end - start));
}

void FmMain::clearSample()
{
    ui->lbSample->clear();
    ui->lbSample->setFont(QFont());
}


void FmMain::showCp(MaybeChar ch)
{
    if (ch.code == shownCp)
        return;
    shownCp = ch.code;

    // Code
    char buf[300];
    { QString ucName;
        uc::sprintUPLUS(buf, ch.code);
        mywiki::appendCopyable(ucName, buf, "' style='" STYLE_BIGCOPY);
        ui->lbCharCode->setText(ucName);
    }

    // Block
    int iBlock = ui->comboBlock->currentIndex();
    auto block = uc::blockOf(ch.code);
    int newIBlock = block->index();
    if (newIBlock != iBlock)
        ui->comboBlock->setCurrentIndex(newIBlock);
    ui->wiCollapse->setVisible(block->flags.have(uc::Bfg::COLLAPSIBLE));

    if (ch) {
        if (ch->isTrueSpace()) {
                auto palette = this->palette();
                auto color = palette.windowText().color();
                color.setAlpha(ALPHA_SPACE);
                palette.setColor(QPalette::WindowText, color);
                ui->lbSample->setPalette(palette);
        } else {
            ui->lbSample->setPalette(this->palette());
        }
        if (ch->category().upCat == uc::UpCategory::MARK) {
            ui->btCopyEx->setText("+25CC");
            ui->btCopyEx->show();
        } else if (ch->isVs16Emoji()) {
            ui->btCopyEx->setText("+VS16");
            ui->btCopyEx->show();
        } else {
            ui->btCopyEx->hide();
        }

        // Sample char
        const bool wantSysFont = !ch->isDefaultIgnorable() && ch->isGraphical();
        switch (ch->drawMethod(uc::DPI_ALL_CHARS)) {
        case uc::DrawMethod::CUSTOM_CONTROL:
            clearSample();
            ui->stackSample->setCurrentWidget(ui->pageSampleCustom);
            ui->pageSampleCustom->setCustomControl(ch.code);
            break;
        case uc::DrawMethod::ABBREVIATION:
            clearSample();
            ui->stackSample->setCurrentWidget(ui->pageSampleCustom);
            ui->pageSampleCustom->setAbbreviation(ch->abbrev());
            break;
        case uc::DrawMethod::SPACE: {
                clearSample();
                ui->stackSample->setCurrentWidget(ui->pageSampleCustom);
                auto font = ch->font(uc::MatchLast::NO);
                auto qfont = font->get(uc::FontPlace::SAMPLE, FSZ_BIG, false, ch.code);
                ui->pageSampleCustom->setSpace(qfont, ch.code);
            } break;
        case uc::DrawMethod::SAMPLE:
        case uc::DrawMethod::CUSTOM_AA:
            drawSampleWithQt(*ch);
            break;
        case uc::DrawMethod::SVG_EMOJI:
            ui->stackSample->setCurrentWidget(ui->pageSampleCustom);
            ui->pageSampleCustom->setEmoji(ch.code);
            break;
        }

        // OS char
        std::optional<QFont> font = std::nullopt;
        QFontDatabase::WritingSystem ws = QFontDatabase::Any;
        if (wantSysFont) {
            QString osProxy = ch->osProxy();
            if (osProxy.isEmpty()) {
                ui->lbOs->setFont(fontBig);
                ui->lbOs->setText({});
                ui->lbOsTitle->setText(loc::get("Prop.Os.Invisible"));
            } else {
                ws = ch->scriptEx().qtCounterpart;
                font = model.match.sysFontFor(*ch, ws, FSZ_BIG);
                if (font) {
                    ui->lbOs->setFont(*font);
                    ui->lbOs->setText(osProxy);
                    snprintf(buf, std::size(buf),
                            "<a href='pf:%d/%d' style='" STYLE_POPUP "'>",
                            ch->subj.val(), static_cast<int>(ws));
                    ui->lbOsTitle->setText(
                        buf + font->family().toHtmlEscaped() + "</a>");
                } else {
                    ui->lbOs->setFont(fontTofu);
                    ui->lbOs->setText(QString::fromUtf16(U16_TOFU));
                    ui->lbOsTitle->setText(qPopupLink("Prop.Os.Tofu", "pt:tofu"));
                }
            }
        } else {
            ui->lbOs->setFont(fontBig);
            ui->lbOs->setText({});
            ui->lbOsTitle->setText(loc::get("Prop.Os.Invisible"));
        }

        QString text = mywiki::buildHtml(*ch);
        ui->vwInfo->setText(text);
    } else {
        // No character
        ui->stackSample->setCurrentWidget(ui->pageSampleQt);
        ui->lbSample->setText({});
        ui->lbOs->setText({});
        const auto& osTitle = loc::get(
                uc::isNonChar(ch.code) ? "Prop.Os.NonChar" : "Prop.Os.Empty");
        ui->lbOsTitle->setText(osTitle);
        ui->btCopyEx->hide();
        if (uc::isNonChar(ch.code)) {
            QString text = mywiki::buildNonCharHtml(ch.code);
            ui->vwInfo->setText(text);
        } else {
            auto color = palette().color(QPalette::Disabled, QPalette::WindowText);
            QString text = mywiki::buildEmptyCpHtml(ch.code, color);
            ui->vwInfo->setText(text);
        }
    }
}


void FmMain::charChanged(const QModelIndex& current)
{
    showCp(model.charAt(current));
}


void FmMain::popupAtAbs(
        QWidget* widget, const QRect& absRect, const QString& html)
{
    popup.ensure(this)
         .setText(html)
         .popupAtAbsBacked(widget, absRect);
}

void FmMain::copyTextAbs(
        QWidget* widget, const QRect& absRect, const QString& text)
{
    QApplication::clipboard()->setText(text);
    showCopied(widget, absRect);
}

void FmMain::followUrl(const QString& x)
{
    QDesktopServices::openUrl(x);
}


FontList FmMain::allSysFonts(
        char32_t cp, QFontDatabase::WritingSystem ws, size_t maxCount)
    { return model.match.allSysFonts(cp, ws, maxCount); }


void FmMain::linkClicked(
        std::string_view link, QWidget* widget, TinyOpt<QRect> rect)
{
    mywiki::go(widget, rect, *this, link);
}


void FmMain::anchorClicked(const QUrl &arg)
{
    auto str = arg.url().toStdString();
    auto snd = qobject_cast<QTextBrowser*>(sender());
    auto rect = snd->cursorRect();
    // Get some parody for link rect
    // Unglitch: we don’t know how to get EXACT coords of link,
    // so improvise somehow
    rect.setLeft(rect.left() - 80);

    linkClicked(str, snd, rect);
}


void FmMain::popupLinkActivated(const QString& link)
{
    // nullptr & {} = last position
    linkClicked(link.toStdString(), nullptr, {});
}


void FmMain::labelLinkActivated(const QString& link)
{
    QRect rect;
    auto snd = qobject_cast<QWidget*>(sender());
    if (snd) {
        rect = snd->rect();
    } else {
        snd = this;
        rect = QRect(snd->rect().center(), QSize{1, 1});
    }
    mywiki::go(snd, rect, *this, link.toStdString());
    // Deselect, does not influence double and triple clicks
    if (auto label = qobject_cast<QLabel*>(sender())) {
        label->setSelection(0, 0);
    }
}


void FmMain::slotSetFocusDefered()
{
    ui->tableChars->setFocus();
}


template<>
void FmMain::selectChar<SelectMode::NONE>(char32_t code)
{
    if (model.isCharCollapsed(code)) {
        cjkSetCollapseState(false);
    }
    if (auto cp = uc::cpsByCode[code]) {
        // Just get font, moving reference to nowhere
        (void)cp->font(uc::MatchLast::NO);
    }
    auto index = model.indexOf(code);
    ui->tableChars->setCurrentIndex(index);
    ui->tableChars->scrollTo(index);
}


void FmMain::cjkReflectCollapseState()
{
    if (model.isCjkCollapsed) {
        ui->lbCollapse->setText(loc::get("Expand.0.Label"));
        ui->btCollapse->setText(loc::get("Expand.0.Button"));
    } else {
        ui->lbCollapse->setText(loc::get("Expand.1.Label"));
        ui->btCollapse->setText(loc::get("Expand.1.Button"));
    }
}


void FmMain::cjkSetCollapseState(bool x)
{
    if (model.isCjkCollapsed == x)
        return;
    model.isCjkCollapsed = x;
    model.build();
    cjkReflectCollapseState();
}


void FmMain::preloadVisibleFonts()
{
    auto vport = ui->tableChars->viewport();
    auto vheader = ui->tableChars->verticalHeader();
    auto rwTop = vheader->logicalIndexAt(0);
    auto rwBottom = vheader->logicalIndexAt(vport->height() - 1);
    int nCols = model.columnCount();
    for (auto row = rwTop; row <= rwBottom; ++row) {
        for (int col = 0; col < nCols; ++col) {
            auto index = model.index(row, col);
            if (auto cp = model.charAt(index)) {
                (void)cp->font(uc::MatchLast::NO);
            }
        }
    }
}


template<>
void FmMain::selectChar<SelectMode::DEFERED>(char32_t code)
{
    selectChar<SelectMode::NONE>(code);
    preloadVisibleFonts();
    timerSetFocus->start();
}


template<>
void FmMain::selectChar<SelectMode::INSTANT>(char32_t code)
{
    selectChar<SelectMode::NONE>(code);
    ui->tableChars->setFocus();
}


void FmMain::on_comboBlock_currentIndexChanged(int index)
{
    if (index < 0)
        return;
    auto oldChar = model.charAt(ui->tableChars->currentIndex());
    auto oldBlock = uc::blockOf(oldChar.code);
    if (oldBlock->index() != static_cast<size_t>(index)) {
        auto& newBlock = uc::blocks[index];
        selectChar<SelectMode::DEFERED>(newBlock.firstAllocated->subj);
    }
}


void FmMain::cjkExpandCollapse()
{
    // Remember current char, index and offset
    auto oldIndex = ui->tableChars->currentIndex();
    auto cp = model.charAt(oldIndex);
    auto scrollTop = ui->tableChars->verticalHeader()->logicalIndexAt(0);
    auto scrollOffset = std::max(0, oldIndex.row() - scrollTop);

    // Rebuild model
    cjkSetCollapseState(!model.isCjkCollapsed);

    // Generate new index
    auto newIndex = model.indexOf(cp.code);
    auto newIndex2 = model.index(newIndex.row(), oldIndex.column());
    auto newScrollTop = std::max(0, newIndex2.row() - scrollOffset);
    auto newTopIndex = model.index(newScrollTop, 0);

    // UI changes
    ui->tableChars->setCurrentIndex(newIndex2);
    ui->tableChars->scrollTo(newTopIndex, QAbstractItemView::PositionAtTop);
    ui->tableChars->scrollTo(newIndex2);
    ui->tableChars->viewport()->setFocus();
}


void FmMain::installTempPrefix()
{
    tempPrefix = model.match.findPrefix();
}


void FmMain::showTofuStats()
{
    fmTofuStats.ensure(this).exec();
}


void FmMain::openSearch()
{
    ui->stackSearch->setCurrentWidget(ui->pageSearch);
}


void FmMain::closeSearch()
{
    if (ui->stackSearch->currentWidget() == ui->pageSearch)
        ui->stackSearch->setCurrentWidget(ui->pageInfo);
}


void FmMain::startSearch()
{
    doSearch(ui->edSearch->text());
}


void FmMain::showSearchError(const QString& text)
{
    fmMessage.ensure(this)
             .showAtWidget(text, ui->edSearch);
}


void FmMain::showSearchResult(uc::MultiResult&& x)
{
    auto one = x.one();
    if (one) {
        closeSearch();
        selectChar<SelectMode::INSTANT>(one->subj);
        return;
    }

    switch (x.err) {
    case uc::SearchError::OK: {
            searchModel.set(std::move(x.v));
            openSearch();
            ui->listSearch->setFocus();
            auto index0 = searchModel.index(0, 0);
            ui->listSearch->setCurrentIndex(index0);
            ui->listSearch->scrollTo(index0);
        } break;
    case uc::SearchError::NO_SEARCH:
        break;
    default:
        showSearchError(loc::get(
                uc::searchErrorKeys[static_cast<int>(x.err)]));
    }
}


void FmMain::doSearch(QString what)
{
    auto r = uc::doSearch(what);
    showSearchResult(std::move(r));
}


void FmMain::focusSearch()
{
    if (searchModel.hasData())
        openSearch();
}


void FmMain::searchEnterPressed(int index)
{
    auto line = searchModel.lineAt(index);
    if (line.cp) {
        selectChar<SelectMode::INSTANT>(line.cp->subj);
    } else {
        auto relRect = ui->listSearch->visualRect(searchModel.index(index, 0));
        fmMessage.ensure(this)
                 .showAtRel("Такого символа нет", ui->listSearch->viewport(), relRect);
    }
}

