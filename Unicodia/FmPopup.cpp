// My header
#include "FmPopup.h"

// Qt
#include <QLabel>
#include <QApplication>
#include <QScreen>
#include <QLayout>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>

namespace {
    enum class PopupMode {
        NATIVE,         ///< Use native popup window (default)
        ARTIFICIAL      ///< Simulate popup with other means
    };
    constexpr auto popupMode = PopupMode::NATIVE;

    constexpr auto WF_POPUP = (popupMode == PopupMode::ARTIFICIAL)
            ? Qt::FramelessWindowHint | Qt::Tool
            : Qt::Popup;
}


///// ClickableLabel ///////////////////////////////////////////////////////////


ClickableLabel::ClickableLabel(const QString& text, QWidget* owner)
    : Super(text, owner)
{
    connect(this, &QLabel::linkActivated, this, &This::onLinkActivated);
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
        if (sel.length == 0)
            emit clicked();
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



///// FmPopup //////////////////////////////////////////////////////////////////


FmPopup::FmPopup(QWidget* owner, const char* color)
    : Super(owner, WF_POPUP), fOwner(owner)
{
    if (!owner)
        throw std::invalid_argument("[FmPopup] Need owner widget");
    auto vl = new QVBoxLayout(this);
    setLayout(vl);
    if constexpr (popupMode == PopupMode::ARTIFICIAL) {
        vl->setContentsMargins(10, 10, 10, 10);
    } else {
        vl->setContentsMargins(1, 1, 1, 1);
    }

    lbText = new ClickableLabel("[QLabel]", this);
    lbText->setWordWrap(true);
    vl->addWidget(lbText);

    if constexpr (popupMode == PopupMode::ARTIFICIAL) {
        setAttribute(Qt::WA_TranslucentBackground); //enable window to be transparent
        //setWindowModality(Qt::ApplicationModal);
        QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect();
        effect->setXOffset(2);
        effect->setYOffset(2);
        effect->setBlurRadius(6);
        lbText->setGraphicsEffect(effect);
    }

    setStyleSheet("QWidget { background-color: black }");
    char buf[100];
    snprintf(buf, std::size(buf),
             "QLabel { background-color: %s; padding: 10px; }",
             color);
    lbText->setStyleSheet(buf);
    connect(lbText, &ClickableLabel::clicked, this, &This::hide);
}


FmPopup& FmPopup::setText(const QString& x)
{
    lbText->setText(x);
    return *this;
}


namespace {

    void eatRightMargin(QRect& rect, int mainMargin, int auxMargin)
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

    void eatBottomMargin(QRect& rect, int margin)
    {
        margin = std::min(margin, rect.height() / 2);
        rect.setHeight(rect.height() - margin);
    }

}   // anon namespace

constexpr int POPUP_RIGHT_MARGIN = 8;
constexpr int POPUP_LEFT_MARGIN = 1;
constexpr int POPUP_BOTTOM_MARGIN = 3;

void FmPopup::popupAtY(
        const QRect& hotspotAbsRect,
        QRect ownerRect,
        const QRect& screenRect,
        int y)
{
    // Modify ownerRect
    eatRightMargin(ownerRect, POPUP_RIGHT_MARGIN, POPUP_LEFT_MARGIN);

    auto myW = width();    
    QRect testRect = (myW <= ownerRect.width()) ? ownerRect : screenRect;
    auto x = std::min(hotspotAbsRect.left(), testRect.right() - myW);
    x = std::max(x, testRect.left());
    move(x, y);
    show();

    lbText->setFocus();
    if constexpr (popupMode == PopupMode::ARTIFICIAL) {
        setFocus();
    }
}


FmPopup& FmPopup::popupAtScreen(QScreen* screen, const QRect& absRect)
{
    adjustSize();
    auto screenRect = screen->availableGeometry();
    auto ownerRect = fOwner->geometry().intersected(screenRect);
    eatBottomMargin(ownerRect, POPUP_BOTTOM_MARGIN);

    auto myH = height();
    if (auto bottomRemainder = ownerRect.bottom() - absRect.bottom();
            bottomRemainder >= myH) {
        // Try on the bottom
        popupAtY(absRect, ownerRect, screenRect, absRect.bottom());
    } else if (auto topRemainder = absRect.top() - ownerRect.top();
            topRemainder >= myH) {
        // Try on the top
        popupAtY(absRect, ownerRect, screenRect, absRect.top() - myH);
    } else if (myH <= ownerRect.height()) {
        // Try stuck to owner rect
        popupAtY(absRect, ownerRect, screenRect, ownerRect.bottom() - myH);
    } else {
        // Try everywhere
        const auto maxY = screenRect.bottom() - myH;
        auto myY = ownerRect.top() + (ownerRect.height() - myH) / 2;
        if (myY > maxY) myY = maxY;
        if (myY < 0) myY = 0;
        popupAtY(absRect, ownerRect, screenRect, myY);
    }
    return *this;
}

FmPopup& FmPopup::popupAtAbs(QWidget* widget, const QRect& absRect)
{
    if (!widget)
        throw std::invalid_argument("[FmPopup.popupAtAbs] Widget should be non-null!");
    fLastAbsRect = absRect;
    fLastWidget = widget;
    auto screen = QApplication::screenAt(absRect.center());
    if (!screen) {
        screen = widget->screen();
        if (!screen)
            screen = QApplication::primaryScreen();
    }
    popupAtScreen(screen, absRect);
    return *this;
}


FmPopup& FmPopup::popupAtAbsBacked(QWidget* widget, const QRect& absRect)
{
    if (widget) {
        return popupAtAbs(widget, absRect);
    } else {
        return popupAtAbs(fLastWidget, fLastAbsRect);
    }
}


FmPopup& FmPopup::popup(QWidget* widget, TinyOpt<QRect> rect)
{
    if (rect) {
        return popupAtAbs(widget, QRect(
                    widget->mapToGlobal(rect->topLeft()), rect->size()));
    } else if (!widget) {
        return popupAtAbs(fLastWidget, fLastAbsRect);
    } else {
        return popup(widget);
    }
}


FmPopup& FmPopup::popup(QWidget* widget)
{
    return popupAtAbs(widget, QRect(
                widget->mapToGlobal(QPoint(0, 0)), widget->size()));
}


void FmPopup::focusOutEvent(QFocusEvent* ev)
{
    Super::focusOutEvent(ev);
    hide();
}


void FmPopup::mouseReleaseEvent(QMouseEvent* ev)
{
    Super::mouseReleaseEvent(ev);
    hide();
}
