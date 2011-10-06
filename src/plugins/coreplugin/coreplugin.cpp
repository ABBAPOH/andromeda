#include "coreplugin.h"

#include <QtCore/QSettings>
#include <QtCore/QTimer>
#include <QtCore/QtPlugin>
#include <QtGui/QApplication>
#include <QtGui/QMenu>
#include <QtGui/QMessageBox>

#include "constants.h"
#include "core.h"
#include "settings.h"
#include "settingspagemanager.h"
#include "settingsdialog.h"

#include <actionmanager.h>
#include <command.h>
#include <commandcontainer.h>
#include <editormanager.h>
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

bool CorePluginImpl::initialize(const QVariantMap &options)
{
    urls = options.value("files").toStringList();
    addObject(new Core);

    Settings *settings = new Settings;
    settings->setObjectName("settings");
    addObject(settings);

    EditorManager *editorManager = new EditorManager;
    editorManager->setObjectName("editorManager");
    addObject(editorManager);

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
    if (!urls.isEmpty()) {
        MainWindow *window = new MainWindow();
        foreach (const QString &url, urls)
            window->openNewTab(url);
        window->show();
        return;
    }

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
    new CommandContainer(Constants::Menus::MenuBar, this);

    createFileMenu();
    createEditMenu();
    createViewMenu();
    createGoToMenu();
    createToolsMenu();
    createHelpMenu();
}

void CorePluginImpl::createFileMenu()
{
    ActionManager *actionManager = ActionManager::instance();
    CommandContainer *menuBarContainer = actionManager->container(Constants::Menus::MenuBar);

    Command *cmd = 0;
    CommandContainer *container = 0;
    const char *group = 0;

    // ================ File Menu ================
    container = new CommandContainer(Constants::Menus::File, this);
    container->setTitle(tr("File"));
    menuBarContainer->addContainer(container);

    // ================ File Menu (New) ================
    container->addGroup(group = Constants::MenuGroups::FileNew);

    cmd = new Command(Constants::Actions::NewWindow, tr("Ctrl+N"), tr("New window"), this);
    cmd->setContext(Command::ApplicationCommand);
    container->addCommand(cmd, group);
    connect(cmd->commandAction(), SIGNAL(triggered()), SLOT(newWindow()));

    cmd = new Command(Constants::Actions::NewTab, tr("Ctrl+T"), tr("New tab"), this);
    cmd->setContext(Command::WindowCommand);
    container->addCommand(cmd, group);

    cmd = new Command(Constants::Actions::Open, tr("Open"), this);
#ifdef Q_OS_MAC
    cmd->setDefaultShortcut(tr("Ctrl+O"));
#else
//    openCommand->setDefaultShortcut(tr("Return"));
#endif
    cmd->setContext(Command::WidgetCommand);
    container->addCommand(cmd, group);

    cmd = new Command(Constants::Actions::CloseTab, tr("Ctrl+W"), tr("Close"), this);
    cmd->setContext(Command::WindowCommand);
    container->addCommand(cmd, group);

//#ifndef Q_OS_MAC
    // ================ File Menu (Quit) ================
    container->addGroup(group = Constants::MenuGroups::FileQuit);

    cmd = new Command(Constants::Actions::Exit, tr("Ctrl+Q"), tr("Quit Andromeda"), this);
    cmd->setContext(Command::ApplicationCommand);
    container->addCommand(cmd, group);
    connect(cmd->commandAction(), SIGNAL(triggered()), this, SLOT(quit()));
//#endif
}

void CorePluginImpl::createEditMenu()
{
    ActionManager *actionManager = ActionManager::instance();
    CommandContainer *menuBarContainer = actionManager->container(Constants::Menus::MenuBar);

    Command *cmd = 0;
    CommandContainer *container = 0;
    const char *group = 0;

    // ================ Edit Menu ================
    container = new CommandContainer(Constants::Menus::Edit, this);
    container->setTitle(tr("Edit"));
    menuBarContainer->addContainer(container);

    // ================ Edit Menu (Redo) ================
    container->addGroup(group = Constants::MenuGroups::EditRedo);

    cmd = new Command(Constants::Actions::Undo, tr("Ctrl+Z"), tr("Undo"), this);
    container->addCommand(cmd, group);

    cmd = new Command(Constants::Actions::Redo, tr("Ctrl+Shift+Z"), tr("Redo"), this);
    container->addCommand(cmd, group);

    // ================ Edit Menu (CopyPaste) ================
    container->addGroup(group = Constants::MenuGroups::EditCopyPaste);

    cmd = new Command(Constants::Actions::Cut, tr("Ctrl+X"), tr("Cut"), this);
    cmd->setAttributes(Command::AttributeUpdateText);
    container->addCommand(cmd, group);

    cmd = new Command(Constants::Actions::Copy, tr("Ctrl+C"), tr("Copy"), this);
    cmd->setAttributes(Command::AttributeUpdateText);
    container->addCommand(cmd, group);

    cmd = new Command(Constants::Actions::Paste, tr("Ctrl+V"), tr("Paste"), this);
    container->addCommand(cmd, group);

    cmd = new Command(Constants::Actions::SelectAll, tr("Ctrl+A"), tr("Select All"), this);
    container->addCommand(cmd, group);
}

