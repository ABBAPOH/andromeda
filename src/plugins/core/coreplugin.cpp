#include "coreplugin.h"

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QSettings>
#include <QtCore/QTimer>
#include <QtCore/QtPlugin>
#include <QtCore/QUrl>
#include <QtGui/QApplication>
#include <QtGui/QDesktopServices>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QMessageBox>
#include <QtGui/QSystemTrayIcon>

#include "commandssettingspage.h"
#include "constants.h"
#include "settingsmodel.h"

#include <extensionsystem/pluginmanager.h>
#include <extensionsystem/pluginview.h>
#include <guisystem/action.h>
#include <guisystem/actionmanager.h>
#include <guisystem/command.h>
#include <guisystem/commandcontainer.h>
#include <guisystem/settingsdialog.h>
#include <guisystem/settingspagemanager.h>

#include "settingswidget.h"

using namespace Core;
using namespace GuiSystem;

static const qint32 corePluginMagic = 0x6330386e; // "c08n"
static const qint8 corePluginVersion = 1;

CorePlugin::CorePlugin() :
    IPlugin()
{
}

CorePlugin::~CorePlugin()
{
}

bool CorePlugin::initialize(const QVariantMap &options)
{
    urls = options.value("files").toStringList();

    SettingsPageManager *pageManager = new SettingsPageManager;
    pageManager->setObjectName(QLatin1String("settingsPageManager"));
    addObject(pageManager);

    pageManager->addPage(new CommandsSettingsPage(this));

    createActions();
    connect(qApp, SIGNAL(messageReceived(QString)), SLOT(handleMessage(QString)));
    connect(PluginManager::instance(), SIGNAL(pluginsLoaded()), SLOT(restoreSession()));

    return true;
}

void CorePlugin::shutdown()
{
    qDeleteAll(BrowserWindow::windows());

    delete dockMenu;
#ifdef Q_OS_MAC
    delete menuBar;
#endif
}

bool CorePlugin::restoreState(const QByteArray &arr)
{
    QByteArray state = arr;
    QDataStream s(&state, QIODevice::ReadOnly);

    qint32 magic;
    qint8 version;
    quint32 windowCount;
    QByteArray windowState;

    s >> magic;
    if (magic != corePluginMagic)
        return false;

    s >> version;
    if (version != corePluginVersion)
        return false;

    s >> windowCount;

    if (windowCount == 0)
        newWindow();

    for (quint32 i = 0; i < windowCount; i++) {
        s >> windowState;

        BrowserWindow *window = new BrowserWindow();
        bool ok = window->restoreState(windowState);
        if (!ok)
            return false;

        window->show();
    }

    s >> settingsDialogState;

    return true;
}

QByteArray CorePlugin::saveState() const
{
    QByteArray state;
    QDataStream s(&state, QIODevice::WriteOnly);

    s << corePluginMagic;
    s << corePluginVersion;

    QList<BrowserWindow*> windows = BrowserWindow::windows();
    quint32 windowCount = windows.count();

    s << windowCount;

    for (quint32 i = 0; i < windowCount; i++) {
        s << windows[i]->saveState();
        // hide window to prevent strange crash when bookmarks editor is opened
        windows[i]->hide();
    }

    s << settingsDialogState;

    return state;
}

void CorePlugin::newWindow()
{
    BrowserWindow::newWindow();
}

void CorePlugin::showPluginView()
{
    PluginView *view = object<PluginView>(QLatin1String("pluginView"));
    view->exec();
}

void CorePlugin::showSettings()
{
    SettingsWidget *widget = new SettingsWidget;
    widget->setAttribute(Qt::WA_DeleteOnClose);

    SettingsModel *settingsModel = new SettingsModel(widget);
    QSettings *settings = new QSettings(settingsModel);
    settingsModel->setSettings(settings);
    widget->setModel(settingsModel);

    widget->show();
}

void CorePlugin::prefenrences()
{
    SettingsPageManager *pageManager = object<SettingsPageManager>("settingsPageManager");

    // TODO: test unloading pages while dialog is running
    if (!settingsDialog) {
        settingsDialog = new SettingsDialog();
        settingsDialog->setAttribute(Qt::WA_DeleteOnClose);
        settingsDialog->setSettingsPageManager(pageManager);
        settingsDialog->restoreState(settingsDialogState);
        settingsDialog->installEventFilter(this);
        settingsDialog->show();
    } else {
        settingsDialog->raise();
        settingsDialog->activateWindow();
    }
}

void CorePlugin::handleMessage(const QString &message)
{
    if (message == QLatin1String("activate"))
        newWindow();
}

