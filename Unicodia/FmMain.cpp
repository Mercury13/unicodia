// My header
#include "FmMain.h"
#include "ui_FmMain.h"

// C++
#include <iostream>
#include <cmath>

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
    constexpr int ALPHA_SPACE = 60;
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
    switch (role) {
    case Qt::DisplayRole: {
            size_t i = index.row();
            if (i >= uc::N_BLOCKS)
                return {};
            auto& block = uc::blocks[i];
            return str::toQ(block.locName);
            /// @todo [ui] how to show character ranges? — now they are bad
            //char buf[200];
            //snprintf(buf, 200, reinterpret_cast<const char*>(u8"%.*s (%04X—%04X)"),
            //         block.name.length(), block.name.data(),
            //         static_cast<int>(block.startingCp),
            //         static_cast<int>(block.endingCp));
        }
    default:
        return {};
    }
}


///// CharsModel ///////////////////////////////////////////////////////////////


CharsModel::CharsModel(QWidget* aOwner) :
    owner(aOwner),
    match(str::toQ(FAMILY_DEFAULT)),
    rows(NCOLS) {}


int CharsModel::rowCount(const QModelIndex&) const
{
    return rows.nRows();
}


int CharsModel::columnCount(const QModelIndex&) const
    { return NCOLS; }


