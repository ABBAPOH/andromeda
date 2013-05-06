#include "application.h"

#include <QtCore/QStringList>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QLocale>
#include <QtCore/QSettings>
#include <QtCore/QTranslator>

#include <QtGui/QIcon>
#include <QtGui/QDesktopServices>
#include <QtGui/QMenuBar>
#include <QtGui/QMessageBox>
#include <QtGui/QSystemTrayIcon>

#include <ExtensionSystem/ErrorsDialog>
#include <ExtensionSystem/PluginManager>
#include <ExtensionSystem/PluginView>

#include <GuiSystem/Action>
#include <GuiSystem/ActionManager>
#include <GuiSystem/Command>
#include <GuiSystem/CommandContainer>
#include <GuiSystem/EditorWindowFactory>
#include <GuiSystem/SettingsPageManager>
#include <GuiSystem/SettingsWindow>
#include <GuiSystem/StandardCommands>
#include <GuiSystem/constants.h>

#include <bookmarksplugin/bookmarksconstants.h>
#include <filemanager/filemanagerconstants.h>
#include <webviewplugin/webviewconstants.h>

#include <Widgets/WindowsMenu>

#include "browserwindow.h"
#include "browserwindow_p.h"
#include "commandssettingspage.h"
#include "generalsettingspage.h"
#include "settingsmodel.h"
#include "settingswidget.h"

using namespace ExtensionSystem;
using namespace GuiSystem;
using namespace Andromeda;

// returns root path of the application
static QString getRootPath()
{
    // Figure out root:  Up one from 'bin' or 'MacOs'
    QDir rootDir = QApplication::applicationDirPath();
    rootDir.cdUp();
    return rootDir.canonicalPath();
}

static inline QString getPluginPath()
{
    const QString rootDirPath = getRootPath();
    // Build path
    QString pluginPath = rootDirPath;
#if defined Q_OS_MACX
    pluginPath += QLatin1Char('/');
    pluginPath += QLatin1String("PlugIns");
#elif defined Q_OS_WIN
    pluginPath += QLatin1Char('/');
    pluginPath += QLatin1String("plugins");
#elif defined Q_OS_UNIX
    // not Mac UNIXes
    pluginPath += QLatin1Char('/');
    pluginPath += QLatin1String("lib");
    pluginPath += QLatin1String(LIB_SUFFIX);
    pluginPath += QLatin1Char('/');
    pluginPath += QLatin1String("andromeda");
    pluginPath += QLatin1Char('/');
    pluginPath += QLatin1String("plugins");
#endif
    return pluginPath;
}

static const qint32 andromedaAppMagic = 0x6330386e; // "c08n"
static const qint8 andromedaAppVersion = 1;

static QUrl urlFromUserInput(const QString &currentPath, const QString &text)
{
    // we try to resolve local paths
    QFileInfo info(text);
    if (info.isAbsolute()) {
        if (info.exists())
            return QUrl::fromLocalFile(info.canonicalFilePath());
        else
            return QUrl::fromUserInput(text);
    } else {
        QDir dir(currentPath);
        QString path = QDir::cleanPath(dir.absoluteFilePath(text));
        QFileInfo info(path);
        if (info.exists())
            return QUrl::fromLocalFile(path);
        else
            return QUrl::fromUserInput(text);
    }
}

class DockContainer : public CommandContainer
{
public:
    explicit DockContainer(const QByteArray &id, QObject *parent = 0);
    ~DockContainer();

    QMenu *createMenu(QWidget *parent) const;

private:
    QPointer<WindowsMenu> m_menu;
};

class WindowsContainer : public CommandContainer
{
public:
    explicit WindowsContainer(const QByteArray &id, QObject *parent = 0);
    ~WindowsContainer();

    QMenu *createMenu(QWidget *parent) const;

private:
    QPointer<WindowsMenu> m_menu;
};

DockContainer::DockContainer(const QByteArray &id, QObject *parent) :
    CommandContainer(id, parent),
    m_menu(new WindowsMenu)
{
}

DockContainer::~DockContainer()
{
    delete m_menu;
}

QMenu * DockContainer::createMenu(QWidget * /*parent*/) const
{
    return m_menu;
}

WindowsContainer::WindowsContainer(const QByteArray &id, QObject *parent) :
    CommandContainer(id, parent),
    m_menu(new WindowsMenu)
{
}

