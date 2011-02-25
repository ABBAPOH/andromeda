#include <QtSingleApplication>
#include <pluginmanager.h>
#include <QtCore/QStringList>
#include <QtWebKit/QWebSecurityOrigin>

using namespace ExtensionSystem;

void preloadLibraries()
{
    QStringList schemes = QWebSecurityOrigin::localSchemes(); // preloading WebKit
}

int main(int argc, char *argv[])
{
    QtSingleApplication app("Andromeda", argc, argv);

    if (app.isRunning()) {
        app.sendMessage("activate");
        return 0;
    }

    app.setQuitOnLastWindowClosed(false);

    preloadLibraries();

    PluginManager manager;
    manager.setPluginsFolder("plugins");
    manager.loadPlugins();

    QObject::connect(&app, SIGNAL(lastWindowClosed()),
                     &manager, SLOT(unloadPlugins()), Qt::QueuedConnection);
    QObject::connect(&manager, SIGNAL(pluginsUnloaded()),
                     &app, SLOT(quit()), Qt::QueuedConnection);

    return app.exec();
}
