#ifndef APPLICATION_H
#define APPLICATION_H

#include <QtSingleApplication>
#include <QPointer>

class QMenu;
class QMenuBar;
class QSettings;
class QSystemTrayIcon;

namespace ExtensionSystem {
class PluginManager;
class PluginView;
} // namespace ExtensionSystem

namespace GuiSystem {
class SettingsPageManager;
class SettingsWindow;
} // namespace GuiSystem

namespace Andromeda {

class Application : public QtSingleApplication
{
    Q_OBJECT
    Q_DISABLE_COPY(Application)
public:
    explicit Application(int &argc, char **argv);
    ~Application();

    bool activateApplication();
    bool loadPlugins();

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

    void createActions();
    void createGoToMenu();
    void createWindowsMenu();
    void createToolsMenu();
    void createDockMenu();
    void registerAtions();

    void createAction(const QByteArray &id, const char *slot);

private:
    ExtensionSystem::PluginManager *m_pluginManager;
    GuiSystem::SettingsPageManager *m_settingsPageManager;

    QPointer<GuiSystem::SettingsWindow> settingsWindow;
    QByteArray settingsWindowState;
    QMenu *dockMenu;
    QSystemTrayIcon *trayIcon;
    QSettings *m_settings;
    bool m_firstStart;
#ifdef Q_OS_MAC
    QMenuBar *menuBar;
#endif
};

} // namespace Andromeda

#endif // APPLICATION_H
