#ifndef WISAMPLE_H
#define WISAMPLE_H

#include <QWidget>


namespace uc {
    struct Cp;
    struct GlyphStyleSets;
    class FontMatcher;
    enum class EmojiDraw;
}

namespace Ui {
class WiSample;
}

class WiSample : public QWidget
{
    Q_OBJECT

public:
    explicit WiSample(QWidget *parent = nullptr);
    ~WiSample() override;

    void showCp(
            const uc::Cp& ch, uc::EmojiDraw emojiDraw,
            const uc::GlyphStyleSets& sets);
    void showEmoji(std::u32string_view text);
    void showNothing();

private:
    Ui::WiSample *ui;
    bool needShowBriefly = true;
    void clearSample();
    ///  @warning  calls showBriefly → first this, then setCurrentWidget
    void setAbbrFont(const uc::Cp& ch);
    bool setFont(const uc::Cp& ch, const uc::FontMatcher& matcher);
    void drawWithQt(
            const uc::Cp& ch, uc::EmojiDraw emojiDraw,
            const uc::GlyphStyleSets& glyphSets);
    ///  Shows pageSampleQt briefly, to set correct height
    void showBriefly();
    ///  Sets character to space and calls showBriefly to set correct height
    void showSpaceBriefly();
    ///  Retrieves font and calls showSpaceBriefly to set corrent height
    ///  @return  font we used, may be reused for other needs (e.g. draw vertically)
    QFont showCpBriefly(const uc::Cp& ch);
};

#endif // WISAMPLE_H
