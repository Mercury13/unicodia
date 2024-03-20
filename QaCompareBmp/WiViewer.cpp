// My header
#include "WiViewer.h"

// Qt
#include <QPainter>

void WiViewer::removePix()
{
    pix = QPixmap{};
}


void WiViewer::setPix(const QPixmap& x)
{
    pix = x;
    update();
}


void WiViewer::setPix(QPixmap&& x)
{
    pix = std::move(x);
    update();
}


void WiViewer::paintEvent(QPaintEvent*)
{
    // Connot draw empty pix
    if (pix.width() == 0 || pix.height() == 0)
        return;

    QPainter painter(this);
    int szX = width() / pix.width();
    int szY = height() / pix.height();
    auto sz = std::min(szX, szY);
    if (sz <= 1) {
        painter.drawPixmap(0, 0, pix);
    } else {
        painter.drawPixmap(0, 0, pix.width() * sz, pix.height() * sz, pix);
    }
}
