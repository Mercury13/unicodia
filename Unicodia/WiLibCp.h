#pragma once

#include <QWidget>

#include "UcFlags.h"

namespace Ui {
    class WiLibCp;
}

namespace uc {
    struct Cp;
    struct GlyphStyleSets;
}

class WiCpImage : public QWidget
{
private:
    using Super = QWidget;
public:
    WiCpImage(QWidget *parent = nullptr, Qt::WindowFlags f = {});
    void setCp(char32_t x, uc::EmojiDraw em, const uc::GlyphStyleSets& y);
    void setCp(const uc::Cp* x, uc::EmojiDraw em, const uc::GlyphStyleSets& y);
protected:
    void paintEvent(QPaintEvent *event) override;
private:
    const uc::Cp* cp = nullptr;
    const uc::GlyphStyleSets* glyphSets;
    uc::EmojiDraw emojiDraw = uc::EmojiDraw::CONSERVATIVE;
};

class WiLibCp : public QWidget
{
    Q_OBJECT
    using This = WiLibCp;
    using Super = QWidget;
public:
    explicit WiLibCp(QWidget *parent = nullptr);
    ~WiLibCp() override;

    void setCp(char32_t cp, uc::EmojiDraw emojiDraw, const uc::GlyphStyleSets& glyphSets);
    void removeCp();

private:
    static constexpr char32_t NO_CP = std::numeric_limits<char32_t>::max();

    Ui::WiLibCp *ui;
    char32_t currentCp = NO_CP;

private slots:
    void linkActivated();

signals:
    void goToCp(char32_t cp);
};
