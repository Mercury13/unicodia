// My header
#include "FmMessage.h"

// Qt
#include <QLabel>
#include <QGridLayout>
#include <QPropertyAnimation>
#include <QTimer>
#include <QPainter>
#include <QApplication>
#include <QScreen>

constexpr auto FGS_MESSAGE =
        Qt::Tool
        | Qt::FramelessWindowHint
        | Qt::WindowStaysOnTopHint
        | Qt::NoDropShadowWindowHint
        | Qt::WindowTransparentForInput
        | Qt::WindowDoesNotAcceptFocus;

constexpr auto STY_LABEL =
        "QLabel { color : white; "
        "font-size: 10pt; "
        "margin-top: 6px; "
        "margin-bottom: 6px; "
        "margin-left: 10px; "
        "margin-right: 10px; }";

constexpr auto ALPHA_DEFAULT = 180;
constexpr auto CORNER_RADIUS = 10;
constexpr auto SHADOW_GAP = 5;

constexpr auto TIME_APPEAR = 150;
constexpr auto TIME_STAY = 1500;
constexpr auto TIME_DISAPPEAR = 1000;


FmMessage::FmMessage(QWidget* parent)
    : Super(parent, FGS_MESSAGE)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);

    // Label
    label = std::make_unique<QLabel>(this);
    label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    label->setStyleSheet(STY_LABEL);

    // Layout
    layout = std::make_unique<QGridLayout>(this);
    layout->addWidget(label.get(), 0, 0);
    setLayout(layout.get());

    // Animation
    animation = std::make_unique<QPropertyAnimation>(this);
    animation->setTargetObject(this);
    animation->setPropertyName("popupOpacity");
    connect(animation.get(), &QAbstractAnimation::finished, this, &This::finalHide);

    // Timer
    timer = std::make_unique<QTimer>(this);
    connect(timer.get(), &QTimer::timeout, this, &This::hideAnimation);
}

// Dtors of partly-defined unique_ptr’s here!!
FmMessage::~FmMessage() = default;


void FmMessage::setPopupOpacity(float x)
{
    fPopupOpacity = x;
    setWindowOpacity(x);
}


void FmMessage::show()
{
    setWindowOpacity(0.0);

    animation->setDuration(TIME_APPEAR);
    animation->setStartValue(0.0);
    animation->setEndValue(1.0);
    Super::show();

    animation->start();
    timer->start(TIME_STAY);
}


void FmMessage::hideAnimation()
{
    timer->stop();
    animation->setDuration(TIME_DISAPPEAR);
    animation->setStartValue(1.0);
    animation->setEndValue(0.0);
    animation->start();
}


void FmMessage::finalHide()
{
    if(popupOpacity() == 0.0) {
        hide();
    }
}

void FmMessage::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect roundedRect;
    roundedRect.setX(rect().x() + SHADOW_GAP);
    roundedRect.setY(rect().y() + SHADOW_GAP);
    roundedRect.setWidth(rect().width() - SHADOW_GAP * 2);
    roundedRect.setHeight(rect().height() - SHADOW_GAP * 2);

    painter.setBrush(QBrush(QColor(0,0,0, ALPHA_DEFAULT)));
    painter.setPen(Qt::NoPen);

    painter.drawRoundedRect(roundedRect, CORNER_RADIUS, CORNER_RADIUS);
}


void FmMessage::setPopupText(const QString &text)
{
    label->setText(text);
    adjustSize();
}


void FmMessage::setY(const QRect& globalRect, const QRect& screenRect, int y)
{
    auto myW = width();
    auto x = std::min(globalRect.left() + (globalRect.width() - myW) / 2,
                      screenRect.right() - myW);
    x = std::max(x, 0);
    move(x, y);
}


void FmMessage::moveTo(const QWidget* widget, const QRect& globalRect)
{
    auto screen = QApplication::screenAt(globalRect.center());
    if (!screen) {
        screen = widget->screen();
        if (!screen)
            screen = QApplication::primaryScreen();
    }

    auto screenRect = screen->availableGeometry();
    auto myH = height();
    if (auto bottomRemainder = screenRect.bottom() - globalRect.bottom();
            bottomRemainder >= myH) {
        // Try on the bottom
        setY(globalRect, screenRect, globalRect.bottom());
    } else if (auto topRemainder = globalRect.top() - screenRect.top();
            topRemainder >= myH) {
        // Try on the top
        setY(globalRect, screenRect, globalRect.top() - myH);
    } else {
        // Try everywhere
        setY(globalRect, screenRect, screenRect.bottom() - myH);
    }
}


void FmMessage::showAtAbs(const QString& text, const QWidget* widget, const QRect& globalRect)
{
    setPopupText(text);
    moveTo(widget, globalRect);
    show();
}
