#include <QtGui/QApplication>
#include <QDebug>

#include <ExtensionSystem/PluginManager>
#include <ExtensionSystem/PluginView>

using namespace ExtensionSystem;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    PluginManager manager;
    manager.setPluginsFolder("plugins");
    manager.loadPlugins();

    qDebug() << "List of plugins: ";
    foreach (PluginSpec * spec, manager.plugins()) {
        qDebug() << "   " << spec->name();
//        qDebug() << "   " << spec->version();
//        qDebug() << "   " << spec->compatibilityVersion();
        qDebug() << "   " << spec->version().toString();
        qDebug() << "   " << spec->compatibilityVersion().toString();
        qDebug() << "   " << spec->vendor();
        qDebug() << "   " << spec->hasError();
        qDebug() << "   " << spec->errorString();
        qDebug() << "";
    }
    qDebug() << "end list";

    PluginView view;
    view.show();

    return app.exec();
}
