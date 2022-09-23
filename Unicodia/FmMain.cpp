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
#include <QSvgRenderer>
#include <QToolBar>
#include <QToolButton>
#include <QActionGroup>

// Misc
#include "u_Strings.h"
#include "u_Qstrings.h"

// Project-local
#include "Skin.h"
#include "Wiki.h"   // used! — we specialize its template
#include "MyWiki.h"
#include "CharPaint.h"

// Forms
#include "FmPopup.h"
#include "FmMessage.h"
#include "FmTofuStats.h"

// L10n
#include "LocDic.h"
#include "LocList.h"

template class LruCache<char32_t, QPixmap>;

using namespace std::string_view_literals;

namespace {
    // No need custom drawing — solves nothing
    constexpr TableDraw TABLE_DRAW = TableDraw::INTERNAL;
    const char16_t U16_TOFU[] { 0xD807, 0xDEE0, 0 };     // Makasar Ka
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


const uc::Block& BlocksModel::at(size_t i) const noexcept
{
    auto r = a.safeGetC(i, &uc::blocks[0]);
    return r ? *r : uc::blocks[0];
}


namespace {

    bool isAlphaLess(const uc::Block* x, const uc::Block* y)
    {
        auto cmp = std::lexicographical_compare_three_way(
                    std::begin(x->loc.sortKey), std::end(x->loc.sortKey),
                    std::begin(y->loc.sortKey), std::end(y->loc.sortKey));
        if (cmp != 0)
            return (cmp < 0);
        return (x->alphaKey.subKey < y->alphaKey.subKey);
    }

