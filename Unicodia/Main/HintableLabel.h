#ifndef HINTABLELABEL_H
#define HINTABLELABEL_H

#include "CachedLabel.h"

class HintableLabel : public CachedLabel
{
    using Super = CachedLabel;
public:
    using Super::Super;
    bool isSmall() const { return fIsSmall; }
    /// @return [+] was changed
    bool setSmall(bool x);
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
private:
    bool fIsSmall = false;
};

#endif // HINTABLELABEL_H
