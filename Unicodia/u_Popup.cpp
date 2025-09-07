#include "u_Popup.h"

void pop::checkOwner(QWidget* owner)
{
    if (!owner)
        throw std::invalid_argument("[MxPopup] Need owner widget");
}
