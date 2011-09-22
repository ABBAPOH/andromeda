#include "coreplugin.h"

#include <QtCore/QSettings>
#include <QtCore/QTimer>
#include <QtCore/QtPlugin>
#include <QtGui/QApplication>
#include <QtGui/QMenu>
#include <QtGui/QMessageBox>

#include "constants.h"
#include "core.h"
#include "perspectivemanager.h"
#include "settings.h"
#include "settingspagemanager.h"
#include "settingsdialog.h"

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

    Settings *settings = new Settings;
    settings->setObjectName("settings");
    addObject(settings);

    SettingsPageManager *pageManager = new SettingsPageManager;
    pageManager->setObjectName(QLatin1String("settingsPageManager"));
    addObject(pageManager);

    createActions();
    connect(qApp, SIGNAL(messageReceived(QString)), SLOT(handleMessage(QString)));
    connect(PluginManager::instance(), SIGNAL(pluginsLoaded()), SLOT(restoreSession()));

    connect(qApp, SIGNAL(lastWindowClosed()), SLOT(quit()), Qt::QueuedConnection);

    return true;
}

void CorePluginImpl::shutdown()
{
    qDeleteAll(MainWindow::windows());
}

void CorePluginImpl::newWindow()
{
    MainWindow::newWindow();
}

void CorePluginImpl::showPluginView()
{
    PluginView *view = object<PluginView>(QLatin1String("pluginView"));
    view->exec();
}

void CorePluginImpl::prefenrences()
{
    SettingsPageManager *pageManager = object<SettingsPageManager>("settingsPageManager");

    SettingsDialog settingsDialog;
    settingsDialog.setSettingsPageManager(pageManager);
    settingsDialog.exec();
}

void CorePluginImpl::handleMessage(const QString &message)
{
    if (message == QLatin1String("activate"))
        newWindow();
}

void CorePluginImpl::restoreSession()
{
    QSettings s(qApp->organizationName(), qApp->applicationName() + ".session");
    int windowCount = s.beginReadArray(QLatin1String("windows"));

    if (!windowCount)
        newWindow();

    for (int i = 0; i < windowCount; i++) {
        s.setArrayIndex(i);

        MainWindow *window = new MainWindow();
        window->restoreSession(s);
        window->show();
    }
    s.endArray();
}

void CorePluginImpl::saveSession()
{
    QSettings s(qApp->organizationName(), qApp->applicationName() + ".session");
    s.clear();
    QList<MainWindow*> windows = MainWindow::windows();
    int windowCount = windows.count();

    s.beginWriteArray(QLatin1String("windows"), windowCount);
    for (int i = 0; i < windowCount; i++) {
        s.setArrayIndex(i);
        windows[i]->saveSession(s);
    }
    s.endArray();
}

void CorePluginImpl::quit()
{
    saveSession();

    QMetaObject::invokeMethod(PluginManager::instance(), "unloadPlugins", Qt::QueuedConnection);
    QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
}

void CorePluginImpl::about()
{
    QMessageBox::about(0, tr("About Andromeda"), tr("Crossplatform file manager."));
}

void CorePluginImpl::aboutQt()
{
    QMessageBox::aboutQt(0);
}