void CorePluginImpl::createViewMenu()
{
    ActionManager *actionManager = ActionManager::instance();
    CommandContainer *menuBarContainer = actionManager->container(Constants::Menus::MenuBar);
//    const char *group = 0;

    // ================ View Menu ================
    CommandContainer *container = new CommandContainer(Constants::Menus::View, this);
    container->setTitle(tr("View"));
    menuBarContainer->addContainer(container);
}

void CorePluginImpl::createGoToMenu()
{
    ActionManager *actionManager = ActionManager::instance();
    CommandContainer *menuBarContainer = actionManager->container(Constants::Menus::MenuBar);

    Command *cmd = 0;
    CommandContainer *container = 0;
//    const char *group = 0;

    // ================ GoTo Menu ================
    container = new CommandContainer(Constants::Menus::GoTo, this);
    container->setTitle(tr("Go to"));
    menuBarContainer->addContainer(container);

    // ================ GoTo Menu (default) ================
    cmd = new Command(Constants::Actions::Back, tr("Back"), this);
    cmd->setDefaultShortcut(tr("Ctrl+["));
    container->addCommand(cmd);

    cmd = new Command(Constants::Actions::Forward, tr("Forward"), this);
    cmd->setDefaultShortcut(tr("Ctrl+]"));
    container->addCommand(cmd);

    cmd = new Command(Constants::Actions::Up, QIcon(":/images/icons/up.png"), tr("Ctrl+Up"), tr("Up one level"), this);
    container->addCommand(cmd);
}

void CorePluginImpl::createToolsMenu()
{
    ActionManager *actionManager = ActionManager::instance();
    CommandContainer *menuBarContainer = actionManager->container(Constants::Menus::MenuBar);
    const char *group = 0;

    // ================ Tools Menu ================
    CommandContainer *toolsContainer = new CommandContainer(Constants::Menus::Tools, this);
    toolsContainer->setTitle(tr("Tools"));
    menuBarContainer->addContainer(toolsContainer);

    Command *pluginsCommand = new Command(Constants::Actions::Plugins, this);
    pluginsCommand->setDefaultText(tr("Plugins..."));
    pluginsCommand->setContext(Command::ApplicationCommand);
    toolsContainer->addCommand(pluginsCommand);
    connect(pluginsCommand->commandAction(), SIGNAL(triggered()), SLOT(showPluginView()));

    // ================ Tools Menu (Preferences) ================
    toolsContainer->addGroup(group = Constants::MenuGroups::ToolsPreferences);

    Command *preferencesCommand = new Command(Constants::Actions::Preferences, this);
    preferencesCommand->setDefaultText(tr("Preferences"));
    preferencesCommand->setDefaultShortcut(tr("Ctrl+,"));
    preferencesCommand->setContext(Command::ApplicationCommand);
    toolsContainer->addCommand(preferencesCommand, group);
    connect(preferencesCommand->commandAction(), SIGNAL(triggered()), SLOT(prefenrences()));

}

void CorePluginImpl::createHelpMenu()
{
    ActionManager *actionManager = ActionManager::instance();
    CommandContainer *menuBarContainer = actionManager->container(Constants::Menus::MenuBar);

    Command *cmd = 0;
    CommandContainer *container = 0;
//    const char *group = 0;

    // ================ Help Menu ================
    container = new CommandContainer(Constants::Menus::Help, this);
    container->setTitle(tr("Help"));
    menuBarContainer->addContainer(container);

    cmd = new Command(Constants::Actions::About, tr("About..."), this);
    cmd->setContext(Command::ApplicationCommand);
    container->addCommand(cmd);
    connect(cmd->commandAction(), SIGNAL(triggered()), SLOT(about()));

    cmd = new Command(Constants::Actions::AboutQt, tr("About Qt..."), this);
    cmd->setContext(Command::ApplicationCommand);
    container->addCommand(cmd);
    connect(cmd->commandAction(), SIGNAL(triggered()), SLOT(aboutQt()));
}

Q_EXPORT_PLUGIN(CorePluginImpl)
