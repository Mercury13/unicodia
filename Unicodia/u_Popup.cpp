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
        BAD,                ///< Cannot keep this
        ACCEPTABLE_TALL,    ///< Min. acceptable
        ACCEPTABLE,         ///< Better
        COOL                ///< The best ever
    };

    struct Info {
        int width;
        int height;
        Quality quality;  // [+] just acceptable

        /// @return [+] nothing more to strive for
        bool isCoolest() const { return (quality == Quality::COOL); }

        /// @return [+] at least everything is visible
        bool isAcceptable() const { return (quality > Quality::BAD); }

        bool isCoolerThan(const Info& other) const {
            return (isAcceptable()
                 && width <= other.width
                 && height <= other.height);
        }
    };

    constexpr bool TESTING_MODE = false;

    // All these values are in dip’s = device-independent pixels
    constexpr int MIN_CONTROLLED_HEIGHT = 350;  ///< shorter? → let it be auto
    constexpr int MIN_SHRUNK_WIDTH = 150;    ///< narrower? → stop
    constexpr int MIN_VARIABLE_WIDTH = 450;  ///< rather small because of e.g. “14.0”
    constexpr int MAX_READABLE_WIDTH = 850;  ///< do not exceed unless you have to because of short screen
    constexpr int COOL_HEIGHT = TESTING_MODE ? 300 : 650;
    constexpr int HEIGHT_LEEWAY = 35;     ///< subtract from screen size
    constexpr int WIDTH_LEEWAY = 20;      ///< subtract from screen size
    constexpr int WIDTH_PRECISION = 40;
    constexpr int PARAGRAPH_UNPACK = 25;  ///< leeway to prevent packed paragraphs
    static_assert(PARAGRAPH_UNPACK <= WIDTH_PRECISION);
    constexpr int PROBE_BACK_DELTA = 50;
    static_assert(PROBE_BACK_DELTA >= WIDTH_PRECISION);
    constexpr int REASONABLE_SIDE = 100;  // 100dip are always present :)
    constexpr int BAD_HEIGHT = -9999;

    inline int toReasonable(int x)
        { return std::max(x, REASONABLE_SIDE);  }

    class Qualimeter {
    public:
        Qualimeter(WiAdjust* aMe, const QSize& screenSize)
            : me(aMe),
              acceptableHeight(toReasonable(screenSize.height() - HEIGHT_LEEWAY)),
              coolHeight(std::min(COOL_HEIGHT, acceptableHeight)),
              fStrictMinWidth(me->minimumWidth() + PARAGRAPH_UNPACK) {}
        inline Quality qualityOf(int w, int h) const;
        Info infoOf(int aWidth) const;
        /// Switches to “unreadable” mode that never says “cool”
        void switchToUnreadable() { coolHeight = BAD_HEIGHT; }
        /// @return [+] we are in “unreadable” mode that’s automatically uncool
        bool isUnreadable() const { return (coolHeight < 0); }
        Info makeNiceInfo(const Info& minInfo, const Info& maxInfo) const;
        int strictMinWidth() const { return fStrictMinWidth; }
        Info probeBack(const Info& startingInfo, int minWidth) const;
    private:
        WiAdjust* const me;
        const int acceptableHeight;
        int coolHeight = BAD_HEIGHT;
        int fStrictMinWidth = 0;

        static bool isTall(int w, int h);
    };

    bool Qualimeter::isTall(int w, int h)
    {
        [[assume(w >= 0)]];
        [[assume(h >= 0)]];
        return (3 * h > 4 * w);  // 1.33
    }

    Quality Qualimeter::qualityOf(int w, int h) const
    {
        if (h > acceptableHeight || w < fStrictMinWidth)
            return Quality::BAD;
        // now at least ACCEPTABLE_TALL

        if (isTall(w, h))
            return Quality::ACCEPTABLE_TALL;
        // now at least ACCEPTABLE

        return (h <= coolHeight) ? Quality::COOL : Quality::ACCEPTABLE;
    }

    Info Qualimeter::infoOf(int aWidth) const
    {
        auto h = me->layout()->heightForWidth(aWidth);
        return {
            .width = aWidth,
            .height = h,
            .quality = qualityOf(aWidth, h),
        };
    }

    ///
    /// Situation: we suspect that there’s just a table
    ///
    Info Qualimeter::probeBack(const Info& startingInfo, int maxWidth) const
    {
        int minWidth = std::max(MIN_SHRUNK_WIDTH, strictMinWidth());
        Info currInfo = startingInfo;
        // Shrink, but only while the window has the same height
        while (currInfo.width > minWidth) {
            auto newWidth = std::max(currInfo.width - WIDTH_PRECISION, minWidth);
            auto newInfo = infoOf(newWidth);
            if (newInfo.height > startingInfo.height)
                break;
            currInfo = newInfo;
        }
        // Fall back and unpack paragraphs
        auto finalWidth = currInfo.width + PARAGRAPH_UNPACK;
        if (finalWidth < maxWidth) {
            return infoOf(finalWidth);
        }
        return startingInfo;
    }

    ///
    /// \brief Qualimeter::makeNiceInfo
    ///    Got a situation: both minInfo and maxInfo make the same height
    ///    Probably it means that in minInfo the paragraphs are tightly packed,
    ///    and it’s bad.
    ///
    Info Qualimeter::makeNiceInfo(const Info& minInfo, const Info& maxInfo) const
    {
        // Unreadable mode → return smaller
        if (isUnreadable())
            return minInfo;
        // Difference is small → return bigger
        if (maxInfo.width - minInfo.width <= WIDTH_PRECISION)
            return maxInfo;
        // Otherwise something in between
        auto newInfo = infoOf(minInfo.width + PARAGRAPH_UNPACK);
        if (newInfo.height <= minInfo.height)  // check once again
            return newInfo;
        return minInfo;
    }

    Info bisectBest(WiAdjust* me, const QSize& screenSize)
    {
        Qualimeter meter(me, screenSize);

        // Min info
        const auto maxWidth = toReasonable(screenSize.width() - WIDTH_LEEWAY);
        auto minWidth = std::min(maxWidth, std::max(meter.strictMinWidth(), MIN_VARIABLE_WIDTH));
        auto minInfo = meter.infoOf(minWidth);
        if (minInfo.isCoolest()) {
            if (me->width() < minInfo.width) {  // If our auto size is smaller than cool
                auto autoInfo = meter.infoOf(me->width());
                // Is auto just cooler? — e.g. some tables when we increase width, height is the same
                if (autoInfo.isCoolerThan(minInfo))
                    return autoInfo;
            }
            return meter.probeBack(minInfo, minWidth);
        }

        // Step as big as PROBE_BACK_DELTA does not change → try
        auto probeInfo = meter.infoOf(minInfo.width - PROBE_BACK_DELTA);
        if (probeInfo.isAcceptable() && probeInfo.height <= minInfo.height) {
            return meter.probeBack(probeInfo, minWidth);
        }

        // Max info
        const auto stillReadableWidth = std::min(MAX_READABLE_WIDTH, maxWidth);
        // Can we ever make readable width and any height?
        auto maxInfo = meter.infoOf(stillReadableWidth);
        // Same: min is just cooler because of e.g. tables
        if (minInfo.isCoolerThan(maxInfo))
            return minInfo;
        if (!maxInfo.isAcceptable()) {   // Max info is acceptable, otherwise…            
            if (maxInfo.width >= maxWidth)  // Width exceeded → do what you want
                return maxInfo;
            // Switch to stillReadableWidth..maxWidth
            meter.switchToUnreadable();
            minInfo = maxInfo;
            maxInfo = meter.infoOf(maxWidth);
            if (!maxInfo.isAcceptable())  // Still bad → do what you want
                return maxInfo;
        }

        // Again check
        if (maxInfo.height >= minInfo.height)
            return minInfo;

        // Check quality: do we need a thing to chase for?
        if (maxInfo.quality <= minInfo.quality) {
            // At least they are both acceptable
            // Max info is in the (roughly) same condition too
            // Unacceptable → return max: maybe it’s more acceptable, should not happen
            // Tall → return max: maybe it’s not so tall
            if (minInfo.quality <= Quality::ACCEPTABLE_TALL)
                return maxInfo;
            // Otherwise just min, mostly in unreadable mode
            // when both are acceptable
            return minInfo;
        }

        // And check!
        if (minInfo.isCoolerThan(maxInfo))
            return meter.makeNiceInfo(minInfo, maxInfo);

        // Max info is better than min
        auto targetQuality = maxInfo.quality;
        while (maxInfo.width - minInfo.width > WIDTH_PRECISION) {
            auto medWidth = (minInfo.width + maxInfo.width) >> 1;
            auto medInfo = meter.infoOf(medWidth);
            if (medInfo.quality >= targetQuality) {
                maxInfo = medInfo;
            } else {
                minInfo = medInfo;
            }
            if (minInfo.isCoolerThan(maxInfo))
                return meter.makeNiceInfo(minInfo, maxInfo);
        }
        // maxInfo has targetquality, min doesn’t
        return maxInfo;
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
