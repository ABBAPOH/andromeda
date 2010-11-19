#include <QtSingleApplication>
#include <pluginmanager.h>
#include <QDebug>

using namespace ExtensionSystem;

int main(int argc, char *argv[])
{
    QtSingleApplication app("Andromeda", argc, argv);

    if (app.isRunning()) {
        app.sendMessage("activate");
        return 0;
    }

    PluginManager manager;
    manager.setPluginsFolder("plugins");
    manager.loadPlugins();

    return app.exec();
}
