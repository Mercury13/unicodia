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

namespace detail {
    using ShownObjFather = std::variant<
                            std::monostate, // NONE — no object held
                            char32_t>;      // CP — any codepoint of Unicode, incl. surrogate
}   // namespace detail

class ShownObj : public detail::ShownObjFather
{
public:
    /// Inherit all ctors
    using detail::ShownObjFather::ShownObjFather;

    /// @return  code point, or null
    [[nodiscard]] TinyOpt<char32_t> maybeCp() const;
    /// @return  code point
    /// @throw   what std::get throws if no code point
    [[nodiscard]] char32_t forceCp() const;
    constexpr ShownClass clazz() const
        { return static_cast<ShownClass>(index()); }
};

/// Sorry, no automatic op==, will use C++20 here until find some compiler
inline bool operator == (char32_t x, const ShownObj& y)
{ return x == y.maybeCp(); }

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
