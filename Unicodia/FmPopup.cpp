// My header
#include "FmPopup.h"

// Qt
#include <QLayout>
#include <QGraphicsDropShadowEffect>


///// FmPopup //////////////////////////////////////////////////////////////////


FmPopup::FmPopup(QWidget* owner, pop::ClickMemory& memory, const char* color)
    : Super(owner, memory)
{
    layout = new QVBoxLayout(this);
    setLayout(layout);
    if constexpr (popupMode == PopupMode::ARTIFICIAL) {
        layout->setContentsMargins(10, 10, 10, 10);
    } else {
        layout->setContentsMargins(0, 0, 0, 0);
    }

    lbText = new ClickableLabel("[QLabel]", this);
    lbText->setWordWrap(true);
    layout->addWidget(lbText);

    if constexpr (popupMode == PopupMode::ARTIFICIAL) {
        setAttribute(Qt::WA_TranslucentBackground); //enable window to be transparent
        //setWindowModality(Qt::ApplicationModal);
        QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect();
        effect->setXOffset(2);
        effect->setYOffset(2);
        effect->setBlurRadius(6);
        lbText->setGraphicsEffect(effect);
    }

    char buf[100];
    snprintf(buf, std::size(buf),
             "QLabel { background-color: %s; padding: 10px; border: 1px solid black }",
             color);
    lbText->setStyleSheet(buf);
    connect(lbText, &ClickableLabel::clicked, this, &This::hide);
}


void FmPopup::mySetFocus()
{
    lbText->setFocus();
}


FmPopup& FmPopup::setText(const QString& x)
{
    lbText->setText(x);
    return *this;
}


void FmPopup::deselectLink()
{
    lbText->setSelection(0, 0);
}
