#include "mainwindowplugin.h"

#include <QtCore/QtPlugin>
#include <QtGui/QMenu>
#include <actionmanager.h>

using namespace GuiSystem;
using namespace MainWindowPlugin;

MainWindowPluginImpl::MainWindowPluginImpl() :
    IPlugin()
{
}

#include "mainwindow.h"

bool MainWindowPluginImpl::initialize()
{
    window = new MainWindow();
    window->show();

    ActionManager *manager = ActionManager::instance();
    manager->addMenu(new QMenu("File"));
    QMenu *editMenu = new QMenu("Edit");
    manager->addMenu(editMenu);
    manager->registerMenu(editMenu, "MENU_EDIT");

    QAction *undoAction = new QAction("Undo", manager);
    undoAction->setShortcut(tr("Ctrl+Z"));
    editMenu->addAction(undoAction);
    manager->registerAction(undoAction, "ACTION_UNDO");

    QAction *redoAction = new QAction("Redo", manager);
    redoAction->setShortcut(tr("Ctrl+Shift+Z"));
    editMenu->addAction(redoAction);
    manager->registerAction(redoAction, "ACTION_REDO");

    editMenu->addSeparator();

    QAction *copyAction = new QAction("Copy", manager);
    copyAction->setShortcut(tr("Ctrl+C"));
    editMenu->addAction(copyAction);
    manager->registerAction(copyAction, "ACTION_COPY");
    manager->connect("ACTION_COPY", "copy()");

    QAction *pasteAction = new QAction("Paste", manager);
    pasteAction->setShortcut(tr("Ctrl+V"));
    editMenu->addAction(pasteAction);
    manager->registerAction(pasteAction, "ACTION_PASTE");
    manager->connect("ACTION_PASTE", "paste()");

    return true;
}

void MainWindowPluginImpl::shutdown()
{
    delete window;
}

Q_EXPORT_PLUGIN(MainWindowPluginImpl)