    bool isContinentLess(const uc::Block* x, const uc::Block* y)
    {
        if (x->synthIcon.ecContinent != y->synthIcon.ecContinent)
            return (x->synthIcon.ecContinent < y->synthIcon.ecContinent);
        return isAlphaLess(x, y);
    }

}   // anon namespace


size_t BlocksModel::build(const BlockOrder& order, size_t iOld)
{
    auto& block = at(iOld);
    for (size_t i = 0; i < uc::N_BLOCKS; ++i)
        a[i] = &uc::blocks[i];

    // Primary sorting
    switch (order) {
    case BlockOrder::CODE: break; // do nothing
    case BlockOrder::ALPHA:
        std::stable_sort(a.begin(), a.end(), isAlphaLess);
        break;
    case BlockOrder::CONTINENT:
        std::stable_sort(a.begin(), a.end(), isContinentLess);
        break;
    }

    for (size_t i = 0; i < uc::N_BLOCKS; ++i)
        a[i]->cachedIndex = i;
    return block.cachedIndex;
}


int BlocksModel::rowCount(const QModelIndex&) const { return uc::N_BLOCKS; }

int BlocksModel::columnCount(const QModelIndex&) const { return 1; }

QVariant BlocksModel::data(const QModelIndex& index, int role) const
{
    #define GET_BLOCK \
            size_t i = index.row();         \
            if (i >= uc::N_BLOCKS)          \
                return {};                  \
            auto block = a[i];              \
            if (!block)                     \
                return {};                  \

    switch (role) {
    case Qt::DisplayRole: {
            GET_BLOCK
            return str::toQ(block->loc.name);
        }
    case Qt::DecorationRole: {
            GET_BLOCK
            if (!block->icon) {
                char buf[48];
                snprintf(buf, std::size(buf), ":/Scripts/%04X.png", static_cast<int>(block->startingCp));
                block->icon = new QPixmap();
                block->icon->load(buf);
            }
            return *(block->icon);
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
        return ::fontAt(uc::EmojiDraw::CONSERVATIVE, *cp);
    return {};
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


QString CharsModel::textAt(const QModelIndex& index) const
{
    auto cp = charAt(index);
    if (!cp)
        return {};
    return ::textAt(*cp);
}


QVariant CharsModel::data(const QModelIndex& index, int role) const
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

void CharsModel::drawChar(QPainter* painter, const QRect& rect,
            const QModelIndex& index, const QColor& color) const
{
    auto ch = charAt(index);
    if (ch) {
        auto color1 = fgAt(*ch, TableColors::YES);
        if (!color1.isValid())
            color1 = color;
        ::drawChar(painter, rect, 100, *ch, color1, TABLE_DRAW, EMOJI_DRAW);
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
}


void CharsModel::paintItem1(
        QPainter* painter,
        const QStyleOptionViewItem& option,
        const QModelIndex& index,
        const QColor& color) const
{
    SuperD::paint(painter, option, index);
    drawChar(painter, option.rect, index, color);
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


///// LangModel ////////////////////////////////////////////////////////////////


int LangModel::rowCount(const QModelIndex&) const
    { return loc::allLangs.size(); }

int LangModel::columnCount(const QModelIndex&) const
    { return 1; }

QVariant LangModel::data(const QModelIndex& index, int role) const
{
    switch (role) {
    case Qt::DisplayRole: {
            size_t i = index.row();
            if (i > loc::allLangs.size())
                return {};
            auto& lang = *loc::allLangs[i];
            if (lang.name.native == lang.name.international) {
                return str::toQ(lang.name.native);
            } else {
                return str::toQ(lang.name.international + u8" / " + lang.name.native);
            }
        }
    default:
        return {};
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
                auto size = sample->pixSize();
                if (pix.size() != QSize{size, size}) {
                    pix = QPixmap{size, size};
                }
                pix.fill(Qt::transparent);

                // Create painter
                QColor clFg = sample->winColor();
                QPainter painter(&pix);
                auto bounds = pix.rect();

                switch (type) {
                case uc::CpType::NONCHARACTER: {
                        drawCharBorder(&painter, bounds, clFg);
                        QBrush brush (clFg, Qt::DiagCrossPattern);
                        painter.fillRect(bounds, brush);
                    } break;
                case uc::CpType::PRIVATE_USE:
                    drawCharBorder(&painter, bounds, clFg);
                    drawAbbreviation(&painter, bounds, u8"PUA", clFg);
                    break;
                case uc::CpType::SURROGATE:
                    drawCharBorder(&painter, bounds, clFg);
                    drawAbbreviation(&painter, bounds, u8"SUR", clFg);
                    break;
                case uc::CpType::RESERVED: {
                        auto& si = cp->block().synthIcon;
                        // No continent → draw murky, otherwise use icon colours
                        if (si.ecContinent == uc::EcContinent::NONE) {
                            drawMurkyRect(&painter, bounds, clFg);
                        } else {
                            auto cont = si.continent();
                            painter.fillRect(bounds, cont.icon.bgColor);
                            drawCharBorder(&painter, bounds, clFg);
                            clFg = cont.icon.fgColor;
                        }
                        // Draw icon a bit larger — 120%
                        drawChar(&painter, bounds, 120, si.cp(), clFg,
                                 TableDraw::CUSTOM, EMOJI_DRAW);
                    } break;
                case uc::CpType::NN:
                case uc::CpType::UNALLOCATED:
                    // Just draw murky w/o foreground
                    drawMurkyRect(&painter, bounds, clFg);
                    break;
                case uc::CpType::EXISTING:
                    // OK w/o size, as 39 ≈ 40
                    drawSearchChar(&painter, bounds, cp, clFg, EMOJI_DRAW);
                }
            });
    default:
        return {};
    }
}


///// LibModel /////////////////////////////////////////////////////////////////

#define STMT(x) x;    // NOLINT(bugprone-macro-parentheses)

#define GETNODE(parent, fallback) \
    auto ii = (parent).internalId(); \
    if (ii >= uc::N_LIBNODES) \
        { STMT(fallback) } \
    auto& node = uc::libNodes[ii];

const uc::LibNode& LibModel::nodeAt(const QModelIndex& index) const
{
    GETNODE(index, return uc::libNodes[0])
    return node;
}

QModelIndex LibModel::index(int row, int, const QModelIndex &parent) const
{
    GETNODE(parent, return {})
    if (row >= node.nChildren)
        return {};
    auto newIndex = node.iFirstChild + row;
    return createIndex(row, COL0, newIndex);
}


QModelIndex LibModel::parent(const QModelIndex &child) const
{
    GETNODE(child, return {})
    auto iParent = node.iParent;
    if (iParent == 0)
        return {};
    auto& parent = uc::libNodes[iParent];
    auto iParent2 = parent.iParent;
    auto& parent2 = uc::libNodes[iParent2];
    return createIndex(iParent - parent2.iFirstChild, COL0, iParent);
}


int LibModel::rowCount(const QModelIndex &parent) const
{
    GETNODE(parent, return 0)
    return node.nChildren;
}


int LibModel::columnCount(const QModelIndex &) const
    { return 1; }


const uc::LibNode* LibModel::goToText(const uc::LibNode& x)
{
    if (x.flags.have(uc::Lfg::NO_TILE))
        return nullptr;
    if (!x.value.empty() && EmojiPainter::hasSkinGender(x.value))
        return nullptr;
    const uc::LibNode* p = &x;
    while (true) {
        if (!p->value.empty())
            return p;
        if (p->nChildren == 0)
            return nullptr;
        p = &uc::libNodes[p->iFirstChild];
    }
}

void LibModel::drawFolderTile(
        QPainter* painter, const QRect& bounds,
        const uc::LibNode& node, const QColor& color)
{
    CharTiles tiles;
    size_t iTile = 0;
    for (int iChild = 0; iChild < node.nChildren; ++iChild) {
        auto textNode = goToText(uc::libNodes[node.iFirstChild + iChild]);
        if (textNode) {
            auto& tile = tiles[iTile];
            tile.text = textNode->value;
            tile.emojiDraw = textNode->emojiDraw();
            if (++iTile >= std::size(tiles))
                break;
        }
    }
    drawCharTiles(painter, bounds, tiles, color);
}


QPixmap& LibModel::pixOf(const uc::LibNode& node) const
{
    return cache.getT(&node,
        [&node, this](QPixmap& pix) {
            auto size = sample->pixSize();
            if (pix.size() != QSize{size, size}) {
                pix = QPixmap{size, size};
            }
            pix.fill(Qt::transparent);

            // Create painter
            QColor clFg = sample->winColor();
            QPainter painter(&pix);
            auto bounds = pix.rect();

            // draw char
            switch (node.value.length()) {
            case 0:
                drawFolderTile(&painter, bounds, node, clFg);
                break;
            default:
                drawSearchChars(&painter, bounds, node.value, clFg, node.emojiDraw());
                break;
            }
        });
}


QVariant LibModel::data(const QModelIndex &index, int role) const
{
    GETNODE(index, return {})
    switch (role) {
    case Qt::DisplayRole:
        if (node.flags.have(uc::Lfg::CODE_AS_NAME)) {
            char data[50];
            node.sprintUPLUS(data);
            return data;
        } else if (!node.text.empty()) {
            return str::toQ(node.text);
        } else if (!node.value.empty()) {
            if (auto cp = uc::cpsByCode[node.value[0]])
                return cp->viewableName();
        }
        // Should not happen
        return {};
    case Qt::DecorationRole:
        return pixOf(node);

    default:
        return {};
    }
}

#undef GETNODE
#undef STMT


///// FmMain ///////////////////////////////////////////////////////////////////


FmMain::FmMain(QWidget *parent)
    : Super(parent),
      ui(new Ui::FmMain),
      model(this),
      searchModel(this),
      libModel(this),
      fontBig(str::toQ(FAM_DEFAULT), FSZ_BIG),
      fontTofu(str::toQ(FAM_TOFU), FSZ_BIG)
{
    ui->setupUi(this);

    // Tabs to 0
    ui->tabsMain->setCurrentIndex(0);

    auto ib = initBlocks();
    initLibrary(ib);
    initTerms();
    initAbout();

    // Tofu stats
    auto shcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_T), this);
    connect(shcut, &QShortcut::activated, this, &This::showTofuStats);

    // Change language
    ui->comboLang->setModel(&langModel);
    connect(ui->comboLang, &QComboBox::currentIndexChanged, this, &This::languageChanged);

    // Reload language
    shcut = new QShortcut(QKeySequence(Qt::Key_F12), this);
    connect(shcut, &QShortcut::activated, this, &This::reloadLanguage);
}


namespace {

    void paintTo(QWidget* widget, const QString& color)
    {
        widget->setStyleSheet("#" + widget->objectName() + " { background-color: " + color + "; }");
    }

}   // anon namespace


FmMain::InitBlocks FmMain::initBlocks()
{
    InitBlocks r;

    // Collapse bar
    ui->wiCollapse->hide();
    ui->wiCollapse->setStyleSheet(
                "#wiCollapse { background-color: " + BG_CJK.name() + "; }"   );
    connect(ui->btCollapse, &QPushButton::clicked,
            this, &This::cjkExpandCollapse);

    // Top bar
    QPalette pal = ui->wiCharBar->palette();
    const QColor& color = pal.color(QPalette::Normal, QPalette::Button);
    r.buttonColor = color.name();

    paintTo(ui->wiCharBar, r.buttonColor);
    paintTo(ui->pageInfo, r.buttonColor);
    paintTo(ui->pageSearch, r.buttonColor);

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
    connect(ui->comboBlock, &QComboBox::currentIndexChanged, this, &This::comboIndexChanged);
    connect(ui->comboBlock, &WideComboBox::droppedDown, this, &This::comboDroppedDown);
    connect(ui->comboBlock, &WideComboBox::pulledUp, this, &This::comboPulledUp);

    // Blocks / table
    ui->tableChars->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableChars->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableChars->setItemDelegate(&model);
    ui->tableChars->setModel(&model);

    // Blocks / divider
    auto wTotal = width();
    auto tw = ui->tableChars->verticalHeader()->width();
    tw += ui->tableChars->horizontalHeader()->defaultSectionSize() * NCOLS;
    tw += qApp->style()->pixelMetric(QStyle::PM_ScrollBarExtent);
    tw += 8;    // This computation is very precise, let it be 8px
    r.sizes = { tw, wTotal - ui->splitBlocks->handleWidth() - tw };
    ui->splitBlocks->setStretchFactor(0, 0);
    ui->splitBlocks->setStretchFactor(1, 1);
    ui->splitBlocks->setSizes(r.sizes);

    // Connect events
    connect(ui->tableChars->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &This::charChanged);

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

    // Sort menu
    QActionGroup* grpSortBy = new QActionGroup(this);
    radioSortOrder.setRadio(BlockOrder::ALPHA,     ui->acSortByAlpha);
    radioSortOrder.setRadio(BlockOrder::CONTINENT, ui->acSortByContinent);
    radioSortOrder.setRadio(BlockOrder::CODE,      ui->acSortByCode);
    grpSortBy->addAction(ui->acSortByAlpha);
    grpSortBy->addAction(ui->acSortByContinent);
    grpSortBy->addAction(ui->acSortByCode);
    QMenu* menuSort = new QMenu(this);
    menuSort->addAction(ui->acSortByAlpha);
    menuSort->addAction(ui->acSortByContinent);
    menuSort->addAction(ui->acSortByCode);
    connect(ui->acSortByAlpha, &QAction::triggered, this, &This::blockOrderChanged);
    connect(ui->acSortByCode, &QAction::triggered, this, &This::blockOrderChanged);
    connect(ui->acSortByContinent, &QAction::triggered, this, &This::blockOrderChanged);

    // Sort bar
    QToolBar* sortBar = new QToolBar(ui->laySortBar->parentWidget());
    ui->laySortBar->addWidget(sortBar);
    btSort = new QToolButton(sortBar);

    // ALPHA is localized!!
    sortIcons[BlockOrder::CONTINENT].addFile(":/Buttons/globe.png",   { 16, 16 });
    sortIcons[BlockOrder::CODE     ].addFile(":/Buttons/sort_0F.png", { 16, 16 });
    btSort->setMenu(menuSort);
    btSort->setPopupMode(QToolButton::InstantPopup);
    sortBar->addWidget(btSort);

    // Select index
    ui->tableChars->setFocus();
    auto index = model.index(0, 0);
    ui->tableChars->selectionModel()->select(index, QItemSelectionModel::SelectCurrent);
    shownCp = uc::cpInfo[0];

    return r;
}


void FmMain::initLibrary(const InitBlocks& ib)
{
    paintTo(ui->wiLibInfo, ib.buttonColor);

    // Tree
    ui->treeLibrary->setModel(&libModel);

    // Divider
    ui->splitLibrary->setStretchFactor(0, 0);
    ui->splitLibrary->setStretchFactor(1, 1);
    ui->splitLibrary->setSizes(ib.sizes);

    // OS style
    auto& font = uc::fontInfo[0];
    ui->lbLibOs->setFont(font.get(uc::FontPlace::SAMPLE, FSZ_BIG, false, NO_TRIGGER));

    // Connect events
    connect(ui->treeLibrary->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &This::libChanged);

    // Clicked
    connect(ui->vwLibInfo, &QTextBrowser::anchorClicked, this, &This::anchorClicked);
    connect(ui->lbLibCharCode, &QLabel::linkActivated, this, &This::labelLinkActivated);

    // Copy
        // Ctrl+C
    auto shcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_C), ui->treeLibrary,
                nullptr, nullptr, Qt::WidgetWithChildrenShortcut);
    connect(shcut, &QShortcut::activated, this, &This::copyCurrentLib);
        // Ctrl+Ins
    shcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Insert), ui->treeLibrary,
                nullptr, nullptr, Qt::WidgetWithChildrenShortcut);
    connect(shcut, &QShortcut::activated, this, &This::copyCurrentLib);
        // Button
    connect(ui->btLibCopy, &QPushButton::clicked, this, &This::copyCurrentLib);
        // 2click
    ui->treeLibrary->viewport()->installEventFilter(this);

    // Select index
    auto index = libModel.index(0, 0);
    ui->treeLibrary->selectionModel()->select(index, QItemSelectionModel::SelectCurrent);
}


