#include "u_Popup.h"

// Qt
#include <QApplication>
#include <QLayout>
#include <QScreen>
#include <QMouseEvent>

///// ClickableLabel ///////////////////////////////////////////////////////////


ClickableLabel::ClickableLabel( QWidget* owner) : Super(owner)
{
    init();
}


ClickableLabel::ClickableLabel(const QString& text, QWidget* owner)
    : Super(text, owner)
{
    init();
}

void ClickableLabel::init()
{
    connect(this, &This::linkActivated, this, &This::onLinkActivated);
}


ClickableLabel::Selection ClickableLabel::selection()
{
    Selection sel { selectionStart(), selectedText().length() };
    if (sel.length == 0)
        sel.start = -1;
    return sel;
}


void ClickableLabel::mousePressEvent(QMouseEvent* ev)
{
    allowClose = (ev->button() == Qt::LeftButton && selection().length == 0);
    Super::mousePressEvent(ev);
}


void ClickableLabel::mouseReleaseEvent(QMouseEvent* ev)
{
    if (ev->button() == Qt::RightButton)
        allowClose = false;

    Super::mouseReleaseEvent(ev);

    if (allowClose) {
        auto sel = selection();
        if (sel.length == 0) {
            emit clicked();
        }
    }
}


void ClickableLabel::onLinkActivated()
{
    allowClose = false;
}


bool ClickableLabel::event(QEvent* ev)
{
    switch (ev->type()) {
    case QEvent::Enter:
        emit mouseEnter();
        break;
    case QEvent::Leave:
        emit mouseLeave();
        break;
    default: ;
    }
    return Super::event(ev);
}


///// ns POP ///////////////////////////////////////////////////////////////////


void pop::eatRightMargin(QRect& rect, int mainMargin, int auxMargin)
{
    mainMargin = std::min(mainMargin, rect.width() / 2);
    rect.setWidth(rect.width() - mainMargin);
    if (QApplication::layoutDirection() == Qt::RightToLeft) {
        // ←
        rect.moveLeft(rect.left() + mainMargin);
        rect.moveRight(rect.right() - auxMargin);
    } else {
        // →
        rect.moveLeft(rect.left() + auxMargin);
    }
}

void pop::eatBottomMargin(QRect& rect, int margin)
{
    margin = std::min(margin, rect.height() / 2);
    rect.setHeight(rect.height() - margin);
}

void pop::popupAtY(
        QWidget* me,
        const QRect& hotspotAbsRect,
        QRect ownerRect,
        const QRect& screenRect,
        int y)
{
    // Modify ownerRect
    pop::eatRightMargin(ownerRect, pop::RIGHT_MARGIN, pop::LEFT_MARGIN);

    auto myW = me->width();
    QRect testRect = (myW <= ownerRect.width()) ? ownerRect : screenRect;
    auto x = std::min(hotspotAbsRect.left(), testRect.right() - myW);
    x = std::max(x, testRect.left());
    me->move(x, y);
    me->show();
}


namespace bi {

    enum class Quality : unsigned char {
        BAD,            ///< Cannot keep this
        ACCEPTABLE_TALL,
        ACCEPTABLE,
        COOL
    };

    struct Info {
        int width;
        int height;
        Quality quality;  // [+] just acceptable

        bool isCoolest() const { return (quality == Quality::COOL); }
        bool isAcceptable() const { return (quality >= Quality::ACCEPTABLE_TALL); }

        bool isCoolerThan(const Info& other) const {
            return (width <= other.width
                 && height <= other.height);
        }
    };

    constexpr bool TESTING_MODE = false;

    constexpr int MIN_CONTROLLED_HEIGHT = 350;
    constexpr int MIN_VARIABLE_WIDTH = 450;
    constexpr int MAX_READABLE_WIDTH = 850;  // do not exceed unless you have to because of short screen
    constexpr int COOL_HEIGHT = TESTING_MODE ? 300 : 625;
    constexpr int HEIGHT_LEEWAY = 35;
    constexpr int WIDTH_LEEWAY = 20;
    constexpr int WIDTH_PRECISION = 40;

    inline Quality qualityOf(int w, int h, int acceptableHeight, int coolHeight)
    {
        if (h <= coolHeight) {
            return Quality::COOL;
        }
        if (h <= acceptableHeight) {
            return (2 * h > 3 * w) ? Quality::ACCEPTABLE_TALL
                                   : Quality::ACCEPTABLE;
        }
        return Quality::BAD;
    }

