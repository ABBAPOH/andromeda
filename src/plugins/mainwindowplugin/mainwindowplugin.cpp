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
#include <menubar.h>

bool MainWindowPluginImpl::initialize()
{
    createActions();
#ifdef Q_OS_MAC
    addObject(new MenuBar);
#endif
    newWindow();
    newWindow();

    return true;
}

void MainWindowPluginImpl::shutdown()
{
}
#include <QApplication>
void MainWindowPluginImpl::newWindow()
{
    MainWindow *window = new MainWindow();
    window->show();
    qApp->processEvents();
    addObject(window);
}

void MainWindowPluginImpl::createActions()
{
    ActionManager *manager = ActionManager::instance();

    QMenu *fileMenu = new QMenu("File");
    manager->addMenu(fileMenu);
    manager->registerMenu(fileMenu, MENU_FILE);

    QAction *newWindowAction = new QAction("New window", manager);
    newWindowAction->setShortcut(tr("Ctrl+N"));
    fileMenu->addAction(newWindowAction);
    manager->registerAction(newWindowAction, ACTION_NEW_WINDOW);
    connect(newWindowAction, SIGNAL(triggered()), SLOT(newWindow()));

    QMenu *editMenu = new QMenu("Edit");
    manager->addMenu(editMenu);
    manager->registerMenu(editMenu, MENU_EDIT);

    QAction *undoAction = new QAction("Undo", manager);
    undoAction->setShortcut(tr("Ctrl+Z"));
    editMenu->addAction(undoAction);
    manager->registerAction(undoAction, ACTION_UNDO);

    QAction *redoAction = new QAction("Redo", manager);
    redoAction->setShortcut(tr("Ctrl+Shift+Z"));
    editMenu->addAction(redoAction);
    manager->registerAction(redoAction, ACTION_REDO);

    editMenu->addSeparator();

    QAction *cutAction = new QAction("Cut", manager);
    cutAction->setShortcut(tr("Ctrl+X"));
    editMenu->addAction(cutAction);
    manager->registerAction(cutAction, ACTION_CUT);
    manager->connect("ACTION_CUT", "cut()");

    QAction *copyAction = new QAction("Copy", manager);
    copyAction->setShortcut(tr("Ctrl+C"));
    editMenu->addAction(copyAction);
    manager->registerAction(copyAction, ACTION_COPY);
    manager->connect("ACTION_COPY", "copy()");

    QAction *pasteAction = new QAction("Paste", manager);
    pasteAction->setShortcut(tr("Ctrl+V"));
    editMenu->addAction(pasteAction);
    manager->registerAction(pasteAction, ACTION_PASTE);
    manager->connect("ACTION_PASTE", "paste()");
}

Q_EXPORT_PLUGIN(MainWindowPluginImpl)