WindowsContainer::~WindowsContainer()
{
    delete m_menu;
}

QMenu * WindowsContainer::createMenu(QWidget *parent) const
{
    return m_menu;
}

Application::Application(int &argc, char **argv) :
    QtSingleApplication("Andromeda", argc, argv),
    m_pluginManager(0),
    dockMenu(0),
    m_firstStart(true)
{
    setOrganizationName("arch");
    setApplicationName("andromeda");

#ifdef ICON_LOCATION
    setWindowIcon(QIcon(ICON_LOCATION + QString("/andromeda.png")));
#endif

    setQuitOnLastWindowClosed(false);
    addLibraryPath(getPluginPath());

    m_pluginManager = new PluginManager(this);
    m_pluginManager->setPluginsFolder("andromeda");
    m_pluginManager->setTranslations(QStringList() <<
                                     "andromeda" <<
                                     "extensionsystem" <<
                                     "filemanager" <<
                                     "guisystem" <<
                                     "imageviewer" <<
                                     "widgets");

    EditorWindowFactory::setDefaultfactory(new BrowserWindowFactory(this));

    m_settingsPageManager = new SettingsPageManager(this);
    m_settingsPageManager->setObjectName("settingsPageManager");
    m_settingsPageManager->addPage(new GeneralSettingsPage(this));
    m_settingsPageManager->addPage(new CommandsSettingsPage(this));
    m_pluginManager->addObject(m_settingsPageManager);

    connect(this, SIGNAL(messageReceived(QString)), SLOT(handleMessage(QString)));
#ifndef Q_OS_MAC
    connect(this, SIGNAL(lastWindowClosed()), SLOT(exit()));
#endif
}

Application::~Application()
{
    delete dockMenu;
    delete trayIcon;
#ifdef Q_OS_MAC
    delete macMenuBar;
#endif
}

Application *Application::instance()
{
    return qobject_cast<Application *>(QApplication::instance());
}

bool Application::activateApplication()
{
    QStringList arguments = this->arguments();
    Q_ASSERT(!arguments.isEmpty());
    arguments[0] = QDir::currentPath();
    return sendMessage(arguments.join("\n"));
}

bool Application::loadPlugins()
{
    m_pluginManager->loadPlugins();

    createMenus();
    setupApplicationActions();
    restoreSession();

    QStringList args = arguments().mid(1);
    args.prepend(QDir::currentPath());
    handleArguments(args);
    m_pluginManager->postInitialize(arguments().mid(1));

    if (m_pluginManager->hasErrors()) {
        ErrorsDialog dlg;
        dlg.setMessage(tr("Errors occured during loading plugins."));
        dlg.setErrors(m_pluginManager->errors());
        dlg.exec();
    }

    if (m_pluginManager->plugins().isEmpty())
        return false;

    return true;
}

bool Application::isTrayIconVisible() const
{
    return trayIcon->isVisible();
}

void Application::setTrayIconVisible(bool visible)
{
    trayIcon->setVisible(visible);
}

/*!
    Creates new BrowserWindow.
*/
void Application::newWindow()
{
    BrowserWindow::newWindow();
}

/*!
    Shows plugins list.
*/
void Application::showPluginView()
{
    PluginView view;
    view.exec();
}

/*!
    Shows all settings.
*/
void Application::showSettings()
{
    SettingsWidget *widget = new SettingsWidget;
    widget->setAttribute(Qt::WA_DeleteOnClose);

    SettingsModel *settingsModel = new SettingsModel(widget);
    QSettings *settings = new QSettings(settingsModel);
    settingsModel->setSettings(settings);
    widget->setModel(settingsModel);

    widget->show();
}

/*!
    Shows preferences window.
*/
void Application::preferences()
{
    // TODO: test unloading pages while dialog is running
    if (!settingsWindow) {
        settingsWindow = new SettingsWindow();
        settingsWindow->setAttribute(Qt::WA_DeleteOnClose);
        settingsWindow->setSettingsPageManager(m_settingsPageManager);
        settingsWindow->restoreState(settingsWindowState);
        settingsWindow->installEventFilter(this);
        settingsWindow->show();
    } else {
        settingsWindow->raise();
        settingsWindow->activateWindow();
    }
}

