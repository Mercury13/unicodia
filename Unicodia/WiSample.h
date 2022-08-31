#ifndef WISAMPLE_H
#define WISAMPLE_H

#include <QWidget>

namespace uc {
    struct Cp;
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

    void showCp(const uc::Cp& ch);
    void showEmoji(std::u32string_view text);
    void showNothing();

private:
    Ui::WiSample *ui;
    void clearSample();
    void drawWithQt(const uc::Cp& cp);
};

#endif // WISAMPLE_H
