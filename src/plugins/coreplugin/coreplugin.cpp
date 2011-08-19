#include "coreplugin.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QTimer>
#include <QtCore/QtPlugin>
#include <QtGui/QMenu>

#include "constants.h"
#include "core.h"
#include "mainwindow.h"
#include "perspectivemanager.h"
#include "settingsdialog_p.h"

#include <actionmanager.h>
#include <command.h>
#include <commandcontainer.h>
#include <pluginview.h>

using namespace CorePlugin;
using namespace GuiSystem;

CorePluginImpl::CorePluginImpl() :
    IPlugin()
{
}

bool CorePluginImpl::initialize()
{
    addObject(new Core);
    addObject(new PerspectiveManager);
    addObject(new CategoryModel);

    createActions();
    QTimer::singleShot(0, this, SLOT(newWindow()));

    return true;
}

void CorePluginImpl::shutdown()
{
}

void CorePluginImpl::newWindow()
{
    MainWindow *window = new MainWindow();
    QAction *newTabAction = new QAction(this);
    newTabAction->setObjectName(Constants::Ids::Actions::NewTab);
    connect(newTabAction, SIGNAL(triggered()), window, SLOT(newTab()));
    window->addAction(newTabAction);
    ActionManager::instance()->command(Constants::Ids::Actions::CloseTab)->action(window, SLOT(closeTab()));
    window->show();
    addObject(window);
}

void CorePluginImpl::showPluginView()
{
    PluginView *view = object<PluginView>("pluginView");
    view->exec();
}

