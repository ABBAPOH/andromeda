#include <QtGui/QApplication>
#include <QDebug>

#include <pluginmanager.h>
#include <pluginview.h>

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
        qDebug() << spec->hasError();
        qDebug() << spec->errorString();
    }

    qDebug() << "Creating view";
    PluginView view;
    view.show();
    qDebug() << "Starting Main Loop";

    return app.exec();
}
