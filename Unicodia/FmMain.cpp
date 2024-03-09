// My header
#include "FmMain.h"
#include "ui_FmMain.h"

// C++
#include <unordered_set>

// XML
#include "pugixml.hpp"

// Qt
#include <QTableView>
#include <QTextFrame>
#include <QClipboard>
#include <QPainter>
#include <QShortcut>
#include <QPaintEngine>
#include <QMessageBox>
#include <QFile>
#include <QSvgRenderer>
#include <QToolBar>
#include <QToolButton>
#include <QActionGroup>
#include <QIconEngine>
#include <QDesktopServices>

// Qt net
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QUrl>

// GitHub
#include "GitHub/parsers.h"

// Misc
#include "u_Strings.h"
#include "u_Qstrings.h"
#include "i_TempFont.h"

// Unicode
#include "UcCp.h"
#include "UcClipboard.h"

// Char drawing
#include "CharPaint/routines.h"
#include "CharPaint/IconEngines.h"
#include "CharPaint/emoji.h"

// Project-local
#include "Skin.h"
#include "MyWiki.h"

// Forms
#include "FmTofuStats.h"
#include "WiOsStyle.h"
#include "WiLibCp.h"

// L10n
#include "LocDic.h"
#include "LocList.h"


template class LruCache<char32_t, QPixmap>;

using namespace std::string_view_literals;

namespace {
    // No need custom drawing — solves nothing
    constexpr TableDraw TABLE_DRAW = TableDraw::INTERNAL;
#define N_CHECKED_VERSIONS "8"
#define SUBURL_REPO "Mercury13/unicodia/releases"
    constinit const char* URL_UPDATE = "https://api.github.com/repos/" SUBURL_REPO "?per_page=" N_CHECKED_VERSIONS;
    constinit const char* URL_REPO = "https://github.com/" SUBURL_REPO;
}

///// RowCache /////////////////////////////////////////////////////////////////

RowCache::RowCache(int anCols)
    : fnCols(anCols), fColMask(anCols - 1), fRowMask(~fColMask) {}


MaybeChar RowCache::charAt(size_t iRow, unsigned iCol) const
{
    // Have row?
    if (iRow >= rows.size() || iCol >= NCOLS)
        return {};
    auto& rw = rows[iRow];
    auto start = rw.startingCp;

    return { start + iCol };
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

    ensureRow(rowCp);
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

    bool isTechLess(const uc::Block* x, const uc::Block* y)
    {
        return strncasecmp(x->name.data(), y->name.data(), 100) < 0;
    }

}   // anon namespace


size_t BlocksModel::build(BlockOrder aOrder, size_t iOld)
{
    order = aOrder;
    auto& block = at(iOld);
    for (size_t i = 0; i < uc::N_BLOCKS; ++i)
        a[i] = &uc::blocks[i];

    // Primary sorting
    switch (aOrder) {
    case BlockOrder::CODE: break; // do nothing
    case BlockOrder::ALPHA:
        std::stable_sort(a.begin(), a.end(), isAlphaLess);
        break;
    case BlockOrder::CONTINENT:
        std::stable_sort(a.begin(), a.end(), isContinentLess);
        break;
    case BlockOrder::TECH:
        std::stable_sort(a.begin(), a.end(), isTechLess);
        break;
    }

    for (size_t i = 0; i < uc::N_BLOCKS; ++i)
        a[i]->cachedIndex = i;
    return block.cachedIndex;
}


int BlocksModel::rowCount(const QModelIndex&) const { return uc::N_BLOCKS; }

int BlocksModel::columnCount(const QModelIndex&) const { return 1; }

namespace {

