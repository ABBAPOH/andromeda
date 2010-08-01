#include <QtGui/QApplication>
#include <QDebug>

#include <pluginmanager.h>

using namespace ExtensionSystem;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    PluginManager manager;
    manager.setPluginsFolder("plugins");
    manager.loadPlugins();

    foreach (PluginSpec * spec, manager.plugins()) {
        qDebug() << spec->name();
        qDebug() << spec->version();
        qDebug() << spec->compatibilityVersion();
        qDebug() << spec->vendor();
    }
    qDebug() << "Starting Main Loop";

    return app.exec();
}
