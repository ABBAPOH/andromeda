#ifndef MAINWINDOW_P_H
#define MAINWINDOW_P_H

#include "mainwindow.h"

namespace GuiSystem {

class MainWindowPrivate
{
    Q_DECLARE_PUBLIC(MainWindow)
public:
    MainWindowPrivate(MainWindow *qq);

    int addWidget(int stateIndex, QWidget *widget);

    State *currentState;
    int currentStateIndex;
    QList<State *> states;

//    QVBoxLayout *layout;

    QTabWidget *tabWidget;
    QToolBar *toolBar;

    MainWindow *q_ptr;
};

} // namespace GuiSystem

#endif // MAINWINDOW_P_H
