#ifndef MAINWINDOW_P_H
#define MAINWINDOW_P_H

#include "mainwindow.h"

namespace GuiSystem {

class ProxyEditor;
class History;
class HistoryButton;

class MainWindowPrivate
{
    Q_DECLARE_PUBLIC(MainWindow)

public:
    explicit MainWindowPrivate(MainWindow *qq) : q_ptr(qq) {}

    void createActions();
    void retranslateUi();
    void registerActions();
    void initGeometry();

public:
    ProxyEditor *contanier;
    History *history;

    HistoryButton *backButton;
    HistoryButton *forwardButton;
    QAction *actions[MainWindow::ActionCount];

private:
    MainWindow *q_ptr;
};

} // namespace GuiSystem

#endif // MAINWINDOW_P_H
