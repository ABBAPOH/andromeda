#ifndef MAINWINDOW_P_H
#define MAINWINDOW_P_H

#include "mainwindow.h"

#include <QtGui/QToolBar>
#include <enteredlineedit.h>

namespace MainWindowPlugin {

class MainWindowPrivate : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(MainWindow)
    MainWindow *q_ptr;
public:
    MainWindowPrivate(MainWindow *qq) : q_ptr(qq) {}

    EnteredLineEdit *lineEdit;
    QToolBar *toolBar;

public slots:
    void onTextEntered(const QString &);
};

} // namespace MainWindowPlugin

#endif // MAINWINDOW_P_H
