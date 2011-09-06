#ifndef ENTEREDLINEEDIT_H
#define ENTEREDLINEEDIT_H

#include "widgets_global.h"

#include <QtGui/QLineEdit>

class WIDGETS_EXPORT EnteredLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit EnteredLineEdit(QWidget *parent = 0);

signals:
    void textEntered(const QString &);

public slots:

protected:
    void keyPressEvent(QKeyEvent *);
};

#endif // ENTEREDLINEEDIT_H
