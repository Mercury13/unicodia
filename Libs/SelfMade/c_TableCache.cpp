// My header
#include "c_TableCache.h"

// C++
#include <cmath>

// Qt
#include <QScreen>
#include <QPainter>
#include <QMessageBox>


bool TableCache::wantDebug = false;


///// ExclusiveHost ////////////////////////////////////////////////////////////


void ExclusiveHost::disband()
{
    for (auto v : clients) {
        v->fHost = nullptr;
    }
    clients.clear();
}


void ExclusiveHost::dropExcept(ExclusiveClient* x)
{
    for (auto v : clients) {
        if (v != x)
            v->drop();
    }
}


///// ExclusiveClient //////////////////////////////////////////////////////////


void ExclusiveClient::linkToHost(ExclusiveHost* aHost)
{
    if (fHost == aHost) {
        return;
    }
    unlinkHost();
    fHost = aHost;
    if (fHost)
        fHost->clients.insert(this);
}


void ExclusiveClient::unlinkHost()
{
    if (fHost) {
        fHost->clients.erase(this);
        fHost = nullptr;
    }
}


void ExclusiveClient::connectSignals(QAbstractItemModel& aModel)
{
    drop();
    using AIM = QAbstractItemModel;
    connect(&aModel, &AIM::modelAboutToBeReset, this, &This::drop);
    connect(&aModel, &AIM::modelReset, this, &This::drop);
    connect(&aModel, &AIM::dataChanged, this, &This::drop);
}


void ExclusiveClient::connectSignals(QAbstractItemModel* aModel)
{
    if (aModel)
        connectSignals(*aModel);
}


void ExclusiveClient::dropExceptMe()
{
    if (fHost)
        fHost->dropExcept(this);
}


///// SizeAssortment ///////////////////////////////////////////////////////////


TableCache::SizeAssortment::SizeAssortment(QPainter* painter, const QRect& rect)
    : dpr(painter->device()->devicePixelRatioF()),
      smallWidth(rect.width()),
      smallHeight(rect.height()),
      bigWidth(std::lround(smallWidth * dpr)),
      bigHeight(std::lround(smallHeight * dpr)) {}



///// TableCache ///////////////////////////////////////////////////////////////


void TableCache::drop()
{
    fFirstRow = std::numeric_limits<int>::max();
    fLastRowPlus = 0;
    fFirstCol = std::numeric_limits<int>::max();
    fLastColPlus = 0;
    cells.clear();
}

//#define DEBUG_PAINT

#ifdef DEBUG_PAINT
    #include <iostream>
#endif

namespace {

#ifdef DEBUG_PAINT

    [[deprecated]] inline void log(const char* s) {
        std::cout << s << std::endl;
    }
#else
    inline void log(const char*) {}
#endif

}   // anon namespace

void TableCache::drawAtPix(
        const SizeAssortment& sa,
        QPixmap& pix,
        const QStyleOptionViewItem& option,
        const QModelIndex& index,
        const ItemPainter& aPainter)
{
    QStyleOptionViewItem option2 = option;
    option2.rect = sa.smallRect();
    if (option.backgroundBrush.style() != Qt::SolidPattern)
        pix.fill( option.widget->palette().base().color() );

    pix.setDevicePixelRatio(sa.dpr);

    // Paint with new pixel ratio
    QPainter painter2 { &pix };
    aPainter.paintItem( &painter2, option2, index );
}

void TableCache::nonCachingPaint(
        QPainter* painter,
        const QStyleOptionViewItem& option,
        const QModelIndex& index,
        const ItemPainter& aPainter)
{
    SizeAssortment sa(painter, option.rect);
    QPixmap eqPix(sa.bigWidth, sa.bigHeight);   // equalizing pixmap
    drawAtPix(sa, eqPix, option, index, aPainter);
    painter->drawPixmap(option.rect.topLeft(), eqPix, sa.bigRect());
}

