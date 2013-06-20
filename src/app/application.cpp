#include "application.h"

#include <QtCore/QStringList>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QLocale>
#include <QtCore/QSettings>
#include <QtCore/QTranslator>

#include <QtGui/QIcon>

#if QT_VERSION >= 0x050000
#include <QtCore/QStandardPaths>
#include <QtWidgets/QCompleter>
#include <QtWidgets/QDirModel>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QSystemTrayIcon>
#include <QtWidgets/QWidgetAction>
#else
#include <IO/QStandardPaths>
#include <QtGui/QCompleter>
#include <QtGui/QDirModel>
#include <QtGui/QMenuBar>
#include <QtGui/QMessageBox>
#include <QtGui/QSystemTrayIcon>
#include <QtGui/QWidgetAction>
#endif

#include <ExtensionSystem/ErrorsDialog>
#include <ExtensionSystem/PluginManager>
#include <ExtensionSystem/PluginView>

#include <Parts/ActionManager>
#include <Parts/ApplicationCommand>
#include <Parts/ContextCommand>
#include <Parts/CommandContainer>
#include <Parts/EditorWindowFactory>
#include <Parts/SettingsPageManager>
#include <Parts/SettingsWindow>
#include <Parts/StandardCommands>
#include <Parts/Menu>
#include <Parts/MenuBar>
#include <Parts/constants.h>

#include <bookmarkspart/bookmarksconstants.h>
#include <filemanager/filemanagerconstants.h>
#include <webviewpart/webviewconstants.h>

#include <Widgets/WindowsMenu>

#include "browserwindow.h"
#include "browserwindow_p.h"
#include "commandssettingspage.h"
#include "generalsettingspage.h"
#include "settingsmodel.h"
#include "settingswidget.h"

using namespace ExtensionSystem;
using namespace Parts;
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

static inline QString getDataLocationPath()
{
    return QStandardPaths::writableLocation(QStandardPaths::DataLocation);
}

template <class T>
static T *findParent(QObject *object)
{
    while (object) {
        T *result = qobject_cast<T*>(object);
        if (result)
            return result;
        object = object->parent();
    }
    return 0;
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

    QAction *createAction(QObject *parent) const;

private:
    QPointer<WindowsMenu> m_menu;
};

class WindowsContainer : public CommandContainer
{
public:
    explicit WindowsContainer(const QByteArray &id, QObject *parent = 0);
    ~WindowsContainer();

    QAction *createAction(QObject *parent) const;

private:
    QPointer<WindowsMenu> m_menu;
};

class AddressBarCommand : public AbstractCommand
{
public:
    explicit AddressBarCommand(const QByteArray &id, QObject *parent = 0);
//    ~AddressBarCommand();

    QAction *createAction(QObject *parent) const;
};

class HistoryButtonCommand : public AbstractCommand
{
public:
    explicit HistoryButtonCommand(HistoryButton::Direction direction, QObject *parent = 0);
//    ~AddressBarCommand();

    QAction *createAction(QObject *parent) const;

private:
    HistoryButton::Direction m_direction;
};

HistoryButtonCommand::HistoryButtonCommand(HistoryButton::Direction direction, QObject *parent) :
    AbstractCommand(direction == HistoryButton::Back ? "ButtonBack" : "ButtonForward", parent),
    m_direction(direction)
{

}

QAction *HistoryButtonCommand::createAction(QObject *parent) const
{
    BrowserWindow *window = ::findParent<BrowserWindow>(parent);
    if (!window)
        return 0;

    HistoryButton *button = new HistoryButton;
    button->setHistory(window->history());
    button->setDirection(m_direction);
    QIcon icon;
    if (m_direction == HistoryButton::Back)
        icon = QIcon::fromTheme("go-previous", QIcon(":/guisystem/icons/go-previous.png"));
    else
        icon = QIcon::fromTheme("go-next", QIcon(":/guisystem/icons/go-next.png"));
    button->setIcon(icon);

    QWidgetAction *action = new QWidgetAction(parent);
    action->setDefaultWidget(button);
    return action;
}

AddressBarCommand::AddressBarCommand(const QByteArray &id, QObject *parent) :
    AbstractCommand(id, parent)
{
}

