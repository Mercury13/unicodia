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
    emit droppedDown();
}


void WideComboBox::hidePopup()
{
    Super::hidePopup();
    fIsDown = false;
    emit pulledUp();
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


///// CharsTable ///////////////////////////////////////////////////////////////


void CharsTable::focusInEvent(QFocusEvent* ev)
{
    Super::focusInEvent(ev);
    emit focusIn();
}


void CharsTable::focusOutEvent(QFocusEvent* ev)
{
    Super::focusOutEvent(ev);
    emit focusOut();
}


///// SearchList ///////////////////////////////////////////////////////////////


void SearchList::keyPressEvent(QKeyEvent* ev)
{
    switch (ev->key()) {
    case Qt::Key_Enter:
    case Qt::Key_Return:
        ev->accept();
        if (auto index = currentIndex(); index.isValid()) {
            emit enterPressed(index.row());
        }
        break;
    default:
        Super::keyPressEvent(ev);
        break;
    }
}


void SearchList::mouseDoubleClickEvent(QMouseEvent* ev)
{
    if (ev->button() == Qt::LeftButton
            && !ev->modifiers().testFlag(Qt::KeyboardModifierMask)) {
        auto pt = ev->pos();
        auto index = indexAt(pt);
        if (index.isValid()) {
            emit enterPressed(index.row());
        }
    } else {
        Super::mouseDoubleClickEvent(ev);
    }
}