void CorePlugin::restoreSession()
{
    loadSettings();

    if (!urls.isEmpty()) {
        BrowserWindow *window = new BrowserWindow();
        foreach (const QString &url, urls)
            window->openNewEditor(QUrl::fromUserInput(url));
        window->show();
        return;
    }

    bool ok = true;
    QString dataPath = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    QString filePath = dataPath + QLatin1Char('/') + QLatin1String("session");
    QFile f(filePath);
    if (ok)
        ok = f.open(QFile::ReadOnly);

    if (ok) {
        QByteArray state = f.readAll();
        ok = restoreState(state);
        if (!ok)
            qWarning() << tr("Couldn't restore session (located at %1)").arg(filePath);
    }

    // We couldn't load session, fallback to creating window and opening default path
    if (!ok)
        newWindow();
}

void CorePlugin::saveSession()
{
    QString dataPath = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    QFile f(dataPath + QLatin1Char('/') + QLatin1String("session"));

    saveSettings();

    if (!f.open(QFile::WriteOnly))
        return;

    f.write(saveState());
}

void CorePlugin::loadSettings() 
{
    m_settings = new QSettings(this);
    m_settings->beginGroup(QLatin1String("mainWindow"));
    QByteArray geometry = m_settings->value(QLatin1String("geometry")).toByteArray();

    if (!geometry.isEmpty())
        BrowserWindow::setWindowGeometry(geometry);
}

void CorePlugin::saveSettings() 
{
    m_settings->setValue(QLatin1String("geometry"), BrowserWindow::windowGeometry());
}

void CorePlugin::quit()
{
    saveSession();

    QMetaObject::invokeMethod(PluginManager::instance(), "unloadPlugins", Qt::QueuedConnection);
    QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
}

void CorePlugin::about()
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

void CorePlugin::aboutQt()
{
    QMessageBox::aboutQt(0);
}

bool CorePlugin::eventFilter(QObject *o, QEvent *e)
{
    if (o == settingsDialog) {
        if (e->type() == QEvent::Close) {
            settingsDialogState = settingsDialog->saveState();
        }
    }
    return false;
}

void CorePlugin::createActions()
{
    new CommandContainer(Constants::Menus::MenuBar, this);

    createFileMenu();
    createEditMenu();
    createViewMenu();
    createGoToMenu();
    createToolsMenu();
    createHelpMenu();
    createDockMenu();
    registerAtions();
}

void CorePlugin::createFileMenu()
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
    group = Constants::MenuGroups::FileNew;

    cmd = new Command(Constants::Actions::NewWindow, QKeySequence("Ctrl+N"), tr("New window"), this);
    cmd->setContext(Command::ApplicationCommand);
    container->addCommand(cmd, group);

    cmd = new Command(Constants::Actions::NewTab, QKeySequence("Ctrl+T"), tr("New tab"), this);
    cmd->setContext(Command::WindowCommand);
    container->addCommand(cmd, group);

    cmd = new Command(Constants::Actions::CloseTab, QKeySequence("Ctrl+W"), tr("Close"), this);
    cmd->setContext(Command::WindowCommand);
    container->addCommand(cmd, group);

    // ================ File Menu (Save) ================
    group = Constants::MenuGroups::FileSave;

    cmd = new Command(Constants::Actions::Save, QKeySequence::Save, tr("Save"), this);
    cmd->setContext(Command::WindowCommand);
    container->addCommand(cmd, group);

    cmd = new Command(Constants::Actions::SaveAs, QKeySequence::SaveAs, tr("Save as..."), this);
    cmd->setContext(Command::WindowCommand);
    container->addCommand(cmd, group);

    // ================ File Menu (Quit) ================
    group = Constants::MenuGroups::FileQuit;

    cmd = new Command(Constants::Actions::Quit, QKeySequence("Ctrl+Q"), tr("Quit Andromeda"), this);
    cmd->setContext(Command::ApplicationCommand);
    cmd->setAttributes(Command::AttributeNonConfigurable);
    cmd->commandAction()->setMenuRole(QAction::QuitRole);
    container->addCommand(cmd, group);
}

void CorePlugin::createEditMenu()
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
    group = Constants::MenuGroups::EditRedo;

    cmd = new Command(Constants::Actions::Undo, QKeySequence::Undo, tr("Undo"), this);
    cmd->setAttributes(Command::AttributeUpdateText);
    container->addCommand(cmd, group);

    cmd = new Command(Constants::Actions::Redo, QKeySequence::Redo, tr("Redo"), this);
    cmd->setAttributes(Command::AttributeUpdateText);
    container->addCommand(cmd, group);

    // ================ Edit Menu (CopyPaste) ================
    group = Constants::MenuGroups::EditCopyPaste;

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

    // ================ Edit Menu (Find) ================

    group = Constants::MenuGroups::EditFind;

    cmd = new Command(Constants::Actions::Find, QKeySequence::Find, tr("Find"), this);
    container->addCommand(cmd, group);

    cmd = new Command(Constants::Actions::FindNext, QKeySequence::FindNext, tr("Find next"), this);
    container->addCommand(cmd, group);

    cmd = new Command(Constants::Actions::FindPrevious, QKeySequence::FindPrevious, tr("Find previous"), this);
    container->addCommand(cmd, group);
}

