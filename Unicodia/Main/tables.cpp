// My header
#include "tables.h"

// Qt
#include <QListView>
#include <QApplication>
#include <QMenu>

// Drawing
#include "CharPaint/routines.h"
#include "Skin.h"

// My widgets
#include "WiShowcase.h"

// Unicode
#include "UcCp.h"

// Misc
#include "i_DarkMode.h"
#include "u_Qstrings.h"

// L10n
#include "LocDic.h"

// No need custom drawing — solves nothing
constexpr TableDraw TABLE_DRAW = TableDraw::INTERNAL;

///// VirtualCharsModel ////////////////////////////////////////////////////////

VirtualCharsModel::VirtualCharsModel(
        QWidget* aOwner, uc::GlyphStyleSets& aGlyphSets)
    : owner(aOwner), glyphSets(aGlyphSets),
      dummyLv(new QListView)
{
    tcache.connectSignals(this);
}


int VirtualCharsModel::columnCount(const QModelIndex&) const
    { return NCOLS; }


QString VirtualCharsModel::textAt(const QModelIndex& index) const
{
    auto cp = charAt(index);
    if (!cp)
        return {};
    return cp->sampleProxy(uc::ProxyType::TABLE_DEFAULT, WiShowcase::EMOJI_DRAW, glyphSets).text;
}


void VirtualCharsModel::initStyleOption(QStyleOptionViewItem *option,
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
}


QColor VirtualCharsModel::fgAt(const uc::Cp&, TableColors) const
    { return {}; }


QColor VirtualCharsModel::fgAt(const QModelIndex& index, TableColors tcl) const
{
    auto cp = charAt(index);
    if (!cp)
        return {};
    return fgAt(*cp, tcl);
}


void VirtualCharsModel::drawChar(QPainter* painter, const QRect& rect,
            const QModelIndex& index, const QColor& color) const
{
    auto ch = charAt(index);
    if (ch) {
        auto color1 = fgAt(*ch, TableColors::YES);
        if (!color1.isValid())
            color1 = color;
        ::drawChar(painter, rect, 100, *ch, color1, TABLE_DRAW, WiShowcase::EMOJI_DRAW, glyphSets);
    }
}

void VirtualCharsModel::paintItem1(
        QPainter* painter,
        const QStyleOptionViewItem& option,
        const QModelIndex& index,
        const QColor& color) const
{
    SuperD::paint(painter, option, index);
    drawChar(painter, option.rect, index, color);
}


void VirtualCharsModel::paintItem(
        QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!dark::isActuallyOn()) {
        if (option.state.testFlag(QStyle::State_HasFocus)) {
            QStyleOptionViewItem vi = option;
            // Workaround: draw selected item from Windows’ skin
            //   (Vista’s skin explicitly bans it from table)
            // 1. Set style
            vi.state = QStyle::State_Selected | QStyle::State_MouseOver
                        | QStyle::State_Active | QStyle::State_Enabled | QStyle::State_Item
                        | QStyle::State_On | QStyle::State_KeyboardFocusChange;
            // 2. Use dummy list view as object instead of table view
            QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &vi, painter, dummyLv.get());
            paintItem1(painter, option, index, owner->palette().windowText().color());
            return;
        } else if (option.state.testFlag(QStyle::State_Selected)) {
            // Selected, not focused? Initial style is bad
            auto opt2 = option;
            opt2.state.setFlag(QStyle::State_Selected, false);
            owner->style()->drawPrimitive(QStyle::PE_FrameMenu, &opt2, painter, option.widget);
            paintItem1(painter, option, index, owner->palette().windowText().color());
            return;
        }
    }
    paintItem1(painter, option, index, owner->palette().windowText().color());
}


std::optional<QFont> VirtualCharsModel::fontAt(const QModelIndex& index) const
{
    if (auto cp = charAt(index))
        return ::fontAt(WiShowcase::EMOJI_DRAW, *cp, glyphSets);
    return {};
}


