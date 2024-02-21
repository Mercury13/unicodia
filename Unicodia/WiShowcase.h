#ifndef WISHOWCASE_H
#define WISHOWCASE_H

#include <QWidget>
#include "LocQt.h"

#include "UcAutoDefines.h"

class QPushButton;
class QLabel;
class WiOsStyle;
class WiSample;
class FontMatch;

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
    QPushButton* btCopyEx() const;

    const uc::MaybeChar& shownCp() const { return fShownCp; }
    void set(uc::MaybeChar ch, FontMatch& fonts);
    void setSilent(uc::MaybeChar ch);
private:
    Ui::WiShowcase *ui;
    uc::MaybeChar fShownCp;
    void init();
signals:
    void charCopied(QWidget* initiator);
    void linkActivated(QWidget* initiator, const QString& link);
private slots:
    void btCopyClicked();
    void lbCharCodeClicked(const QString& link);
};

#endif // WISHOWCASE_H
