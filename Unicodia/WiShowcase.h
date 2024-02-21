#ifndef WISHOWCASE_H
#define WISHOWCASE_H

#include <QWidget>
#include "LocQt.h"

class QPushButton;
class QLabel;
class WiOsStyle;
class WiSample;

namespace Ui {
class WiShowcase;
}

class WiShowcase :
        public QWidget,
        public loc::Form<WiShowcase>
{
    Q_OBJECT

    using This = WiShowcase;
    using Form = loc::Form<WiShowcase>;
public:
    explicit WiShowcase(QWidget *parent = nullptr);
    ~WiShowcase() override;

    // loc::Form
    void translateMe() override;

    /// @todo [urgent] deprecate these funcs
    WiSample* wiSample() const;
    WiOsStyle* wiOsStyle() const;
    QLabel* lbCharCode() const;
    QPushButton* btCopyEx() const;
private:
    Ui::WiShowcase *ui;
    void init();
signals:
    void charCopied(QWidget* initiator);
private slots:
    void btCopyClicked();
};

#endif // WISHOWCASE_H
