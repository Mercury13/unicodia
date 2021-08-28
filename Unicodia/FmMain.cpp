// My header
#include "FmMain.h"
#include "ui_FmMain.h"

// C++
#include <iostream>
#include <cmath>
#include <bit>

// Qt
#include <QTableView>
#include <QTextFrame>
#include <QClipboard>
#include <QPainter>
#include <QShortcut>
#include <QPaintEngine>

// Misc
#include "u_Strings.h"
#include "u_Qstrings.h"

// Project-local
#include "Skin.h"
#include "Wiki.h"

// Forms
#include "FmPopup.h"

using namespace std::string_view_literals;

namespace {
    /// Alpha for space characters
    constexpr int ALPHA_SPACE = 60;

    enum class TableDraw { INTERNAL, CUSTOM };
    // No need custom drawing — solves nothing
    constexpr TableDraw TABLE_DRAW = TableDraw::INTERNAL;
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
            return str::toQ(block.locName);
            /// @todo [ui] how to show character ranges? — now they are bad
            //char buf[200];
            //snprintf(buf, 200, reinterpret_cast<const char*>(u8"%.*s (%04X—%04X)"),
            //         block.name.length(), block.name.data(),
            //         static_cast<int>(block.startingCp),
            //         static_cast<int>(block.endingCp));
        }
    case Qt::DecorationRole: {
            GET_BLOCK
            if (!block.icon) {
                char buf[48];
                snprintf(buf, std::size(buf), ":/Scripts/%04X.png", static_cast<int>(block.startingCp));
                block.icon.reset(new QPixmap());
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
    rows(NCOLS) {}


int CharsModel::rowCount(const QModelIndex&) const
{
    return rows.nRows();
}


int CharsModel::columnCount(const QModelIndex&) const
    { return NCOLS; }


const QFont* CharsModel::fontAt(const QModelIndex& index) const
{
    auto cp = charAt(index);
    if (!cp)
        return nullptr;
    return fontAt(*cp);
}


const QFont* CharsModel::fontAt(const uc::Cp& cp) const
{
    auto& font = cp.font(hint.cell);
    return &font.get(font.q.table, FSZ_TABLE);
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
    if (cp.isTrueSpace()) {
        auto c = owner->palette().text().color();
        c.setAlpha(ALPHA_SPACE);
        return c;
    }
    if (tcl != TableColors::NO) {
        if (isCjkCollapsed) {
            auto block = uc::blockOf(cp.subj, hint.cell);
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
    return textAt(*cp);
}


QString CharsModel::textAt(const uc::Cp& cp) const
{
    return cp.sampleProxy(hint.cell).text;
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
                    auto block = uc::blockOf(cp->subj, hint.cell);
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
    rows.clear();
    beginResetModel();
    const uc::Block* hint = &uc::blocks[0];
    for (auto& cp : uc::cpInfo) {
        if (isCjkCollapsed) {
            auto blk = uc::blockOf(cp.subj, hint);
            if (blk->flags.have(uc::Bfg::COLLAPSIBLE)
                    && static_cast<int>(cp.subj) - blk->startingCp >= NCOLS)
                continue;
        }
        addCp(cp);
    }
    endResetModel();
}

///// CharsDelegate ////////////////////////////////////////////////////////////


namespace {

    struct StrPair {
        std::u8string_view line1, line2;

        bool wasSplit() const { return !line2.empty(); }
    };

    StrPair splitAbbr(std::u8string_view abbr)
    {
        if (auto pSpace = abbr.find(' '); pSpace != std::u8string_view::npos)
            return { abbr.substr(0, pSpace), abbr.substr(pSpace + 1) };
        switch (abbr.size()) {
        case 4:
        case 6: {
                auto split = abbr.size() / 2;
                return { abbr.substr(0, split), abbr.substr(split) };
            }
        case 5: {
                auto split = 3;
                if (isdigit(abbr[2]) && isdigit(abbr[3]))
                    split = 2;
                return { abbr.substr(0, split), abbr.substr(split) };
            }
        default:
            return { abbr, {} };
        }
    }

    struct RcPair {
        QRectF rc1, rc2;

        RcPair(const QRectF& rcFrame) : rc1(rcFrame), rc2(rcFrame) {
            auto dh = rcFrame.height() / 2.5;
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

    void drawAbbreviation(
            QPainter* painter, const QRect& rect, std::u8string_view abbreviation,
            const QColor& color, char32_t subj)
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
        auto thickness = availSize / 60.0;
        painter->setPen(QPen(color, thickness, Qt::DashLine));
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(rcFrame);

        // Need this brush for both rects and fonts

        if (abbreviation == u8"`"sv) {
            // Draw special image
            switch (subj) {
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
            }
        } else {
            // Draw text
            auto sz = (abbreviation.size() == 1)
                    ? availSize / 2
                    : availSize / 3.3;
            QFont font { str::toQ(FAM_CONDENSED) };
                font.setStyleStrategy(QFont::PreferAntialias);
                font.setPointSizeF(sz);
            painter->setFont(font);
            painter->setBrush(QBrush(color, Qt::SolidPattern));
            rcFrame.setLeft(rcFrame.left() + std::max(thickness, 1.0));
            auto sp = splitAbbr(abbreviation);
            if (sp.wasSplit()) {
                RcPair p(rcFrame);
                painter->drawText(p.rc1, Qt::AlignCenter, str::toQ(sp.line1));
                painter->drawText(p.rc2, Qt::AlignCenter, str::toQ(sp.line2));
            } else {
                painter->drawText(rcFrame, Qt::AlignCenter, str::toQ(sp.line1));
            }
        }
    }

    void drawDeprecated(QPainter* painter, const QRect& r)
    {
        static constexpr int SZ = 8;    // we draw lines between pixel centers, actually +1
        static constexpr int OFS = 4;
        const int x1 = r.right() - OFS;
        const int x0 = x1 - SZ;
        const int y0 = r.top() + OFS;
        const int y1 = y0 + SZ;
        painter->setPen(TX_DEPRECATED);
        painter->drawLine(x0, y0, x1, y1);
        painter->drawLine(x0, y1, x1, y0);
    }

}   // anon namespace


void FmMain::CharsDelegate::tryDrawCustom(QPainter* painter, const QRect& rect,
            const QModelIndex& index, const QColor& color) const
{
    auto& model = owner.model;
    auto ch = model.charAt(index);
    if (ch) {
        auto abbr = ch->abbrev();
        if (!abbr.empty()) {
            // Abbreviation
            drawAbbreviation(painter, rect, abbr, color, ch->subj);
        }
        else if constexpr (TABLE_DRAW == TableDraw::CUSTOM) {
            // Char
            painter->setFont(*model.fontAt(*ch));
            auto specialColor = model.fgAt(*ch, TableColors::YES);
            painter->setBrush(specialColor.isValid() ? specialColor : color);
            painter->drawText(rect,
                              Qt::AlignCenter | Qt::TextSingleLine,
                              model.textAt(*ch));
        }
        if (ch->isDeprecated())
            drawDeprecated(painter, rect);
    }
}


void FmMain::CharsDelegate::initStyleOption(QStyleOptionViewItem *option,
                     const QModelIndex &index) const
{
    Super::initStyleOption(option, index);
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


void FmMain::CharsDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
           const QModelIndex &index) const
{
    if (option.state.testFlag(QStyle::State_HasFocus)) {
        // It’d be nice to draw some nice focus using Windows skin, but cannot
        //Super::paint(painter, option, index);
        // Draw it as a button
        QStyleOptionButton sob;
            sob.state = QStyle::State_HasFocus | QStyle::State_MouseOver | QStyle::State_Selected
                        | QStyle::State_Active | QStyle::State_Enabled;
            sob.rect = option.rect;
        owner.style()->drawControl(QStyle::CE_PushButton, &sob, painter, option.widget);
        Super::paint(painter, option, index);
        tryDrawCustom(painter, option.rect, index, owner.palette().buttonText().color());
    } else if (option.state.testFlag(QStyle::State_Selected)) {
        // Selected, not focused? Initial style is bad
        auto opt2 = option;
        opt2.state.setFlag(QStyle::State_Selected, false);
        owner.style()->drawPrimitive(QStyle::PE_FrameMenu, &opt2, painter, option.widget);
        Super::paint(painter, option, index);
        tryDrawCustom(painter, option.rect, index, owner.palette().windowText().color());
    } else {
        Super::paint(painter, option, index);
        tryDrawCustom(painter, option.rect, index, owner.palette().windowText().color());
    }
}


///// WiCustomDraw /////////////////////////////////////////////////////////////


void WiCustomDraw::paintEvent(QPaintEvent *event)
{
    Super::paintEvent(event);
    switch (mode) {
    case Mode::NONE: break;
    case Mode::ABBREVIATION: {
            QPainter painter(this);
            drawAbbreviation(&painter, geometry(), abbreviation,
                             palette().windowText().color(),
                             subj);
        }
    }
}


void WiCustomDraw::setAbbreviation(std::u8string_view x, char32_t aSubj)
{
    mode = Mode::ABBREVIATION;
    abbreviation = x;
    subj = aSubj;
    update();
}


///// FmMain ///////////////////////////////////////////////////////////////////

FmMain::FmMain(QWidget *parent)
    : Super(parent),
      ui(new Ui::FmMain), model(this),
      fontBig(str::toQ(FAM_DEFAULT), FSZ_BIG),
      charsDelegate(*this)
{
    ui->setupUi(this);

    // Collapse bar
    ui->wiCollapse->hide();
    ui->wiCollapse->setStyleSheet(
                "#wiCollapse { background-color: " + BG_CJK.name() + "; }"   );
    connect(ui->btCollapse, &QPushButton::clicked,
            this, &This::cjkExpandCollapse);
    reflectCjkCollapseState();

    // Top bar
    QPalette pal = ui->wiCharBar->palette();
    const QColor& color = pal.color(QPalette::Normal, QPalette::Button);
    ui->wiCharBar->setStyleSheet("#wiCharBar { background-color: " + color.name() + " }");

    // Fill chars
    model.build();

    // Combobox
    ui->comboBlock->setModel(&blocksModel);

    // Table
    ui->tableChars->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableChars->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableChars->setItemDelegate(&charsDelegate);
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

    // OS style
    auto& font = uc::fontInfo[0];
    ui->lbOs->setFont(font.get(font.q.big, FSZ_BIG));

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

    // Copy sample
    shcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_C), ui->tableChars,
                nullptr, nullptr, Qt::WidgetWithChildrenShortcut);
    connect(shcut, &QShortcut::activated, this, &This::copyCurrentSample);
    shcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_Insert), ui->tableChars,
                nullptr, nullptr, Qt::WidgetWithChildrenShortcut);
    connect(shcut, &QShortcut::activated, this, &This::copyCurrentSample);

    // Clicked
    connect(ui->vwInfo, &QTextBrowser::anchorClicked, this, &This::anchorClicked);

    // Terms
    initTerms();

    // Select index
    ui->tableChars->setFocus();
    ui->tableChars->selectionModel()->select(model.index(0, 0), QItemSelectionModel::SelectCurrent);
}


void FmMain::initTerms()
{
    QString text;

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
        str::append(text, "'" SUBTAG_POPUP "><b>");
        str::append(text, term.locName);
        str::append(text, "</b></a>");
        if (!term.engName.empty()) {
            str::append(text, "&nbsp;/ ");
            str::append(text, term.engName);
        }
    }

    ui->vwTerms->setText(text);
    connect(ui->vwTerms, &QTextBrowser::anchorClicked, this, &This::anchorClicked);
}