void CorePlugin::createViewMenu()
{
    ActionManager *actionManager = ActionManager::instance();
    CommandContainer *menuBarContainer = actionManager->container(Constants::Menus::MenuBar);
//    const char *group = 0;

    // ================ View Menu ================
    CommandContainer *container = new CommandContainer(Constants::Menus::View, this);
    container->setTitle(tr("View"));
    menuBarContainer->addContainer(container);
}

void CorePlugin::createGoToMenu()
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

void CorePlugin::createToolsMenu()
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

    Command *settingsCommand = new Command(Constants::Actions::Settings, this);
    settingsCommand->setDefaultText(tr("View all settings..."));
    settingsCommand->setContext(Command::ApplicationCommand);
    toolsContainer->addCommand(settingsCommand);

    // ================ Tools Menu (Preferences) ================
    group = Constants::MenuGroups::ToolsPreferences;

    Command *preferencesCommand = new Command(Constants::Actions::Preferences, this);
    preferencesCommand->setDefaultText(tr("Preferences"));
    preferencesCommand->setDefaultShortcut(QKeySequence::Preferences);
    preferencesCommand->setContext(Command::ApplicationCommand);
    preferencesCommand->setAttributes(Command::AttributeNonConfigurable);
    preferencesCommand->commandAction()->setMenuRole(QAction::PreferencesRole);
    toolsContainer->addCommand(preferencesCommand, group);
}

void CorePlugin::createHelpMenu()
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
    cmd->setAttributes(Command::AttributeNonConfigurable);
    cmd->commandAction()->setMenuRole(QAction::AboutRole);
    container->addCommand(cmd);
//    connect(cmd->commandAction(), SIGNAL(triggered()), SLOT(about()));

    cmd = new Command(Constants::Actions::AboutQt, tr("About Qt..."), this);
    cmd->setContext(Command::ApplicationCommand);
    cmd->setAttributes(Command::AttributeNonConfigurable);
    cmd->commandAction()->setMenuRole(QAction::AboutQtRole);
    container->addCommand(cmd);
//    connect(cmd->commandAction(), SIGNAL(triggered()), SLOT(aboutQt()));
}

#ifdef Q_OS_MAC
void qt_mac_set_dock_menu(QMenu *menu);
#endif

void CorePlugin::createDockMenu()
{
    ActionManager *actionManager = ActionManager::instance();

    Command *cmd = 0;
    CommandContainer *container = new CommandContainer(Constants::Menus::Dock, this);
#ifdef Q_OS_MAC
    container->setTitle(tr("Dock menu"));
#else
    container->setTitle(tr("Tray menu"));
#endif

    cmd = actionManager->command(Constants::Actions::Quit);
    container->addCommand(actionManager->command(Constants::Actions::NewWindow));
#ifndef Q_OS_MAC
    container->addCommand(actionManager->command(Constants::Actions::Quit), "quit");
#endif

    dockMenu = container->menu();

#ifdef Q_OS_MAC
    qt_mac_set_dock_menu(dockMenu);
#else
    QSystemTrayIcon *tray = new QSystemTrayIcon(this);
    tray->setIcon(QIcon(":/images/icons/andromeda.png"));
    tray->setContextMenu(dockMenu);
    tray->show();
#endif
}

void CorePlugin::registerAtions()
{
    createAction(Constants::Actions::NewWindow, SLOT(newWindow()));
    createAction(Constants::Actions::Quit, SLOT(quit()));
    createAction(Constants::Actions::Plugins, SLOT(showPluginView()));
    createAction(Constants::Actions::Settings, SLOT(showSettings()));
    createAction(Constants::Actions::Preferences, SLOT(prefenrences()));
    createAction(Constants::Actions::About, SLOT(about()));
    createAction(Constants::Actions::AboutQt, SLOT(aboutQt()));

#ifdef Q_OS_MAC
    // Create menu bar now
    menuBar = ActionManager::instance()->container(Constants::Menus::MenuBar)->menuBar();
#endif
}

void CorePlugin::createAction(const QByteArray &id, const char *slot)
{
    Action *action = new Action(id, this);
    connect(action, SIGNAL(triggered()), slot);
}

Q_EXPORT_PLUGIN(CorePlugin)
