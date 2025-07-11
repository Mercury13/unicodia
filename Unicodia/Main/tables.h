#pragma once

#include <QAbstractItemModel>
#include <QStyledItemDelegate>

#include "c_TableCache.h"

// Unicode
#include "UcData.h"
#include "FontMatch.h"
#include "UcClipboard.h"

// Utils
#include "u_TinyOpt.h"
#include "u_DumbSp.h"

// C++
#include <memory>


class QListView;
class QMenu;

constexpr int NCOLS = 8;
constexpr char32_t NO_CHAR = 0xFFFFFFFF;

struct MaybeChar {
    char32_t code = NO_CHAR;
    const uc::Cp* cp = nullptr;

    // Funcs
    explicit operator bool() const { return cp; }
    const uc::Cp& operator * () const { return *cp; }
    const uc::Cp* operator ->() const { return  cp; }
    constexpr MaybeChar() = default;
    constexpr MaybeChar(const uc::Cp& x)
        : code(x.subj), cp(&x) {}
    MaybeChar(char32_t aCode)
        : code(aCode), cp(uc::cpsByCode[aCode]) {}
    constexpr MaybeChar& operator = (const uc::Cp& x)
        { code = x.subj; cp = &x; return *this; }
    bool hasCp() const { return cp; }
    bool hasCode() const { return (code != NO_CHAR); }
};


enum class TableColors : unsigned char { NO, YES };


class VirtualCharsModel;

class HighlightHost : public dumb::SpTarget
{
public:
    void addClient(VirtualCharsModel* client);

    /// Highlights a family by name
    /// Used first of all for Egyp: both Sesh and Gardiner cover, and Sesh is
    ///    not really reliable
    void highlightFamily(std::string_view x);
    const std::string& highlightedFamily() const { return fHighlightedFamily; }

    std::span<VirtualCharsModel* const> clients() const noexcept
        { return { fClients, nClients }; }

    bool isHighlighted(const uc::Cp& cp) const;
    bool isHighlighted(const uc::Cp* cp) const
        { return cp && isHighlighted(*cp); }
private:
    static constexpr unsigned MAX_CLIENTS = 4;
    VirtualCharsModel* fClients[MAX_CLIENTS];
    unsigned nClients = 0;

    std::string fHighlightedFamily {};
};


class VirtualCharsModel
        : public QAbstractTableModel,
          public QStyledItemDelegate,
          protected ItemPainter
{
    using SuperD = QStyledItemDelegate;
public:
    QWidget* const owner;
    const dumb::Sp<HighlightHost> hiHost;
    VirtualCharsModel(QWidget* aOwner, dumb::Sp<HighlightHost> aHiHost, uc::GlyphStyleSets& aGlyphSets);

    // QAbstractTableModel
    int columnCount(const QModelIndex& = {}) const override;
    QVariant data(const QModelIndex& index, int role) const override;

    virtual MaybeChar charAt(const QModelIndex& index) const = 0;
    virtual QString textAt(const QModelIndex& index) const;
    virtual QColor fgAt(const uc::Cp& cp, TableColors tcl) const;
    QColor fgAt(const QModelIndex& index, TableColors tcl) const;
    std::optional<QFont> fontAt(const QModelIndex& index) const;

    // Delegate
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
protected:
    uc::GlyphStyleSets& glyphSets;
    mutable TableCache tcache;
    std::unique_ptr<QListView> dummyLv;  ///< Windows’ workaround, see paintItem

    // Delegate
    void initStyleOption(QStyleOptionViewItem *option,
                         const QModelIndex &index) const override;
    void paintItem1(
            QPainter* painter,
            const QStyleOptionViewItem& option,
            const QModelIndex& index,
            const QColor& color) const;
    virtual void drawChar(QPainter* painter, const QRect& rect,
            const QModelIndex& index, const QColor& color) const;
    // ItemPainter
    void paintItem(
            QPainter* painter,
            const QStyleOptionViewItem& option,
            const QModelIndex& index) const override;
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

        constexpr Row() {}
        constexpr Row(int aStartingCp) : startingCp(aStartingCp) {}
    };

    SafeVector<Row> rows;
    Row& ensureRow(unsigned aStartingCp);
    static bool isLessRC(const Row& x, char32_t y);
};


class CharsModel final
        : public VirtualCharsModel
{
    using Super = VirtualCharsModel;
public:
    FontMatch match;
    bool isCjkCollapsed = true;

    CharsModel(QWidget* aOwner, dumb::Sp<HighlightHost> aHiHost,
               uc::GlyphStyleSets& glyphSets);
    ~CharsModel();  // forward-defined class here

    int rowCount(const QModelIndex& = {}) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QColor fgAt(const uc::Cp& cp, TableColors tcl) const override;
    using Super::fgAt;
    void addCp(const uc::Cp& aCp);
    MaybeChar charAt(const QModelIndex& index) const override;
    QModelIndex indexOf(char32_t code);

    /// @brief isCharCollapsed
    /// @param code Code of valid (in a block) character
    /// @return  [+] it is collapsed
    bool isCharCollapsed(char32_t code) const;

    void build();
    using Super::beginResetModel;
    using Super::endResetModel;
private:
    RowCache rows;
};


class FavsModel final : public VirtualCharsModel
{
    using Super = VirtualCharsModel;
public:
    using Super::Super;
    ~FavsModel();  // forward-defined class here

    int rowCount(const QModelIndex& = {}) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    MaybeChar charAt(const QModelIndex& index) const override;

    TinySizet toOrder(int row, int col) const;
    TinySizet toOrder(const QModelIndex& index) const;
    QModelIndex toCoords(unsigned iOrder) const;
    /// Does not check existence, used for deletion
    QModelIndex toCoordsDumb(unsigned iOrder) const;

    using Super::beginResetModel;
    using Super::endResetModel;
    using Super::beginInsertRows;
    using Super::endInsertRows;
    using Super::beginRemoveRows;
    using Super::endRemoveRows;
};


class TableLocalMenu : public QObject
{
    Q_OBJECT
    using This = TableLocalMenu;
public:
    void init(QTableView* aTable, VirtualCharsModel* aModel);
    void addSeparator();
    QAction* addCustomAction(std::string_view aLocKey);
    QAction* customAction(size_t index) { return customActions.at(index).ac; }
    static void popupMenu(QWidget* widget, QMenu* menu, QPoint where);
    void popup(QWidget* widget, const QPoint& where);
    void translate();
private:
    QTableView* table = nullptr;
    VirtualCharsModel* model = nullptr;
    QAction *acCopy = nullptr,
            *acCopyVs15 = nullptr,
            *acCopyVs16 = nullptr,
            *acCopyDotc = nullptr;
    struct CustomAction {
        QAction* ac = nullptr;
        std::string_view locKey;
    };
    std::vector<CustomAction> customActions;
    QMenu* menu;
signals:
    void thingCopied(uc::CopiedChannel channel, QWidget* initiator);
    void menuActivated();
};
