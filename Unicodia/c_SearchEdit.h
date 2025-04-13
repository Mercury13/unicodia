#pragma once

// Qt
#include <QLineEdit>
#include <QComboBox>
#include <QTreeView>

// Qt ex
#include "c_WrapAroundTable.h"


//template <class Signal, class Receiver, class Slot>
//void installResizeChain(
//        QWidget* sender, QWidget* chainEnd,
//        Receiver* receiver, Slot slot)
//{
//    for (QWidget* widget = sender; widget != nullptr;
//            widget = qobject_cast<QWidget*>(sender->parent())) {
//        connect(widget, &QWidget::, receiver, slot);
//        if (widget == chainEnd)
//            break;
//    }
//}


class WideComboBox : public QComboBox
{
    Q_OBJECT
private:
    using Super = QComboBox;
    bool fIsDown = false;
    int fMaxWidth = 999'999'999;
protected:
    void resizeEvent(QResizeEvent* ev) override;
    void focusInEvent(QFocusEvent* ev) override;
signals:
    void droppedDown();
    void pulledUp();
public:
    explicit WideComboBox(QWidget* aParent) : Super(aParent) {}

    void showPopup() override;
    void hidePopup() override;
    bool isDown() const noexcept { return fIsDown; }
    void resizeView();
    void setMaxWidth(int x) noexcept { fMaxWidth = x; }
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


class SearchCombo : public WideComboBox
{
    Q_OBJECT
    using Super = WideComboBox;
    using This = SearchCombo;
public:
    SearchCombo(QWidget *parent = nullptr);
    void addToHistory();
protected:
    void focusInEvent(QFocusEvent* ev) override;
    void focusOutEvent(QFocusEvent* ev) override;
signals:
    void focusIn();
    void focusOut();
    void searchPressed();
};


class CharsTable : public WrapAroundTable
{
    Q_OBJECT
    using Super = WrapAroundTable;
public:
    using Super::Super;
protected:
    void focusInEvent(QFocusEvent* ev) override;
    void focusOutEvent(QFocusEvent* ev) override;
signals:
    void focusIn();
    void focusOut();
};


constexpr int DEFAULT_TREE_INDENT = 20;
constexpr int EMOJI_INDENT = 10;


class SearchTree : public QTreeView
{
    Q_OBJECT
    using Super = QTreeView;
public:
    using Super::Super;
    void setFlat(bool x);
protected:
    void keyPressEvent(QKeyEvent* ev) override;
    void mouseDoubleClickEvent(QMouseEvent* ev) override;
signals:
    void enterPressed(const QModelIndex& index);
public slots:
    void simulateEnterPress();
};
