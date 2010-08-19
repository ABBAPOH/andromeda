#include <QtSingleApplication>
#include <pluginmanager.h>

using namespace ExtensionSystem;

int main(int argc, char *argv[])
{
    QtSingleApplication app("Andromeda", argc, argv);

    if (app.isRunning()) {
        app.sendMessage("activate");
        return 0;
    }

    PluginManager manager;
    manager.setPluginsFolder("andromeda");
    manager.loadPlugins();

    return app.exec();
}
