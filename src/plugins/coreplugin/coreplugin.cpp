#include "coreplugin.h"

#include <QtCore/QSettings>
#include <QtCore/QTimer>
#include <QtCore/QtPlugin>
#include <QtCore/QUrl>
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
            window->openNewTab(QUrl::fromUserInput(url));
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
    QString text = tr("<h3>Andromeda %1</h3><br/>"
                      "Revision %2<br/>"
                      "<br/>"
                      "Copyright 2010-2011 %3<br/>"
                      "Bugreports send to %4<br/>"
                      "<br/>"
                      "This is alpha version.<br/>"
                      "<br/>"
                      "The program is provided as is with no warranty of any kind, "
                      "including the warranty of design, merchantability "
                      "and fitness for a particular purpose.").
            arg(QLatin1String(PROJECT_VERSION)).
            arg(QString(QLatin1String(GIT_REVISION)).left(10)).
            arg(QLatin1String("ABBAPOH")).
            arg(QLatin1String("ABBAPOH@nextmail.ru"));

//    QMessageBox::about(0, tr("About Andromeda"), text);
    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("About Andromeda"));
    msgBox.setText(text);
    msgBox.setIconPixmap(QPixmap(":/images/icons/andromeda.png"));
    msgBox.exec();
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

    cmd = new Command(Constants::Actions::NewWindow, QKeySequence::New, tr("New window"), this);
    cmd->setContext(Command::ApplicationCommand);
    container->addCommand(cmd, group);
    connect(cmd->commandAction(), SIGNAL(triggered()), SLOT(newWindow()));

    cmd = new Command(Constants::Actions::NewTab, QKeySequence::AddTab, tr("New tab"), this);
    cmd->setContext(Command::WindowCommand);
    container->addCommand(cmd, group);

    cmd = new Command(Constants::Actions::Open, tr("Open"), this);
#ifdef Q_OS_MAC
    cmd->setDefaultShortcut(QKeySequence::Open);
#else
//    openCommand->setDefaultShortcut(tr("Return"));
#endif
    cmd->setContext(Command::WidgetCommand);
    container->addCommand(cmd, group);

    cmd = new Command(Constants::Actions::CloseTab, QKeySequence::Close, tr("Close"), this);
    cmd->setContext(Command::WindowCommand);
    container->addCommand(cmd, group);

//#ifndef Q_OS_MAC
    // ================ File Menu (Quit) ================
    container->addGroup(group = Constants::MenuGroups::FileQuit);

    cmd = new Command(Constants::Actions::Exit, QKeySequence::Quit, tr("Quit Andromeda"), this);
    cmd->setContext(Command::ApplicationCommand);
    cmd->commandAction()->setMenuRole(QAction::QuitRole);
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

    cmd = new Command(Constants::Actions::Undo, QKeySequence::Undo, tr("Undo"), this);
    cmd->setAttributes(Command::AttributeUpdateText);
    container->addCommand(cmd, group);

    cmd = new Command(Constants::Actions::Redo, QKeySequence::Redo, tr("Redo"), this);
    cmd->setAttributes(Command::AttributeUpdateText);
    container->addCommand(cmd, group);

    // ================ Edit Menu (CopyPaste) ================
    container->addGroup(group = Constants::MenuGroups::EditCopyPaste);

    cmd = new Command(Constants::Actions::Cut, QKeySequence::Cut, tr("Cut"), this);
    cmd->setAttributes(Command::AttributeUpdateText);
    container->addCommand(cmd, group);

    cmd = new Command(Constants::Actions::Copy, QKeySequence::Copy, tr("Copy"), this);
    cmd->setAttributes(Command::AttributeUpdateText);
    container->addCommand(cmd, group);

    cmd = new Command(Constants::Actions::Paste, QKeySequence::Paste, tr("Paste"), this);
    container->addCommand(cmd, group);

    cmd = new Command(Constants::Actions::SelectAll, QKeySequence::SelectAll, tr("Select All"), this);
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
    cmd->setDefaultShortcut(QKeySequence::Back);
    container->addCommand(cmd);

    cmd = new Command(Constants::Actions::Forward, tr("Forward"), this);
    cmd->setDefaultShortcut(QKeySequence::Forward);
    container->addCommand(cmd);

    cmd = new Command(Constants::Actions::Up, QKeySequence(QLatin1String("Ctrl+Up")), tr("Up one level"), this);
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
    preferencesCommand->setDefaultShortcut(QKeySequence::Preferences);
    preferencesCommand->setContext(Command::ApplicationCommand);
    preferencesCommand->commandAction()->setMenuRole(QAction::PreferencesRole);
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

    cmd = new Command(Constants::Actions::About, tr("About Andromeda..."), this);
    cmd->setContext(Command::ApplicationCommand);
    cmd->commandAction()->setMenuRole(QAction::AboutRole);
    container->addCommand(cmd);
    connect(cmd->commandAction(), SIGNAL(triggered()), SLOT(about()));

    cmd = new Command(Constants::Actions::AboutQt, tr("About Qt..."), this);
    cmd->setContext(Command::ApplicationCommand);
    cmd->commandAction()->setMenuRole(QAction::AboutQtRole);
    container->addCommand(cmd);
    connect(cmd->commandAction(), SIGNAL(triggered()), SLOT(aboutQt()));
}

Q_EXPORT_PLUGIN(CorePluginImpl)
