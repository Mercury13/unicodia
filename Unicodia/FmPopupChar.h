#ifndef FMPOPUPCHAR_H
#define FMPOPUPCHAR_H

#include <QWidget>

// Project-local
#include "u_Popup.h"

namespace uc {
    struct Cp;
}

namespace Ui {
class FmPopupChar;
}

class FmPopupChar : public WiPopup
{
    Q_OBJECT
    using Super = WiPopup;
    using This = FmPopupChar;
public:
    explicit FmPopupChar(QWidget *parent = nullptr);
    ~FmPopupChar() override;
    This& setCp(const uc::Cp& cp);

private:
    Ui::FmPopupChar *ui;
};

#endif // FMPOPUPCHAR_H