void CorePluginImpl::createActions()
{
    CommandContainer *menuBarContainer = new CommandContainer(Constants::Menus::MenuBar, this);

    const char *group = 0;

    // ================ File Menu ================
    CommandContainer *fileContainer = new CommandContainer(Constants::Menus::File, this);
    fileContainer->setTitle(tr("File"));
    menuBarContainer->addContainer(fileContainer);

    // ================ File Menu (New) ================
    fileContainer->addGroup(group = Constants::MenuGroups::FileNew);

    Command *newWindowCommand = new Command(Constants::Actions::NewWindow, this);
    newWindowCommand->setDefaultText(tr("New window"));
    newWindowCommand->setDefaultShortcut(tr("Ctrl+N"));
    newWindowCommand->setContext(Command::ApplicationCommand);
    connect(newWindowCommand->commandAction(), SIGNAL(triggered()), SLOT(newWindow()));
    fileContainer->addCommand(newWindowCommand, group);

    Command *newTabCommand = new Command(Constants::Actions::NewTab, this);
    newTabCommand->setDefaultText(tr("New tab"));
    newTabCommand->setDefaultShortcut(tr("Ctrl+T"));
    newTabCommand->setContext(Command::WindowCommand);
    fileContainer->addCommand(newTabCommand, group);

    Command *openCommand = new Command(Constants::Actions::Open, this);
    openCommand->setDefaultText(tr("Open"));
#ifdef Q_OS_MAC
    openCommand->setDefaultShortcut(tr("Ctrl+O"));
#else
//    openCommand->setDefaultShortcut(tr("Return"));
#endif
    openCommand->setContext(Command::WidgetCommand);
    fileContainer->addCommand(openCommand, group);

    Command *closeTabCommand = new Command(Constants::Actions::CloseTab, this);
    closeTabCommand->setDefaultText(tr("Close"));
    closeTabCommand->setDefaultShortcut(tr("Ctrl+W"));
    closeTabCommand->setContext(Command::WindowCommand);
    fileContainer->addCommand(closeTabCommand, group);

    // ================ File Menu (Info) ================
    fileContainer->addGroup(group = Constants::MenuGroups::FileInfo);

    Command *fileInfoCommand = new Command(Constants::Actions::FileInfo, this);
    fileInfoCommand->setDefaultText(tr("File info"));
    fileInfoCommand->setDefaultShortcut(tr("Ctrl+I"));
    fileContainer->addCommand(fileInfoCommand, group);

    // ================ File Menu (Change) ================
    fileContainer->addGroup(group = Constants::MenuGroups::FileChange);

    Command *newFolderCommand = new Command(Constants::Actions::NewFolder, this);
    newFolderCommand->setDefaultText(tr("New folder"));
    newFolderCommand->setDefaultShortcut(tr("Ctrl+Shift+N"));
    fileContainer->addCommand(newFolderCommand, group);

    Command *renameCommand = new Command(Constants::Actions::Rename, this);
    renameCommand->setDefaultText(tr("Rename"));
#ifdef Q_OS_MAC
//    renameCommand->setDefaultShortcut(tr("Return")); // Can't set shorcut to prevent overriding edit triggers
#else
    renameCommand->setDefaultShortcut(tr("F2"));
#endif
    fileContainer->addCommand(renameCommand, group);

    Command *removeCommand = new Command(Constants::Actions::Remove, this);
    removeCommand->setDefaultText(tr("Remove"));
#ifdef Q_OS_MAC
    removeCommand->setDefaultShortcut(tr("Ctrl+Shift+Backspace"));
#else
    removeCommand->setDefaultShortcut(tr("Shift+Del"));
#endif

    fileContainer->addCommand(removeCommand, group);

//#ifndef Q_OS_MAC
    // ================ File Menu (Quit) ================
    fileContainer->addGroup(group = Constants::MenuGroups::FileQuit);

    Command *exitCommand = new Command(Constants::Actions::Exit, this);
    exitCommand->setDefaultText(tr("Quit Andromeda"));
    exitCommand->setDefaultShortcut(tr("Ctrl+Q"));
    exitCommand->setContext(Command::ApplicationCommand);
    fileContainer->addCommand(exitCommand, group);
    connect(exitCommand->commandAction(), SIGNAL(triggered()), this, SLOT(quit()));
//#endif

    // ================ Edit Menu ================
    CommandContainer *editContainer = new CommandContainer(Constants::Menus::Edit, this);
    editContainer->setTitle(tr("Edit"));
    menuBarContainer->addContainer(editContainer);

    // ================ Edit Menu (Redo) ================
    editContainer->addGroup(group = Constants::MenuGroups::EditRedo);

    Command *undoCommand = new Command(Constants::Actions::Undo, this);
    undoCommand->setDefaultText(tr("Undo"));
    undoCommand->setDefaultShortcut(tr("Ctrl+Z"));
    editContainer->addCommand(undoCommand, group);

    Command *redoCommand = new Command(Constants::Actions::Redo, this);
    redoCommand->setDefaultText(tr("Redo"));
    redoCommand->setDefaultShortcut(tr("Ctrl+Shift+Z"));
    editContainer->addCommand(redoCommand, group);

    // ================ Edit Menu (CopyPaste) ================
    editContainer->addGroup(group = Constants::MenuGroups::EditCopyPaste);

    Command *cutCommand = new Command(Constants::Actions::Cut, this);
    cutCommand->setDefaultText(tr("Cut"));
    cutCommand->setDefaultShortcut(tr("Ctrl+X"));
    editContainer->addCommand(cutCommand, group);

    Command *copyCommand = new Command(Constants::Actions::Copy, this);
    copyCommand->setDefaultText(tr("Copy"));
    copyCommand->setDefaultShortcut(tr("Ctrl+C"));
    editContainer->addCommand(copyCommand, group);

    Command *pasteCommand = new Command(Constants::Actions::Paste, this);
    pasteCommand->setDefaultText(tr("Paste"));
    pasteCommand->setDefaultShortcut(tr("Ctrl+V"));
    editContainer->addCommand(pasteCommand, group);

    Command *selectAllCommand = new Command(Constants::Actions::SelectAll, this);
    selectAllCommand->setDefaultText(tr("Select All"));
    selectAllCommand->setDefaultShortcut(tr("Ctrl+A"));
    editContainer->addCommand(selectAllCommand, group);

    // ================ Edit Menu (CopyPaste) ================
    editContainer->addGroup(group = Constants::MenuGroups::EditPreferences);

    Command *preferencesCommand = new Command(Constants::Actions::Preferences, this);
    preferencesCommand->setDefaultText(tr("Preferences"));
    preferencesCommand->setDefaultShortcut(tr("Ctrl+,"));
    preferencesCommand->setContext(Command::ApplicationCommand);
    editContainer->addCommand(preferencesCommand, group);
    connect(preferencesCommand->commandAction(), SIGNAL(triggered()), SLOT(prefenrences()));

    // ================ View Menu ================
    CommandContainer *viewContainer = new CommandContainer(Constants::Menus::View, this);
    viewContainer->setTitle(tr("View"));
    menuBarContainer->addContainer(viewContainer);

    viewContainer->addGroup(group = Constants::MenuGroups::ViewViewMode, true);

    Command *iconModeCommand = new Command(Constants::Actions::IconMode, this);
    iconModeCommand->setDefaultText(tr("Icon View"));
    iconModeCommand->setDefaultShortcut(tr("Ctrl+1"));
    iconModeCommand->setCheckable(true);
    iconModeCommand->setContext(Command::WindowCommand);
    viewContainer->addCommand(iconModeCommand, group);

    Command *columnModeCommand = new Command(Constants::Actions::ColumnMode, this);
    columnModeCommand->setDefaultText(tr("Column View"));
    columnModeCommand->setDefaultShortcut(tr("Ctrl+2"));
    columnModeCommand->setCheckable(true);
    columnModeCommand->setContext(Command::WindowCommand);
    viewContainer->addCommand(columnModeCommand, group);

    Command *treeModeCommand = new Command(Constants::Actions::TreeMode, this);
    treeModeCommand->setDefaultText(tr("Tree View"));
    treeModeCommand->setDefaultShortcut(tr("Ctrl+3"));
    treeModeCommand->setCheckable(true);
    treeModeCommand->setContext(Command::WindowCommand);
    viewContainer->addCommand(treeModeCommand, group);

    Command *dualPaneCommand = new Command(Constants::Actions::DualPane, this);
    dualPaneCommand->setDefaultText(tr("Dual Pane"));
    dualPaneCommand->setDefaultShortcut(tr("Ctrl+4"));
    // TODO: change API or realization!!!
    dualPaneCommand->setCheckable(true);
    dualPaneCommand->setContext(Command::WindowCommand);
    viewContainer->addCommand(dualPaneCommand, group);

    // ================ GoTo Menu ================
    CommandContainer *goToContainer = new CommandContainer(Constants::Menus::GoTo, this);
    goToContainer->setTitle(tr("Go to"));
    menuBarContainer->addContainer(goToContainer);

    // ================ GoTo Menu (default) ================
    Command *backCommand = new Command(Constants::Actions::Back, this);
    backCommand->setDefaultText(tr("Back"));
    backCommand->setDefaultShortcut(tr("Ctrl+["));
    goToContainer->addCommand(backCommand);

    Command *forwardCommand = new Command(Constants::Actions::Forward, this);
    forwardCommand->setDefaultText(tr("Forward"));
    forwardCommand->setDefaultShortcut(tr("Ctrl+]"));
    goToContainer->addCommand(forwardCommand);

    Command *upOneLevelCommand = new Command(Constants::Actions::Up, this);
    upOneLevelCommand->setDefaultText(tr("Up one level"));
    upOneLevelCommand->setDefaultIcon(QIcon(":/images/icons/up.png"));
    upOneLevelCommand->setDefaultShortcut(tr("Ctrl+Up"));
    goToContainer->addCommand(upOneLevelCommand);

    // ================ Tools Menu ================
    CommandContainer *toolsContainer = new CommandContainer(Constants::Menus::Tools, this);
    toolsContainer->setTitle(tr("Tools"));
    menuBarContainer->addContainer(toolsContainer);

    Command *pluginsCommand = new Command(Constants::Actions::Plugins, this);
    pluginsCommand->setDefaultText(tr("Plugins..."));
    pluginsCommand->setContext(Command::ApplicationCommand);
    connect(pluginsCommand->action(), SIGNAL(triggered()), SLOT(showPluginView()));
    toolsContainer->addCommand(pluginsCommand);

    // ================ Help Menu ================
    CommandContainer *helpContainer = new CommandContainer(Constants::Menus::Help, this);
    helpContainer->setTitle(tr("Help"));
    menuBarContainer->addContainer(helpContainer);

    Command *aboutCommand = new Command(Constants::Actions::Plugins, this);
    aboutCommand->setDefaultText(tr("About..."));
    aboutCommand->setContext(Command::ApplicationCommand);
    connect(aboutCommand->action(), SIGNAL(triggered()), SLOT(about()));
    helpContainer->addCommand(aboutCommand);

    Command *aboutQtCommand = new Command(Constants::Actions::Plugins, this);
    aboutQtCommand->setDefaultText(tr("About Qt..."));
    aboutQtCommand->setContext(Command::ApplicationCommand);
    connect(aboutQtCommand->action(), SIGNAL(triggered()), SLOT(aboutQt()));
    helpContainer->addCommand(aboutQtCommand);
}

Q_EXPORT_PLUGIN(CorePluginImpl)