QVariant CharsModel::data(const QModelIndex& index, int role) const
{
    switch (role) {
    case Qt::DisplayRole: {
            auto cp = charAt(index);
            if (!cp)
                return {};
            return cp->sampleProxy(hint).text;
        }

    case Qt::FontRole: {
            auto cp = charAt(index);
            if (!cp)
                return {};
            auto& font = cp->font(hint);
            return font.get(font.q.table, FSZ_TABLE);
        }

    case Qt::ForegroundRole: {
            auto cp = charAt(index);
            if (!cp)
                return {};
            if (cp->isTrueSpace()) {
                auto c = owner->palette().text().color();
                c.setAlpha(ALPHA_SPACE);
                return c;
            }
            return {};
        }

    case Qt::BackgroundRole: {
            auto cp = charAt(index);
            if (!cp) {
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


///// CharsDelegate ////////////////////////////////////////////////////////////


namespace {

    struct RcPair {
        QRectF rc1, rc2;

        RcPair(const QRectF& rcFrame) : rc1(rcFrame), rc2(rcFrame) {
            auto dh = rcFrame.height() / 2.5;
            rc1.setBottom(rc1.bottom() - dh);
            rc2.setTop(rc2.top() + dh);
        }
    };

    /// @todo [urgent] draw abbreviation
    void drawAbbreviation(
            QPainter* painter, const QRect& rect, std::u8string_view abbreviation,
            const QColor& color)
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
//        if (thickness < 1) {
//            QColor clAlpha = color;
//            clAlpha.setAlphaF(thickness);
//            painter->setPen(QPen(color, 1, Qt::DashLine));
//        } else {
        painter->setPen(QPen(color, thickness, Qt::DashLine));
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(rcFrame);
        // Draw text
        auto sz = availSize / 3.3;
        QFont font { str::toQ(FAMILY_CONDENSED) };
            font.setStyleStrategy(QFont::PreferAntialias);
            font.setPointSizeF(sz);
        painter->setFont(font);
        painter->setBrush(QBrush(color, Qt::SolidPattern));
        rcFrame.setLeft(rcFrame.left() + std::max(thickness, 1.0));
        switch (abbreviation.size()) {
        case 4:
        case 6: {
                RcPair p(rcFrame);
                auto split = abbreviation.size() / 2;
                painter->drawText(p.rc1,
                                  Qt::AlignCenter,
                                  str::toQ(abbreviation.substr(0, split)));
                painter->drawText(p.rc2,
                                  Qt::AlignCenter,
                                  str::toQ(abbreviation.substr(split)));
            } break;
        case 5: {
            RcPair p(rcFrame);
                auto split = 3;
                if (isdigit(abbreviation[2]) && isdigit(abbreviation[3]))
                    split = 2;
                painter->drawText(p.rc1,
                                  Qt::AlignCenter,
                                  str::toQ(abbreviation.substr(0, split)));
                painter->drawText(p.rc2,
                                  Qt::AlignCenter,
                                  str::toQ(abbreviation.substr(split)));
            } break;
        default:
            painter->drawText(rcFrame,
                              Qt::AlignCenter,
                              str::toQ(abbreviation));
            break;
        }
    }

}   // anon namespace


void FmMain::CharsDelegate::tryDrawCustom(QPainter* painter, const QRect& rect,
            const QModelIndex& index, const QColor& color) const
{
    auto ch = owner.model.charAt(index);
    if (ch) {
        auto abbr = ch->name.abbrev();
        if (!abbr.empty()) {
            drawAbbreviation(painter, rect, abbr, color);
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
        // Draw the rest as usual
        auto opt2 = option;
        opt2.state.setFlag(QStyle::State_Selected, false);
        opt2.state.setFlag(QStyle::State_HasFocus, false);
        Super::paint(painter, opt2, index);
        tryDrawCustom(painter, option.rect, index, owner.palette().buttonText().color());
    } else if (option.state.testFlag(QStyle::State_Selected)) {
        // Selected, not focused? Initial style is bad
        auto opt2 = option;
        opt2.state.setFlag(QStyle::State_Selected, false);
        owner.style()->drawPrimitive(QStyle::PE_FrameMenu, &opt2, painter, option.widget);
        Super::paint(painter, opt2, index);
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
                             palette().windowText().color());
        }
    }
}


void WiCustomDraw::setAbbreviation(std::u8string_view x)
{
    mode = Mode::ABBREVIATION;
    abbreviation = x;
    update();
}


///// FmMain ///////////////////////////////////////////////////////////////////

FmMain::FmMain(QWidget *parent)
    : Super(parent),
      ui(new Ui::FmMain), model(this),
      fontBig(str::toQ(FAMILY_DEFAULT), FSZ_BIG),
      charsDelegate(*this)
{
    ui->setupUi(this);

    // Top bar
    QPalette pal = ui->wiCharBar->palette();
    const QColor& color = pal.color(QPalette::Normal, QPalette::Button);
    ui->wiCharBar->setStyleSheet("#wiCharBar { background-color: " + color.name() + " }");

    // Fill chars
    for (auto& cp : uc::cpInfo) {
        model.addCp(cp);
    }

    // Combobox
    ui->comboBlock->setModel(&blocksModel);

    // Table
    ui->tableChars->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableChars->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableChars->setItemDelegate(&charsDelegate);
    ui->tableChars->setModel(&model);

    // Divider
    auto w = width();
    QList<int> sizes { w * 50 / 100, w * 50 / 100 };
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

    // Select index
    ui->tableChars->setFocus();
    ui->tableChars->selectionModel()->select(model.index(0, 0), QItemSelectionModel::SelectCurrent);
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


void FmMain::drawSampleWithQt(const uc::Cp& ch)
{
    // Font
    auto& font = ch.font(hint);
    ui->lbSample->setFont(font.get(font.q.big, FSZ_BIG));

    // Sample char
    ui->stackSample->setCurrentWidget(ui->pageSampleQt);
    auto proxy = ch.sampleProxy(hint);
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
        std::string_view style = "";
        if (dest.starts_with("pop_") && dest.find(':') != std::string_view::npos)
            style = " style='color:" CNAME_LINK_POPUP ";'";

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
            font.load();
            str::append(s, "<font size='+2' face='"sv);
            str::append(s, font.family);
            str::append(s, "'>"sv);
            str::append(s, x.safeGetV(1, {}));
            str::append(s, "</font>");
        } else if (name == "_"sv) {
            s.append(QChar(0x00A0));
        } else if (name == "-"sv) {
            str::append(s, "<span style='font-size:4pt'>\u00A0</span>—<span style='font-size:4pt'> </span>"sv);
        } else if (name == "%"sv) {
            str::append(s, x.safeGetV(1, {}));
            str::append(s, "<span style='font-size:3pt'>\u00A0</span>%"sv);
        } else {
            wiki::appendHtml(s, x[0]);
        }
    }

    using StrCache = char[100];

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
                 ": <a href='%s:%.*s' style='color:" CNAME_LINK_POPUP "'>",
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

}   // anon namespace


void FmMain::showCp(MaybeChar ch)
{
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

    // Copy
    ui->btCopy->setEnabled(ch.hasCp());

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
        if (ch->name.isAbbreviated()) {
            ui->stackSample->setCurrentWidget(ui->pageSampleCustom);
            ui->pageSampleCustom->setAbbreviation(ch->name.abbrev());
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
                ws = ch->scriptEx(hint).qtCounterpart;
                font = model.match.sysFontFor(ch.code, ws, FSZ_BIG);
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

        {   // Info box
            str::append(text, "<p>");
            str::QSep sp(text, "<br>");

            // Script
            auto& scr = ch->script();
            appendValuePopup(text, scr, u8"Письменность", "pop_scr");

            // Unicode version
            sp.sep();
            appendVersion(text, {}, ch->version());

            // Character type
            sp.sep();
            appendValuePopup(text, ch->category(), u8"Тип", "pop_cat");

            // Numeric value
            if (ch->numeric.isPresent()) {
                sp.sep();
                str::append(text, ch->numeric.type().locName);
                str::append(text, ": ");
                auto frac = ch->numeric.val.val();
                str::append(text, frac.num);
                if (frac.denom != 1) {
                    str::append(text, "/");
                    str::append(text, frac.denom);
                }
            }

            // Bidi writing
            sp.sep();
            appendValuePopup(text, ch.cp->bidiClass(), u8"В двунаправленном письме", "pop_bidi");

            // Block
            hint2 = uc::blockOf(ch->subj, hint2);
            sp.sep();
            appendValuePopup(text, *hint2, u8"Блок", "pop_blk");

            // Font
            if (font) {
                sp.sep();
                FontLink lnk { font->family(), ch.code, ws };
                appendValuePopup(text, lnk, u8"Системный шрифт", "pop_font");
            }

            // HTML
            sp.sep();
            str::append(text, u8"HTML: ");
            snprintf(buf, std::size(buf), "&amp;#%d;", static_cast<int>(ch->subj));
            str::append(text, buf);

            // UTF-8
            sp.sep();
            auto sChar = str::toQ(ch.code);
            str::append(text, u8"UTF-8:");
            auto u8 = sChar.toUtf8();
            for (unsigned char v : u8) {
                snprintf(buf, 10, " %02X", static_cast<int>(v));
                str::append(text, buf);
            }

            // UTF-16: QString us UTF-16
            sp.sep();
            str::append(text, u8"UTF-16:");
            for (auto v : sChar) {
                snprintf(buf, std::size(buf), " %04X", static_cast<int>(v.unicode()));
                str::append(text, buf);
            }

            text.append("</p>");
        }
        ui->vwInfo->setText(text);
    } else {
        // No character
        ui->stackSample->setCurrentWidget(ui->pageSampleQt);
        ui->lbSample->setText({});
        ui->lbOs->setText({});
        auto color = palette().color(QPalette::Disabled, QPalette::WindowText);
        ui->vwInfo->setText("<h1 style='color:" + color.name() + "'>Отсутствует в Юникоде</h1>");
    }
}


void FmMain::charChanged(const QModelIndex& current)
{
    showCp(model.charAt(current));
}


namespace {
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

    void appendScript(QString& text, const uc::Script& x, bool isScript)
    {
        if (x.ecType != uc::EcScriptType::NONE) {
            str::append(text, "<p>");
            str::QSep sp(text, "<br>");
            str::append(text, u8"• Тип: ");
            str::append(text, x.type().locName);
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

    auto fonts = model.match.allSysFonts(cp, ws, 15);
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
    if (scheme == "pop_bidi"sv) {
        if (auto* bidi = uc::findBidiClass(target))
            showPopup(*bidi, widget, rect);
    } else if (scheme == "pop_cat"sv) {
        if (auto* cat = uc::findCategory(target))
            showPopup(*cat, widget, rect);
    } else if (scheme == "pop_scr"sv) {
        if (auto* script = uc::findScript(target))
            showPopup(*script, widget, rect);
    } else if (scheme == "pop_blk"sv) {
        int iBlock = 0;
        fromChars(target, iBlock);
        showPopup(uc::blocks[iBlock], widget, rect);
    } else if (scheme == "pop_font"sv) {
        auto sv = str::splitSv(target, '/');
        if (sv.size() >= 2) {
            unsigned int cp = 0, ws = 0;
            fromChars(sv[0], cp);
            fromChars(sv[1], ws);
            showFonts(cp, static_cast<QFontDatabase::WritingSystem>(ws), widget, rect);
        }
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


void FmMain::on_vwInfo_anchorClicked(const QUrl &arg)
{
    auto str = arg.path().toStdString();
    auto rect = ui->vwInfo->cursorRect();
    // Get some parody for link rect
    // Unglitch: we don’t know how to get EXACT coords of link,
    // so improvise somehow
    rect.setLeft(rect.left() - 80);

    linkClicked(str, ui->vwInfo, rect);
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
    ui->tableChars->setCurrentIndex(model.indexOf(code));
}
