#pragma once

// C++
#include <memory>

// Qt
#include <QWidget>
#include <QLabel>

// Project-local
#include "u_Popup.h"


template <class Wi, class ... Args>
Wi& ensure(std::unique_ptr<Wi>& x, Args&& ... args)
{
    if (!x) {
        x = std::make_unique<Wi>(std::forward<Args>(args)...);
    }
    return *x;
}


class FmPopup : public WiPopup
{
    Q_OBJECT
    using Super = WiPopup;
    using This = FmPopup;
public:
    FmPopup(QWidget* owner, pop::ClickMemory& memory, const char* color);
    ~FmPopup() override = default;
    FmPopup& setText(const QString& x);

    ClickableLabel* viewport() const { return lbText; }
    void deselectLink();
    void mySetFocus() override;
private:
    ClickableLabel* lbText;    
    class QVBoxLayout* layout;
};