BlockOrder FmMain::blockOrder() const
    { return radioSortOrder.get(); }


void FmMain::setBlockOrder(BlockOrder x)
{
    radioSortOrder.set(x);
    rebuildBlocks();
}


void FmMain::rebuildBlocks()
{
    // Do nothing if no lang loaded
    // (specifically for loading settings)
    if (!loc::currLang)
        return;

    auto order = blockOrder();
    btSort->setIcon(sortIcons[order]);
    auto index = ui->comboBlock->currentIndex();
    auto index2 = blocksModel.build(order, index);
    ui->comboBlock->setCurrentIndex(index2);
}

namespace {

    void loadIcon(QIcon& r, std::string_view name)
    {
        r = QIcon{};
        auto fullName = str::cat(":/Buttons/", name, ".png");
        r.addFile(QString::fromStdString(fullName), {16, 16});
    }

}   // anon namespace


void FmMain::translateMe()
{
    Form::translateMe();

    // Misc. widgets loaded from UI files
    loc::translateForm(ui->wiSample);
    loc::translateForm(ui->wiLibSample);

    translateTerms();
    translateAbout();
    cjkReflectCollapseState();

    ui->comboBlock->resizeView();

    // International icons
    loadIcon(sortIcons[BlockOrder::ALPHA], loc::active::icons.sortAZ);

    // Sort order
    btSort->setToolTip(loc::get("Main.Sort"));
    rebuildBlocks();

    // Main tab
    forceShowCp(shownCp);
}


