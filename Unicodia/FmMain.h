#pragma once

// C++
#include <unordered_set>

// Qt
#include <QMainWindow>
#include <QAbstractTableModel>
#include <QStyledItemDelegate>

// My libs
#include "u_Vector.h"
#include "u_TinyOpt.h"
#include "c_TableCache.h"
#include "u_LruCache.h"

// Project-local
#include "FontMatch.h"
#include "MyWiki.h"

// Forms
#include "FmPopup.h"

// Unicode data
#include "UcData.h"
#include "UcSearch.h"

QT_BEGIN_NAMESPACE
namespace Ui { class FmMain; }
QT_END_NAMESPACE


constexpr int NCOLS = 8;
constexpr int FSZ_TABLE = 15;
constexpr int FSZ_BIG = 50;

class FmPopup2;
class FmMessage;

struct MaybeChar {
    char32_t code = 0;
    const uc::Cp* cp = nullptr;
    explicit operator bool() const { return cp; }
    const uc::Cp& operator * () const { return *cp; }
    const uc::Cp* operator ->() const { return  cp; }
    bool hasCp() const { return cp; }
};


struct CacheCoords {
    size_t row = 0;
    unsigned col = 0;
};


class RowCache
{
public:
    RowCache(int anCols);

    size_t nRows() const { return rows.size(); }
    size_t nCols() const { return fnCols; }

    void addCp(const uc::Cp& aCp);

    /// @return  code point if it’s really present
    MaybeChar charAt(size_t iRow, unsigned iCol) const;

    /// @return  starting code point of row; or NO_CHAR if bad row
    int startingCpAt(size_t iRow) const;

    CacheCoords findCode(char32_t code) const;
    void clear() { rows.clear(); }
protected:
    const int fnCols, fColMask, fRowMask;

    struct Row
    {
        unsigned startingCp = 0xFFFFFF;

        const uc::Cp* cps[NCOLS];

        Row() { clear(); }
        Row(int aStartingCp) : startingCp(aStartingCp) { clear(); }

        void clear() { std::fill(std::begin(cps), std::end(cps), nullptr); }
    };

    SafeVector<Row> rows;
    Row& ensureRow(unsigned aStartingCp);
    static bool isLessRC(const Row& x, char32_t y);
};


enum class TableColors { NO, YES };
enum class CharSet { SAFE, FULL };
enum class TableDraw { INTERNAL, CUSTOM };

class CharsModel
        : public QAbstractTableModel,
          public QStyledItemDelegate,
          protected ItemPainter
{
    using Super = QAbstractTableModel;
    using SuperD = QStyledItemDelegate;
public:
    QWidget* const owner;
    FontMatch match;
    bool isCjkCollapsed = true;
    mutable TableCache tcache;

    CharsModel(QWidget* aOwner);

    int rowCount(const QModelIndex& = {}) const override;
    int columnCount(const QModelIndex&) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    const QFont* fontAt(const QModelIndex& index) const;
    const QFont* fontAt(const uc::Cp& cp) const;
    static const QFont* fontAt(const uc::Cp& cp, const uc::Block*& hint);
    QColor fgAt(const QModelIndex& index, TableColors tcl) const;
    QColor fgAt(const uc::Cp& cp, TableColors tcl) const;
    QString textAt(const QModelIndex& index, CharSet chset = CharSet::FULL) const;
    QString textAt(const uc::Cp& cp, CharSet chset = CharSet::FULL) const;
    static QString textAt(const uc::Cp& cp, const uc::Block*& hint, CharSet chset = CharSet::FULL);
    void addCp(const uc::Cp& aCp);
    MaybeChar charAt(const QModelIndex& index) const
            { return rows.charAt(index.row(), index.column()); }
    QModelIndex indexOf(char32_t code);

    /// \brief isCharCollapsed
    /// \param code
    ///    Code of valid (in a block) character
    /// \return
    ///    [+] it is collapsed
    ///
    bool isCharCollapsed(char32_t code) const;
    bool isCharCollapsed(char32_t code, const uc::Block*& hint) const;
    void build();
    using Super::beginResetModel;
    using Super::endResetModel;

    // Delegate
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
    static void drawChar(QPainter* painter, const QRect& rect,
                const uc::Cp& cp, const QColor& color,
                const uc::Block*& hint, TableDraw mode);
protected:
    // Delegate
    void initStyleOption(QStyleOptionViewItem *option,
                         const QModelIndex &index) const override;
    void paintItem(
            QPainter* painter,
            const QStyleOptionViewItem& option,
            const QModelIndex& index) const override;
    void drawChar(QPainter* painter, const QRect& rect,
                const QModelIndex& index, const QColor& color) const;
private:
    RowCache rows;
    static constexpr auto SHRINK_Q = 4;
    static constexpr auto SHRINK_Q1 = 5;    // draw a bit larger, to counter drawing problems
    mutable struct Hint {
        const uc::Block* cell = &uc::blocks[0];
    } hint;
};