QAction * AddressBarCommand::createAction(QObject *parent) const
{
    BrowserWindow *window = ::findParent<BrowserWindow>(parent);
    if (!window)
        return 0;

    AddressBar *lineEdit = new MyAddressBar();
    lineEdit->setContextMenuPolicy(Qt::ActionsContextMenu);
//    lineEdit->setStyleSheet("QLineEdit { border-radius: 2px; }");
    connect(lineEdit, SIGNAL(open(QUrl)), window, SLOT(open(QUrl)));
    connect(lineEdit, SIGNAL(refresh()), window, SLOT(reload()));
    connect(lineEdit, SIGNAL(canceled()), window, SLOT(stop()));
    connect(window, SIGNAL(urlChanged(QUrl)), lineEdit, SLOT(setUrl(QUrl)));

    // ### fixme QDirModel is used in QCompleter because QFileSystemModel seems broken
    // This is an example how to use completers to help directory listing.
    // I'm not sure if it shouldn't be a part of plugins (standalone for web...)
    // TODO/FIXME: QFileSystemModel is probably broken for qcompleter so the obsolete
    //             QDirModel is used here.
    //    QFileSystemModel * dirModel = new QFileSystemModel(this);
    QDirModel *dirModel = new QDirModel(lineEdit);
    //    dirModel->setRootPath(QDir::rootPath());
    QCompleter *completer = new QCompleter(dirModel, lineEdit);
    completer->setCompletionMode(QCompleter::InlineCompletion);
    lineEdit->setCompleter(completer);

    QWidgetAction *action = new QWidgetAction(parent);
    action->setDefaultWidget(lineEdit);
    return action;
}

DockContainer::DockContainer(const QByteArray &id, QObject *parent) :
    CommandContainer(id, parent),
    m_menu(new WindowsMenu)
{
}

DockContainer::~DockContainer()
{
    delete m_menu;
}

QAction * DockContainer::createAction(QObject * /*parent*/) const
{
    return m_menu->menuAction();
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

QAction * WindowsContainer::createAction(QObject * /*parent*/) const
{
    return m_menu->menuAction();
}

Application::Application(int &argc, char **argv) :
#if QT_VERSION >= 0x050000
    QApplication(argc, argv),
#else
    QtSingleApplication("Andromeda", argc, argv),
#endif
    m_pluginManager(0),
    dockMenu(0),
    trayIcon(0),
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
                                     "parts" <<
                                     "imageviewer" <<
                                     "widgets");

    EditorWindowFactory::setDefaultfactory(new BrowserWindowFactory(this));

    m_settingsPageManager = new SettingsPageManager(this);
    m_settingsPageManager->setObjectName("settingsPageManager");
    m_settingsPageManager->addPage(new GeneralSettingsPage(this));
    m_settingsPageManager->addPage(new CommandsSettingsPage(this));
    m_pluginManager->addObject(m_settingsPageManager);

#if QT_VERSION < 0x050000
    connect(this, SIGNAL(messageReceived(QString)), SLOT(handleMessage(QString)));
#endif
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

#if QT_VERSION < 0x050000
bool Application::activateApplication()
{
    QStringList arguments = this->arguments();
    Q_ASSERT(!arguments.isEmpty());
    arguments[0] = QDir::currentPath();
    return sendMessage(arguments.join("\n"));
}
#endif

