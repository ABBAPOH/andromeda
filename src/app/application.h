#ifndef APPLICATION_H
#define APPLICATION_H

#include <QtCore/qglobal.h>

#if QT_VERSION >= 0x050000
#include <QtWidgets/QApplication>
#else
#include <QtSingleApplication>
#endif
#include <QPointer>

#ifdef qApp
#undef qApp
#endif
#define qApp Application::instance()

class QMenu;
class QSettings;
class QSystemTrayIcon;

namespace ExtensionSystem {
class PluginManager;
class PluginView;
} // namespace ExtensionSystem

namespace Parts {
class CommandContainer;
class Menu;
class MenuBar;
class SettingsPageManager;
class SettingsWindow;
} // namespace Parts

namespace Andromeda {

#if QT_VERSION >= 0x050000
class Application : public QApplication
#else
class Application : public QtSingleApplication
#endif
{
    Q_OBJECT
    Q_DISABLE_COPY(Application)
public:
    explicit Application(int &argc, char **argv);
    ~Application();

    static Application *instance();

#if QT_VERSION < 0x050000
    bool activateApplication();
#endif
    bool loadPlugins();

    bool isTrayIconVisible() const;
    void setTrayIconVisible(bool visible);

public slots:
    void newWindow();
    void showPluginView();
    void showSettings();
    void preferences();

    void restoreSession();
    void saveSession();

    void exit();

    void about();
    void aboutQt();

protected:
    bool eventFilter(QObject *object, QEvent *event);

private slots:
    void handleMessage(const QString &message);

private:
    void handleArguments(const QStringList &arguments);

    bool restoreApplicationState(const QByteArray &state);
    QByteArray saveApplicationState() const;
    void loadSettings();
    void saveSettings();

    void createToolBar();
    void createMenus();
    void createFileMenu();
    void createEditMenu();
    void createViewMenu();
    void createGoToMenu();
    void createBookmarksMenu();
    void createToolsMenu();
    void createWindowsMenu();
    void createHelpMenu();
    void createDockMenu();
    void setupApplicationActions();

    void createAction(const QByteArray &id, const char *slot);

private:
    ExtensionSystem::PluginManager *m_pluginManager;
    Parts::SettingsPageManager *m_settingsPageManager;

    QPointer<Parts::SettingsWindow> settingsWindow;
    QByteArray settingsWindowState;
    Parts::Menu *dockMenu;
    QSystemTrayIcon *trayIcon;
    QSettings *m_settings;
    bool m_firstStart;
#ifdef Q_OS_MAC
    Parts::MenuBar *macMenuBar;
#endif

    Parts::CommandContainer *menuBar;
    Parts::CommandContainer *toolBar;
};

} // namespace Andromeda

#endif // APPLICATION_H
