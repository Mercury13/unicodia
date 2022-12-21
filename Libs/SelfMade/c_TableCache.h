#pragma once

// STL
#include <set>

// Qt
#include <QAbstractItemModel>
#include <QTableView>


class ItemPainter   // interface
{
public:
    virtual void paintItem(
        QPainter* painter,
        const QStyleOptionViewItem& option,
        const QModelIndex& index) const = 0;
    virtual ~ItemPainter() = default;
};


template <class Body>
class ItemPainterT : public ItemPainter
{
public:
    ItemPainterT(const Body& aBody) : body(aBody) {}
    void paintItem(
            QPainter* painter,
            const QStyleOptionViewItem& option,
            const QModelIndex& index) const final
        { body(painter, option, index); }
private:
    const Body& body;
};


class ExclusiveClient;


class ExclusiveHost
{
public:
    ~ExclusiveHost() { disband(); }
    void disband();
    void dropExcept(ExclusiveClient* x);
    void dropExcept(ExclusiveClient& x) { dropExcept(&x); }
private:
    friend class ExclusiveClient;
    std::set<ExclusiveClient*> clients;
};


class ExclusiveClient : public QObject
{
    Q_OBJECT
    using This = ExclusiveClient;
public:
    /// Connects to a model
    void connectSignals(QAbstractItemModel& aModel);
    void connectSignals(QAbstractItemModel* aModel);

    ExclusiveClient() = default;
    // Disable copy/move
    ExclusiveClient(const ExclusiveClient&) = delete;
    ExclusiveClient(ExclusiveClient&&) = delete;
    ExclusiveClient& operator = (const ExclusiveClient&) = delete;
    ExclusiveClient& operator = (ExclusiveClient&&) = delete;

    ~ExclusiveClient() { unlinkHost(); }

    ExclusiveHost* host() { return fHost; }

    void linkToHost(ExclusiveHost* aHost);
    void linkToHost(ExclusiveHost& aHost) { linkToHost(&aHost); }
    void unlinkHost();
public slots:
    ///  Drops all cached data.
    virtual void drop() {}
    void dropExceptMe();
private:
    friend class ExclusiveHost;
    ExclusiveHost* fHost = nullptr;
};


class TableCache : public ExclusiveClient
{
public:
    /// @return  widget we cached data for
    const QTableView* widget() { return fWidget; }

    void nonCachingPaint(
            QPainter* painter,
            const QStyleOptionViewItem& option,
            const QModelIndex& index,
            const ItemPainter& aPainter);
    void paint(
            QPainter* painter,
            const QStyleOptionViewItem& option,
            const QModelIndex& index,
            const ItemPainter& aPainter);

    template <class Body>
    void paintT(
            QPainter* painter,
            const QStyleOptionViewItem& option,
            const QModelIndex& index,
            const Body& aBody)
        { paint(painter, option, index, ItemPainterT<Body>(aBody) ); }
    void drop() override;
    static bool wantDebug;
private:
    using IntPair = std::pair<int, int>;
    struct Cell {
        QPixmap pix;
        QStyle::State state;

        void clear() { pix = QPixmap(); }
    };
    std::map<IntPair, Cell> cells;
    int fFirstRow = std::numeric_limits<int>::max(),
        fLastRowPlus = 0,
        fFirstCol = std::numeric_limits<int>::max(),
        fLastColPlus = 0;
    const QTableView* fWidget = nullptr;
    QPixmap eqPix;  // equalizing pixmap, for painting w/o caching in consistent manner

    struct SizeAssortment {
        const qreal dpr;
        const int smallWidth, smallHeight, bigWidth, bigHeight;

        SizeAssortment(QPainter* painter, const QRect& rect);
        QRect smallRect() const { return { 0, 0, smallWidth, smallHeight }; }
        QRect bigRect() const { return { 0, 0, bigWidth, bigHeight }; }
        QSize bigSize() const { return { bigWidth, bigHeight}; }
    };

    void drawAtPix(
            const SizeAssortment& sa,
            QPixmap& pix,
            const QStyleOptionViewItem& option,
            const QModelIndex& index,
            const ItemPainter& aPainter);
};
