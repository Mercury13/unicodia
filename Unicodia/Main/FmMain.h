#ifndef FMMAIN_H
#define FMMAIN_H
// −warn: this include guard somehow works better than pragma once
// when promoting widgets

// Qt
#include <QMainWindow>
#include <QAbstractTableModel>
#include <QStyledItemDelegate>
#include <QElapsedTimer>

// My libs
#include "u_Vector.h"
#include "u_EcArray.h"
#include "u_TinyOpt.h"
#include "u_LruCache.h"
#include "QtMultiRadio.h"

// Parts of Main
#include "tables.h"

// CharPaint
#include "CharPaint/global.h"

// Project-local
#include "MyWiki.h"
#include "MainGui.h"
#include "d_Config.h"

// Unicode data
#include "UcData.h"

// Search
#include "Search/uc.h"

// L10n
#include "LocQt.h"

QT_BEGIN_NAMESPACE
namespace Ui { class FmMain; }
QT_END_NAMESPACE


class QToolButton;
class QTextBrowser;
class QNetworkAccessManager;
class QNetworkReply;
class FmTofuStats;
class WiLibCp;
class WiShowcase;
class BangButton;


namespace uc {
    enum class CopiedChannel;
}


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

    /// @return [+] are those telltales on AFTERWARDS
    bool toggleSortingTelltales();

    using Super::beginResetModel;
    using Super::endResetModel;
private:    
    Fix1d<const uc::Block*, uc::N_BLOCKS> a { nullptr };    
    BlockOrder order = BlockOrder::DEFAULT;
    bool showSortingTelltales = false;

    void prependTelltales(QString& s, const uc::Block* block) const;
};


class SearchModel final : public QAbstractItemModel, public QStyledItemDelegate
{
public:
    SearchModel(const PixSource* aSample, const uc::GlyphStyleSets& aGlyphSets) noexcept
        : sample(aSample), glyphSets(aGlyphSets) {}
    int rowCount(const QModelIndex&) const override;
    int columnCount(const QModelIndex&) const override { return 1; }
    QModelIndex index(int row, int column,
            const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    QVariant groupData(size_t index, int role) const;
    QVariant data(const QModelIndex& index, int role) const override;

    void set(uc::ReplyStyle st, uc::EcVersion ver, uc::PrimaryObj obj,
             SafeVector<uc::SearchGroup>&& x);
    void clear();
    bool hasData() const { return !groups.empty(); }
    const uc::SearchLine& lineAt(size_t iGroup, size_t iLine) const;
    size_t groupSizeAt(size_t iGroup) const;
    static bool isGroup(const QModelIndex& index);
    const uc::SearchLine* lineAt(const QModelIndex& index) const;
protected:
    void initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
private:
    uc::ReplyStyle style = uc::ReplyStyle::FLAT;
    uc::EcVersion version = uc::EcVersion::NO_VALUE;
    uc::PrimaryObj primaryObj = uc::PrimaryObj::DFLT;
    const PixSource* const sample;
    const uc::GlyphStyleSets& glyphSets;
    SafeVector<uc::SearchGroup> groups;
    mutable LruCache<char32_t, QPixmap> cache { 400 };

    static constexpr auto EMOJI_DRAW = uc::EmojiDraw::CONSERVATIVE;    
    static constexpr quintptr ZERO = 0;
    static constexpr auto GROUP = std::numeric_limits<quintptr>::max();
};


class LibModel final : public QAbstractItemModel, public QStyledItemDelegate
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
    QModelIndex indexOf(const uc::LibNode& node);
    static CharTiles getCharTiles(const uc::LibNode& node);
    // QStyledItemDelegate
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
private:
    const PixSource* const sample;
    static constexpr auto COL0 = 0;
};


enum class SelectMode : unsigned char { NONE, INSTANT };

constexpr bool DIR_ADD = true;
constexpr bool DIR_REMOVE = false;

