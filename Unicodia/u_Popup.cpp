#include "u_Popup.h"

// Qt
#include <QApplication>
#include <QLayout>
#include <QScreen>

void pop::checkOwner(QWidget* owner)
{
    if (!owner)
        throw std::invalid_argument("[MxPopup] Need owner widget");
}

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


void pop::myAdjustSize(QWidget* me, const QRect& screenRect)
{
    // My params
    static constexpr int MAX_WIDTH = 900;       // maximum (non really tight) width
    static constexpr int WIDTH_STEP = 50;
    static constexpr int COOL_WIDTH = 450;
    static constexpr int COOL_HEIGHT = 650;
    static constexpr int HEIGHT_LEEWAY = 50;
    static constexpr int WIDTH_LEEWAY = 20;

    me->adjustSize();
    if (me->height() >= 350) {
        auto maxWidth = std::min(MAX_WIDTH, screenRect.width() - WIDTH_LEEWAY);
        auto rqHeight = std::min(COOL_HEIGHT, screenRect.height() - HEIGHT_LEEWAY);
        auto myW = std::max(me->width(), COOL_WIDTH - WIDTH_STEP);
        int h;
        do {
            // Took 2nd (maxWidth) → will surely break!
            myW = std::min(myW + WIDTH_STEP, maxWidth);
            h = me->layout()->heightForWidth(myW);
        } while (h > rqHeight && myW < maxWidth);
        me->resize(myW, h);
    }
}


void pop::popupAtScreen(
        QWidget* me, QWidget* owner, QScreen* screen, const QRect& absRect)
{
    auto screenRect = screen->availableGeometry();
    me->hide();  // if shown
    myAdjustSize(me, screenRect);
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
