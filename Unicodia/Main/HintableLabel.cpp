#include "HintableLabel.h"

bool HintableLabel::setSmall(bool x)
{
    bool r = (fIsSmall != x);
    if (r) {
        fIsSmall = x;
        auto pol = sizePolicy();
        pol.setVerticalPolicy(
            x ? QSizePolicy::MinimumExpanding : QSizePolicy::Expanding);
    }
    return r;
}


QSize HintableLabel::sizeHint() const
{
    auto r = Super::sizeHint();
    if (isSmall())
        r.setHeight(this->minimumHeight());
    return r;
}


QSize HintableLabel::minimumSizeHint() const
{
    auto r = Super::minimumSizeHint();
    if (isSmall())
        r.setHeight(this->minimumHeight());
    return r;
}