QVariant VirtualCharsModel::data(const QModelIndex& index, int role) const
{
    switch (role) {
    case Qt::DisplayRole:
        if constexpr (TABLE_DRAW == TableDraw::INTERNAL) {
            if (auto q = textAt(index); !q.isEmpty())
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
        return {};

    case Qt::TextAlignmentRole:
        return Qt::AlignCenter;
    default:
        return {};
    }
}


void VirtualCharsModel::paint(QPainter *painter, const QStyleOptionViewItem &option,
           const QModelIndex &index) const
{
    tcache.paint(painter, option, index, *this);
}


///// CharsModel ///////////////////////////////////////////////////////////////


CharsModel::CharsModel(QWidget* aOwner, uc::GlyphStyleSets& glyphSets) :
    Super(aOwner, glyphSets),
    match(str::toQ(FAM_DEFAULT)),
    rows(NCOLS) {}

// -warn: strange with =default
CharsModel::~CharsModel() {}


int CharsModel::rowCount(const QModelIndex&) const
{
    return rows.nRows();
}


QColor CharsModel::fgAt(const uc::Cp& cp, TableColors tcl) const
{
    if (tcl != TableColors::NO) {
        if (isCjkCollapsed) {
            auto block = uc::blockOf(cp.subj);
            if (block->flags.have(uc::Bfg::COLLAPSIBLE)) {
                return block->synthIcon.normalContinent().collapse.textColor;
            }
        }
    }
    return {};
}


QVariant CharsModel::data(const QModelIndex& index, int role) const
{
    switch (role) {
    case Qt::BackgroundRole: {
            auto cp = charAt(index);
            if (cp) {
                if (isCjkCollapsed) {
                    auto block = uc::blockOf(cp->subj);
                    if (block->flags.have(uc::Bfg::COLLAPSIBLE)) {
                        return block->synthIcon.normalContinent().collapse.bgColor;
                    }
                }
            } else {
                if (uc::isNonChar(cp.code)) {
                    return QBrush(owner->palette().windowText().color(), Qt::DiagCrossPattern);
                }
                return owner->palette().button().color();
            }
            return {};
        }

    default:
        return Super::data(index, role);
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
            return QString{buf};
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


MaybeChar CharsModel::charAt(const QModelIndex& index) const
{
    if (!index.isValid())
        return {};
    return rows.charAt(index.row(), index.column());
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


///// TableLocalMenu ///////////////////////////////////////////////////////////


void TableLocalMenu::init(QTableView* aTable, VirtualCharsModel* aModel)
{
    table = aTable;  model = aModel;
    menu = new QMenu(table);
    acCopy = new QAction("[Copy]", menu);
        menu->addAction(acCopy);
        QWidget::connect(acCopy, &QAction::triggered, table,
                [this]() {
                    emit thingCopied(uc::CopiedChannel::CHAR, nullptr);
                });
    acCopyVs15 = new QAction("[CopyVs15]", menu);
        menu->addAction(acCopyVs15);
        QWidget::connect(acCopyVs15, &QAction::triggered, table,
                [this]() {
                    emit thingCopied(uc::CopiedChannel::VS15, nullptr);
                });
    acCopyVs16 = new QAction("[CopyVs16]", menu);
        menu->addAction(acCopyVs16);
        QWidget::connect(acCopyVs16, &QAction::triggered, table,
                [this]() {
                    emit thingCopied(uc::CopiedChannel::SAMPLE, nullptr);
                });
    acCopyDotc = new QAction("[CopyDotc]", menu);
        menu->addAction(acCopyDotc);
        QWidget::connect(acCopyDotc, &QAction::triggered, table,
                [this]() {
                    emit thingCopied(uc::CopiedChannel::SAMPLE, nullptr);
                });
    QWidget::connect(table, &QWidget::customContextMenuRequested, table,
            [this](const QPoint& pt) {
                popup(table->viewport(), pt);
            });
}


void TableLocalMenu::translate()
{
    acCopy->setText(loc::get("Main.Local.Copy").q());
    acCopyVs15->setText(loc::get("Main.Local.CopyVs15").q());
    acCopyVs16->setText(loc::get("Main.Local.CopyVs16").q());
    acCopyDotc->setText(loc::get("Main.Local.CopyDotc").q());
}


namespace {

    void normalize(QPoint& point, const QRect& rect)
    {
        if (!rect.contains(point)) {
            // X
            if (point.x() < rect.left())
                point.setX(rect.left());
            if (point.x() > rect.right())
                point.setX(rect.right());
            // Y
            if (point.y() < rect.top())
                point.setY(rect.top());
            if (point.y() > rect.bottom())
                point.setY(rect.bottom());
        }
    }

}   // anon namespace


void TableLocalMenu::popupMenu(QWidget* widget, QMenu* menu, QPoint where)
{
    if (widget) {
        // Move within rect
        auto rect = widget->contentsRect();
        normalize(where, rect);
        // Map to desktop coords
        where = widget->mapToGlobal(where);
    }
    menu->popup(where);
}


void TableLocalMenu::popup(QWidget* widget, const QPoint& where)
{
    // Check actions
    auto charIf = model->charAt(table->currentIndex());
    acCopy->setEnabled(charIf.hasCode());
    acCopyVs15->setVisible(charIf.hasCp() && charIf->isVs15Emoji());
    acCopyVs16->setVisible(charIf.hasCp() && charIf->isVs16Emoji());
    acCopyDotc->setVisible(charIf.hasCp() && charIf->isMark());
    // Get point
    popupMenu(widget, menu, where);
}
