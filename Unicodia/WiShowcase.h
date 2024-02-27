#ifndef WISHOWCASE_H
#define WISHOWCASE_H

// C++
#include <variant>

// Qt
#include <QWidget>
#include <QRadioButton>

// Qt misc
#include "LocQt.h"
#include "QtMultiRadio.h"

// Misc libs
#include "magic_enum.hpp"
#include "u_TinyOpt.h"

#include "UcAutoDefines.h"

class QPushButton;
class QLabel;
class QTextBrowser;
class WiOsStyle;
class WiSample;
class FontMatch;

namespace Ui {
class WiShowcase;
}

extern template struct TinyOpt<char32_t>;

enum class ShownClass { NONE, CP };
using ShownObj = std::variant<
                    std::monostate, // NONE — no object held
                    char32_t>;      // CP — any codepoint of Unicode, incl. surrogate

inline constexpr ShownClass toUnderlying(ShownObj x)
    { return static_cast<ShownClass>(x.index()); }

/// Sorry, no automatic op==, will use C++20 here until find some compiler
bool operator == (char32_t x, const ShownObj& y);

/// Ban other comparisons
template <class T> requires std::is_arithmetic_v<T>
bool operator == (T x, const ShownObj& y) = delete;


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

    const ShownObj& shownObj() const { return fShownObj; }
    /// @todo [bad] shownCode is bad, but IDK what to do
    TinyOpt<char32_t> shownCode() const;
    void set(char32_t ch,
             QTextBrowser* viewer,
             FontMatch& fonts,
             const uc::GlyphStyleSets& glyphSets);
    void setSilent(char32_t ch);
    void redrawSampleChar(const uc::GlyphStyleSets& glyphSets);
    void redrawViewer(QTextBrowser* viewer);
private:
    Ui::WiShowcase *ui;

    ShownObj fShownObj;
    uc::EcGlyphStyleChannel fCurrChannel = uc::EcGlyphStyleChannel::NONE;
    UintRadio<QRadioButton> radioGlyphStyle;

    void init();
signals:
    void charCopied(QWidget* initiator);
    void advancedCopied(QWidget* initiator);
    void linkActivated(QWidget* initiator, const QString& link);
    void glyphStyleChanged(uc::EcGlyphStyleChannel channel, unsigned setting);
    void copiedPopped(QWidget* initiator);
private slots:
    void btCopyClicked();
    void btCopyExClicked();
    void lbCharCodeLinkActivated(const QString& link);
    void lbStyleHelpLinkActivated(const QString& link);
    void glyphStyleClicked();
};

#endif // WISHOWCASE_H