void Application::restoreSession()
{
#ifdef Q_OS_MAC
    // Create menu bar now
    macMenuBar = menuBar->menuBar();
#endif

    loadSettings();

    bool ok = true;
    QString dataPath = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    QString filePath = dataPath + QLatin1Char('/') + QLatin1String("session");
    QFile f(filePath);
    if (ok)
        ok = f.open(QFile::ReadOnly);

    if (ok) {
        QByteArray state = f.readAll();
        ok = restoreApplicationState(state);
        if (!ok)
            qWarning() << tr("Couldn't restore session (located at %1)").arg(filePath);
    }

    // We couldn't load session, fallback to creating window and opening default path
    if (!ok)
        newWindow();
}

void Application::saveSession()
{
    QString dataPath = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    QFile f(dataPath + QLatin1Char('/') + QLatin1String("session"));

    saveSettings();

    if (!f.open(QFile::WriteOnly))
        return;

    f.write(saveApplicationState());
}

/*!
    Saves session and exits application.
*/
void Application::exit()
{
    saveSession();

    qDeleteAll(BrowserWindow::windows());

    QMetaObject::invokeMethod(PluginManager::instance(), "unloadPlugins", Qt::QueuedConnection);
    QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
}

/*!
    Shows "about" dialog.
*/
void Application::about()
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

    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("About Andromeda"));
    msgBox.setText(text);
    msgBox.setIconPixmap(QPixmap(":/andromeda/icons/andromeda.png"));
    msgBox.exec();
}

/*!
    Shows "about Qt" dialog.
*/
void Application::aboutQt()
{
    QMessageBox::aboutQt(0);
}

bool Application::eventFilter(QObject *object, QEvent *event)
{
    if (object == settingsWindow) {
        if (event->type() == QEvent::Close) {
            settingsWindowState = settingsWindow->saveState();
        }
    }
    return false;
}

void Application::handleMessage(const QString &message)
{
    QStringList arguments = message.split("\n");
    if (arguments.isEmpty())
        return;

    handleArguments(arguments);
    m_pluginManager->postInitialize(arguments.mid(1));
}

void Application::handleArguments(const QStringList &arguments)
{
    Q_ASSERT(arguments.count() > 0);

    QStringList urls = arguments.mid(1);
    QString currentDir = arguments.first();

    if (!urls.isEmpty()) {
        BrowserWindow *window = new BrowserWindow();
        foreach (const QString &url, urls)
            window->openNewTab(urlFromUserInput(currentDir, url));
        window->show();
        return;
    }

    if (EditorWindow::windows().isEmpty() || !m_firstStart) {
        BrowserWindow::newWindow();
    }

    m_firstStart = false;
}

bool Application::restoreApplicationState(const QByteArray &arr)
{
    QByteArray state = arr;
    QDataStream s(&state, QIODevice::ReadOnly);

    qint32 magic;
    qint8 version;
    quint32 windowCount;
    QByteArray windowState;
    bool trayIconVisible;

    s >> magic;
    if (magic != andromedaAppMagic)
        return false;

    s >> version;
    if (version != andromedaAppVersion)
        return false;

    s >> windowCount;

    if (windowCount == 0)
        newWindow();

    for (quint32 i = 0; i < windowCount; i++) {
        s >> windowState;

        BrowserWindow *window = new BrowserWindow();
        window->show();
        bool ok = window->restoreState(windowState);
        if (!ok)
            return false;
    }

    s >> settingsWindowState;
    s >> trayIconVisible;

    setTrayIconVisible(trayIconVisible);

    return true;
}

QByteArray Application::saveApplicationState() const
{
    QByteArray state;
    QDataStream s(&state, QIODevice::WriteOnly);

    s << andromedaAppMagic;
    s << andromedaAppVersion;

    QList<BrowserWindow*> windows = BrowserWindow::windows();
    quint32 windowCount = windows.count();

    s << windowCount;

    for (quint32 i = 0; i < windowCount; i++) {
        s << windows[i]->saveState();
        // hide window to prevent strange crash when bookmarks editor is opened
        windows[i]->hide();
    }

    s << settingsWindowState;
    s << isTrayIconVisible();

    return state;
}

void Application::loadSettings()
{
    m_settings = new QSettings(this);
    m_settings->beginGroup("mainWindow");
    QByteArray geometry = m_settings->value("geometry").toByteArray();

    if (!geometry.isEmpty())
        BrowserWindow::setWindowGeometry(geometry);
}

