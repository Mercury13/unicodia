#pragma once

// Qt
#include <QLineEdit>
#include <QComboBox>


class WideComboBox : public QComboBox
{
private:
    using Super = QComboBox;
    bool fIsDown = false;
protected:
    void resizeView();
    void resizeEvent(QResizeEvent* ev) override;
    void focusInEvent(QFocusEvent* ev) override;
public:
    explicit WideComboBox(QWidget* aParent) : Super(aParent) {}

    void showPopup() override;
    void hidePopup() override;
    bool isDown() const noexcept { return fIsDown; }
};


class SearchEdit : public QLineEdit
{
    Q_OBJECT
    using Super = QLineEdit;
public:
    using Super::Super;
protected:
    void focusInEvent(QFocusEvent* ev) override;
    void focusOutEvent(QFocusEvent* ev) override;
    void keyPressEvent(QKeyEvent* ev) override;
signals:
    void focusIn();
    void focusOut();
    void searchPressed();
};
