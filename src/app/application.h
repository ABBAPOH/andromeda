#ifndef APPLICATION_H
#define APPLICATION_H

#include <QtSingleApplication>

#include <extensionsystem/pluginmanager.h>
#include <extensionsystem/pluginview.h>

class Application : public QtSingleApplication
{
    Q_OBJECT

    Q_PROPERTY(QString currentPath READ currentPath)

public:
    explicit Application(int &argc, char **argv);
    ~Application();

    QString currentPath() const;

    bool activateApplication();
    bool loadPlugins();

private slots:
    void handleMessage(const QString &message);

private:
    QString m_currentPath;
    ExtensionSystem::PluginManager *m_pluginManager;
    ExtensionSystem::PluginView *m_pluginView;
};

#endif // APPLICATION_H
