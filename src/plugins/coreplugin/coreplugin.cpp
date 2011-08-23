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

CorePluginImpl::~CorePluginImpl()
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
    ActionManager::instance()->command(Constants::Ids::Actions::NewTab)->action(window, SLOT(newTab()));
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
    newWindowCommand->setContext(Command::ApplicationCommand);
    connect(newWindowCommand->commandAction(), SIGNAL(triggered()), SLOT(newWindow()));
    fileContainer->addCommand(newWindowCommand, group);

    Command *newTabCommand = new Command(Constants::Ids::Actions::NewTab, this);
    newTabCommand->setDefaultText(tr("New tab"));
    newTabCommand->setDefaultShortcut(tr("Ctrl+T"));
    newTabCommand->setContext(Command::WindowCommand);
    fileContainer->addCommand(newTabCommand, group);

    Command *openCommand = new Command(Constants::Ids::Actions::Open, this);
    openCommand->setDefaultText(tr("Open"));
#ifdef Q_OS_MAC
    openCommand->setDefaultShortcut(tr("Ctrl+O"));
#else
    openCommand->setDefaultShortcut(tr("Return"));
#endif
    openCommand->setContext(Command::WindowCommand);
    fileContainer->addCommand(openCommand, group);

    Command *closeTabCommand = new Command(Constants::Ids::Actions::CloseTab, this);
    closeTabCommand->setDefaultText(tr("Close"));
    closeTabCommand->setDefaultShortcut(tr("Ctrl+W"));
    closeTabCommand->setContext(Command::WindowCommand);
    fileContainer->addCommand(closeTabCommand, group);

    // ================ File Menu (Change) ================
    fileContainer->addGroup(group = Constants::Ids::MenuGroups::FileChange);

    Command *newFolderCommand = new Command(Constants::Ids::Actions::NewFolder, this);
    newFolderCommand->setDefaultText(tr("New folder"));
    newFolderCommand->setDefaultShortcut(tr("Ctrl+Shift+N"));
    fileContainer->addCommand(newFolderCommand, group);

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
    exitCommand->setDefaultText(tr("Quit Andromeda"));
    exitCommand->setDefaultShortcut(tr("Ctrl+Q"));
    exitCommand->setAlwaysEnabled(true);
    fileContainer->addCommand(exitCommand, group);
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
    iconModeCommand->setContext(Command::WindowCommand);
    viewContainer->addCommand(iconModeCommand, group);

    Command *columnModeCommand = new Command(Constants::Ids::Actions::ColumnMode, this);
    columnModeCommand->setDefaultText(tr("Column View"));
    columnModeCommand->setDefaultShortcut(tr("Ctrl+2"));
    columnModeCommand->setCheckable(true);
    columnModeCommand->setContext(Command::WindowCommand);
    viewContainer->addCommand(columnModeCommand, group);

    Command *treeModeCommand = new Command(Constants::Ids::Actions::TreeMode, this);
    treeModeCommand->setDefaultText(tr("Tree View"));
    treeModeCommand->setDefaultShortcut(tr("Ctrl+3"));
    treeModeCommand->setCheckable(true);
    treeModeCommand->setContext(Command::WindowCommand);
    viewContainer->addCommand(treeModeCommand, group);

    Command *dualPaneCommand = new Command(Constants::Ids::Actions::DualPane, this);
    dualPaneCommand->setDefaultText(tr("Dual Pane"));
    dualPaneCommand->setDefaultShortcut(tr("Ctrl+4"));
    // TODO: change API or realization!!!
    dualPaneCommand->setCheckable(true);
    dualPaneCommand->setContext(Command::WindowCommand);
    viewContainer->addCommand(dualPaneCommand, group);

    // ================ GoTo Menu ================
    CommandContainer *goToContainer = new CommandContainer(Constants::Ids::Menus::GoTo, this);
    goToContainer->setTitle(tr("Go to"));
    menuBarContainer->addContainer(goToContainer);

    // ================ GoTo Menu (default) ================
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
    upOneLevelCommand->setDefaultIcon(QIcon(":/images/icons/up.png"));
    upOneLevelCommand->setDefaultShortcut(tr("Ctrl+Up"));
    goToContainer->addCommand(upOneLevelCommand);
    
    // ================ GoTo Menu (Locations) ================
    goToContainer->addGroup(Constants::Ids::MenuGroups::Locations);

    createGotoDirCommand(QDesktopServices::DesktopLocation, QIcon(":/images/icons/desktopFolder.png"), tr("Ctrl+Shift+D"));
    createGotoDirCommand(QDesktopServices::HomeLocation,
                         QIcon::fromTheme("go-home", QIcon(":/images/icons/homeFolder.png")),
                         tr("Ctrl+Shift+H"));
    createGotoDirCommand(QDesktopServices::DocumentsLocation, QIcon(":/images/icons/documentsFolder.png"), tr("Ctrl+Shift+O"));
#ifdef Q_OS_MAC
    createGotoDirCommand(QDesktopServices::ApplicationsLocation, QIcon(":/images/icons/appsFolder.png"), tr("Ctrl+Shift+A"));
#endif
    createGotoDirCommand(QDesktopServices::MusicLocation, QIcon(":/images/icons/musicFolder.png"));
    createGotoDirCommand(QDesktopServices::MoviesLocation, QIcon(":/images/icons/movieFolder.png"));
    createGotoDirCommand(QDesktopServices::PicturesLocation, QIcon(":/images/icons/picturesFolder.png"));

    // ================ Tools Menu ================
    CommandContainer *toolsContainer = new CommandContainer(Constants::Ids::Menus::Tools, this);
    toolsContainer->setTitle(tr("Tools"));
    menuBarContainer->addContainer(toolsContainer);

    Command *pluginsCommand = new Command(Constants::Ids::Actions::Plugins, this);
    pluginsCommand->setDefaultText(tr("Plugins..."));
//    pluginsCommand->setAlwaysEnabled(true);
    pluginsCommand->setContext(Command::ApplicationCommand);
    connect(pluginsCommand->action(), SIGNAL(triggered()), SLOT(showPluginView()));
    toolsContainer->addCommand(pluginsCommand);
}

void CorePluginImpl::createGotoDirCommand(QDesktopServices::StandardLocation location, const QIcon &icon, const QKeySequence &key)
{
    GuiSystem::CommandContainer * container = ActionManager::instance()->container(Constants::Ids::Menus::GoTo);
    QDir dir(QDesktopServices::storageLocation(location));

    if (!dir.exists())
        return;

    QString id = QString(QLatin1String(Constants::Ids::Actions::Goto)).arg(location);
    Command *cmd = new Command(id.toLatin1(), this);
    cmd->setData(dir.absolutePath());
    QString displayName(QDesktopServices::displayName(location));
    // fir for broken linux Qt
    if (displayName.isEmpty())
    {
        QString path = dir.absolutePath();
        int index = path.lastIndexOf(QLatin1Char('/'));
        if (index != -1)
            displayName = path.mid(index + 1);
    }

    cmd->setDefaultText(displayName);
    cmd->setDefaultShortcut(key);

    if (!icon.isNull())
        cmd->setDefaultIcon(icon);

    cmd->setData(dir.absolutePath());
    cmd->setContext(Command::ApplicationCommand);

    container->addCommand(cmd, Constants::Ids::MenuGroups::Locations);
}

Q_EXPORT_PLUGIN(CorePluginImpl)
