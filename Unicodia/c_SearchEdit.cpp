#include "c_SearchEdit.h"

#include <QScrollBar>
#include <QAbstractItemView>
#include <QKeyEvent>


///// WideComboBox /////////////////////////////////////////////////////////////


void WideComboBox::resizeView()
{
    auto vw = view();
    int newWidth = vw->sizeHintForColumn(0);
    QScrollBar* sb = vw->verticalScrollBar();
    if (sb && sb->isVisible())
        newWidth += sb->width();
    vw->setMinimumWidth(newWidth);
}


void WideComboBox::showPopup()
{
    fIsDown = true;
    Super::showPopup();
    resizeView();
}


void WideComboBox::hidePopup()
{
    Super::hidePopup();
    fIsDown = false;
}


void WideComboBox::resizeEvent(QResizeEvent* ev)
{
    Super::resizeEvent(ev);
    // BIG glitches: e.g. viewport is smaller than view, and the rest is grey.
    // The simplest way to overcome is to hide-show
    if (isDown()) {
        hidePopup();
        showPopup();
    } else {
        resizeView();
    }
}


void WideComboBox::focusInEvent(QFocusEvent* ev)
{
    Super::focusInEvent(ev);
    resizeView();
}


///// SearchEdit ///////////////////////////////////////////////////////////////

void SearchEdit::focusInEvent(QFocusEvent* ev)
{
    Super::focusInEvent(ev);
    emit focusIn();
}


void SearchEdit::focusOutEvent(QFocusEvent* ev)
{
    Super::focusOutEvent(ev);
    emit focusOut();
}


void SearchEdit::keyPressEvent(QKeyEvent* ev)
{
    switch (ev->key()) {
    case Qt::Key_Enter:
    case Qt::Key_Return:
        ev->accept();
        emit searchPressed();
        return;
    default:
        Super::keyPressEvent(ev);
    }
}


///// SearchCombo //////////////////////////////////////////////////////////////


SearchCombo::SearchCombo(QWidget* parent) : Super(parent)
{
    setStyleSheet("SearchCombo:drop-down { width:0; }");
    auto edit = new SearchEdit(this);
    setLineEdit(edit);
    connect(edit, &SearchEdit::searchPressed, this, &This::emitSearchPressed);
    addItem("Test");
}


void SearchCombo::focusInEvent(QFocusEvent* ev)
{
    Super::focusInEvent(ev);
    showPopup();
}


void SearchCombo::emitSearchPressed()
{
    emit searchPressed();
}
