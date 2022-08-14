#ifndef WISAMPLE_H
#define WISAMPLE_H

#include <QWidget>

namespace Ui {
class WiSample;
}

class WiSample : public QWidget
{
    Q_OBJECT

public:
    explicit WiSample(QWidget *parent = nullptr);
    ~WiSample();

private:
    Ui::WiSample *ui;
};

#endif // WISAMPLE_H