void Application::saveSettings()
{
    m_settings->setValue("geometry", BrowserWindow::windowGeometry());
}

void Application::createMenus()
{
    menuBar = new CommandContainer("MenuBar", this);
    createFileMenu();
    createEditMenu();
    createViewMenu();
    createGoToMenu();
    createBookmarksMenu();
    createPanesMenu();
    createToolsMenu();
    createWindowsMenu();
    createHelpMenu();
    createDockMenu();
}

void Application::createFileMenu()
{
    ActionManager *am = ActionManager::instance();
    CommandContainer *fileMenu = new CommandContainer(Constants::Menus::File, this);
    fileMenu->setText(tr("File"));
    menuBar->addCommand(fileMenu);

    fileMenu->addCommand(StandardCommands::standardCommand(StandardCommands::NewWindow));
    fileMenu->addCommand(StandardCommands::standardCommand(StandardCommands::NewTab));
    fileMenu->addCommand(StandardCommands::standardCommand(StandardCommands::Open));
    fileMenu->addCommand(am->command(Constants::Actions::OpenEditor));
    fileMenu->addCommand(StandardCommands::standardCommand(StandardCommands::Close));
    fileMenu->addSeparator();
    fileMenu->addCommand(StandardCommands::standardCommand(StandardCommands::Save));
    fileMenu->addCommand(StandardCommands::standardCommand(StandardCommands::SaveAs));
    fileMenu->addSeparator();
    fileMenu->addCommand(StandardCommands::standardCommand(StandardCommands::Refresh));
    fileMenu->addCommand(StandardCommands::standardCommand(StandardCommands::Cancel));
    fileMenu->addSeparator();
    fileMenu->addCommand(am->command(Constants::Actions::ShowFileInfo));
    fileMenu->addSeparator();
    fileMenu->addCommand(am->command(Constants::Actions::NewFolder));
    fileMenu->addCommand(am->command(Constants::Actions::Rename));
    fileMenu->addCommand(am->command(Constants::Actions::MoveToTrash));
    fileMenu->addCommand(am->command(Constants::Actions::Remove));
}

void Application::createEditMenu()
{
    ActionManager *am = ActionManager::instance();
    CommandContainer *editMenu = new CommandContainer(Constants::Menus::Edit, this);
    editMenu->setText(tr("Edit"));
    menuBar->addCommand(editMenu);

    editMenu->addCommand(StandardCommands::standardCommand(StandardCommands::Undo));
    editMenu->addCommand(StandardCommands::standardCommand(StandardCommands::Redo));
    editMenu->addSeparator();
    editMenu->addCommand(StandardCommands::standardCommand(StandardCommands::Cut));
    editMenu->addCommand(StandardCommands::standardCommand(StandardCommands::Copy));
    editMenu->addCommand(StandardCommands::standardCommand(StandardCommands::Paste));
    editMenu->addCommand(am->command(Constants::Actions::MoveHere));
    editMenu->addCommand(StandardCommands::standardCommand(StandardCommands::SelectAll));
    editMenu->addSeparator();
    editMenu->addCommand(StandardCommands::standardCommand(StandardCommands::Find));
    editMenu->addCommand(StandardCommands::standardCommand(StandardCommands::FindNext));
    editMenu->addCommand(StandardCommands::standardCommand(StandardCommands::FindPrev));
    editMenu->addSeparator();
    editMenu->addCommand(StandardCommands::standardCommand(StandardCommands::Preferences));
}

void Application::createViewMenu()
{
    ActionManager *am = ActionManager::instance();
    CommandContainer *viewMenu = new CommandContainer(Constants::Menus::View, this);
    viewMenu->setText(tr("View"));
    menuBar->addCommand(viewMenu);

    viewMenu->addCommand(am->command(Constants::Actions::ShowHiddenFiles));
    viewMenu->addCommand(StandardCommands::standardCommand(StandardCommands::ShowLeftPanel));
    viewMenu->addCommand(StandardCommands::standardCommand(StandardCommands::ShowStatusBar));
    viewMenu->addCommand(am->command(Constants::Actions::ShowBookmarks));
    viewMenu->addSeparator();
    viewMenu->addCommand(am->command(Constants::Actions::IconMode));
    viewMenu->addCommand(am->command(Constants::Actions::ColumnMode));
    viewMenu->addCommand(am->command(Constants::Actions::TreeMode));
    viewMenu->addSeparator();
    viewMenu->addCommand(am->container(Constants::Menus::SortBy));
}

