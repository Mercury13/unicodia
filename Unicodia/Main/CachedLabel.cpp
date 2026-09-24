#include "CachedLabel.h"

#include <QPainter>

#ifdef _WIN32
    #include <windows.h>
#endif


QRect CachedLabel::CacheEntry::srcRect() const
{
    return QRect { 0, 0, pix.width(), pix.height() };
}


QSize CachedLabel::deviceSize() const noexcept
{
#ifdef _WIN32
    RECT r;
    GetClientRect(reinterpret_cast<HWND>(winId()), &r);
    return { r.right - r.left, r.bottom - r.top };
#else
    QSize sz = size();
    return { lround(sz.width() * devicePixelRatioF()),
             lround(sz.height() * devicePixelRatioF()) };
#endif
}


void CachedLabel::checkForTotalInvalidation()
{
    auto pal = palette();
    if (total.bg != pal.button().color()
            || total.fg != pal.buttonText().color()) {
        total.bg = pal.button().color();
        total.fg = pal.buttonText().color();
        cache.clear();
    }
}


bool CachedLabel::isOk(CacheEntry& x)
{
    bool r = true;
    auto devSize = deviceSize();
    if (x.pix.size() != devSize) {
        x.pix = QPixmap(devSize);
        r = false;
    }
    return r;
}


void CachedLabel::paintEvent(QPaintEvent* ev)
{
    QString s = text();
    if (isCached() && !s.isEmpty()) {
        checkForTotalInvalidation();
        CacheEntry& whereIt = cache[s];
        auto dpr = devicePixelRatioF();
        if (!isOk(whereIt)) {
            // recache
            QPainter pai(&whereIt.pix);
            auto srcr = whereIt.srcRect();
            pai.fillRect(srcr, total.bg);
            // We always draw in the centre, let it be this way
            QFont fnt = font();
            fnt.setPointSizeF(fnt.pointSizeF() * dpr);
            pai.setFont(fnt);
            QTextOption opt(Qt::AlignHCenter | Qt::AlignVCenter);
            pai.drawText(srcr, s, opt);
        }
        // draw
        QRectF rcDest(0, 0, whereIt.pix.width() / dpr, whereIt.pix.height() / dpr);
        QPainter paiNew(this);
        paiNew.drawPixmap(rcDest, whereIt.pix, whereIt.srcRect());
    } else {
        Super::paintEvent(ev);
    }
}
