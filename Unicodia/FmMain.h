#ifndef FMMAIN_H
#define FMMAIN_H
// −warn: this include guard somehow works better than pragma once
// when promoting widgets

// C++
#include <unordered_set>

// Qt
#include <QMainWindow>
#include <QAbstractTableModel>
#include <QStyledItemDelegate>
#include <QElapsedTimer>
#include <QRadioButton>

// My libs
#include "u_Vector.h"
#include "u_EcArray.h"
#include "u_TinyOpt.h"
#include "c_TableCache.h"
#include "u_LruCache.h"
#include "QtMultiRadio.h"

// CharPaint
#include "CharPaint/global.h"

// Project-local
#include "FontMatch.h"
#include "MyWiki.h"
#include "MainGui.h"
#include "d_Config.h"

// Unicode data
#include "UcData.h"
#include "UcSearch.h"

// L10n
#include "LocQt.h"

QT_BEGIN_NAMESPACE
namespace Ui { class FmMain; }
QT_END_NAMESPACE


constexpr int NCOLS = 8;

class QToolButton;
class FmTofuStats;
class WiLibCp;

struct MaybeChar {
    char32_t code = 0;
    const uc::Cp* cp = nullptr;

    // Funcs
    explicit operator bool() const { return cp; }
    const uc::Cp& operator * () const { return *cp; }
    const uc::Cp* operator ->() const { return  cp; }
    MaybeChar& operator = (const uc::Cp& x)
        { code = x.subj; cp = &x; return *this; }
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

class CharsModel final
        : public QAbstractTableModel,
          public QStyledItemDelegate,
          protected ItemPainter
{
    using Super = QAbstractTableModel;
    using SuperD = QStyledItemDelegate;
public:
    static constexpr auto EMOJI_DRAW = uc::EmojiDraw::CONSERVATIVE;

    QWidget* const owner;
    FontMatch match;
    bool isCjkCollapsed = true;
    mutable TableCache tcache;

    struct Style {
        uc::GlyphStyleSets sets;
        uc::EcGlyphStyleChannel currChannel = uc::EcGlyphStyleChannel::NONE;
        constexpr unsigned currSetting() const { return sets[currChannel]; }
        constexpr void setCurrSetting(unsigned x)
        {
            if (currChannel != uc::EcGlyphStyleChannel::NONE)
                sets[currChannel] = x;
        }
    } glyphStyle;

    CharsModel(QWidget* aOwner);
    ~CharsModel();  // forward-defined class here

    int rowCount(const QModelIndex& = {}) const override;
    int columnCount(const QModelIndex& = {}) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    std::optional<QFont> fontAt(const QModelIndex& index) const;
    QColor fgAt(const QModelIndex& index, TableColors tcl) const;
    QColor fgAt(const uc::Cp& cp, TableColors tcl) const;
    QString textAt(const QModelIndex& index) const;
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
    void build();
    using Super::beginResetModel;
    using Super::endResetModel;

    // Delegate
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
protected:
    // Delegate
    void initStyleOption(QStyleOptionViewItem *option,
                         const QModelIndex &index) const override;
    void paintItem(
            QPainter* painter,
            const QStyleOptionViewItem& option,
            const QModelIndex& index) const override;
    void paintItem1(
            QPainter* painter,
            const QStyleOptionViewItem& option,
            const QModelIndex& index,
            const QColor& color) const;
    void drawChar(QPainter* painter, const QRect& rect,
                const QModelIndex& index, const QColor& color) const;
private:
    RowCache rows;
    static constexpr auto SHRINK_Q = 4;
    static constexpr auto SHRINK_Q1 = 5;    // draw a bit larger, to counter drawing problems
};


class LangModel final : public QAbstractTableModel
{
public:
    int rowCount(const QModelIndex&) const override;
    int columnCount(const QModelIndex&) const override;
    QVariant data(const QModelIndex& index, int role) const override;
};


class BlocksModel final : public QAbstractTableModel
{
private:
    using Super = QAbstractTableModel;
public:
    int rowCount(const QModelIndex&) const override;
    int columnCount(const QModelIndex&) const override;
    QVariant data(const QModelIndex& index, int role) const override;

    const uc::Block& at(size_t i) const noexcept;
    const uc::Block& operator [] (size_t i) const noexcept { return at(i); }

    size_t build(BlockOrder aOrder, size_t iOld);