bool FmMain::eventFilter(QObject *obj, QEvent *event)
{
    switch (event->type()) {
    case QEvent::MouseButtonDblClick:
        if (obj == ui->tableChars->viewport()) {
            copyCurrentChar();
            return true;
        }
    default:;
    }
    return Super::eventFilter(obj, event);
}


FmMain::~FmMain()
{
    delete ui;
}


void FmMain::copyCurrentChar()
{
    auto ch = model.charAt(ui->tableChars->currentIndex());
    if (ch) {
        auto q = str::toQ(ch->subj);
        QApplication::clipboard()->setText(q);
    }
}


void FmMain::copyCurrentSample()
{
    auto ch = model.charAt(ui->tableChars->currentIndex());
    if (ch) {
        auto q = model.textAt(*ch);
        QApplication::clipboard()->setText(q);
    }
}


void FmMain::drawSampleWithQt(const uc::Cp& ch)
{
    // Font
    auto& font = ch.font(hint.sample);
    ui->lbSample->setFont(font.get(font.q.big, FSZ_BIG));

    // Sample char
    ui->stackSample->setCurrentWidget(ui->pageSampleQt);
    auto proxy = ch.sampleProxy(hint.sample);
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

namespace {

    class Eng : public wiki::Engine
    {
    public:
        QString& s;
        const uc::Font& font;

        Eng(QString& aS, const uc::Font& aFont) : s(aS), font(aFont) {}
        void appendPlain(std::string_view x) override;
        void appendLink(
                const SafeVector<std::string_view> x,
                bool hasRemainder) override;
        void appendTemplate(
                const SafeVector<std::string_view> x,
                bool hasRemainder) override;
        void toggleWeight(Flags<wiki::Weight> changed) override;
        void finish() override;
    protected:
        wiki::HtWeight weight;
        bool isSuppressed = false;

        [[nodiscard]] bool prepareRecursion(std::string_view text);
        void runRecursive(std::string_view text);
        void finishRecursion(bool hasRemainder, bool prepareResult);
    };

    void Eng::toggleWeight(Flags<wiki::Weight> changed)
    {
        auto tag = weight.toggle(changed);
        str::append(s, tag);
    }

    void Eng::finish()
    {
        if (!isSuppressed) {
            auto tag = weight.finish();
            str::append(s, tag);
        }
    }

    void Eng::appendPlain(std::string_view x)
    {
        str::append(s, x);
    }

    bool Eng::prepareRecursion(std::string_view text)
    {
        if (isSuppressed)
            return true;
        if (weight.flags() & wiki::probeWeights(text)) {
            auto tag = weight.finish();
            str::append(s, tag);
        } else {
            isSuppressed = true;
        }
        return false;
    }

    void Eng::runRecursive(std::string_view text)
    {
        if (!isSuppressed) {
            auto tag = weight.restart();
            str::append(s, tag);
        }
        wiki::run(*this, text);
    }

    void Eng::finishRecursion(bool hasRemainder, bool prepareResult)
    {
        if (!isSuppressed) {
            if (hasRemainder) {
                auto tag = weight.restart();
                str::append(s, tag);
            } else {
                weight.clear();
            }
        }
        isSuppressed = prepareResult;
    }

    void Eng::appendLink(const SafeVector<std::string_view> x, bool hasRemainder)
    {
        auto dest = x[0];
        auto text = x[1];
        std::string_view style;
        if (dest.size() >= 4 && dest[0] == 'p' && dest[2] == ':')
            style = SUBTAG_POPUP;

        auto q = prepareRecursion(text);

        s.append("<a");
        str::append(s, style);
        s.append(" href='");
        str::append(s, dest);
        s.append("'>");

        runRecursive(text);

        s.append("</a>");

        finishRecursion(hasRemainder, q);
    }

    void Eng::appendTemplate(const SafeVector<std::string_view> x, bool)
    {
        auto name = x[0];
        if (name == "sm"sv) {
            bool useCustom = !font.flags.have(uc::Ffg::DESC_STD);
            if (useCustom)
                font.load();
            auto fontSize =
                    font.flags.have(uc::Ffg::DESC_BIGGER) ? '3'
                  : font.flags.have(uc::Ffg::DESC_SMALLER) ? '1' : '2';
            str::append(s, "<font size='+"sv);
            str::append(s, fontSize);
            str::append(s, '\'');
            if (useCustom) {
                str::append(s, " face='"sv);
                str::append(s, font.family);
                str::append(s, '\'');
            }
            str::append(s, '>');
            str::append(s, x.safeGetV(1, {}));
            str::append(s, "</font>");
        } else if (name == "_"sv) {
            s.append(QChar(0x00A0));
        } else if (name == "-"sv) {
            str::append(s, "<span style='font-size:4pt'>\u00A0</span>—<span style='font-size:4pt'> </span>"sv);
        } else if (name == "%"sv) {
            str::append(s, x.safeGetV(1, {}));
            str::append(s, "<span style='font-size:3pt'>\u00A0</span>%"sv);
        } else if (name == "em"sv) {
            str::append(s, "<font size='+2' face='Segoe UI Emoji,Noto Sans Symbols,Noto Sans Symbols2'>"sv);
            str::append(s, x.safeGetV(1, {}));
            str::append(s, "</font>");
        } else if (name == "№"sv) {
            str::append(s, u8"№<span style='font-size:3pt'>\u00A0</span>"sv);
            str::append(s, x.safeGetV(1, {}));
        } else {
            wiki::appendHtml(s, x[0]);
        }
    }

    using StrCache = char[300];

    template <class T>
    std::string_view idOf(const T& value, StrCache&) { return value.id; }

    template <>
    std::string_view idOf(const uc::Block& value, StrCache& cache)
    {
        auto beg = std::begin(cache);
        auto r = std::to_chars(beg, std::end(cache), value.index());
        return { beg, r.ptr };
    }

    struct FontLink {
        QString family;
        char32_t cp;
        QFontDatabase::WritingSystem ws;
    };

    template <>
    std::string_view idOf(const FontLink& value, StrCache& cache)
    {
        snprintf(cache, std::size(cache), "%d/%d", (int)value.cp, (int)value.ws);
        return cache;
    }

    template <class T>
    inline void appendVal(QString& text, const T& value)
        { str::append(text, value.locName); }

    template<>
    inline void appendVal(QString& text, const uc::BidiClass& value)
        { str::append(text, value.locShortName); }

    template<>
    inline void appendVal(QString& text, const FontLink& value)
        { text += value.family.toHtmlEscaped(); }

    template <class T, class Name1>
    inline void appendValuePopup(
            QString& text, const T& value, Name1 name, const char* scheme)
    {
        StrCache cache, buf;
        str::append(text, name);
        auto vid = idOf(value, cache);
        snprintf(buf, std::size(buf),
                 ": <a href='%s:%.*s'" SUBTAG_POPUP ">",
                scheme, int(vid.size()), vid.data());
        str::append(text, buf);
        appendVal(text, value);
        str::append(text, "</a>");
    }

    void appendVersion(
            QString& text, const uc::Version& version)
    {
        str::append(text, version.name);
        str::append(text, " (");
        str::append(text, version.year);
        str::append(text, ")");
    }


    void appendVersion(
            QString& text, std::u8string_view caption, const uc::Version& version)
    {
        str::append(text, caption);
        str::append(text, u8"Версия Юникода: ");
        appendVersion(text, version);
    }

    void appendUtf(QString& text, str::QSep& sp, char32_t code)
    {
        char buf[30];

        // UTF-8
        sp.sep();
        auto sChar = str::toQ(code);
        str::append(text, u8"<a href='pt:utf8'" SUBTAG_POPUP ">UTF-8</a>:");
        auto u8 = sChar.toUtf8();
        for (unsigned char v : u8) {
            snprintf(buf, 10, " %02X", static_cast<int>(v));
            str::append(text, buf);
        }

        // UTF-16: QString us UTF-16
        sp.sep();
        str::append(text, u8"<a href='pt:utf16'" SUBTAG_POPUP ">UTF-16</a>:");
        for (auto v : sChar) {
            snprintf(buf, std::size(buf), " %04X", static_cast<int>(v.unicode()));
            str::append(text, buf);
        }
    }

    void appendMissingCharInfo(QString& text, char32_t code)
    {
        str::append(text, "<p>");
        str::QSep sp(text, "<br>");
        appendUtf(text, sp, code);
    }

    template <class X>
    void appendWiki(QString& text, const X& obj, std::u8string_view x)
    {
        Eng eng(text, obj.font());
        wiki::run(eng, x);
    }

    void appendWikiNoFont(QString& text, std::u8string_view x)
    {
        Eng eng(text, uc::fontInfo[0]);
        wiki::run(eng, x);
    }

    void appendScript(QString& text, const uc::Script& x, bool isScript)
    {
        if (x.ecType != uc::EcScriptType::NONE) {
            str::append(text, "<p>");
            str::QSep sp(text, "<br>");
            str::append(text, u8"• Тип: ");
            appendWiki(text, x, x.type().locName);
            if (x.ecDir != uc::EcWritingDir::NOMATTER) {
                sp.sep();
                str::append(text, u8"• Направление: ");
                str::append(text, x.dir().locName);
            }
            if (!x.locLangs.empty()) {
                sp.sep();
                str::append(text, u8"• Языки: ");
                str::append(text, x.locLangs);
            }
            if (!x.locTime.empty()) {
                sp.sep();
                str::append(text, u8"• Появилась: ");
                str::append(text, x.locTime);
            }
            if (x.ecLife != uc::EcLangLife::NOMATTER) {
                sp.sep();
                str::append(text, u8"• Состояние: ");
                str::append(text, x.life().locName);
            }
            if (isScript) {
                if (x.ecVersion != uc::EcVersion::UNKNOWN) {
                    sp.sep();
                    appendVersion(text, u8"• "sv, x.version());
                }

                sp.sep();
                str::append(text, u8"• Плоскость: ");
                if (x.plane == uc::PLANE_BASE) {
                    str::append(text, u8"базовая");
                } else {
                    str::append(text, std::to_string(x.plane));
                }
            }

            str::append(text, "</p>");
        }

        str::append(text, "<p>");
        appendWiki(text, x, x.locDescription);
        str::append(text, "</p>");
    }

}   // anon namespace


void FmMain::showCp(MaybeChar ch)
{
    if (ch.code == shownCp)
        return;
    shownCp = ch.code;

    // Code
    char buf[30];
    snprintf(buf, std::size(buf), "U+%04X", static_cast<unsigned>(ch.code));
    ui->lbCharCode->setText(buf);

    // Block
    int iBlock = ui->comboBlock->currentIndex();
    auto block = uc::blockOf(ch.code, iBlock);
    int newIBlock = block->index();
    if (newIBlock != iBlock)
        ui->comboBlock->setCurrentIndex(newIBlock);
    ui->wiCollapse->setVisible(block->flags.have(uc::Bfg::COLLAPSIBLE));

    // Copy
    ui->btCopy->setEnabled(ch.hasCp());

    hint.sample = uc::blockOf(ch.code, hint.sample);
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

        // Sample char
        bool wantSysFont = true;
        if (ch->isAbbreviated()) {
            ui->stackSample->setCurrentWidget(ui->pageSampleCustom);
            ui->pageSampleCustom->setAbbreviation(ch->abbrev(), ch.code);
            wantSysFont = false;
        } else {
            drawSampleWithQt(*ch);
        }

        // OS char
        std::optional<QFont> font = std::nullopt;
        QFontDatabase::WritingSystem ws = QFontDatabase::Any;
        if (wantSysFont) {
            QString osProxy = ch->osProxy();
            if (osProxy.isEmpty()) {
                ui->lbOs->setFont(fontBig);
                ui->lbOs->setText({});
            } else {
                ws = ch->scriptEx(hint.sample).qtCounterpart;
                font = model.match.sysFontFor(*ch, ws, FSZ_BIG);
                if (font) {
                    ui->lbOs->setFont(*font);
                    ui->lbOs->setText(osProxy);
                } else {
                    ui->lbOs->setFont(fontBig);
                    ui->lbOs->setText("?");
                }
            }
        } else {
            ui->lbOs->setFont(fontBig);
            ui->lbOs->setText({});
        }

        // Text
        // Header
        QString text;
        str::append(text, "<h1>");
        str::append(text, ch->name.tech());
        str::append(text, "</h1>");

        // Deprecated
        if (ch->isDeprecated()) {
            str::append(text, u8"<h3><a href='pt:deprecated'" SUBTAG_DEPRECATED ">Запрещённый символ</a></h3>"sv);
        }

        {   // Info box
            str::append(text, "<p>");
            str::QSep sp(text, "<br>");

            // Script
            sp.sep();
            auto& scr = ch->script();
            appendValuePopup(text, scr, u8"Письменность", "ps");

            // Unicode version
            sp.sep();
            appendVersion(text, {}, ch->version());

            // Character type
            sp.sep();
            appendValuePopup(text, ch->category(), u8"Тип", "pc");

            // Numeric value
            auto& numc = ch->numeric();
            if (numc.isPresent()) {
                sp.sep();
                str::append(text, "<a href='pt:number'" SUBTAG_POPUP ">");
                str::append(text, numc.type().locName);
                str::append(text, "</a>");
                str::append(text, ": ");
                str::append(text, numc.num);
                if (numc.denom != 1) {
                    str::append(text, "/");
                    str::append(text, numc.denom);
                }
            }

            // Bidi writing
            sp.sep();
            appendValuePopup(text, ch.cp->bidiClass(), u8"В двунаправленном письме", "pb");

            // Block
            sp.sep();
            appendValuePopup(text, *hint.sample, u8"Блок", "pk");

            auto comps = uc::cpOldComps(ch.code);
            if (comps) {
                sp.sep();
                str::append(text, u8"Компьютеры: ");
                str::QSep spC(text, ", ");
                while (comps) {
                    // Extract and remove bit
                    auto bit = comps.smallest();
                    comps.remove(bit);
                    // Turn bit to index
                    auto iBit = std::countr_zero(static_cast<unsigned>(bit));
                    // Write what we got
                    spC.sep();
                    str::append(text, uc::oldCompNames[iBit]);
                }
            }

            // Font
            if (font) {
                sp.sep();
                FontLink lnk { font->family(), ch.code, ws };
                appendValuePopup(text, lnk, u8"Системный шрифт", "pf");
            }

            // HTML
            sp.sep();
            str::append(text, u8"HTML: ");
            snprintf(buf, std::size(buf), "&amp;#%d;", static_cast<int>(ch->subj));
            str::append(text, buf);

            appendUtf(text, sp, ch.code);

            text.append("</p>");

            if (ch->ecCategory == uc::EcCategory::CONTROL) {
                //  Control char description
                str::append(text, u8"<h2>Об управляющих символах</h2>");
                appendWiki(text, *hint.sample,
                           uc::categoryInfo[static_cast<int>(uc::EcCategory::CONTROL)].locDescription);
            } else if (!hint.sample->locDescription.empty()) {
                // Block description
                str::append(text, u8"<h2>О блоке</h2>");
                appendWiki(text, *hint.sample, hint.sample->locDescription);
            } else if (auto sc = ch->scriptEx(hint.sample); &sc != uc::scriptInfo){
                // Script description
                str::append(text, u8"<h2>О письменности</h2>");
                appendScript(text, sc, false);
            }
        }
        //std::cout << text.toStdString() << std::endl;
        ui->vwInfo->setText(text);
    } else {
        // No character
        ui->stackSample->setCurrentWidget(ui->pageSampleQt);
        ui->lbSample->setText({});
        ui->lbOs->setText({});
        if (uc::isNonChar(ch.code)) {
            QString text;
            str::append(text, u8"<h1>Зарезервирован как отсутствующий</h1>"sv);
            appendMissingCharInfo(text, ch.code);
            appendWiki(text, *hint.sample, uc::TX_NOCHAR);
            ui->vwInfo->setText(text);
        } else {
            auto color = palette().color(QPalette::Disabled, QPalette::WindowText);
            QString text;
            text = "<h1 style='color:" + color.name() + "'>Отсутствует в Юникоде</h1>";
            appendMissingCharInfo(text, ch.code);
            ui->vwInfo->setText(text);
        }
    }
}


