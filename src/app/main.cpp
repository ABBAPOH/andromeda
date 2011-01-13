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

    app.setQuitOnLastWindowClosed(false);

    PluginManager manager;
    manager.setPluginsFolder("plugins");
    manager.loadPlugins();

    QObject::connect(&app, SIGNAL(lastWindowClosed()),
                     &manager, SLOT(unloadPlugins()), Qt::QueuedConnection);
    QObject::connect(&manager, SIGNAL(pluginsUnloaded()),
                     &app, SLOT(quit()), Qt::QueuedConnection);

    return app.exec();
}

//long __stdcall WinMain(...)
//{
////AllocConsole();
//main(__argc, __argv);
////FreeConsole();
//}
