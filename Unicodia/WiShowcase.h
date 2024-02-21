#ifndef WISHOWCASE_H
#define WISHOWCASE_H

#include <QWidget>
#include <QRadioButton>

// Qt misc
#include "LocQt.h"
#include "QtMultiRadio.h"

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
    static constexpr auto EMOJI_DRAW = uc::EmojiDraw::CONSERVATIVE;

    explicit WiShowcase(QWidget *parent = nullptr);
    ~WiShowcase() override;

    // loc::Form
    void translateMe() override;

    const uc::MaybeChar& shownCp() const { return fShownCp; }
    void set(uc::MaybeChar ch, FontMatch& fonts, const uc::GlyphStyleSets& glyphSets);
    void setSilent(uc::MaybeChar ch);
    void redrawSampleChar(const uc::GlyphStyleSets& glyphSets);
private:
    Ui::WiShowcase *ui;

    uc::MaybeChar fShownCp;
    uc::EcGlyphStyleChannel fCurrChannel = uc::EcGlyphStyleChannel::NONE;
    UintRadio<QRadioButton> radioGlyphStyle;

    void init();
signals:
    void charCopied(QWidget* initiator);
    void advancedCopied(QWidget* initiator);
    void linkActivated(QWidget* initiator, const QString& link);
    void glyphStyleChanged(uc::EcGlyphStyleChannel channel, unsigned setting);
private slots:
    void btCopyClicked();
    void btCopyExClicked();
    void lbCharCodeLinkActivated(const QString& link);
    void lbStyleHelpLinkActivated(const QString& link);
    void glyphStyleClicked();
};

#endif // WISHOWCASE_H