void FmMain::charChanged(const QModelIndex& current)
{
    showCp(model.charAt(current));
}


namespace {
    inline void appendHeader(QString& text, std::string_view x)
    {
        str::append(text, "<p><nobr><b>");
        str::append(text, x);
        str::append(text, "</b></nobr></p>");
    }

    template <int N>
    inline void appendHeader(QString& text, char (&x)[N])
    {
        size_t len = strnlen(x, N);
        appendHeader(text, std::string_view{ x, len });
    }

    template <class T>
    inline void appendHeader(QString& text, const T& x)
    {
        str::append(text, "<p><nobr><b>");
        str::append(text, x.locName);
        str::append(text, "</b> ("sv);
        str::append(text, x.nChars);
        str::append(text, " шт.)</nobr></p>");
    }

}   // anon namespace


void FmMain::popupText(const QString& text, QWidget* widget, TinyOpt<QRect> rect)
{
    ensure(popup, this)
          .setText(text)
          .popup(widget, rect);
}


template <class T>
void FmMain::showPopupT(const T& x, QWidget* widget, TinyOpt<QRect> rect)
{
    QString text;
    appendHeader(text, x);
    str::append(text, "<p>");
    appendWikiNoFont(text, x.locDescription);
    str::append(text, "</p>");
    popupText(text, widget, rect);
}