    Info bisectBest(WiAdjust* me, const QSize& screenSize) {
        const auto acceptableHeight = screenSize.height() - HEIGHT_LEEWAY;
        const auto coolHeight = std::min(COOL_HEIGHT, acceptableHeight);

        auto infoFor = [me, acceptableHeight, coolHeight](int aWidth) -> Info {
            auto h = me->layout()->heightForWidth(aWidth);
            return {
                .width = aWidth,
                .height = h,
                .quality = qualityOf(aWidth, h, acceptableHeight, coolHeight),
            };
        };

        // Min info
        const auto maxWidth = std::max(screenSize.width() - WIDTH_LEEWAY, 100);  // 100dip are always present :)
        auto minWidth = std::min(maxWidth, MIN_VARIABLE_WIDTH);
        auto minInfo = infoFor(minWidth);
        if (me->width() < minInfo.width) {  // If our auto size is smaller than cool
            auto autoInfo = infoFor(me->width());
            if (autoInfo.isCoolest() && autoInfo.isCoolerThan(minInfo))  // is auto just cooler?
                return autoInfo;
        }
        if (minInfo.isCoolest())   // If min info is just cool → just return
            return minInfo;

        // Max info
        const auto stillReadableWidth = std::min(MAX_READABLE_WIDTH, maxWidth);
        // The 1st iteration is on COOL_WIDTH..stillComfortableWidth
        auto maxInfo = infoFor(stillReadableWidth);
        if (minInfo.isCoolerThan(maxInfo))
            return minInfo;
        if (!maxInfo.isAcceptable()) {   // Max info is acceptable, otherwise…
            if (maxInfo.width >= maxWidth)  // Width exceeded → do what you want
                return maxInfo;
            // Switch to stillConfortableWidth..maxWidth
            minInfo = maxInfo;
            maxInfo = infoFor(maxWidth);
            if (!maxInfo.isAcceptable())  // Still bad → do what you want
                return maxInfo;
        }

        // Again check
        if (maxInfo.height >= minInfo.height)
            return minInfo;

        // Check quality: do we need a thing to chase for?
        if (maxInfo.quality <= minInfo.quality) {
            // Should not happen
            if (maxInfo.quality < minInfo.quality) {
                return minInfo;
            }
            return maxInfo;
        }

        // Max info is better than min
        auto targetQuality = maxInfo.quality;
        while (maxInfo.width - minInfo.width > WIDTH_PRECISION) {
            auto medWidth = (minInfo.width + maxInfo.width) >> 1;
            auto medInfo = infoFor(medWidth);
            if (medInfo.quality >= targetQuality) {
                maxInfo = medInfo;
            } else {
                minInfo = medInfo;
            }
        }
        return (minInfo.isCoolerThan(maxInfo)) ? minInfo : maxInfo;
    }

}   // anon namespace


void pop::myAdjustSize(WiAdjust* me, const QSize& screenSize)
{
    // My params

    me->adjustSize();
    auto h = me->height();
    if (h >= bi::MIN_CONTROLLED_HEIGHT) {
        auto bestInfo = bi::bisectBest(me, screenSize);
        me->resize(bestInfo.width, bestInfo.height);
    }
}


void pop::popupAtScreen(
        WiAdjust* me, QWidget* owner, QScreen* screen, const QRect& absRect)
{
    auto screenRect = screen->availableGeometry();
    me->hide();  // if shown
    myAdjustSize(me, screenRect.size());
    auto ownerRect = owner->geometry().intersected(screenRect);
    pop::eatBottomMargin(ownerRect, pop::BOTTOM_MARGIN);

    auto myH = me->height();
    if (auto bottomRemainder = ownerRect.bottom() - absRect.bottom();
            bottomRemainder >= myH) {
        // Try on the bottom
        popupAtY(me, absRect, ownerRect, screenRect, absRect.bottom());
    } else if (auto topRemainder = absRect.top() - ownerRect.top();
            topRemainder >= myH) {
        // Try on the top
        popupAtY(me, absRect, ownerRect, screenRect, absRect.top() - myH);
    } else if (myH <= ownerRect.height()) {
        // Try stuck to owner rect
        popupAtY(me, absRect, ownerRect, screenRect, ownerRect.bottom() - myH);
    } else {
        // Try everywhere
        const auto maxY = screenRect.bottom() - myH;
        auto myY = ownerRect.top() + (ownerRect.height() - myH) / 2;
        if (myY > maxY) myY = maxY;
        if (myY < 0) myY = 0;
        popupAtY(me, absRect, ownerRect, screenRect, myY);
    }
}


QScreen* pop::findScreen(QWidget* widget, const QRect& absRect)
{
    // By coords
    if (auto screen = QApplication::screenAt(absRect.center())) {
        return screen;
    }
    // By widget
    if (auto screen = widget->screen()) {
        return screen;
    }
    // Somehow
    return QApplication::primaryScreen();
}


///// WiPopup //////////////////////////////////////////////////////////////////


WiPopup::WiPopup(QWidget* aOwner, pop::ClickMemory& aMemory)
    : Super(aOwner, WF_POPUP), fOwner(aOwner), fMemory(aMemory)
{
    if (!aOwner)
        throw std::invalid_argument("[WiPopup] Need owner widget");
}


void WiPopup::adjustAfterPopup()
{
    mySetFocus();
    if constexpr (popupMode == PopupMode::ARTIFICIAL) {
        setFocus();
    }
}

void WiPopup::focusOutEvent(QFocusEvent* ev)
{
    Super::focusOutEvent(ev);
    hide();
}


void WiPopup::mouseReleaseEvent(QMouseEvent* ev)
{
    Super::mouseReleaseEvent(ev);
    hide();
}

void WiPopup::popupAtAbs(QWidget* widget, const QRect& absRect)
{
    if (!widget)
        throw std::invalid_argument("[MxPopup.popupAtAbs] Widget should be non-null!");
    fMemory.lastAbsRect = absRect;
    fMemory.lastWidget = widget;
    auto screen = pop::findScreen(widget, absRect);
    popupAtScreen(screen, absRect);
}

void WiPopup::popupAtAbsBacked(QWidget* widget, const QRect& absRect)
{
    if (widget) {
        popupAtAbs(widget, absRect);
    } else {
        popupAtAbs(fMemory.lastWidget, fMemory.lastAbsRect);
    }
}

void WiPopup::popup(QWidget* widget)
{
    popupAtAbs(widget, QRect(
               widget->mapToGlobal(QPoint(0, 0)), widget->size()));
}

void WiPopup::popup(QWidget* widget, TinyOpt<QRect> rect)
{
    if (rect) {
        popupAtAbs(widget, QRect(
                    widget->mapToGlobal(rect->topLeft()), rect->size()));
    } else if (!widget) {
        popupAtAbs(fMemory.lastWidget, fMemory.lastAbsRect);
    } else {
        popup(widget);
    }
}
