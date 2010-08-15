#include <QtGui/QApplication>
#include <pluginmanager.h>

using namespace ExtensionSystem;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    PluginManager manager;
    manager.setPluginsFolder("andromeda");
    manager.loadPlugins();

    return app.exec();
}
