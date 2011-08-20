#include "coreplugin.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QTimer>
#include <QtCore/QtPlugin>
#include <QtCore/QDir>
#include <QtGui/QMenu>
#include <QtGui/QDesktopServices>

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
    connect(qApp, SIGNAL(messageReceived(QString)), SLOT(handleMessage(QString)));
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
    PluginView *view = object<PluginView>(QLatin1String("pluginView"));
    view->exec();
}

void CorePluginImpl::handleMessage(const QString &message)
{
    if (message == QLatin1String("activate"))
        newWindow();
}

void CorePluginImpl::createActions()
{
    // shared separator
    Command *separator = new Command(Constants::Ids::Actions::Separator, this);
    separator->setSeparator(true);

    CommandContainer *menuBarContainer = new CommandContainer(Constants::Ids::Menus::MenuBar, this);

    const char *group = 0;

    // ================ File Menu ================
    CommandContainer *fileContainer = new CommandContainer(Constants::Ids::Menus::File, this);
    fileContainer->setTitle(tr("File"));
    menuBarContainer->addContainer(fileContainer);

    // ================ File Menu (New) ================
    fileContainer->addGroup(group = Constants::Ids::MenuGroups::FileNew);

    Command *newWindowCommand = new Command(Constants::Ids::Actions::NewWindow, this);
    newWindowCommand->setDefaultText(tr("New window"));
    newWindowCommand->setDefaultShortcut(tr("Ctrl+N"));
    newWindowCommand->setAlwaysEnabled(true);
    connect(newWindowCommand->commandAction(), SIGNAL(triggered()), SLOT(newWindow()));
    fileContainer->addCommand(newWindowCommand, group);

    Command *newTabCommand = new Command(Constants::Ids::Actions::NewTab, this);
    newTabCommand->setDefaultText(tr("New tab"));
    newTabCommand->setDefaultShortcut(tr("Ctrl+T"));
    fileContainer->addCommand(newTabCommand, group);

    Command *newFolderCommand = new Command(Constants::Ids::Actions::NewFolder, this);
    newFolderCommand->setDefaultText(tr("New folder"));
    newFolderCommand->setDefaultShortcut(tr("Ctrl+Shift+N"));
    fileContainer->addCommand(newFolderCommand, group);

    Command *openCommand = new Command(Constants::Ids::Actions::Open, this);
    openCommand->setDefaultText(tr("Open"));
#ifdef Q_OS_MAC
    openCommand->setDefaultShortcut(tr("Ctrl+O"));
#else
    openCommand->setDefaultShortcut(tr("Return"));
#endif
    fileContainer->addCommand(openCommand, group);

    Command *closeTabCommand = new Command(Constants::Ids::Actions::CloseTab, this);
    closeTabCommand->setDefaultText(tr("Close"));
    closeTabCommand->setDefaultShortcut(tr("Ctrl+W"));
    fileContainer->addCommand(closeTabCommand, group);

    // ================ File Menu (Change) ================
    fileContainer->addGroup(group = Constants::Ids::MenuGroups::FileChange);

    Command *renameCommand = new Command(Constants::Ids::Actions::Rename, this);
    renameCommand->setDefaultText(tr("Rename"));
#ifdef Q_OS_MAC
    renameCommand->setDefaultShortcut(tr("Return"));
#else
    renameCommand->setDefaultShortcut(tr("F2"));
#endif
    fileContainer->addCommand(renameCommand, group);

    Command *removeCommand = new Command(Constants::Ids::Actions::Remove, this);
    removeCommand->setDefaultText(tr("Remove"));
    removeCommand->setDefaultShortcut(tr("Ctrl+Shift+Backspace"));
    fileContainer->addCommand(removeCommand, group);

#ifndef Q_OS_MAC
    // ================ File Menu (Quit) ================
    fileContainer->addGroup(group = Constants::Ids::MenuGroups::FileQuit);

    Command *exitCommand = new Command(Constants::Ids::Actions::Exit, this);
    exitCommand->setDefaultText(tr("Exit"));
    exitCommand->setDefaultShortcut(tr("Ctrl+Q"));
    exitCommand->setAlwaysEnabled(true);
    fileContainer->addCommand(exitCommand);
    connect(exitCommand->commandAction(), SIGNAL(triggered()), qApp, SLOT(quit()));
#endif

    // ================ Edit Menu ================
    CommandContainer *editContainer = new CommandContainer(Constants::Ids::Menus::Edit, this);
    editContainer->setTitle(tr("Edit"));
    menuBarContainer->addContainer(editContainer);

    // ================ Edit Menu (Redo) ================
    editContainer->addGroup(group = Constants::Ids::MenuGroups::EditRedo);

    Command *undoCommand = new Command(Constants::Ids::Actions::Undo, this);
    undoCommand->setDefaultText(tr("Undo"));
    undoCommand->setDefaultShortcut(tr("Ctrl+Z"));
    editContainer->addCommand(undoCommand, group);

    Command *redoCommand = new Command(Constants::Ids::Actions::Redo, this);
    redoCommand->setDefaultText(tr("Redo"));
    redoCommand->setDefaultShortcut(tr("Ctrl+Shift+Z"));
    editContainer->addCommand(redoCommand, group);

    // ================ Edit Menu (CopyPaste) ================
    editContainer->addGroup(group = Constants::Ids::MenuGroups::EditCopyPaste);

    Command *cutCommand = new Command(Constants::Ids::Actions::Cut, this);
    cutCommand->setDefaultText(tr("Cut"));
    cutCommand->setDefaultShortcut(tr("Ctrl+X"));
    editContainer->addCommand(cutCommand, group);

    Command *copyCommand = new Command(Constants::Ids::Actions::Copy, this);
    copyCommand->setDefaultText(tr("Copy"));
    copyCommand->setDefaultShortcut(tr("Ctrl+C"));
    editContainer->addCommand(copyCommand, group);

    Command *pasteCommand = new Command(Constants::Ids::Actions::Paste, this);
    pasteCommand->setDefaultText(tr("Paste"));
    pasteCommand->setDefaultShortcut(tr("Ctrl+V"));
    editContainer->addCommand(pasteCommand, group);

    Command *selectAllCommand = new Command(Constants::Ids::Actions::SelectAll, this);
    selectAllCommand->setDefaultText(tr("Select All"));
    selectAllCommand->setDefaultShortcut(tr("Ctrl+A"));
    editContainer->addCommand(selectAllCommand, group);

    // ================ View Menu ================
    CommandContainer *viewContainer = new CommandContainer(Constants::Ids::Menus::View, this);
    viewContainer->setTitle(tr("View"));
    menuBarContainer->addContainer(viewContainer);

    viewContainer->addGroup(group = Constants::Ids::MenuGroups::ViewViewMode, true);

    Command *iconModeCommand = new Command(Constants::Ids::Actions::IconMode, this);
    iconModeCommand->setDefaultText(tr("Icon View"));
    iconModeCommand->setDefaultShortcut(tr("Ctrl+1"));
    iconModeCommand->setCheckable(true);
    viewContainer->addCommand(iconModeCommand, group);

    Command *columnModeCommand = new Command(Constants::Ids::Actions::ColumnMode, this);
    columnModeCommand->setDefaultText(tr("Column View"));
    columnModeCommand->setDefaultShortcut(tr("Ctrl+2"));
    columnModeCommand->setCheckable(true);
    viewContainer->addCommand(columnModeCommand, group);

    Command *treeModeCommand = new Command(Constants::Ids::Actions::TreeMode, this);
    treeModeCommand->setDefaultText(tr("Tree View"));
    treeModeCommand->setDefaultShortcut(tr("Ctrl+3"));
    treeModeCommand->setCheckable(true);
    viewContainer->addCommand(treeModeCommand, group);

    Command *dualPaneCommand = new Command(Constants::Ids::Actions::DualPane, this);
    dualPaneCommand->setDefaultText(tr("Dual Pane"));
    dualPaneCommand->setDefaultShortcut(tr("Ctrl+4"));
    // TODO: change API or realization!!!
    dualPaneCommand->setCheckable(true);
    viewContainer->addCommand(dualPaneCommand, group);

    // ================ GoTo Menu ================
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
    upOneLevelCommand->setDefaultIcon(QIcon::fromTheme("go-up", QIcon(":/images/icons/go-up.png")));
    upOneLevelCommand->setDefaultShortcut(tr("Ctrl+Up"));
    goToContainer->addCommand(upOneLevelCommand);
    
    goToContainer->addCommand(separator);
        
    // home is little bit different from DesktopLocation etc. - it has shortcut
    Command *homeCommand = new Command(Constants::Ids::Actions::Home, this);
    homeCommand->setDefaultText(tr("Home Directory"));
    homeCommand->setDefaultIcon(QIcon::fromTheme("go-home", QIcon(":/images/icons/go-home.png")));
    homeCommand->setDefaultShortcut(tr("Ctrl+H"));
    homeCommand->setData(QDir::homePath());
    homeCommand->setAlwaysEnabled(true);
    GuiSystem::ActionManager::instance()->addDefaultDirHandler(homeCommand);
    goToContainer->addCommand(homeCommand);

    createGotoDirCommand(goToContainer, QDesktopServices::DesktopLocation);
    createGotoDirCommand(goToContainer, QDesktopServices::DocumentsLocation);
    createGotoDirCommand(goToContainer, QDesktopServices::ApplicationsLocation);
    createGotoDirCommand(goToContainer, QDesktopServices::MusicLocation);
    createGotoDirCommand(goToContainer, QDesktopServices::MoviesLocation);
    createGotoDirCommand(goToContainer, QDesktopServices::PicturesLocation);

    // ================ Tools Menu ================
    CommandContainer *toolsContainer = new CommandContainer(Constants::Ids::Menus::Tools, this);
    toolsContainer->setTitle(tr("Tools"));
    menuBarContainer->addContainer(toolsContainer);

    Command *pluginsCommand = new Command(Constants::Ids::Actions::Plugins, this);
    pluginsCommand->setDefaultText(tr("Plugins..."));
    pluginsCommand->setAlwaysEnabled(true);
    connect(pluginsCommand->commandAction(), SIGNAL(triggered()), SLOT(showPluginView()));
    toolsContainer->addCommand(pluginsCommand);
}

void CorePluginImpl::createGotoDirCommand(CommandContainer * container,
                                               QDesktopServices::StandardLocation location,
                                               const QIcon &icon)
{
//    qDebug() << "CREATING" << location << QDesktopServices::displayName(location) << QDesktopServices::storageLocation(location);
    QDir path(QDesktopServices::storageLocation(location));
    if (!path.exists())
    {
//        qDebug() << "!E" << path.absolutePath();
        return;
    }

    Command *ret = new Command(path.absolutePath().toUtf8(), this);
    ret->setDefaultText(QDesktopServices::displayName(location));

    if (!icon.isNull())
        ret->setDefaultIcon(icon);

    ret->setData(path.absolutePath());
    ret->setAlwaysEnabled(true);
    
    container->addCommand(ret);
    
    GuiSystem::ActionManager::instance()->addDefaultDirHandler(ret);
}

Q_EXPORT_PLUGIN(CorePluginImpl)