void FmMain::translateTerms()
{
    QString text;
    mywiki::appendStylesheet(text);

    const size_t n = uc::nTerms();
    auto lastCat = uc::EcTermCat::NN;
    for (size_t i = 0; i < n; ++i) {
        const uc::Term& term = *uc::sortedTerms[i];
        if (term.ecCat != lastCat) {
            lastCat = term.ecCat;
            str::append(text, "<h1>");
            str::append(text, loc::get(term.cat().locKey));
            str::append(text, "</h1>");
        }
        str::append(text, "<p><a href='pt:");
        str::append(text, term.key);
        str::append(text, "' class='popup'><b>");
        str::append(text, term.loc.name);
        str::append(text, "</b></a>");
        if (mywiki::isEngTermShown(term)) {
            str::append(text, "&nbsp;/ ");
            str::append(text, term.engName);
        }
    }

    ui->vwTerms->setText(text);
}


void FmMain::initTerms()
{
    connect(ui->vwTerms, &QTextBrowser::anchorClicked, this, &This::anchorClicked);
}


namespace {

    QString qPopupLink(
            std::string_view locKey, const char* target)
    {
        return QString("<a href='") + target + "' style='" STYLE_POPUP "'>"
                + loc::get(locKey) + "</a>";
    }

}   // anon namespace


