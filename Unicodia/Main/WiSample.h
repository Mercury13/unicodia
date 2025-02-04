#ifndef WISAMPLE_H
#define WISAMPLE_H

#include <QWidget>


namespace uc {
    struct Cp;
    struct GlyphStyleSets;
    class FontMatcher;
    enum class EmojiDraw : unsigned char;
    struct Font;
}

namespace Ui {
class WiSample;
}

class WiSample : public QWidget
{
    Q_OBJECT
    using This = WiSample;
public:
    explicit WiSample(QWidget *parent = nullptr);
    ~WiSample() override;

    void showCp(
            const uc::Cp& ch, uc::EmojiDraw emojiDraw,
            const uc::GlyphStyleSets& sets,
            bool isMainEmoji);
    void showEmoji(std::u32string_view text, bool isMainEmoji);
    void showNothing();

    void translateMe();
    void switchToLib();
private:
    enum class HeadMode : unsigned char { NONE, SAMPLE, TOLIB };

    Ui::WiSample *ui;
    /// Qt’s peculiarity: once the label was shown with non-empty text,
    ///    you are not obliged to show once again
    const uc::Font* shownBrieflyFont = nullptr;
    HeadMode headMode = HeadMode::NONE;
    QString sSample, sToLib;
    bool isLib = false;

    /// Removes everything from lbSample — returns it to normal size
    void clearSample();
    ///  @warning  calls showBriefly → first this, then setCurrentWidget
    void setAbbrFont(const uc::Cp& ch);
    const uc::Font* setFont(const uc::Cp& ch, const uc::FontMatcher& matcher);
    void drawWithQt(
            const uc::Cp& ch, const uc::Font& font,
            uc::EmojiDraw emojiDraw, const uc::GlyphStyleSets& glyphSets);
    /// @return  active font if haltIfGraphic and actually halted,
    ///          to make some graphic things
    const uc::Font* drawWithQt(
            const uc::Cp& ch, uc::EmojiDraw emojiDraw,
            const uc::GlyphStyleSets& glyphSets,
            bool haltIfGraphic = false);
    ///  Sets character to space and shows pageSampleQt briefly to set correct height
    void showBriefly(const uc::Font* font);
    ///  Retrieves font and calls showBriefly to set corrent height
    ///  @return  font we used, may be reused for other needs (e.g. draw vertically)
    QFont showCpBriefly(const uc::Cp& ch);

    void headToSample();
    void headToLib(char32_t subj);
    void headToLibIf(char32_t subj, bool isMainEmoji);
    void headToLibIf(std::u32string_view text, bool isMainEmoji);
signals:
    void linkActivated(QWidget* initiator, const QString& link);
private slots:
    void labelLinkActivated(const QString& link);
};

#endif // WISAMPLE_H
