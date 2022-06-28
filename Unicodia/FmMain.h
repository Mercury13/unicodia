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

// L10n
#include "LocQt.h"

QT_BEGIN_NAMESPACE
namespace Ui { class FmMain; }
QT_END_NAMESPACE


constexpr int NCOLS = 8;

class QToolButton;
class FmPopup2;
class FmMessage;
class FmTofuStats;

struct MaybeChar {
    char32_t code = 0;
    const uc::Cp* cp = nullptr;
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
    ~CharsModel();  // forward-defined class here

    int rowCount(const QModelIndex& = {}) const override;
    int columnCount(const QModelIndex& = {}) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    std::optional<QFont> fontAt(const QModelIndex& index) const;
    static std::optional<QFont> fontAt(const uc::Cp& cp);
    QColor fgAt(const QModelIndex& index, TableColors tcl) const;
    QColor fgAt(const uc::Cp& cp, TableColors tcl) const;
    QString textAt(const QModelIndex& index, int aDpi = uc::DPI_ALL_CHARS) const;
    static QString textAt(const uc::Cp& cp, int dpi = uc::DPI_ALL_CHARS);
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
    static void drawChar(QPainter* painter, const QRect& rect,
                const uc::Cp& cp, const QColor& color, TableDraw mode, int dpi);
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
                const QModelIndex& index, const QColor& color, int dpi) const;
private:
    RowCache rows;
    mutable bool hasText = true;
    static constexpr auto SHRINK_Q = 4;
    static constexpr auto SHRINK_Q1 = 5;    // draw a bit larger, to counter drawing problems
};


class LangModel : public QAbstractTableModel
{
public:
    int rowCount(const QModelIndex&) const override;
    int columnCount(const QModelIndex&) const override;
    QVariant data(const QModelIndex& index, int role) const override;
};


class BlocksModel : public QAbstractTableModel
{
private:
    using Super = QAbstractTableModel;
public:
    int rowCount(const QModelIndex&) const override;
    int columnCount(const QModelIndex&) const override;
    QVariant data(const QModelIndex& index, int role) const override;

    using Super::beginResetModel;
    using Super::endResetModel;
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
    const uc::SearchLine& lineAt(size_t index) const;
    int pixSize() const;
private:
    QWidget* const sample;
    SafeVector<uc::SearchLine> v;
    mutable LruCache<char32_t, QPixmap> cache { 400 };
};


class WiCustomDraw : public QWidget
{
    using Super = QWidget;
public:
    using Super::Super;
    void setAbbreviation(std::u8string_view x);
    void setSpace(const QFont& font, char32_t aSubj);
    void setCustomControl(char32_t aSubj);
    void setEmoji(char32_t aSubj);
    void setEgyptianHatch(const QFont& font, char32_t aSubj);
    void setNormal();
    void init();
protected:
    void paintEvent(QPaintEvent *event);
private:
    QSize initialSize;
    enum class Mode { NONE, SPACE, ABBREVIATION, CUSTOM_CONTROL, EMOJI, EGYPTIAN_HATCH };
    Mode mode = Mode::NONE;
    std::u8string_view abbreviation;
    QFont fontSpace;
    char32_t subj = 0;
    void setSpace1(const QFont& font, char32_t aSubj, Mode aMode);
};


template <class T>
class Uptr
{
public:
    using Pointer = T *;

    template <class... UU>
    T& ensure(UU&&... u) {
        if (!*this) {
            v = new T(std::forward<UU>(u)...);
        }
        return *v;
    }

    ~Uptr() { delete v; }

    operator Pointer() const { return v; }
    Pointer operator->() const { return v; }
    explicit operator bool() const { return v; }
private:
    std::atomic<T*> v = nullptr;
};


enum class CurrThing { CHAR, SAMPLE };
enum class SelectMode { NONE, INSTANT, DEFERED };


class FmMain : public QMainWindow,
               public loc::Form<FmMain>,
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
    void translateMe() override;
    void chooseFirstLanguage();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    Ui::FmMain *ui = nullptr;
    CharsModel model;
    BlocksModel blocksModel;
    SearchModel searchModel;
    LangModel langModel;
    Uptr<FmPopup2> popup;
    Uptr<FmMessage> fmMessage;
    Uptr<FmTofuStats> fmTofuStats;
    QFont fontBig, fontTofu;
    MaybeChar shownCp;
    QToolButton *btSortAlpha, *btSortCode;

    struct PullUpDetector {
        bool isCocked = false;
        QElapsedTimer timer;

        bool detect() const
            { return !isCocked && timer.isValid()
                    && static_cast<quint64>(timer.elapsed()) <= 100; }
    } pullUpDetector;

    void initAbout();
    void showCp(MaybeChar ch);
    void forceShowCp(MaybeChar ch);
    void linkClicked(std::string_view link, QWidget* widget, TinyOpt<QRect> rect);
    template <SelectMode mode> void selectChar(char32_t code);
    void drawSampleWithQt(const uc::Cp& cp);
    void initTerms();
    void copyCurrentThing(CurrThing thing);
    void showCopied(QAbstractItemView* table);
    void showCopied(QWidget* widget, const QRect& absRect);
    void clearSample();
    void doSearch(const QString& what);
    void showSearchResult(uc::MultiResult&& x);
    void showSearchError(const QString& text);
    void cjkSetCollapseState(bool x);
    void cjkReflectCollapseState();
    void preloadVisibleFonts();

    // mywiki::Gui
    void popupAtAbs(
            QWidget* widget, const QRect& absRect, const QString& html) override;
    FontList allSysFonts(
            char32_t cp, QFontDatabase::WritingSystem ws, size_t maxCount) override;
    void copyTextAbs(
            QWidget* widget, const QRect& absRect, const QString& text) override;    
    void followUrl(const QString& x) override;    
private slots:
    void charChanged(const QModelIndex& current);
    void copyCurrentChar();
    void copyCurrentSample();
    void popupLinkActivated(const QString& link);
    void labelLinkActivated(const QString& link);
    void anchorClicked(const QUrl &arg1);
    void cjkExpandCollapse();
    void showTofuStats();
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

#endif
