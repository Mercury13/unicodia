// My header
#include "FmMain.h"
#include "ui_FmMain.h"

// Qt
#include <QTableView>
#include <QTextFrame>
#include <QClipboard>
#include <QPainter>
#include <QShortcut>

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


CharsModel::CharsModel(QWidget* aOwner) : owner(aOwner), rows(NCOLS) {}


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
            auto cp = rows.charAt(index.row(), index.column());
            if (!cp)
                return {};
            return cp->sampleProxy(hint).text;
        }

    case Qt::FontRole: {
            auto cp = rows.charAt(index.row(), index.column());
            if (!cp)
                return {};
            auto& font = cp->font(hint);
            return font.get(font.q.table, FSZ_TABLE);
        }

    case Qt::ForegroundRole: {
            auto cp = rows.charAt(index.row(), index.column());
            if (!cp)
                return {};
            if (cp->isTrueSpace()) {
                auto c = owner->palette().text().color();
                c.setAlpha(ALPHA_SPACE);
                return c;
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


void FmMain::CharsDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
           const QModelIndex &index) const
{
    if (option.state.testFlag(QStyle::State_HasFocus)) {
        // Focused?
        //QStyleOptionFocusRect o;
        //o.QStyleOption::operator=(option);
        //o.state |= QStyle::State_KeyboardFocusChange;
        //owner.style()->drawPrimitive(QStyle::PE_FrameFocusRect, &o, painter, option.widget);
        Super::paint(painter, option, index);
    } else if (option.state.testFlag(QStyle::State_Selected)) {
        // Selected, not focused? Initial style is bad
        auto opt2 = option;
        opt2.state.setFlag(QStyle::State_Selected, false);
        owner.style()->drawPrimitive(QStyle::PE_FrameMenu, &opt2, painter, option.widget);
        Super::paint(painter, opt2, index);
    } else {
        // Selected, use special way of BG color
        auto cp = owner.model.charAt(index);
        if (!cp) {
            painter->fillRect(option.rect, owner.palette().button());
        }
        Super::paint(painter, option, index);
    }
}


///// FmMain ///////////////////////////////////////////////////////////////////

FmMain::FmMain(QWidget *parent)
    : Super(parent),
      ui(new Ui::FmMain), model(this), charsDelegate(*this)
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
        Eng(QString& aS) : s(aS) {}
        void appendPlain(std::string_view x) override;
        virtual void appendLink(const SafeVector<std::string_view> x) override;
        virtual void appendTemplate(const SafeVector<std::string_view> x) override;
    };

    void Eng::appendPlain(std::string_view x)
    {
        str::append(s, x);
    }

    void Eng::appendLink(const SafeVector<std::string_view> x)
    {
        auto dest = x[0];
        auto text = x[1];
        std::string_view style = "";
        if (dest.starts_with("pop_") && dest.find(':') != std::string_view::npos)
            style = " style='color:" CNAME_LINK_POPUP ";'";
        s.append("<a");
        str::append(s, style);
        s.append(" href='");
        str::append(s, dest);
        s.append("'>");
        wiki::run(*this, text);
        s.append("</a>");
    }

    void Eng::appendTemplate(const SafeVector<std::string_view> x)
    {
        wiki::appendHtml(s, x[0]);
    }


    template <class T>
    std::string_view idOf(const T& value, std::string&) { return value.id; }

    template <>
    std::string_view idOf(const uc::Block& value, std::string& cache)
    {
        cache = std::to_string(value.index());
        return cache;
    }

    template <class T>
    std::u8string_view locNameOf(const T& value) { return value.locName; }

    template<>
    std::u8string_view locNameOf(const uc::BidiClass& value)
        { return value.locShortName; }

    template <class T, class Name1>
    inline void appendValuePopup(
            QString& text, const T& value, Name1 name, const char* scheme)
    {
        std::string cache;
        str::append(text, name);
        char buf[100];
        auto vid = idOf(value, cache);
        snprintf(buf, std::size(buf),
                 ": <a href='%s:%.*s' style='color:" CNAME_LINK_POPUP "'>",
                scheme, int(vid.size()), vid.data());
        str::append(text, buf);
        str::append(text, locNameOf(value));
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
        drawSampleWithQt(*ch);

        // OS char
        ui->lbOs->setText(ch->osProxy());

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
        ui->lbSample->setText(" ");
        ui->lbOs->setText(" ");
        auto color = palette().color(QPalette::Disabled, QPalette::WindowText);
        ui->vwInfo->setText("<h1 style='color:" + color.name() + "'>Отсутствует в Юникоде</h1>");
    }
}


void FmMain::charChanged(const QModelIndex& current)
{
    showCp(model.charAt(current));
}


namespace {
    void appendWiki(QString& text, std::u8string_view x)
    {
        Eng eng(text);
        wiki::run(eng, x);
    }

    template <class T>
    inline void appendHeader(QString& text, const T& x)
    {
        str::append(text, "<p><nobr><b>");
        str::append(text, x.locName);
        str::append(text, "</b> ("sv);
        str::append(text, x.nChars);
        str::append(text, " шт.)</nobr>");
        str::append(text, "</p>");
    }
}


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
    appendWiki(text, x.locDescription);
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
    appendWiki(text, x.locDescription);
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

        str::append(text, "</p>");
    }

    str::append(text, "<p>");
    appendWiki(text, x.locDescription);
//    if (!x.hintFont.empty()) {
//        auto qdesc = str::toQ(x.locDescription);
//        auto ex = QString("font face='");
//            str::append(ex, x.hintFont);
//            str::append(ex, "'");
//        qdesc.replace(":FACE:", ex);
//        text += qdesc;
//    } else {
//        str::append(text, x.locDescription);
//    }
    str::append(text, "</p>");

    popupText(text, widget, rect);
}


namespace {
    const char8_t* STR_RANGE = u8"%04X…%04X";
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
        appendWiki(text, x.locDescription);
        str::append(text, "</p>");
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
        std::from_chars(target.data(), target.data() + target.length(), iBlock);
        showPopup(uc::blocks[iBlock], widget, rect);
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