    QIconEngine* getCustomEngine(char32_t startingCp)
    {
        switch (startingCp) {
        case 0x2460:    // Enclosed alnum
            return new ie::OneCircle;
        case 0x2580:    // Block elements
            return new ie::BlockElem;
        case 0x2FF0:    // Ideographic description
            return new ie::CjkStructure;
        case 0x4DC0:    // Yijing
            return new ie::CoarseImage(BG_CJK, { 1,0 }, ":ScCustom/yijing.png");
        case 0x10100:   // Aegeam numbers
            return new ie::CoarseImage(BG_EUROPE, { 1,1 }, ":ScCustom/aegean.png");
        case 0x1D300:   // Tai Xuan
            return new ie::Taixu;
        case 0x1D360:   // Counting rods
            return new ie::TallyMark;
        case 0x1F000:   // Mahjong
            return new ie::Mahjong;
        case 0x1F0A0:   // Playing cards
            return new ie::PlayingCard;
        case 0x1F100:   // Enclosed alnum supp
            return new ie::ThreeD;
        case 0x1F200:   // Eclosed ideo supp
            return new ie::SqIdeo;
        case 0x1FB00:   // Legacy computing
            return new ie::Legacy;
        default:
            throw std::logic_error("No custom engine right now");
        }
    }

}   // anon namespace

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
            switch (order) {
            case BlockOrder::ALPHA:
            case BlockOrder::CONTINENT:
                if (block->loc.hasEllipsis) {
                    return str::toQ(str::cat(
                            loc::currLang->ellipsis.text, block->loc.name));
                }
                [[fallthrough]];
            case BlockOrder::CODE:
                return str::toQ(block->loc.name);
            case BlockOrder::TECH:
                return str::toQ(block->name);
            }
            __builtin_unreachable();
        }
    case Qt::DecorationRole: {
            GET_BLOCK
            if (!block->icon) {
                if (block->synthIcon.flags.have(uc::Ifg::FORMAT)) {
                    block->icon = new QIcon(new ie::Format(*block));
                } else if (block->synthIcon.flags.have(uc::Ifg::CUSTOM_ENGINE)) {
                    block->icon = new QIcon(getCustomEngine(block->startingCp));
                } else {
                    block->icon = new QIcon(new ie::Hint(*block));
                }
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

// -warn: strange with =default
CharsModel::~CharsModel() {}


int CharsModel::rowCount(const QModelIndex&) const
{
    return rows.nRows();
}


int CharsModel::columnCount(const QModelIndex&) const
    { return NCOLS; }


std::optional<QFont> CharsModel::fontAt(const QModelIndex& index) const
{
    if (auto cp = charAt(index))
        return ::fontAt(WiShowcase::EMOJI_DRAW, *cp, glyphStyle.sets);
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
    return cp->sampleProxy(uc::ProxyType::TABLE_DEFAULT, WiShowcase::EMOJI_DRAW, glyphStyle.sets).text;
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
        ::drawChar(painter, rect, 100, *ch, color1, TABLE_DRAW, WiShowcase::EMOJI_DRAW, glyphStyle.sets);
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
    char buf[60];

    switch (role) {
    case Qt::DisplayRole: {
            QString s;
            if (line.code < uc::CAPACITY) {
                // Character code
                uc::sprintUPLUS(buf, line.code);
                s += buf;
                if (line.prio.high == uc::HIPRIO_DEC) {
                    // Found by decimal
                    s += " = ";
                    s += QString::number(line.code);
                    s += "₁₀";
                }
            } else if (line.node) {
                // Library node
                std::u32string_view val = line.node->value;
                static constexpr auto SHORTLEN = 2;
                bool needShort = (val.length() > SHORTLEN && !line.triggerName.empty());
                if (needShort) {
                    val = val.substr(0, SHORTLEN);
                    auto n = 0;
                    for (auto c : val)
                        n = uc::appendUPLUS(buf, n, c);
                    s += buf;
                    s += "+…";
                } else {
                    line.node->sprintUPLUS(buf);
                    s += buf;
                }
            }
            if (!line.triggerName.empty()) {
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
            case uc::CpType::LIBNODE:
                if (!line.node) // -warn, should not happen for now
                    return s;
                return s + line.node->viewableTitle(uc::TitleMode::LONG);
            default:
                return s + loc::get(uc::cpTypeKeys[static_cast<int>(line.type)]);
            }
        }

    case Qt::DecorationRole: {
            QIconEngine* ie = nullptr;
            switch (line.type) {
            case uc::CpType::EXISTING:
                ie = new ie::Cp(*sample, EMOJI_DRAW, line.cp, glyphSets);
                break;
            case uc::CpType::NN:
            case uc::CpType::NONCHARACTER:
                ie = new ie::Nonchar(*sample);
                break;
            case uc::CpType::PRIVATE_USE:
                ie = new ie::CustomAbbr(*sample, u8"PUA");
                break;
            case uc::CpType::SURROGATE:
                ie = new ie::CustomAbbr(*sample, u8"SUR");
                break;
            case uc::CpType::UNALLOCATED:
                ie = new ie::Murky(*sample);
                break;
            case uc::CpType::RESERVED:
                ie = new ie::Synth(*sample, line.cp->block().synthIcon);
                break;
            case uc::CpType::LIBNODE:
                ie = new ie::Node(*sample, *line.node);
                break;
            }
            QIcon icon(ie);
            return icon;
        }
    default:
        return {};
    }
}


void SearchModel::initStyleOption(
        QStyleOptionViewItem *option, const QModelIndex &index) const
{
    QStyledItemDelegate::initStyleOption(option, index);
    auto& line = lineAt(index.row());
    if (line.nestLevel != 0) {
        option->rect.setLeft(option->rect.left() + 10 * line.nestLevel);
    }
}


///// LibModel /////////////////////////////////////////////////////////////////

#define STMT(x) x;    // NOLINT(bugprone-macro-parentheses)

#define GETNODE(parent, fallback) \
    auto ii = (parent).internalId(); \
    if (ii >= uc::nLibNodes()) \
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


QModelIndex LibModel::indexOf(const uc::LibNode& node)
{
    size_t iNode = &node - uc::libNodes;
    if (iNode == 0 || iNode >= uc::nLibNodes())
        return {};
    auto& parentNode = uc::libNodes[node.iParent];
    size_t iFirstChild = parentNode.iFirstChild;
    if (iFirstChild > iNode)
        return {};
    auto row = iNode - iFirstChild;
    return createIndex(row, COL0, iNode);
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


QVariant LibModel::data(const QModelIndex &index, int role) const
{
    GETNODE(index, return {})
    switch (role) {
    case Qt::DisplayRole:
        return node.viewableTitle(uc::TitleMode::SHORT);
    case Qt::DecorationRole: {
            QIconEngine* ie = new ie::Node(*sample, node);
            return QIcon(ie);
        }
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
      searchModel(this, model.glyphStyle.sets),
      libModel(this),
      fontBig(str::toQ(FAM_DEFAULT), FSZ_BIG),
      fontTofu(str::toQ(FAM_TOFU), FSZ_BIG),
      mainGui(this, model.match)
{
    ui->setupUi(this);

    // Tabs to 0
    ui->tabsMain->setCurrentIndex(0);

    auto ib = initBlocks();
    initLibrary(ib);
    initFavs(ib);
    initTerms();
    initAbout();

    // Search
    ui->listSearch->setIconSize(pixQsize());
    ui->edSearch->lineEdit()->setPlaceholderText("[Search]");

    // Popup link
    connect(&mainGui, &MyGui::linkActivated, this, &This::popupLinkActivated);

    // Tofu stats
    auto shcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_T), this);
    connect(shcut, &QShortcut::activated, this, &This::showTofuStats);

    // Change language
    ui->comboLang->setModel(&langModel);
    connect(ui->comboLang, &QComboBox::currentIndexChanged, this, &This::languageChanged);

    // Reload language
    shcut = new QShortcut(QKeySequence(Qt::Key_F12), this);
    connect(shcut, &QShortcut::activated, this, &This::reloadLanguage);

    // Write tiles
    shcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_F12), this);
    connect(shcut, &QShortcut::activated, this, &This::dumpTiles);

    // Debug painter
    //shcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_F11), this);
    //connect(shcut, &QShortcut::activated, []{ TableCache::wantDebug = true; });
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
    const QColor& color = pal.color(QPalette::Normal, QPalette::Window);
    r.buttonColor = color.name();

    paintTo(ui->wiCharBar, r.buttonColor);
    paintTo(ui->pageInfo, r.buttonColor);
    paintTo(ui->pageSearch, r.buttonColor);

    // Fill chars
    model.build();

    // Combobox
    ui->comboBlock->setModel(&blocksModel);
    if (auto view = qobject_cast<QListView*>(ui->comboBlock->view())) {
        // true for Qt6, maybe I’ll add Qt5 x86?
        view->setUniformItemSizes(true);
    }
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
    connect(ui->wiCharShowcase, &WiShowcase::glyphStyleChanged, this, &This::glyphStyleChanged);

    // Copy
        // Ctrl+C
    auto shcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_C), ui->tableChars,
                nullptr, nullptr, Qt::WidgetWithChildrenShortcut);
    connect(shcut, &QShortcut::activated, this, &This::copyCurrentCharNull);
        // Ctrl+Ins
    shcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Insert), ui->tableChars,
                nullptr, nullptr, Qt::WidgetWithChildrenShortcut);
    connect(shcut, &QShortcut::activated, this, &This::copyCurrentCharNull);
        // Button
    connect(ui->wiCharShowcase, &WiShowcase::copiedPopped, this, &This::blinkCopiedForWidget);
        // 2click
    ui->tableChars->viewport()->installEventFilter(this);

    // Copy sample: Ctrl+Shift+C
    shcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_C), ui->tableChars,
                nullptr, nullptr, Qt::WidgetWithChildrenShortcut);
    connect(shcut, &QShortcut::activated, this, &This::copyCurrentSampleNull);
    shcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_Insert), ui->tableChars,
                nullptr, nullptr, Qt::WidgetWithChildrenShortcut);
    connect(shcut, &QShortcut::activated, this, &This::copyCurrentSampleNull);

    // Clicked
    connect(ui->wiCharShowcase, &WiShowcase::linkActivated, this, &This::advancedLinkActivated);
    connect(ui->vwInfo, &QTextBrowser::anchorClicked, this, &This::anchorClicked);

    // Search
    ui->stackSearch->setCurrentWidget(ui->pageInfo);
    connect(ui->btCloseSearch, &QPushButton::clicked, this, &This::closeSearch);
    connect(ui->tableChars, &CharsTable::focusIn, this, &This::closeSearch);
    ui->listSearch->setUniformItemSizes(true);
    ui->listSearch->setModel(&searchModel);
    ui->listSearch->setItemDelegate(&searchModel);
    connect(ui->edSearch, &SearchCombo::searchPressed, this, &This::startSearch);
    connect(ui->edSearch, &SearchCombo::focusIn, this, &This::focusSearch);
    connect(ui->listSearch, &SearchList::enterPressed, this, &This::searchEnterPressed);

    // Sort menu
    QActionGroup* grpSortBy = new QActionGroup(this);
    QMenu* menuSort = new QMenu(this);
    auto addAc = [this, grpSortBy, menuSort](BlockOrder order, QAction* action) {
        radioSortOrder.setRadio(order, action);
        grpSortBy->addAction(action);
        menuSort->addAction(action);
        connect(action, &QAction::triggered, this, &This::blockOrderChanged);
    };
    addAc(BlockOrder::ALPHA,     ui->acSortByAlpha);
    addAc(BlockOrder::CONTINENT, ui->acSortByContinent);
    addAc(BlockOrder::CODE,      ui->acSortByCode);
    addAc(BlockOrder::TECH,      ui->acSortByTech);

    // Sort bar
    QToolBar* sortBar = new QToolBar(ui->laySortBar->parentWidget());
    ui->laySortBar->addWidget(sortBar);
    btSort = new QToolButton(sortBar);

    // ALPHA is localized!!
    sortIcons[BlockOrder::CONTINENT].addFile(":/Buttons/globe.svg",     { 24, 24 });
    sortIcons[BlockOrder::CODE     ].addFile(":/Buttons/sort_0F.svg",   { 24, 24 });
    sortIcons[BlockOrder::TECH     ].addFile(":/Buttons/sort_tech.svg", { 24, 24 });
    btSort->setMenu(menuSort);
    btSort->setPopupMode(QToolButton::InstantPopup);
    sortBar->addWidget(btSort);    

    // Select index
    ui->tableChars->setFocus();
    auto index = model.index(0, 0);
    ui->tableChars->selectionModel()->select(index, QItemSelectionModel::SelectCurrent);
    ui->wiCharShowcase->setSilent(0);

    return r;
}


