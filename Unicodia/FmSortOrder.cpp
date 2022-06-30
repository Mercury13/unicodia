#include "FmSortOrder.h"
#include "ui_FmSortOrder.h"

// Qt
#include <QScreen>

FmSortOrder::FmSortOrder(QWidget *parent) :
    QWidget(parent, Qt::Popup),
    ui(new Ui::FmSortOrder)
{
    ui->setupUi(this);
}

FmSortOrder::~FmSortOrder()
{
    delete ui;
}


void FmSortOrder::popupAtY(
        const QRect& hotspotAbsRect,
        const QRect& screenRect,
        int y)
{
    // Modify ownerRect
    auto myW = width();
    auto x = std::min(hotspotAbsRect.left(), screenRect.right() - myW);
    x = std::max(x, screenRect.left());
    move(x, y);
    show();

    /// @todo [urgent] set some focus?
}


FmSortOrder& FmSortOrder::popupAtScreen(QScreen* screen, const QRect& absRect)
{
    /// @todo [repeat] u_PopupGeometry here?
    adjustSize();
    auto screenRect = screen->availableGeometry();

    auto myH = height();
    if (auto bottomRemainder = screenRect.bottom() - absRect.bottom();
            bottomRemainder >= myH) {
        // Try on the bottom
        popupAtY(absRect, screenRect, absRect.bottom());
    } else if (auto topRemainder = absRect.top() - screenRect.top();
            topRemainder >= myH) {
        // Try on the top
        popupAtY(absRect, screenRect, absRect.top() - myH);
    } else {
        // Try stuck to owner rect
        popupAtY(absRect, screenRect, screenRect.bottom() - myH);
    }
    return *this;
}
