#ifndef MAINWINDOW_P_H
#define MAINWINDOW_P_H

#include "mainwindow.h"

#include <QtCore/QPointer>

class QToolButton;

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
    QPointer<AbstractEditor> editor;
    History *history;

    HistoryButton *backButton;
    HistoryButton *forwardButton;
    QAction *actions[MainWindow::ActionCount];

    bool menuVisible;
    QToolButton *menuBarButton;

private:
    MainWindow *q_ptr;
};

} // namespace GuiSystem

#endif // MAINWINDOW_P_H