void FmMain::initLibrary(const InitBlocks& ib)
{
    paintTo(ui->wiLibContainer, ib.buttonColor);

    // Tree
    ui->treeLibrary->setModel(&libModel);
    ui->treeLibrary->setIconSize(pixQsize());

    // Divider
    ui->splitLibrary->setStretchFactor(0, 0);
    ui->splitLibrary->setStretchFactor(1, 1);
    ui->splitLibrary->setSizes(ib.sizes);

    // Codepoints
    auto layout = qobject_cast<QBoxLayout*>(ui->wiLibCps->layout());
    for (unsigned i = 0; i < uc::LONGEST_LIB; ++i) {
        auto wi = new WiLibCp(ui->wiLibCps);
        layout->addWidget(wi);
        libCpWidgets[i] = wi;
        connect(wi, &WiLibCp::goToCp, this, &This::goToCp);
    }
    layout->addStretch(1);

    // Connect events
    connect(ui->treeLibrary->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &This::libChanged);

    // Clicked
    connect(ui->vwLibInfo, &QTextBrowser::anchorClicked, this, &This::anchorClicked);
    connect(ui->wiLibShowcase, &WiShowcase::copiedPopped, this, &This::blinkCopiedForWidget);
    connect(ui->wiLibShowcase, &WiShowcase::linkActivated, this, &This::advancedLinkActivated);
    // GlyphStyleChanged is unused for now

    // Copy
        // Ctrl+C
    auto shcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_C), ui->treeLibrary,
                nullptr, nullptr, Qt::WidgetWithChildrenShortcut);
    connect(shcut, &QShortcut::activated, this, &This::copyCurrentLib);
        // Ctrl+Ins
    shcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Insert), ui->treeLibrary,
                nullptr, nullptr, Qt::WidgetWithChildrenShortcut);
    connect(shcut, &QShortcut::activated, this, &This::copyCurrentLib);
        // 2click
    ui->treeLibrary->viewport()->installEventFilter(this);

    // Select index
    auto index = libModel.index(0, 0);
    ui->treeLibrary->selectionModel()->select(index, QItemSelectionModel::SelectCurrent);
}


