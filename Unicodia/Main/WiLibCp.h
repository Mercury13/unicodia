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
    Q_OBJECT
private:
    using Super = QWidget;
public:
    WiCpImage(QWidget *parent = nullptr, Qt::WindowFlags f = {});
    void setCp(char32_t x, const uc::GlyphStyleSets& y);
    void setCp(const uc::Cp* x, const uc::GlyphStyleSets& y);
protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
private:
    const uc::Cp* cp = nullptr;
    const uc::GlyphStyleSets* glyphSets;
    bool isLeftButton = false;
signals:
    void clicked();
};

class WiLibCp : public QWidget
{
    Q_OBJECT
    using This = WiLibCp;
    using Super = QWidget;
public:
    explicit WiLibCp(QWidget *parent = nullptr);
    ~WiLibCp() override;

    void setCp(char32_t cp, const uc::GlyphStyleSets& glyphSets);
    void removeCp();

private:
    static constexpr char32_t NO_CP = std::numeric_limits<char32_t>::max();

    Ui::WiLibCp *ui;
    char32_t currentCp = NO_CP;

private slots:
    void linkActivated();

signals:
    void goToCp(QWidget* initiator, char32_t cp);
};
