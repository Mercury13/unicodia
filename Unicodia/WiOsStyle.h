#ifndef WIOSSTYLE_H
#define WIOSSTYLE_H

#include <QWidget>

namespace uc {
    struct Cp;
}

class FontMatch;

namespace Ui {
class WiOsStyle;
}

class WiOsStyle : public QWidget
{
    Q_OBJECT

public:
    explicit WiOsStyle(QWidget *parent = nullptr);
    ~WiOsStyle();

    void setEmptyCode(char32_t code);
    void setCp(const uc::Cp& ch, FontMatch& fontMatch);

private:
    Ui::WiOsStyle *ui;
    QFont fontBig, fontTofu;

signals:
    void linkActivated(const QString& url);
};

QString qPopupLink(
        std::string_view locKey, const char* target);

#endif // WIOSSTYLE_H
