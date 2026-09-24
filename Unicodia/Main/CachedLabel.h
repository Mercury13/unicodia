#ifndef CACHEDLABEL_H
#define CACHEDLABEL_H

// STL
#include "unordered_map"

#include <QLabel>
#include <QPixmap>

class CachedLabel : public QLabel
{
private:
    using Super = QLabel;
public:
    using Super::Super;
    bool isCached() const noexcept { return fIsCached; }
    /// @warning  Redraws nothing!
    void setCached(bool x) { fIsCached = x; }
protected:
    struct CacheEntry {
        QPixmap pix;
        QRect srcRect() const;
    };
    void paintEvent(QPaintEvent* ev) override;
    QSize deviceSize() const noexcept;
    bool isOk(CacheEntry& x);
private:
    bool fIsCached = false;
    struct Total {
        QColor fg, bg;
    } total;
    std::unordered_map<QString, CacheEntry> cache;
    void checkForTotalInvalidation();
};

#endif // CACHEDLABEL_H
