#pragma once

#include <QWidget>

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
    void setCp(char32_t x, const uc::GlyphStyleSets& y);
    void setCp(const uc::Cp* x, const uc::GlyphStyleSets& y);
protected:
    void paintEvent(QPaintEvent *event) override;
private:
    const uc::Cp* cp = nullptr;
    const uc::GlyphStyleSets* glyphSets;
};

class WiLibCp : public QWidget
{
    Q_OBJECT

public:
    explicit WiLibCp(QWidget *parent = nullptr);
    ~WiLibCp() override;

    void setCp(char32_t cp, const uc::GlyphStyleSets& glyphSets);
    void removeCp();

private:
    Ui::WiLibCp *ui;
};
