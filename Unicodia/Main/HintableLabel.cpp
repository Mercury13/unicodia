#include "HintableLabel.h"

bool HintableLabel::setSmall(bool x)
{
    bool r = (fIsSmall != x);
    fIsSmall = x;
    return r;
}


QSize HintableLabel::sizeHint() const
{
    auto r = Super::sizeHint();
    if (isSmall()) {
        r.setHeight(this->minimumHeight());
    }
    return r;
}
