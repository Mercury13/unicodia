#ifndef HINTABLELABEL_H
#define HINTABLELABEL_H

#include <QLabel>

class HintableLabel : public QLabel
{
    using Super = QLabel;
public:
    using QLabel::QLabel;
    bool isSmall() const { return fIsSmall; }
    /// @return [+] was changed
    bool setSmall(bool x);
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
private:
    bool fIsSmall = false;
};

#endif // HINTABLELABEL_H