void FmMain::showPopup(
        const uc::BidiClass& x, QWidget* widget, TinyOpt<QRect> rect)
{
    QString text;
    appendHeader(text, x);

    str::append(text, "<p>");
    str::QSep sp(text, "<br>");

    sp.sep();
    str::append(text, u8"• В техдокументации: ");
    str::append(text, x.locId);

    str::append(text, "</p>");

    str::append(text, "<p>");
    appendWikiNoFont(text, x.locDescription);
    str::append(text, "</p>");
    popupText(text, widget, rect);
}


void FmMain::showPopup(
        const uc::Category& x, QWidget* widget, TinyOpt<QRect> rect)
{
    showPopupT(x, widget, rect);
}


void FmMain::showPopup(
        const uc::Script& x, QWidget* widget, TinyOpt<QRect> rect)
{
    QString text;
    appendHeader(text, x);
    appendScript(text, x, true);

    popupText(text, widget, rect);
}


namespace {
    const char8_t* STR_RANGE = u8"%04X…%04X";

    template <class T>
    inline auto fromChars(std::string_view s, T& v)
        { return std::from_chars(s.data(), s.data() + s.size(), v); }
}


void FmMain::showPopup(const uc::Block& x, QWidget* widget, TinyOpt<QRect> rect)
{
    QString text;
    appendHeader(text, x);

    str::append(text, "<p>");
    str::QSep sp(text, "<br>");

    sp.sep();
    str::append(text, u8"• Английское: ");
    str::append(text, x.name);

    sp.sep();
    str::append(text, u8"• Диапазон: ");
    char buf[30];
    snprintf(buf, std::size(buf), reinterpret_cast<const char*>(STR_RANGE),
                int(x.startingCp), int(x.endingCp));
    str::append(text, buf);

    sp.sep();
    str::append(text, u8"• Появился в версии: ");
    appendVersion(text, x.version());

    if (!x.locDescription.empty()) {
        str::append(text, "<p>");
        appendWiki(text, x, x.locDescription);
        str::append(text, "</p>");
    } else if (x.ecScript != uc::EcScript::NONE) {
        str::append(text, "<p><b>О письменности</b></p>"sv);
        appendScript(text, x.script(), false);
    }
    popupText(text, widget, rect);
}