void FmMain::translateAbout()
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
                     uc::versionInfo[static_cast<int>(uc::EcVersion::LAST)].name)));

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
}


void FmMain::initAbout()
{
    connect(ui->lbTofuStats, &QLabel::linkActivated, this, &This::showTofuStats);
}


bool FmMain::eventFilter(QObject *obj, QEvent *event)
{
    switch (event->type()) {
    case QEvent::MouseButtonDblClick: {
            auto mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton) {
                if (obj == ui->tableChars->viewport()) {
                    copyCurrentChar();
                    return true;
                } else if (obj == ui->treeLibrary->viewport()) {
                    auto index = ui->treeLibrary->currentIndex();
                    if (libModel.rowCount(index) == 0) {
                        copyCurrentLib();
                        return true;
                    }
                }
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
             .showAtAbs(loc::get("Common.Copied"), widget, absRect);
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
            q += QChar(VS16);
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


void FmMain::copyCurrentLib()
{
    auto index = ui->treeLibrary->currentIndex();
    if (!index.isValid())
        return;
    auto& node = libModel.nodeAt(index);
    if (node.value.empty())
        return;
    auto q = str::toQ(node.value);
    QApplication::clipboard()->setText(q);
    showCopied(ui->treeLibrary);
}


template<>
void wiki::append(QString& s, const char* start, const char* end)
{
    s.append(QByteArray::fromRawData(start, end - start));
}

namespace {

    void setWiki(QTextBrowser* view, const QString& text)
    {
        view->setText(text);
        // I won’t hack: my underline is lower, and it’s nice.
        // What to do with that one — IDK.
        //mywiki::hackDocument(view->document());
    }

}   // anon namespace


void FmMain::forceShowCp(MaybeChar ch)
{
    shownCp = ch;

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
    int newIBlock = block->cachedIndex;
    if (newIBlock != iBlock)
        ui->comboBlock->setCurrentIndex(newIBlock);
    ui->wiCollapse->setVisible(block->flags.have(uc::Bfg::COLLAPSIBLE));

    if (ch) {
        if (ch->category().upCat == uc::UpCategory::MARK) {
            ui->btCopyEx->setText("+25CC");
            ui->btCopyEx->show();
        } else if (ch->isVs16Emoji()) {
            ui->btCopyEx->setText("+VS16");
            ui->btCopyEx->show();
        } else {
            ui->btCopyEx->hide();
        }
        const bool wantSysFont = !ch->isDefaultIgnorable() && ch->isGraphical();

        // Sample char
        ui->wiSample->showCp(*ch);

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
        setWiki(ui->vwInfo, text);
    } else {
        // No character
        ui->wiSample->showNothing();
        ui->lbOs->setText({});
        const auto& osTitle = loc::get(
                uc::isNonChar(ch.code) ? "Prop.Os.NonChar" : "Prop.Os.Empty");
        ui->lbOsTitle->setText(osTitle);
        ui->btCopyEx->hide();
        if (uc::isNonChar(ch.code)) {
            QString text = mywiki::buildNonCharHtml(ch.code);
            setWiki(ui->vwInfo, text);
        } else {
            auto color = palette().color(QPalette::Disabled, QPalette::WindowText);
            QString text = mywiki::buildEmptyCpHtml(ch.code, color);
            setWiki(ui->vwInfo, text);
        }
    }
}


void FmMain::showCp(MaybeChar ch)
{
    if (ch.code == shownCp.code)
        return;
    forceShowCp(ch);
}


void FmMain::charChanged(const QModelIndex& current)
{
    showCp(model.charAt(current));
}


void FmMain::libChanged(const QModelIndex& current)
{
    auto& node = libModel.nodeAt(current);
    if (node.value.empty()) {
        // Folder
        ui->wiLibSample->showNothing();
        ui->lbLibCharCode->clear();
        ui->btLibCopy->setEnabled(false);
        ui->lbLibOs->clear();
        ui->lbLibOsTitle->setText(loc::get("Prop.Os.Style"));

        auto color = palette().color(QPalette::Disabled, QPalette::WindowText);
        QString s = mywiki::buildLibFolderHtml(node, color);
        setWiki(ui->vwLibInfo, s);
    } else {
        // Actual node
        // Show sample
        if (node.flags.have(uc::Lfg::GRAPHIC_EMOJI) || node.value.length() > 1) {
            ui->wiLibSample->showEmoji(node.value);
        } else if (auto cp = uc::cpsByCode[node.value[0]]) {
            ui->wiLibSample->showCp(*cp);
        } else  {
            ui->wiLibSample->showNothing();
        }

        auto osText = str::toQ(node.value);
        if (node.flags.have(uc::Lfg::GRAPHIC_EMOJI)) {
            // 0x1F300 = very old emoji from Webdings
            constexpr char32_t SOME_EMOJI = 0x1F300;
            auto char0 = node.value[0];
            std::optional<QFont> font;
            if (char0 <= 0xFFFF) {
                // BMP emoji, really old
                font = model.match.sysFontFor(SOME_EMOJI, FSZ_BIG);
            } else {
                // SMP emoji
                font = model.match.sysFontFor(char0, FSZ_BIG);
            }
            if (font) {
                ui->lbLibOs->setFont(*font);
                ui->lbLibOsTitle->setText(font->family());
            } else {
                ui->lbLibOs->setFont(fontBig);
                ui->lbLibOsTitle->setText(loc::get("Prop.Os.Tofu"));
            }
        } else {
            /// @todo [future] not necessarily emoji
            ui->lbLibOs->setFont(fontBig);
            ui->lbLibOsTitle->setText(fontBig.family());
        }
        ui->lbLibOs->setText(osText);

        // Codes
        char buf[150], shortBuf[50];
        node.sprintUPLUS(buf);
        switch (node.value.length()) {
        case 0:     // never happens
        case 1:
        case 2:
            strcpy_s(shortBuf, buf);
            break;
        default:
            uc::sprintUPLUS(shortBuf, node.value[0]);
            strcat_s(shortBuf, "+…");
        }
        QString ucText;
        mywiki::appendCopyable2(ucText, buf, shortBuf, "' style='" STYLE_BIGCOPY);
        ui->lbLibCharCode->setText(ucText);
        ui->btLibCopy->setEnabled(true);

        QString s = mywiki::buildHtml(node);
        setWiki(ui->vwLibInfo, s);
    }
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
void FmMain::selectChar<SelectMode::INSTANT>(char32_t code)
{
    selectChar<SelectMode::NONE>(code);
    ui->tableChars->setFocus();
}


void FmMain::comboDroppedDown()
{
    pullUpDetector.timer.invalidate();
    pullUpDetector.isCocked = true;
}


void FmMain::comboPulledUp()
{
    if (pullUpDetector.isCocked) {
        pullUpDetector.isCocked = false;
        pullUpDetector.timer.start();
    }
}


void FmMain::comboIndexChanged(int index)
{
    if (index < 0)
        return;
    auto oldChar = model.charAt(ui->tableChars->currentIndex());
    auto oldBlock = uc::blockOf(oldChar.code);
    if (oldBlock->cachedIndex != static_cast<size_t>(index)) {
        auto& newBlock = blocksModel.at(index);
        bool detected = pullUpDetector.detect();
        selectChar<SelectMode::NONE>(newBlock.firstAllocated->subj);
        if (detected) {
            ui->tableChars->setFocus();
            // Unglitch combobox: lingering highlight was HOVER,
            // not focus.
            ui->comboBlock->setAttribute(Qt::WA_UnderMouse, false);
        }
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


void FmMain::doSearch(const QString& what)
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
                 .showAtRel(loc::get("Search.NoSuch"), ui->listSearch->viewport(), relRect);
    }
}


void FmMain::languageChanged(int index)
{
    size_t x = index;
    if (x < loc::allLangs.size()) {
        loc::allLangs[x]->load();
    }
}


void FmMain::reloadLanguage()
{
    if (loc::currLang)
        loc::currLang->forceLoad();
}


void FmMain::chooseFirstLanguage()
{
    loc::allLangs.loadStarting(
                config::lang::wanted, config::lang::savedStamp);
    auto index = loc::allLangs.byPtr(loc::currLang);
    ui->comboLang->setCurrentIndex(index);
}


void FmMain::blockOrderChanged()
{
    rebuildBlocks();
}


int FmMain::pixSize() const
{
    auto& fn = font();
    QFontMetrics metrics{fn};
    return (metrics.ascent() + metrics.descent()) * 3;
}
