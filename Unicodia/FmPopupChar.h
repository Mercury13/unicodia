#ifndef FMPOPUPCHAR_H
#define FMPOPUPCHAR_H

#include <QWidget>

// Project-local
#include "u_Popup.h"

namespace uc {
    struct Cp;
    struct GlyphStyleSets;
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
    explicit FmPopupChar(QWidget *parent, pop::ClickMemory& memory);
    char32_t charCode = 0;
    ~FmPopupChar() override;
    This& setCp(const uc::Cp& cp, const uc::GlyphStyleSets& glyphSets);

protected:
    Ui::FmPopupChar *ui;
};

#endif // FMPOPUPCHAR_H
