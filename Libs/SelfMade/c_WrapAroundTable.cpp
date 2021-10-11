// My header
#include "c_WrapAroundTable.h"

// Qt
#include <QKeyEvent>


void WrapAroundTable::keyPressEvent(QKeyEvent* ev)
{
    switch (ev->key()) {
    case Qt::Key_Left:
        if ((ev->modifiers() & Qt::KeyboardModifierMask) == 0) {
            auto sel = currentIndex();
            if (sel.row() > 0 && sel.column() == 0) {
                auto md = model();
                setCurrentIndex(md->index(sel.row() - 1, md->columnCount() - 1));
                ev->accept();
                return;
            }
        }
        break;
    case Qt::Key_Right:
        if ((ev->modifiers() & Qt::KeyboardModifierMask) == 0) {
            auto sel = currentIndex();
            auto md = model();
            if (sel.row() + 1 < md->rowCount() && sel.column() + 1 == md->columnCount()) {
                setCurrentIndex(md->index(sel.row() + 1, 0));
                ev->accept();
                return;
            }
        }
        break;
    default: ;
    }
    Super::keyPressEvent(ev);
}
