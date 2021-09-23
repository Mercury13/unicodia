// My header
#include "c_TableCache.h"

// Qt
#include <QPainter>

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
                // Selected/focused — just handpaint, less glitches
            || (option.state & (QStyle::State_Selected | QStyle::State_HasFocus))
                ) {
            //   Used to be (the trouble is typography engine → do not cache empty strings)
            //   Because of another architecture of table painting in Qt 5.15
            //   (initStyleOption is inside paint) it effectively disables cache.
            //|| option.text.isEmpty()) {
        log("Std drawing!!!!");
        aPainter.paintItem(painter, option, index);
        return;
    }

    // Changed widget — drop everything!
    if (option.widget != fWidget) {
        auto w = qobject_cast<const QTableView*>(option.widget);
        if (!w) {
            log("[TableCache:paint] Widget of unknown type!");
            aPainter.paintItem(painter, option, index);
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
    const int cw = option.rect.width();
    const int ch = option.rect.height();
    if (cell.state != option.state || cell.pix.width() != cw || cell.pix.height() != ch) {
        QStyleOptionViewItem option2 = option;
        option2.rect = QRect{ 0, 0, cw, ch };
        cell.state = option.state;
        cell.pix = QPixmap { cw, ch };
        if (option.backgroundBrush.style() != Qt::SolidPattern)
            cell.pix.fill( option.widget->palette().base().color() );
        QPainter painter2 { &cell.pix };

        aPainter.paintItem( &painter2, option2, index );
        log("Cached cell");
    } else {
        log("Cell in cache!");
    }
    painter->drawPixmap(option.rect.topLeft(), cell.pix);
}