    using Super::beginResetModel;
    using Super::endResetModel;
private:    
    Fix1d<const uc::Block*, uc::N_BLOCKS> a { nullptr };    
    BlockOrder order = BlockOrder::DEFAULT;
};


class SearchModel final : public QAbstractTableModel
{
public:
    SearchModel(const PixSource* aSample, const uc::GlyphStyleSets& aGlyphSets) noexcept
        : sample(aSample), glyphSets(aGlyphSets) {}
    int size() const { return v.size(); }
    int rowCount(const QModelIndex&) const override { return v.size(); }
    int columnCount(const QModelIndex&) const override { return 1; }
    QVariant data(const QModelIndex& index, int role) const override;
    void set(SafeVector<uc::SearchLine>&& x);
    void clear();
    bool hasData() const { return !v.empty(); }
    const uc::SearchLine& lineAt(size_t index) const;
private:
    const PixSource* const sample;
    const uc::GlyphStyleSets& glyphSets;
    SafeVector<uc::SearchLine> v;
    mutable LruCache<char32_t, QPixmap> cache { 400 };
    static constexpr auto EMOJI_DRAW = uc::EmojiDraw::CONSERVATIVE;
};


class LibModel final : public QAbstractItemModel
{
public:
    LibModel(const PixSource* aSample) : sample(aSample) {}
    QModelIndex index(
            int row, int column,
            const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    const uc::LibNode& nodeAt(const QModelIndex& index) const;
    static CharTiles getCharTiles(const uc::LibNode& node);
private:
    const PixSource* const sample;
    static constexpr auto COL0 = 0;
    mutable LruCache<const uc::LibNode*, QPixmap> cache { 300 };
};


enum class CurrThing { CHAR, SAMPLE };
enum class SelectMode { NONE, INSTANT };


class FmMain : public QMainWindow,
               public loc::Form<FmMain>,
               private PixSource
{
    Q_OBJECT
    using Super = QMainWindow;
    using This = FmMain;
    friend class FmPopup2;

public:
    FmMain(QWidget *parent = nullptr);
    ~FmMain() override;
    void installTempPrefix();
    void translateMe() override;
    void chooseFirstLanguage();
    BlockOrder blockOrder() const;
    void setBlockOrder(BlockOrder x);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    Ui::FmMain *ui = nullptr;
    CharsModel model;
    BlocksModel blocksModel;
    SearchModel searchModel;
    LangModel langModel;
    LibModel libModel;
    Uptr<FmTofuStats> fmTofuStats;
    QFont fontBig, fontTofu;
    MaybeChar shownCp;
    QToolButton* btSort;
    EcRadio<BlockOrder, QAction> radioSortOrder;
    ec::Array<QIcon, BlockOrder> sortIcons;
    UintRadio<QRadioButton> radioGlyphStyle;
    WiLibCp* libCpWidgets[uc::LONGEST_LIB] { nullptr };
    MyGui mainGui;
    PopupGui popupGui{mainGui};

    struct PullUpDetector {
        bool isCocked = false;
        QElapsedTimer timer;

        bool detect() const
            { return !isCocked && timer.isValid()
                    && static_cast<quint64>(timer.elapsed()) <= 100; }
    } pullUpDetector;

    struct InitBlocks {
        QString buttonColor;
        QList<int> sizes;
    };

    InitBlocks initBlocks();
    void initLibrary(const InitBlocks& ib);
    void translateAbout();
    void initAbout();
    void showCp(MaybeChar ch);
    void forceShowCp(MaybeChar ch);
    void linkClicked(
            mywiki::Gui& gui,
            std::string_view link,
            QWidget* widget,
            TinyOpt<QRect> rect);
    template <SelectMode mode> void selectChar(char32_t code);
    void initTerms();
    void translateTerms();
    void copyCurrentThing(CurrThing thing);
    void blinkCopied(QWidget* widget, const QRect& absRect);
    void blinkCopied(QAbstractItemView* table);
    void clearSample();
    bool doSearch(const QString& what);
    void showSearchResult(uc::MultiResult&& x);
    void showSearchError(const QString& text);
    void cjkSetCollapseState(bool x);
    void cjkReflectCollapseState();
    void rebuildBlocks();
    void redrawSampleChar();

    // PixSource
    int pixSize() const override;
    QColor winColor() const override { return palette().windowText().color(); }
private slots:
    void charChanged(const QModelIndex& current);
    void libChanged(const QModelIndex& current);
    void copyCurrentChar();
    void copyCurrentSample();
    void copyCurrentLib();
    void popupLinkActivated(QWidget* widget, const QString& link);
    void labelLinkActivated(const QString& link);
    void advancedLinkActivated(QWidget* widget, const QString& link);
    void anchorClicked(const QUrl &arg1);
    void cjkExpandCollapse();
    void showTofuStats();
    void dumpTiles();
    void openSearch();
    void closeSearch();
    void startSearch();
    void focusSearch();
    void searchEnterPressed(int index);
    void languageChanged(int index);
    void reloadLanguage();
    void comboIndexChanged(int index);
    void comboDroppedDown();
    void comboPulledUp();
    void blockOrderChanged();
    void glyphStyleChanged();
    void goToCp(char32_t cp);
};


extern template class LruCache<char32_t, QPixmap>;

#endif