void FmMain::initFavs(const InitBlocks& ib)
{
    paintTo(ui->wiFavsBar, ib.buttonColor);

    /// @todo [favs] initFavs
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
        auto fullName = str::cat(":/Buttons/", name, ".svg");
        r.addFile(QString::fromStdString(fullName), {24, 24});
    }

}   // anon namespace


void FmMain::translateMe()
{
    Form::translateMe();

    // Misc. widgets loaded from UI files
    ui->wiCharShowcase->translateMe();
    ui->wiLibShowcase->translateMe();

    translateTerms();
    translateAbout();
    cjkReflectCollapseState();

    ui->comboBlock->resizeView();

    // International icons
    loadIcon(sortIcons[BlockOrder::ALPHA], loc::active::icons.sortAZ);

    // Search
    ui->edSearch->lineEdit()->setPlaceholderText(loc::get("Main.Search"));

    // Sort order
    btSort->setToolTip(loc::get("Main.Sort"));
    rebuildBlocks();

    // Main tab
    if (auto p = ui->wiCharShowcase->shownObj().maybeCp())
        forceShowCp(*p);

    // Library tab
    libChanged(ui->treeLibrary->currentIndex());
}


void FmMain::translateTerms()
{
    QString text;
    mywiki::appendStylesheet(text);

    const size_t n = uc::nTerms();
    auto lastCat = uc::EcTermCat::NN;
    for (size_t i = 0; i < n; ++i) {
        const uc::Term& term = *uc::sortedTerms[i];
        // Term hidden?
        if (term.ecCat == uc::EcTermCat::HIDDEN)
            continue;
        // Category changed?
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
    // Versions
    str::append(text, "<h1>");
    str::append(text, loc::get("TermCat.Vers"));
    str::append(text, "</h1><p>");
    {
        unsigned n = 0;
        str::QSep sp(text, "&nbsp;· ");
        for (auto& v : uc::allVersions()) {
            if (v.date) {
                if (n != 0 && n % 10 == 0) {
                    text += "<p>";
                } else {
                    sp.sep();
                }
                str::append(text, "<b><a class='popup' href='");
                str::append(text, v.link(u8"pv:"));
                str::append(text, "'>");
                str::append(text, v.termName());
                str::append(text, "</a></b>");
                ++n;
            }
        }
    }

    ui->vwTerms->setText(text);
}


void FmMain::initTerms()
{
    connect(ui->vwTerms, &QTextBrowser::anchorClicked, this, &This::anchorClicked);
}


void FmMain::translateAbout()
{
    // lbVersion
    char8_t buf[50];
    QString s = loc::get("About.Version")
                .argQ(updatever::version.toSv(buf),
                      uc::versionInfo[static_cast<int>(uc::EcVersion::LAST)].locLongName());
    if (updatever::isDebuggingVersion) {
        s += " <b>[DEBUGGING]</b>";
    }
    ui->lbAboutVersion->setText(s);

    // vwVersion
    QFile f(":/Texts/about.htm");
    f.open(QIODevice::ReadOnly);
    s = f.readAll();
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
    if (!updatever::version)
        updatever::version = Version::parsePermissive(QApplication::applicationVersion());
    setUpdating(false);
    ui->wiLogo->load(QString{":/Misc/about.svg"});
    connect(ui->lbTofuStats, &QLabel::linkActivated, this, &This::showTofuStats);
    connect(ui->btCheckForUpdate, &QPushButton::clicked, this, &This::startUpdate);
}


bool FmMain::eventFilter(QObject *obj, QEvent *event)
{
    switch (event->type()) {
    case QEvent::MouseButtonDblClick: {
            auto mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton) {
                if (obj == ui->tableChars->viewport()) {
                    copyCurrentChar(nullptr);
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


void FmMain::blinkCopied(QWidget* widget, const QRect& absRect)
{
    mainGui.blinkCopied(widget, absRect);
}


void FmMain::blinkCopied(QAbstractItemView* table, QWidget* initiator)
{
    if (!initiator)
        initiator = qobject_cast<QWidget*>(sender());
    QPoint corner { 0, 0 };
    QSize size { 0, 0 };
    if (initiator) {
        // corner is (0,0)
        size = initiator->size();
    } else {
        auto selIndex = table->currentIndex();
        auto selRect = table->visualRect(selIndex);
        initiator = table->viewport();
        // Geometry is in PARENT’s coord system
        // Rect is (0,0) W×H
        auto visibleGeo = initiator->rect();
        selRect = selRect.intersected(visibleGeo);
        if (selRect.isEmpty()) {
            corner = visibleGeo.center();
            // Size is still 0
        } else {
            corner = selRect.topLeft();
            size = selRect.size();
        }
    }

    corner = initiator->mapToGlobal(corner);
    blinkCopied(initiator, QRect{ corner, size});
}


void FmMain::copyCurrentThing(uc::CopiedChannel channel, QWidget* initiator)
{
    auto ch = model.charAt(ui->tableChars->currentIndex());
    uc::copyCp(ch.code, channel);
    blinkCopied(ui->tableChars, initiator);
}


void FmMain::blinkCopiedForWidget(QWidget* initiator)
    { blinkCopied(nullptr, initiator); }


void FmMain::copyCurrentCharNull()
{
    copyCurrentChar(nullptr);
}


void FmMain::copyCurrentChar(QWidget* initiator)
{
    copyCurrentThing(uc::CopiedChannel::CHAR, initiator);
}


void FmMain::copyCurrentSampleNull()
{
    copyCurrentSample(nullptr);
}


void FmMain::copyCurrentSample(QWidget* initiator)
{
    copyCurrentThing(uc::CopiedChannel::SAMPLE, initiator);
}


void FmMain::copyCurrentLib()
{
    auto index = ui->treeLibrary->currentIndex();
    if (!index.isValid())
        return;
    if (uc::copyNode(libModel.nodeAt(index)))
        blinkCopied(ui->treeLibrary, nullptr);
}


void FmMain::forceShowCp(MaybeChar ch)
{
    ui->wiCharShowcase->set(ch.code, ui->vwInfo, model.match, model.glyphStyle.sets);

    // Block
    int iBlock = ui->comboBlock->currentIndex();
    auto block = uc::blockOf(ch.code);
    int newIBlock = block->cachedIndex;
    if (newIBlock != iBlock)
        ui->comboBlock->setCurrentIndex(newIBlock);
    ui->wiCollapse->setVisible(block->flags.have(uc::Bfg::COLLAPSIBLE));
}


void FmMain::showCp(MaybeChar ch)
{
    if (ch.code == ui->wiCharShowcase->shownObj())
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
    ui->wiLibShowcase->set(node, ui->vwLibInfo, model.match);
    if (node.value.empty()) {
        // Cps
        libCpWidgets[0]->removeCp();
        libCpWidgets[0]->show();
        for (unsigned i = 1; i < uc::LONGEST_LIB; ++i) {
            libCpWidgets[i]->hide();
        }
    } else {
        // Count independent chars
        size_t nIndependent = 0;
        for (auto c : node.value) {
            if (auto pCp = uc::cpsByCode[c]) {
                if (pCp->category().isIndependent())
                    ++nIndependent;
            }
        }
        // Actual node
        // CPs
        auto len = node.value.length();
        for (unsigned i = 0; i < len; ++i) {
            auto emojiDraw = uc::EmojiDraw::CONSERVATIVE;
            if (nIndependent > 1        // At least 2 independent
                    && i + 1 < len      // Next is VS16
                    && node.value[i+1] == cp::VS16) {
                emojiDraw = uc::EmojiDraw::GRAPHIC;
            }
            auto& wi = libCpWidgets[i];
            wi->setCp(node.value[i], emojiDraw, model.glyphStyle.sets);
            wi->show();
        }
        for (unsigned i = len; i < uc::LONGEST_LIB; ++i) {
            libCpWidgets[i]->hide();
        }
    }
}


void FmMain::linkClicked(
        mywiki::Gui& gui, std::string_view link, QWidget* widget,
        TinyOpt<QRect> rect)
{
    mywiki::go(widget, rect, gui, link);
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

    linkClicked(mainGui, str, snd, rect);
}


void FmMain::popupLinkActivated(QWidget* widget, const QString& link)
{
    // How to get focus rect from QLabel?
    linkClicked(popupGui, link.toStdString(), widget, TINY_NULL);
}


void FmMain::advancedLinkActivated(QWidget* widget, const QString& link)
{
    QRect rect;
    if (widget) {
        rect = widget->rect();
    } else {
        widget = this;
        rect = QRect(widget->rect().center(), QSize{1, 1});
    }
    mywiki::go(widget, rect, mainGui, link.toStdString());
    // Deselect, does not influence double and triple clicks
    if (auto label = qobject_cast<QLabel*>(widget)) {
        label->setSelection(0, 0);
    }
}


void FmMain::labelLinkActivated(const QString& link)
{
    advancedLinkActivated(
                qobject_cast<QWidget*>(sender()), link);
}


template<>
void FmMain::selectChar<SelectMode::NONE>(char32_t code)
{
    if (model.isCharCollapsed(code)) {
        cjkSetCollapseState(false);
    }
    if (auto cp = uc::cpsByCode[code]) {
        // Just get font, moving reference to nowhere
        (void)cp->font(match::Normal::INST);
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
    QString s = ui->edSearch->currentText();
    if (doSearch(s)) {
        ui->edSearch->addToHistory();
    }
}


void FmMain::showSearchError(const QString& text)
{
    mainGui.blinkAtWidget(text, ui->edSearch);
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


bool FmMain::doSearch(const QString& what)
{
    auto results = uc::doSearch(what);
    auto retValue = !results.v.empty();
    showSearchResult(std::move(results));
    return retValue;
}


void FmMain::focusSearch()
{
    if (searchModel.hasData())
        openSearch();
}


void FmMain::searchEnterPressed(int index)
{
    auto& line = searchModel.lineAt(index);
    if (line.cp) {
        goToCp(line.cp->subj);
    } else if (line.node) {
        goToNode(*line.node);
    } else {
        auto relRect = ui->listSearch->visualRect(searchModel.index(index, 0));
        mainGui.blinkAtRel(loc::get("Search.NoSuch"), ui->listSearch->viewport(), relRect);
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


namespace {

    void addPriority(
            pugi::xml_node node, std::u32string_view text, int priority,
            bool allowSingleChar)
    {
        char buf[80];
        EmojiPainter::getFileName(buf, text, {});
        if (allowSingleChar || strchr(buf, '-') != nullptr) {
            auto h = node.append_child("file");
            h.append_attribute("name") = buf;
            h.append_attribute("prio") = priority;
        }
    }

}   // anon namespace


void FmMain::dumpTiles()
{
    std::unordered_set<std::u32string_view> prio0;
    pugi::xml_document doc;
    auto hRoot = doc.root().append_child("opt");
    // Priority 0
    for (auto& v : uc::allLibNodes()) {
        auto tiles = getCharTiles(v);
        for (auto& tile : tiles) {
            if (tile.isEmoji(model.glyphStyle.sets)) {
                addPriority(hRoot, tile.text, 0, true);
                prio0.insert(tile.text);
            }
        }
    }
    // Other priority
    for (auto& v : uc::allLibNodes()) {
        if (!v.value.empty() && !prio0.contains(v.value)) {
            addPriority(hRoot, v.value, v.iParent, false);
        }
    }
    doc.save_file("opt.xml");
}


void FmMain::glyphStyleChanged(uc::EcGlyphStyleChannel channel, unsigned setting)
{
    if (channel != uc::EcGlyphStyleChannel::NONE)
        model.glyphStyle.sets[channel] = setting;

    emit model.dataChanged({}, {});
    ui->wiCharShowcase->redrawSampleChar(model.glyphStyle.sets);

    /// @todo [favs] do the same for favs
}


void FmMain::goToCp(char32_t cp)
{
    ui->tabsMain->setCurrentWidget(ui->tabBlocks);
    selectChar<SelectMode::INSTANT>(cp);
}


void FmMain::goToNode(const uc::LibNode& node)
{
    ui->tabsMain->setCurrentWidget(ui->tabLibrary);
    auto index = libModel.indexOf(node);
    ui->treeLibrary->setCurrentIndex(index);
    //selectChar<SelectMode::INSTANT>(cp);
}


void FmMain::setUpdating(bool value)
{
    ui->btCheckForUpdate->setEnabled(!value);
    ui->lbChecking->setVisible(value);
}


void FmMain::ensureNetMan()
{
    if (!netMan) {
        netMan.reset(new QNetworkAccessManager);
        connect(netMan.get(), &QNetworkAccessManager::finished, this, &This::updateFinished);
    }
}


void FmMain::startUpdate()
{
    ensureNetMan();
    QNetworkRequest rq(QUrl{URL_UPDATE});
    netMan->get(rq);
    setUpdating(true);
}


void FmMain::updateFinished(QNetworkReply* reply)
{
    if (reply) {
        int err = reply->error();
        // Debug
        if (updatever::version == github::VER_BAD_REQUEST)
            err = 418;  // I’m a teapot

        auto head = loc::get("Update.Head").q();
        char8_t buf[50], buf2[50];
        if (err == QNetworkReply::NoError) {
            auto bytes = reply->readAll();
            std::string_view sv(bytes.data(), bytes.length());
            auto res = github::checkPageForUpdate(
                        sv, updatever::version, updatever::equivPlatforms);
            switch (res.code) {
            case github::UpdateCode::BAD_DOCUMENT:
                QMessageBox::critical(this, head, loc::get("Update.Parse"));
                break;
            case github::UpdateCode::BAD_VERSION: {
                    auto qtext = QString::fromStdString(res.versionText).toHtmlEscaped().toStdString();
                    QMessageBox::critical(this, head,
                            loc::get("Update.BadVer").argQ(str::toU8sv(qtext)));
                } break;
            case github::UpdateCode::COINCIDE:
                QMessageBox::information(this, head,
                        loc::get("Update.Ok").argQ(updatever::version.toSv(buf)));
                break;
            case github::UpdateCode::FOUND_EARLIER:
                QMessageBox::warning(this, head,
                        loc::get("Update.Earlier").argQ(
                                    updatever::version.toSv(buf), res.version.toSv(buf2)));
                break;
            case github::UpdateCode::FOUND_LATER: {
                    QMessageBox msg(QMessageBox::Question, head,
                            loc::get("Update.Later").argQ(
                                updatever::version.toSv(buf), res.version.toSv(buf2)),
                            QMessageBox::Yes | QMessageBox::No, this);
                    msg.setButtonText(QMessageBox::Yes, loc::get("Update.Go"));
                    msg.setButtonText(QMessageBox::No,  loc::get("Update.Close"));
                    if (msg.exec() == QMessageBox::Yes) {
                        QDesktopServices::openUrl(QUrl{URL_REPO});
                    }
                } break;
            case github::UpdateCode::ABANDONED:
                QMessageBox::warning(this, head,
                        loc::get("Update.Abandon").argQ(
                                str::toU8sv(res.myPlatform)));
                break;
            }
        } else if (err < 100) {
            // System error
            QMessageBox::warning(this, head, loc::get("Update.Connect"));
        } else {
            // HTTP error
            QMessageBox::critical(this, head, loc::get("Update.Err").argQ(err));
        }
        reply->deleteLater();
    }
    setUpdating(false);
}