class FmMain : public QMainWindow,
               public loc::Form<FmMain>,
               private PixSource,
               private mywiki::InternalLinkWalker
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
    void configLoaded();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    Ui::FmMain *ui = nullptr;
    uc::GlyphStyleSets glyphSets;
    CharsModel model;
    BlocksModel blocksModel;
    SearchModel searchModel;
    LangModel langModel;
    LibModel libModel;
    FavsModel favsModel;
    Uptr<FmTofuStats> fmTofuStats;
    QFont fontTofu;
    QToolButton *btSort = nullptr,
                *btAddCpToFavs = nullptr,
                *btRemoveFromFavs = nullptr;
    BangButton*  btAddLibToFavs = nullptr;
    EcRadio<BlockOrder, QAction> radioSortOrder;
    ec::Array<QIcon, BlockOrder> sortIcons;
    WiLibCp* libCpWidgets[uc::LONGEST_LIB] { nullptr };
    MyGui mainGui;
    PopupGui popupGui{mainGui};
    std::unique_ptr<QNetworkAccessManager> netMan;
    QColor clCollapse;

    class BlocksLocalMenu : public TableLocalMenu {
    public:
        QAction* acToLib = nullptr;
        QAction* acFavs = nullptr;
        bool direction = DIR_ADD;  // Favs’ direction is always removal
    } localBlocks;

    class FavsLocalMenu : public TableLocalMenu {
    public:
        QAction *acToBlocks = nullptr,
                *acToLib = nullptr,
                *acRemove = nullptr;
    } localFavs;

    struct LibLocalMenu {
        QMenu* menu = nullptr;
        QAction* acCopy = nullptr;
        QAction* acCopyBare = nullptr;
        QAction* acCopyVs15 = nullptr;
        QAction* acAddToFavs = nullptr;
        bool direction = DIR_ADD;
    } localLib;

    struct SearchLocalMenu {
        QMenu* menu = nullptr;
        QAction *acGo = nullptr,
                *acCopy = nullptr;
    } localSearch;

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

    /// Installs the same event for both Ctrl+C and Ctrl+Ins
    void installCtrlCEvent(
            QAbstractScrollArea* widget, void(This::* funcCopy)());
    /// Installs similar copy events for all three tabs
    void installCopyEvents(
            QAbstractScrollArea* widget,
            void(This::* funcMain)(), void(This::* funcSample)(),
            WiShowcase* showcase, QTextBrowser* browser);
    InitBlocks initBlocks();
    void initLibrary(const InitBlocks& ib);
    void initFavs(const InitBlocks& ib);
    void initAbout();
    void translateAbout();
    void initSearch();
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
    void copyCurrentThing(uc::CopiedChannel channel, QWidget* initiator);
    void copyFavsThing(uc::CopiedChannel channel);
    void copyLibChar(uc::CopiedChannel channel);
    void blinkCopied(QWidget* widget, const QRect& absRect, LocKey locKey);
    /// @param [in] initiator   other initiator widget besides table
    void blinkCopied(QAbstractItemView* table, QWidget* initiator, LocKey locKey);
    void clearSample();
    bool doSearch(const QString& what);
    void showSearchResult(uc::MultiResult&& x);
    void showSearchError(const QString& text);
    void cjkSetCollapseState(bool x);
    void cjkReflectCollapseState();
    void rebuildBlocks();
    void setUpdating(bool value);
    void ensureNetMan();
    void redrawFavsTab();
    void redrawFavsTable();
    /// @param [in]  initiator [0] default [+] some other widget
    void blinkAtFavs(QWidget* initiator, const QString& text);
    void reenableAllFavs();
    /// @param [in]  direction [+] add [-] remove
    void addRemoveFromFavs(WiShowcase* showcase, QWidget* initiator, bool direction);
    void setCollapseColor(const QColor& x);

    // PixSource
    int pixSize() const override;
    QColor winColor() const override { return palette().windowText().color(); }

    // InternalWalker
    void blinkAddCpToFavs() override;
    void gotoLibCp(QWidget* initiator, char32_t cp) override;
    void searchForRequest(const uc::Request& request) override;
private slots:
    void charChanged(const QModelIndex& current);
    void libChanged(const QModelIndex& current);
    void favsCurrentChanged(const QModelIndex& current);
    void blinkCopiedForWidget(QWidget* initiator);
    void copyCurrentChar(QWidget* initiator);
    void copyCurrentCharNull();
    void copyCurrentSample(QWidget* initiator);
    void copyCurrentSampleNull();
    void copyCurrentLib();
    void copyCurrentLibVs15();
    void copyCurrentLibBare();
    void copyCurrentFavs();
    void libLocalMenuRequested(const QPoint& where);
    void copyFavsSample();
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
    void searchEnterPressed(const QModelIndex& index);
    void languageChanged(int index);
    void reloadLanguage();
    void comboIndexChanged(int index);
    void comboDroppedDown();
    void comboPulledUp();
    void blockOrderChanged();
    void glyphStyleChanged(uc::EcGlyphStyleChannel channel, unsigned setting);
    void blocksLocalMenuActivated();
    void blocksFavsCalled();
    void blocksToLibCalled();
    void favsLocalMenuActivated();
    void favsToBlocks();
    void favsToLibCalled();
    void favsRemoveCalled();
    void libFavsCalled();
    void searchLocalMenuRequested(const QPoint& where);
    void copyCurrentSearch();
    void debugFontLayout();
    void highlightFont();

    // InternalWalker + slot
    void gotoCp(QWidget* initiator, char32_t cp) override;

    bool goToNode(const uc::LibNode& node);
    void startUpdate();
    void updateFinished(QNetworkReply* reply);
    void acAddCpToFavsTriggered(bool isChecked);
    void acAddLibToFavsTriggered(bool isChecked);
    void addLibToFavsBanged();
    void acRemoveFromFavsTriggered();
    void slotSkinToneQa();
    void searchForVs16();
    void toggleSortingTelltales();
    void goToSearch();
    void goToBlocks();
    void goToLib();
    void goToFavs();
};


extern template class LruCache<char32_t, QPixmap>;

#endif