void Application::createGoToMenu()
{
    ActionManager *am = ActionManager::instance();
    menuBar->addCommand(am->container(Constants::Menus::GoTo));
}

void Application::createBookmarksMenu()
{
    ActionManager *am = ActionManager::instance();
    menuBar->addCommand(am->container(Constants::Menus::Bookmarks));
}

void Application::createPanesMenu()
{
    ActionManager *am = ActionManager::instance();
    menuBar->addCommand(am->container(Constants::Menus::Panes));
}

void Application::createToolsMenu()
{
    ActionManager *am = ActionManager::instance();
    CommandContainer *toolsMenu = new CommandContainer(Constants::Menus::Tools, this);
    toolsMenu->setText(tr("Tools"));
    menuBar->addCommand(toolsMenu);

    toolsMenu->addCommand(am->container("ImageViewMenu"));

#ifdef QT_DEBUG
    Command *plugins = new Command("Plugins", this);
    plugins->setText(tr("Plugins..."));
    plugins->setAttributes(Command::AttributeUpdateEnabled);
    toolsMenu->addCommand(plugins);
    connect(plugins->commandAction(), SIGNAL(triggered()), SLOT(showPluginView()));

    Command *settings = new Command("Settings", this);
    settings->setText(tr("View all settings..."));
    settings->setAttributes(Command::AttributeUpdateEnabled);
    toolsMenu->addCommand(settings);
    connect(settings->commandAction(), SIGNAL(triggered()), SLOT(showSettings()));
#endif
    toolsMenu->addSeparator();
    toolsMenu->addCommand(am->command(Constants::Actions::ShowWebInspector));
}

void Application::createWindowsMenu()
{
    CommandContainer *windowsMenu = new WindowsContainer(Constants::Menus::Windows, this);
    windowsMenu->setText(tr("Windows"));
    menuBar->addCommand(windowsMenu);
}

void Application::createHelpMenu()
{
    CommandContainer *helpMenu = new WindowsContainer(Constants::Menus::Help, this);
    helpMenu->setText(tr("Help"));
    menuBar->addCommand(helpMenu);

    helpMenu->addCommand(StandardCommands::standardCommand(StandardCommands::About));
    helpMenu->addCommand(StandardCommands::standardCommand(StandardCommands::AboutQt));
}

#ifdef Q_OS_MAC
void qt_mac_set_dock_menu(QMenu *menu);
#endif

void Application::createDockMenu()
{
    CommandContainer *dock = new DockContainer(Constants::Menus::Dock, this);
#ifdef Q_OS_MAC
    dock->setText(tr("Dock menu"));
#else
    dock->setText(tr("Tray menu"));
#endif

    dock->addCommand(StandardCommands::standardCommand(StandardCommands::NewWindow));
#ifndef Q_OS_MAC
    dock->addSeparator();
    dock->addCommand(StandardCommands::standardCommand(StandardCommands::Quit));
#endif

    dockMenu = dock->menu();

#ifdef Q_OS_MAC
    qt_mac_set_dock_menu(dockMenu);
#endif

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/andromeda/icons/andromeda.png"));
    trayIcon->setContextMenu(dockMenu);
#ifdef Q_OS_MAC
    setTrayIconVisible(false);
#else
    setTrayIconVisible(true);
#endif
}

void Application::setupApplicationActions()
{
    struct ConnectInfo {
        StandardCommands::StandardCommand id;
        const char *slot;
    };
    static ConnectInfo infos[] = {
        { StandardCommands::NewWindow, SLOT(newWindow()) },
        { StandardCommands::Quit, SLOT(exit()) },
        { StandardCommands::Preferences, SLOT(preferences()) },
        { StandardCommands::About, SLOT(about()) },
        { StandardCommands::AboutQt, SLOT(aboutQt()) }
    };

    for (size_t i = 0; i < sizeof(infos)/sizeof(ConnectInfo); ++i) {
        Command *c = StandardCommands::standardCommand(infos[i].id);
        c->setAttributes(Command::AttributeUpdateEnabled);
        connect(c->commandAction(), SIGNAL(triggered(bool)), infos[i].slot);
    }
}

void Application::createAction(const QByteArray &id, const char *slot)
{
    Action *action = new Action(id, this);
    connect(action, SIGNAL(triggered()), slot);
}