void FmMain::showPopup(const uc::Term& x, QWidget* widget, TinyOpt<QRect> rect)
{
    QString text;
    str::append(text, "<p><b>");
    str::append(text, x.locName);
    str::append(text, "</b>"sv);
    if (!x.engName.empty()) {
        str::append(text, u8"\u00A0/ "sv);
        str::append(text, x.engName);
    }

    str::append(text, "<p>");
    appendWiki(text, x, x.locDesc);
    popupText(text, widget, rect);
}


void FmMain::showFonts(
        char32_t cp, QFontDatabase::WritingSystem ws,
        QWidget* widget, TinyOpt<QRect> rect)
{
    if (cp >= uc::N_CHARS || ws >= QFontDatabase::WritingSystemsCount)
        return;
    QString text;
    char buf[50];
    auto format = u8"Шрифты для U+%04X";
    snprintf(buf, std::size(buf), reinterpret_cast<const char*>(format), (int)cp);
    appendHeader(text, buf);

    auto fonts = model.match.allSysFonts(cp, ws, 20);
    str::append(text, "<p>");
    str::QSep sp(text, "<br>");
    for (auto& v : fonts.lines) {
        sp.sep();
        str::append(text, u8"•\u00A0");
        text += v.name;
    }
    if (fonts.hasMore) {
        sp.sep();
        str::append(text, u8"•\u00A0…");
    }

    popupText(text, widget, rect);
}


