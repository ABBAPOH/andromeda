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
    window->show();
    addObject(window);
}

void MainWindowPluginImpl::createActions()
{
    CommandContainer *menuBarContainer = new CommandContainer(MENU_BAR, this);

    CommandContainer *fileContainer = new CommandContainer(MENU_FILE, this);
    fileContainer->setTitle(tr("File"));

    Command *newWindowCommand = new Command(ACTION_NEW_WINDOW, this);
    newWindowCommand->setDefaultText(tr("New window"));
    newWindowCommand->setDefaultShortcut(tr("Ctrl+N"));
    newWindowCommand->action()->setEnabled(true);
    fileContainer->addCommand(newWindowCommand);
    connect(newWindowCommand->action(), SIGNAL(triggered()), SLOT(newWindow()));

    Command *newTabCommand = new Command(ACTION_NEW_TAB, this);
    newTabCommand->setDefaultText(tr("New tab"));
    newTabCommand->setDefaultShortcut(tr("Ctrl+T"));
    fileContainer->addCommand(newTabCommand);

    CommandContainer *editContainer = new CommandContainer(MENU_EDIT, this);
    editContainer->setTitle(tr("Edit"));

    menuBarContainer->addContainer(fileContainer);
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
}

Q_EXPORT_PLUGIN(MainWindowPluginImpl)