void TableCache::paint(
        QPainter* painter,
        const QStyleOptionViewItem& option,
        const QModelIndex& index,
        const ItemPainter& aPainter)
{
    const int ir = index.row();
    const int ic = index.column();

    // Situations when cannot cache
    if (!option.widget      // unreal
            || ir < 0       // unreal
            || ic < 0       // unreal
                // Selected/focused — just handpaint, fewer glitches
            || (option.state & (QStyle::State_Selected | QStyle::State_HasFocus))
                ) {
            //   Used to be (the trouble is typography engine → do not cache empty strings)
            //   Because of another architecture of table painting in Qt 5.15
            //   (initStyleOption is inside paint) it effectively disables cache.
            //|| option.text.isEmpty()) {
        log("Std drawing!!!!");
        nonCachingPaint(painter, option, index, aPainter);
        return;
    }

    // Changed widget — drop everything!
    if (option.widget != fWidget) {
        auto w = qobject_cast<const QTableView*>(option.widget);
        if (!w) {
            log("[TableCache:paint] Widget of unknown type!");
            nonCachingPaint(painter, option, index, aPainter);
            return;
        }
        drop();
        fWidget = w;
    }

    dropExceptMe();

    // Partial drop
    if (fWidget) {  // just for reliability
        // Leave ±N rows, ±M cols in cache
        enum { LEAVE_ROWS = 15, LEAVE_COLS = 2 };
        if (ir < fFirstRow || ir >= fLastRowPlus) {
            // Back for 1st
            int newFirst = ir;
            while (newFirst > 0) {
                int test = newFirst-1;
                int posY = fWidget->rowViewportPosition(test) + fWidget->rowHeight(test);
                if (posY <= 0)
                    break;
                newFirst = test;
            }
            // Forward for last
            int newLastPlus = ir + 1;
            int vph = fWidget->viewport()->height();
            int nRows = fWidget->model()->rowCount();
            for (; newLastPlus < nRows; ++newLastPlus) {
                int posY = fWidget->rowViewportPosition(newLastPlus);
                if (posY >= vph)
                    break;
            }
            // Set
            newFirst -= LEAVE_ROWS;
            newLastPlus += LEAVE_ROWS;
            //std::cout << Printf("Set rows {1}-{2} to {3}-{4}")
            //                   (fFirstRow)(fLastRowPlus)(newFirst)(newLastPlus) << std::endl;
            fFirstRow = newFirst;
            fLastRowPlus = newLastPlus;
            // Delete
            auto posFirst = cells.lower_bound({ fFirstRow, std::numeric_limits<int>::max() });
            cells.erase(cells.begin(), posFirst);
            auto posLast = cells.upper_bound({ fLastRowPlus, std::numeric_limits<int>::min() });
            cells.erase(posLast, cells.end());
        }
        if (ic < fFirstCol || ic >= fLastColPlus) {
            // Back for 1st
            int newFirst = ic;
            while (newFirst > 0) {
                int test = newFirst-1;
                int posY = fWidget->columnViewportPosition(test) + fWidget->columnWidth(test);
                if (posY <= 0)
                    break;
                newFirst = test;
            }
            // Forward for last
            int newLastPlus = ir + 1;
            int vpw = fWidget->viewport()->width();
            int nCols = fWidget->model()->columnCount();
            for (; newLastPlus < nCols; ++newLastPlus) {
                int posX = fWidget->columnViewportPosition(newLastPlus);
                if (posX >= vpw)
                    break;
            }
            // Set
            newFirst -= LEAVE_ROWS;
            newLastPlus += LEAVE_ROWS;
            //std::cout << Printf("Set cols {1}-{2} to {3}-{4}")
            //                   (fFirstCol)(fLastColPlus)(newFirst)(newLastPlus) << std::endl;
            fFirstCol = newFirst - LEAVE_COLS;
            fLastColPlus = newLastPlus + LEAVE_COLS;
            // Delete
            for (auto it = cells.begin(); it != cells.end(); ) {
                auto it2 = it; ++it2;
                if (it->first.second < fFirstCol || it->first.second >= fLastColPlus) {
                    cells.erase(it);
                }
                it = it2;
            }
        }
    }

    // Get cache cell
    Cell& cell = cells[{ ir, ic }];
    SizeAssortment sa(painter, option.rect);
    if (cell.state != option.state
            || cell.pix.width() != sa.bigWidth
            || cell.pix.height() != sa.bigHeight
            || cell.pix.devicePixelRatio() != sa.dpr) {
        cell.state = option.state;
        cell.pix = QPixmap { sa.bigSize() };

        drawAtPix(sa, cell.pix, option, index, aPainter);
        log("Cached cell");
        if (wantDebug) {
            wantDebug = false;
            char buf[1000];
            snprintf(buf, std::size(buf),
                     "orig rect=[%d*%d], new rect=[%d*%d], dpr=%g",
                     sa.smallWidth, sa.smallHeight, sa.bigWidth, sa.bigHeight, sa.dpr);
            QMessageBox::information(
                        const_cast<QTableView*>(fWidget), QString{"CellCache debug"}, QString{buf});
        }
    } else {
        log("Cell in cache!");
    }
    painter->drawPixmap(option.rect.topLeft(), cell.pix);
}