bool Application::loadPlugins()
{
    m_pluginManager->loadPlugins();

    createMenus();
    createToolBar();
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

    if (m_pluginManager->plugins().isEmpty()) {
        QMessageBox box;
        box.setWindowTitle(tr("Warning"));
        box.setText(tr("No plugins were loaded. Do you wish to continue?"));
        box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        if (box.exec() == QMessageBox::Yes)
            return true;
        return false;
    }

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
    macMenuBar = new MenuBar;
    macMenuBar->setContainer("MenuBar");
#endif

    loadSettings();

    bool ok = true;
    QString dataPath = ::getDataLocationPath();
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
    QString dataPath = ::getDataLocationPath();
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

void Application::createToolBar()
{
    toolBar = new CommandContainer("ToolBar", this);
    toolBar->addCommand(new HistoryButtonCommand(HistoryButton::Back));
    toolBar->addCommand(new HistoryButtonCommand(HistoryButton::Forward));
//    toolBar->addCommand(StandardCommands::contextCommand(StandardCommands::Back));
//    toolBar->addCommand(StandardCommands::contextCommand(StandardCommands::Forward));
    toolBar->addCommand(StandardCommands::contextCommand(StandardCommands::Up));
    toolBar->addSeparator();
    toolBar->addCommand(new AddressBarCommand("AddressBar", toolBar));
}

void Application::createMenus()
{
    menuBar = new CommandContainer("MenuBar", this);
    createFileMenu();
    createEditMenu();
    createViewMenu();
    createGoToMenu();
    createBookmarksMenu();
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

    fileMenu->addCommand(StandardCommands::applicationCommand(StandardCommands::NewWindow));
    fileMenu->addCommand(StandardCommands::contextCommand(StandardCommands::NewTab));
    fileMenu->addCommand(StandardCommands::contextCommand(StandardCommands::Open));
    fileMenu->addCommand(am->command(Constants::Actions::OpenEditor));
    fileMenu->addCommand(StandardCommands::contextCommand(StandardCommands::Close));
    fileMenu->addSeparator();
    fileMenu->addCommand(StandardCommands::contextCommand(StandardCommands::Save));
    fileMenu->addCommand(StandardCommands::contextCommand(StandardCommands::SaveAs));
    fileMenu->addSeparator();
    fileMenu->addCommand(StandardCommands::contextCommand(StandardCommands::Refresh));
    fileMenu->addCommand(StandardCommands::contextCommand(StandardCommands::Cancel));
    fileMenu->addSeparator();
    fileMenu->addCommand(am->command(Constants::Actions::ShowFileInfo));
    fileMenu->addSeparator();
    fileMenu->addCommand(am->command(Constants::Actions::NewFolder));
    fileMenu->addCommand(am->command(Constants::Actions::Rename));
    fileMenu->addCommand(am->command(Constants::Actions::MoveToTrash));
    fileMenu->addCommand(am->command(Constants::Actions::Remove));
    fileMenu->addSeparator();
    fileMenu->addCommand(StandardCommands::applicationCommand(StandardCommands::Quit));
}

void Application::createEditMenu()
{
    ActionManager *am = ActionManager::instance();
    CommandContainer *editMenu = new CommandContainer(Constants::Menus::Edit, this);
    editMenu->setText(tr("Edit"));
    menuBar->addCommand(editMenu);

    editMenu->addCommand(StandardCommands::contextCommand(StandardCommands::Undo));
    editMenu->addCommand(StandardCommands::contextCommand(StandardCommands::Redo));
    editMenu->addSeparator();
    editMenu->addCommand(StandardCommands::contextCommand(StandardCommands::Cut));
    editMenu->addCommand(StandardCommands::contextCommand(StandardCommands::Copy));
    editMenu->addCommand(StandardCommands::contextCommand(StandardCommands::Paste));
    editMenu->addCommand(am->command(Constants::Actions::MoveHere));
    editMenu->addCommand(StandardCommands::contextCommand(StandardCommands::SelectAll));
    editMenu->addSeparator();
    editMenu->addCommand(StandardCommands::contextCommand(StandardCommands::Find));
    editMenu->addCommand(StandardCommands::contextCommand(StandardCommands::FindNext));
    editMenu->addCommand(StandardCommands::contextCommand(StandardCommands::FindPrev));
    editMenu->addSeparator();
    editMenu->addCommand(StandardCommands::applicationCommand(StandardCommands::Preferences));
}

void Application::createViewMenu()
{
    ActionManager *am = ActionManager::instance();
    CommandContainer *viewMenu = new CommandContainer(Constants::Menus::View, this);
    viewMenu->setText(tr("View"));
    menuBar->addCommand(viewMenu);

    viewMenu->addCommand(am->command(Constants::Actions::ShowHiddenFiles));
    viewMenu->addCommand(StandardCommands::contextCommand(StandardCommands::ShowLeftPanel));
    viewMenu->addCommand(StandardCommands::contextCommand(StandardCommands::ShowStatusBar));
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

void Application::createToolsMenu()
{
    ActionManager *am = ActionManager::instance();
    CommandContainer *toolsMenu = new CommandContainer(Constants::Menus::Tools, this);
    toolsMenu->setText(tr("Tools"));
    menuBar->addCommand(toolsMenu);

    toolsMenu->addCommand(am->container("ImageViewMenu"));

#ifdef QT_DEBUG
    Command *plugins = new ApplicationCommand("Plugins", this);
    plugins->setText(tr("Plugins..."));
    toolsMenu->addCommand(plugins);
    connect(plugins, SIGNAL(triggered()), SLOT(showPluginView()));

    Command *settings = new ApplicationCommand("Settings", this);
    settings->setText(tr("View all settings..."));
    toolsMenu->addCommand(settings);
    connect(settings, SIGNAL(triggered()), SLOT(showSettings()));
#endif
    toolsMenu->addSeparator();
    toolsMenu->addCommand(am->command(Constants::Actions::ShowWebInspector));
}

void Application::createWindowsMenu()
{
//    CommandContainer *windowsMenu = new WindowsContainer(Constants::Menus::Windows, this);
//    windowsMenu->setText(tr("Windows"));
//    menuBar->addCommand(windowsMenu);
}

void Application::createHelpMenu()
{
    CommandContainer *helpMenu = new WindowsContainer(Constants::Menus::Help, this);
    helpMenu->setText(tr("Help"));
    menuBar->addCommand(helpMenu);

    helpMenu->addCommand(StandardCommands::applicationCommand(StandardCommands::About));
    helpMenu->addCommand(StandardCommands::applicationCommand(StandardCommands::AboutQt));
}

#if defined(Q_OS_MAC) && QT_VERSION < 0x050000
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

    dock->addCommand(StandardCommands::applicationCommand(StandardCommands::NewWindow));
#ifndef Q_OS_MAC
    dock->addSeparator();
    dock->addCommand(StandardCommands::applicationCommand(StandardCommands::Quit));
#endif

    dockMenu = new Menu;
    dockMenu->setContainer(dock);

#if defined(Q_OS_MAC) && QT_VERSION < 0x050000
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
        ApplicationCommand *c = StandardCommands::applicationCommand(infos[i].id);
        connect(c, SIGNAL(triggered(bool)), infos[i].slot);
    }
}

void Application::createAction(const QByteArray &id, const char *slot)
{
    QAction *action = new QAction(id, this);
    action->setObjectName(id);
    connect(action, SIGNAL(triggered()), slot);
}
