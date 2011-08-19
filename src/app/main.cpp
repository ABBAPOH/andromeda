#include <QtSingleApplication>
#include <QtCore/QStringList>
#include <QtCore/QDir>
#include <QtWebKit/QWebSecurityOrigin>
#include <pluginmanager.h>
#include <pluginview.h>

using namespace ExtensionSystem;

void preloadLibraries()
{
    QStringList schemes = QWebSecurityOrigin::localSchemes(); // preloading WebKit
}

static inline QString getPluginPath()
{
    // Figure out root:  Up one from 'bin' or 'MacOs'
    QDir rootDir = QApplication::applicationDirPath();
    rootDir.cdUp();
    const QString rootDirPath = rootDir.canonicalPath();
    // Build path
    QString pluginPath = rootDirPath;
    pluginPath += QLatin1Char('/');
#ifdef Q_OS_UNIX
#ifndef Q_OS_MACX
    // not Mac UNIXes
    pluginPath += QLatin1String("lib");
    pluginPath += QLatin1String(LIB_SUFFIX);
    pluginPath += QLatin1String("/andromeda/");
#endif
#endif
    pluginPath += QLatin1String("plugins");
    return pluginPath;
}

int main(int argc, char *argv[])
{
    QtSingleApplication app("Andromeda", argc, argv);

    if (app.isRunning()) {
        app.sendMessage(QLatin1String("activate"));
        return 0;
    }

    app.setQuitOnLastWindowClosed(false);
    app.addLibraryPath(getPluginPath());

    preloadLibraries();

    PluginManager manager;
    manager.setPluginsFolder("andromeda");
    manager.loadPlugins();

    PluginView view;
    manager.addObject(&view, "pluginView");

    QObject::connect(&app, SIGNAL(lastWindowClosed()),
                     &manager, SLOT(unloadPlugins()), Qt::QueuedConnection);
    QObject::connect(&manager, SIGNAL(pluginsUnloaded()),
                     &app, SLOT(quit()), Qt::QueuedConnection);

    return app.exec();
}