void FmMain::linkClicked(std::string_view scheme, std::string_view target,
                 QWidget* widget, TinyOpt<QRect> rect)
{
    if (scheme == "pb"sv) {
        if (auto* bidi = uc::findBidiClass(target))
            showPopup(*bidi, widget, rect);
    } else if (scheme == "pc"sv) {
        if (auto* cat = uc::findCategory(target))
            showPopup(*cat, widget, rect);
    } else if (scheme == "ps"sv) {
        if (auto* script = uc::findScript(target))
            showPopup(*script, widget, rect);
    } else if (scheme == "pk"sv) {
        int iBlock = 0;
        fromChars(target, iBlock);
        showPopup(uc::blocks[iBlock], widget, rect);
    } else if (scheme == "pf"sv) {
        auto sv = str::splitSv(target, '/');
        if (sv.size() >= 2) {
            unsigned int cp = 0, ws = 0;
            fromChars(sv[0], cp);
            fromChars(sv[1], ws);
            showFonts(cp, static_cast<QFontDatabase::WritingSystem>(ws), widget, rect);
        }
    } else if (scheme == "pt"sv) {
        if (auto* term = uc::findTerm(target))
            showPopup(*term, widget, rect);
    }
}


void FmMain::linkClicked(
        std::string_view link, QWidget* widget, TinyOpt<QRect> rect)
{
    auto things = str::splitSv(link, ':');
    if (things.size() >= 2) {
        linkClicked(things[0], things[1], widget, rect);
    }
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


void FmMain::on_comboBlock_currentIndexChanged(int index)
{
    if (index < 0)
        return;
    auto oldChar = model.charAt(ui->tableChars->currentIndex());
    auto oldBlock = uc::blockOf(oldChar.code, index);
    if (oldBlock->index() != static_cast<size_t>(index)) {
        auto& newBlock = uc::blocks[index];
        selectChar(newBlock.firstAllocated->subj);
        ui->tableChars->setFocus();
    }
}


void FmMain::selectChar(char32_t code)
{
    /// @todo [urgent] collapsed — what to do?
    ui->tableChars->setCurrentIndex(model.indexOf(code));
}


void FmMain::reflectCjkCollapseState()
{
    if (model.isCjkCollapsed) {
        ui->lbCollapse->setText(str::toQ(u8"ККЯ свёрнуты (кроме слоговых азбук и маленьких блоков)."sv));
        ui->btCollapse->setText(str::toQ(u8"Развернуть"sv));
    } else {
        ui->lbCollapse->setText(str::toQ(u8"ККЯ развёрнуты."sv));
        ui->btCollapse->setText(str::toQ(u8"Свернуть"sv));
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
    model.isCjkCollapsed = !model.isCjkCollapsed;
    model.build();

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
    reflectCjkCollapseState();
}