class BlocksModel : public QAbstractTableModel
{
public:
    int rowCount(const QModelIndex&) const override;
    int columnCount(const QModelIndex&) const override;
    QVariant data(const QModelIndex& index, int role) const override;
};


class SearchModel : public QAbstractTableModel
{
public:
    SearchModel(QWidget* aSample) : sample(aSample) {}
    int size() const { return v.size(); }
    int rowCount(const QModelIndex&) const override { return v.size(); }
    int columnCount(const QModelIndex&) const override { return 1; }
    QVariant data(const QModelIndex& index, int role) const override;
    void set(SafeVector<uc::SearchLine>&& x);
    void clear();
    bool hasData() const { return !v.empty(); }
    const uc::Cp& cpAt(size_t index) const;
    int pixSize() const;
private:
    QWidget* const sample;
    SafeVector<uc::SearchLine> v;
    mutable LruCache<char32_t, QPixmap> cache { 300 };
    mutable const uc::Block* hint = &uc::blocks[0];
};


class WiCustomDraw : public QWidget
{
    using Super = QWidget;
public:
    using Super::Super;
    void setAbbreviation(std::u8string_view x, char32_t aSubj);
    void setSpace(const QFont& font, char32_t aSubj);
    void setNormal();
    void init();
protected:
    void paintEvent(QPaintEvent *event);
private:
    QSize initialSize;
    enum class Mode { NONE, SPACE, ABBREVIATION };
    Mode mode = Mode::NONE;
    std::u8string_view abbreviation;
    const QFont* fontSpace = nullptr;
    char32_t subj = 0;
};


template <class T>
class Uptr : public std::unique_ptr<T>
{
    using Super = std::unique_ptr<T>;
public:
    using Super::operator =;

    template <class... UU>
    T& ensure(UU&&... u) {
        if (!*this) {
            *this = std::make_unique<T>(std::forward<UU>(u)...);
        }
        return **this;
    }
};


enum class CurrThing { CHAR, SAMPLE };


class FmMain : public QMainWindow,
               private mywiki::Gui
{
    Q_OBJECT
    using Super = QMainWindow;
    using This = FmMain;
    friend class FmPopup2;

public:
    FmMain(QWidget *parent = nullptr);
    ~FmMain() override;
    void installTempPrefix();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    Ui::FmMain *ui = nullptr;
    CharsModel model;
    BlocksModel blocksModel;
    SearchModel searchModel;
    Uptr<FmPopup2> popup;
    Uptr<FmMessage> fmMessage;
    QFont fontBig;
    char32_t shownCp = uc::NO_CHAR;
    mutable struct Hint {
        const uc::Block* sample = &uc::blocks[0];
    } hint;

    void initAbout();
    void showCp(MaybeChar ch);
    void linkClicked(std::string_view link, QWidget* widget, TinyOpt<QRect> rect);
    void selectChar(char32_t code);
    void selectCharEx(char32_t code);
    void drawSampleWithQt(const uc::Cp& cp);
    void initTerms();
    void copyCurrentThing(CurrThing thing);
    void showCopied(QAbstractItemView* table);
    void showCopied(QWidget* widget, const QRect& absRect);
    void clearSample();
    void doSearch(QString what);
    void showSearchResult(uc::SearchResult&& x);
    void showSearchError(const QString& text);
    void cjkSetCollapseState(bool x);
    void cjkReflectCollapseState();

    // mywiki::Gui
    void popupAtAbs(
            QWidget* widget, const QRect& absRect, const QString& html) override;
    FontList allSysFonts(
            char32_t cp, QFontDatabase::WritingSystem ws, size_t maxCount) override;
    void copyTextAbs(
            QWidget* widget, const QRect& absRect, const QString& text) override;    
signals:
    void setFocusDefered(QWidget* wi);
private slots:
    void charChanged(const QModelIndex& current);
    void copyCurrentChar();
    void copyCurrentSample();
    void popupLinkActivated(const QString& link);
    void labelLinkActivated(const QString& link);
    void anchorClicked(const QUrl &arg1);
    void cjkExpandCollapse();
    void showTofuStats();
    void slotSetFocusDefered(QWidget* wi);
    void openSearch();
    void closeSearch();
    void startSearch();
    void focusSearch();
    void searchEnterPressed(int index);
    void on_comboBlock_currentIndexChanged(int index);
};


class FmPopup2 : public FmPopup
{
    using Super = FmPopup;
    using This = FmPopup2;
public:
    FmPopup2(FmMain* owner);
    ~FmPopup2() override = default;
};

extern template class LruCache<char32_t, QPixmap>;
