#include "coreplugin.h"

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
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
#include <guisystem/menubarcontainer.h>
#include <guisystem/settingswindow.h>
#include <guisystem/settingspagemanager.h>

#include <widgets/windowsmenu.h>

#include "settingswidget.h"
#include "browserwindow_p.h"

using namespace Core;
using namespace GuiSystem;

static const qint32 corePluginMagic = 0x6330386e; // "c08n"
static const qint8 corePluginVersion = 1;

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

CorePlugin::CorePlugin() :
    IPlugin(),
    m_firstStart(true)
{
}

CorePlugin::~CorePlugin()
{
}

bool CorePlugin::initialize()
{
    EditorWindowFactory::setDefaultfactory(new BrowserWindowFactory(this));

    SettingsPageManager *pageManager = new SettingsPageManager;
    pageManager->setObjectName(QLatin1String("settingsPageManager"));
    addObject(pageManager);

    pageManager->addPage(new CommandsSettingsPage(this));

    createActions();
    connect(PluginManager::instance(), SIGNAL(pluginsLoaded()), SLOT(restoreSession()));

    return true;
}

void CorePlugin::postInitialize(const QVariantMap &options)
{
    QStringList urls = options.value("files").toStringList();

    if (!urls.isEmpty()) {
        BrowserWindow *window = new BrowserWindow();
        foreach (const QString &url, urls)
            window->openNewTab(urlFromUserInput(qApp->property("currentPath").toString(), url));
        window->show();
        return;
    }

    if (EditorWindow::windows().isEmpty() || !m_firstStart) {
        BrowserWindow::newWindow();
    }

    m_firstStart = false;
}

void CorePlugin::shutdown()
{
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

    s >> settingsWindowState;

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

    s << settingsWindowState;

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
    if (!settingsWindow) {
        settingsWindow = new SettingsWindow();
        settingsWindow->setAttribute(Qt::WA_DeleteOnClose);
        settingsWindow->setSettingsPageManager(pageManager);
        settingsWindow->restoreState(settingsWindowState);
        settingsWindow->installEventFilter(this);
        settingsWindow->show();
    } else {
        settingsWindow->raise();
        settingsWindow->activateWindow();
    }
}

void CorePlugin::restoreSession()
{
#ifdef Q_OS_MAC
    // Create menu bar now
    menuBar = MenuBarContainer::instance()->menuBar();
#endif

    loadSettings();

    if (!urls.isEmpty()) {
        BrowserWindow *window = new BrowserWindow();
        foreach (const QString &url, urls)
            window->openNewTab(QUrl::fromUserInput(url));
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

    qDeleteAll(BrowserWindow::windows());

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
    if (o == settingsWindow) {
        if (e->type() == QEvent::Close) {
            settingsWindowState = settingsWindow->saveState();
        }
    }
    return false;
}

void CorePlugin::createActions()
{
    MenuBarContainer *menuBar = new MenuBarContainer(this);
    menuBar->createMenus();

    createGoToMenu();
    createToolsMenu();
    createWindowsMenu();
    createDockMenu();
    registerAtions();
}

void CorePlugin::createGoToMenu()
{
    MenuBarContainer *menuBar = MenuBarContainer::instance();

    Command *c = 0;

    // ================ GoTo Menu ================
    CommandContainer *goToMenu = new CommandContainer(Constants::Menus::GoTo, this);
    goToMenu->setTitle(tr("Go to"));
    menuBar->addContainer(goToMenu);

    // ================ GoTo Menu (default) ================
    c = new Command(Constants::Actions::Back, tr("Back"), this);
    c->setDefaultShortcut(QKeySequence::Back);
    goToMenu->addCommand(c);

    c = new Command(Constants::Actions::Forward, tr("Forward"), this);
    c->setDefaultShortcut(QKeySequence::Forward);
    goToMenu->addCommand(c);

#ifdef Q_OS_MAC
    c = new Command(Constants::Actions::Up, QKeySequence(QLatin1String("Ctrl+Up")), tr("Up one level"), this);
#else
    c = new Command(Constants::Actions::Up, QKeySequence(QLatin1String("Alt+Up")), tr("Up one level"), this);
#endif
    goToMenu->addCommand(c);
}

void CorePlugin::createWindowsMenu()
{
    MenuBarContainer *menuBar = MenuBarContainer::instance();

    CommandContainer *windowsMenu = new WindowsContainer(Constants::Menus::Windows, this);
    windowsMenu->setTitle(tr("Windows"));
    menuBar->addContainer(windowsMenu);
}

void CorePlugin::createToolsMenu()
{
    MenuBarContainer *menuBar = MenuBarContainer::instance();

    // ================ Tools Menu ================
    CommandContainer *toolsMenu = menuBar->container(MenuBarContainer::ToolsMenu);

#ifdef QT_DEBUG
    Command *plugins = new Command(Constants::Actions::Plugins, this);
    plugins->setDefaultText(tr("Plugins..."));
    plugins->setContext(Command::ApplicationCommand);
    toolsMenu->addCommand(plugins);
#endif

#ifdef QT_DEBUG
    Command *settings = new Command(Constants::Actions::Settings, this);
    settings->setDefaultText(tr("View all settings..."));
    settings->setContext(Command::ApplicationCommand);
    toolsMenu->addCommand(settings);
#endif
}

#ifdef Q_OS_MAC
void qt_mac_set_dock_menu(QMenu *menu);
#endif

void CorePlugin::createDockMenu()
{
    MenuBarContainer *menuBar = MenuBarContainer::instance();

    CommandContainer *dock = new DockContainer(Constants::Menus::Dock, this);
#ifdef Q_OS_MAC
    dock->setTitle(tr("Dock menu"));
#else
    dock->setTitle(tr("Tray menu"));
#endif

    dock->addCommand(menuBar->command(MenuBarContainer::NewWindow));
#ifndef Q_OS_MAC
    dock->addCommand(new Separator(this));
    dock->addCommand(menuBar->command(MenuBarContainer::Quit));
#endif

    dockMenu = dock->menu();

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
}

void CorePlugin::createAction(const QByteArray &id, const char *slot)
{
    Action *action = new Action(id, this);
    connect(action, SIGNAL(triggered()), slot);
}

Q_EXPORT_PLUGIN(CorePlugin)
