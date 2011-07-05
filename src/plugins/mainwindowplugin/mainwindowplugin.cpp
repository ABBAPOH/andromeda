#include "mainwindowplugin.h"

#include "mainwindow.h"

#include <QtCore/QtPlugin>
#include <QtGui/QMenu>
#include <actionmanager.h>
#include <command.h>
#include <commandcontainer.h>

using namespace GuiSystem;
using namespace MainWindowPlugin;

MainWindowPluginImpl::MainWindowPluginImpl() :
    IPlugin()
{
}

bool MainWindowPluginImpl::initialize()
{
    createActions();
    newWindow();

    return true;
}

void MainWindowPluginImpl::shutdown()
{
}

void MainWindowPluginImpl::newWindow()
{
    MainWindow *window = new MainWindow();
    QAction *newTabAction = new QAction(this);
    newTabAction->setObjectName(ACTION_NEW_TAB);
    connect(newTabAction, SIGNAL(triggered()), window, SLOT(newTab()));
    window->addAction(newTabAction);
    ActionManager::instance()->command(ACTION_CLOSE_TAB)->action(window, SLOT(closeTab()));
    window->show();
    addObject(window);
}

void MainWindowPluginImpl::createActions()
{
    CommandContainer *menuBarContainer = new CommandContainer(MENU_BAR, this);

    CommandContainer *fileContainer = new CommandContainer(MENU_FILE, this);
    fileContainer->setTitle(tr("File"));
    menuBarContainer->addContainer(fileContainer);

    Command *newWindowCommand = new Command(ACTION_NEW_WINDOW, this);
    newWindowCommand->setDefaultText(tr("New window"));
    newWindowCommand->setDefaultShortcut(tr("Ctrl+N"));
    newWindowCommand->commandAction()->setEnabled(true);
    fileContainer->addCommand(newWindowCommand);
    connect(newWindowCommand->commandAction(), SIGNAL(triggered()), SLOT(newWindow()));

    Command *newTabCommand = new Command(ACTION_NEW_TAB, this);
    newTabCommand->setDefaultText(tr("New tab"));
    newTabCommand->setDefaultShortcut(tr("Ctrl+T"));
    fileContainer->addCommand(newTabCommand);

    Command *newFolderCommand = new Command(ACTION_NEW_FOLDER, this);
    newFolderCommand->setDefaultText(tr("New folder"));
    newFolderCommand->setDefaultShortcut(tr("Ctrl+Shift+N"));
    fileContainer->addCommand(newFolderCommand);

    Command *openCommand = new Command(ACTION_OPEN, this);
    openCommand->setDefaultText(tr("Open"));
    openCommand->setDefaultShortcut(tr("Ctrl+O"));
    fileContainer->addCommand(openCommand);

    Command *closeTabCommand = new Command(ACTION_CLOSE_TAB, this);
    closeTabCommand->setDefaultText(tr("Close"));
    closeTabCommand->setDefaultShortcut(tr("Ctrl+W"));
    fileContainer->addCommand(closeTabCommand);

    CommandContainer *editContainer = new CommandContainer(MENU_EDIT, this);
    editContainer->setTitle(tr("Edit"));
    menuBarContainer->addContainer(editContainer);

    Command *undoCommand = new Command(ACTION_UNDO, this);
    undoCommand->setDefaultText(tr("Undo"));
    undoCommand->setDefaultShortcut(tr("Ctrl+Z"));
    editContainer->addCommand(undoCommand);

    Command *redoCommand = new Command(ACTION_REDO, this);
    redoCommand->setDefaultText(tr("Redo"));
    redoCommand->setDefaultShortcut(tr("Ctrl+Shift+Z"));
    editContainer->addCommand(redoCommand);

    Command *cutCommand = new Command(ACTION_CUT, this);
    cutCommand->setDefaultText(tr("Cut"));
    cutCommand->setDefaultShortcut(tr("Ctrl+X"));
    editContainer->addCommand(cutCommand);

    Command *copyCommand = new Command(ACTION_COPY, this);
    copyCommand->setDefaultText(tr("Copy"));
    copyCommand->setDefaultShortcut(tr("Ctrl+C"));
    editContainer->addCommand(copyCommand);

    Command *pasteCommand = new Command(ACTION_PASTE, this);
    pasteCommand->setDefaultText(tr("Paste"));
    pasteCommand->setDefaultShortcut(tr("Ctrl+V"));
    editContainer->addCommand(pasteCommand);

    Command *selectAllCommand = new Command(ACTION_SELECT_ALL, this);
    selectAllCommand->setDefaultText(tr("Select All"));
    selectAllCommand->setDefaultShortcut(tr("Ctrl+A"));
    editContainer->addCommand(selectAllCommand);

    CommandContainer *viewContainer = new CommandContainer(MENU_EDIT, this);
    viewContainer->setTitle(tr("View"));
    menuBarContainer->addContainer(viewContainer);

    Command *dualPaneCommand = new Command(ACTION_SELECT_ALL, this);
    dualPaneCommand->setDefaultText(tr("Dual Pane"));
    dualPaneCommand->setDefaultShortcut(tr("Ctrl+D"));
    viewContainer->addCommand(dualPaneCommand);

    CommandContainer *goToContainer = new CommandContainer(MENU_EDIT, this);
    goToContainer->setTitle(tr("Go To"));
    menuBarContainer->addContainer(goToContainer);

    Command *backCommand = new Command(ACTION_SELECT_ALL, this);
    backCommand->setDefaultText(tr("Back"));
    backCommand->setDefaultShortcut(tr("Ctrl+["));
    goToContainer->addCommand(backCommand);

    Command *forwardCommand = new Command(ACTION_SELECT_ALL, this);
    forwardCommand->setDefaultText(tr("Forward"));
    forwardCommand->setDefaultShortcut(tr("Ctrl+]"));
    goToContainer->addCommand(forwardCommand);

    Command *upOneLevelCommand = new Command(ACTION_SELECT_ALL, this);
    upOneLevelCommand->setDefaultText(tr("Up one level"));
    upOneLevelCommand->setDefaultShortcut(tr("Ctrl+Up"));
    goToContainer->addCommand(upOneLevelCommand);
}

Q_EXPORT_PLUGIN(MainWindowPluginImpl)