void CorePluginImpl::createActions()
{
    CommandContainer *menuBarContainer = new CommandContainer(Constants::Ids::Menus::MenuBar, this);

    CommandContainer *fileContainer = new CommandContainer(Constants::Ids::Menus::File, this);
    fileContainer->setTitle(tr("File"));
    menuBarContainer->addContainer(fileContainer);

    Command *newWindowCommand = new Command(Constants::Ids::Actions::NewWindow, this);
    newWindowCommand->setDefaultText(tr("New window"));
    newWindowCommand->setDefaultShortcut(tr("Ctrl+N"));
    newWindowCommand->setAlwaysEnabled(true);
    fileContainer->addCommand(newWindowCommand);
    connect(newWindowCommand->commandAction(), SIGNAL(triggered()), SLOT(newWindow()));

    Command *newTabCommand = new Command(Constants::Ids::Actions::NewTab, this);
    newTabCommand->setDefaultText(tr("New tab"));
    newTabCommand->setDefaultShortcut(tr("Ctrl+T"));
    fileContainer->addCommand(newTabCommand);

    Command *newFolderCommand = new Command(Constants::Ids::Actions::NewFolder, this);
    newFolderCommand->setDefaultText(tr("New folder"));
    newFolderCommand->setDefaultShortcut(tr("Ctrl+Shift+N"));
    fileContainer->addCommand(newFolderCommand);

    Command *openCommand = new Command(Constants::Ids::Actions::Open, this);
    openCommand->setDefaultText(tr("Open"));
    openCommand->setDefaultShortcut(tr("Ctrl+O"));
    fileContainer->addCommand(openCommand);

    Command *closeTabCommand = new Command(Constants::Ids::Actions::CloseTab, this);
    closeTabCommand->setDefaultText(tr("Close"));
    closeTabCommand->setDefaultShortcut(tr("Ctrl+W"));
    fileContainer->addCommand(closeTabCommand);

    fileContainer->addSeparator();

    Command *renameCommand = new Command(Constants::Ids::Actions::Rename, this);
    renameCommand->setDefaultText(tr("Rename"));
#ifdef Q_OS_MAC
    renameCommand->setDefaultShortcut(tr("Return"));
#else
    renameCommand->setDefaultShortcut(tr("F2"));
#endif
    fileContainer->addCommand(renameCommand);

    Command *removeCommand = new Command(Constants::Ids::Actions::Remove, this);
    removeCommand->setDefaultText(tr("Remove"));
    removeCommand->setDefaultShortcut(tr("Ctrl+Shift+Backspace"));
    fileContainer->addCommand(removeCommand);

#ifndef Q_OS_MAC
    fileContainer->addSeparator();
    Command *exitCommand = new Command(Constants::Ids::Actions::Exit, this);
    exitCommand->setDefaultText(tr("Exit"));
    exitCommand->setDefaultShortcut(tr("Ctrl+Q"));
    exitCommand->setAlwaysEnabled(true);
    fileContainer->addCommand(exitCommand);
    connect(exitCommand->commandAction(), SIGNAL(triggered()), qApp, SLOT(quit()));
#endif

    CommandContainer *editContainer = new CommandContainer(Constants::Ids::Menus::Edit, this);
    editContainer->setTitle(tr("Edit"));
    menuBarContainer->addContainer(editContainer);

    Command *undoCommand = new Command(Constants::Ids::Actions::Undo, this);
    undoCommand->setDefaultText(tr("Undo"));
    undoCommand->setDefaultShortcut(tr("Ctrl+Z"));
    editContainer->addCommand(undoCommand);

    Command *redoCommand = new Command(Constants::Ids::Actions::Redo, this);
    redoCommand->setDefaultText(tr("Redo"));
    redoCommand->setDefaultShortcut(tr("Ctrl+Shift+Z"));
    editContainer->addCommand(redoCommand);

    editContainer->addSeparator();

    Command *cutCommand = new Command(Constants::Ids::Actions::Cut, this);
    cutCommand->setDefaultText(tr("Cut"));
    cutCommand->setDefaultShortcut(tr("Ctrl+X"));
    editContainer->addCommand(cutCommand);

    Command *copyCommand = new Command(Constants::Ids::Actions::Copy, this);
    copyCommand->setDefaultText(tr("Copy"));
    copyCommand->setDefaultShortcut(tr("Ctrl+C"));
    editContainer->addCommand(copyCommand);

    Command *pasteCommand = new Command(Constants::Ids::Actions::Paste, this);
    pasteCommand->setDefaultText(tr("Paste"));
    pasteCommand->setDefaultShortcut(tr("Ctrl+V"));
    editContainer->addCommand(pasteCommand);

    Command *selectAllCommand = new Command(Constants::Ids::Actions::SelectAll, this);
    selectAllCommand->setDefaultText(tr("Select All"));
    selectAllCommand->setDefaultShortcut(tr("Ctrl+A"));
    editContainer->addCommand(selectAllCommand);

    CommandContainer *viewContainer = new CommandContainer(Constants::Ids::Menus::View, this);
    viewContainer->setTitle(tr("View"));
    menuBarContainer->addContainer(viewContainer);

    Command *dualPaneCommand = new Command(Constants::Ids::Actions::DualPane, this);
    dualPaneCommand->setDefaultText(tr("Dual Pane"));
    dualPaneCommand->setDefaultShortcut(tr("Ctrl+D"));
    // TODO: change API or realization!!!
    dualPaneCommand->setCheckable(true);
    viewContainer->addCommand(dualPaneCommand);

    CommandContainer *goToContainer = new CommandContainer(Constants::Ids::Menus::GoTo, this);
    goToContainer->setTitle(tr("Go to"));
    menuBarContainer->addContainer(goToContainer);

    Command *backCommand = new Command(Constants::Ids::Actions::Back, this);
    backCommand->setDefaultText(tr("Back"));
    backCommand->setDefaultShortcut(tr("Ctrl+["));
    goToContainer->addCommand(backCommand);

    Command *forwardCommand = new Command(Constants::Ids::Actions::Forward, this);
    forwardCommand->setDefaultText(tr("Forward"));
    forwardCommand->setDefaultShortcut(tr("Ctrl+]"));
    goToContainer->addCommand(forwardCommand);

    Command *upOneLevelCommand = new Command(Constants::Ids::Actions::Up, this);
    upOneLevelCommand->setDefaultText(tr("Up one level"));
    upOneLevelCommand->setDefaultShortcut(tr("Ctrl+Up"));
    goToContainer->addCommand(upOneLevelCommand);
    
    Command *homeCommand = new Command(Constants::Ids::Actions::Home, this);
    homeCommand->setDefaultText(tr("Go to Home Directory"));
    homeCommand->setDefaultShortcut(tr("Ctrl+H"));
    goToContainer->addCommand(homeCommand);

    CommandContainer *toolsContainer = new CommandContainer(Constants::Ids::Menus::Tools, this);
    toolsContainer->setTitle(tr("Tools"));
    menuBarContainer->addContainer(toolsContainer);

    Command *pluginsCommand = new Command(Constants::Ids::Actions::Plugins, this);
    pluginsCommand->setDefaultText(tr("Plugins..."));
    pluginsCommand->setAlwaysEnabled(true);
    connect(pluginsCommand->commandAction(), SIGNAL(triggered()), SLOT(showPluginView()));
    toolsContainer->addCommand(pluginsCommand);
}

Q_EXPORT_PLUGIN(CorePluginImpl)
