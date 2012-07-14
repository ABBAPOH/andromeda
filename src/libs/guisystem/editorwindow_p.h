#ifndef MAINWINDOW_P_H
#define MAINWINDOW_P_H

#include "editorwindow.h"

#include <QtCore/QPointer>

class QToolButton;

namespace GuiSystem {

class ProxyEditor;
class History;
class HistoryButton;

class EditorWindowPrivate
{
    Q_DECLARE_PUBLIC(EditorWindow)

public:
    explicit EditorWindowPrivate(EditorWindow *qq) : q_ptr(qq) {}

    void createActions();
    void retranslateUi();
    void registerActions();
    void initGeometry();

public:
    QPointer<AbstractEditor> editor;
    History *history;

    HistoryButton *backButton;
    HistoryButton *forwardButton;
    QAction *actions[EditorWindow::ActionCount];

    bool menuVisible;
    QToolButton *menuBarButton;

private:
    EditorWindow *q_ptr;
};

} // namespace GuiSystem

#endif // MAINWINDOW_P_H
