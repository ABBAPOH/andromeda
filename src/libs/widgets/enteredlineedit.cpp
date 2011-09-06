#include "enteredlineedit.h"

#include <QtGui/QKeyEvent>

EnteredLineEdit::EnteredLineEdit(QWidget *parent) :
    QLineEdit(parent)
{
}

void EnteredLineEdit::keyPressEvent(QKeyEvent *e)
{
    QLineEdit::keyPressEvent(e);

    if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return) {
        emit textEntered(text());
    }
}
