#include <QtSingleApplication>
#include <QtGui/QIcon>
#include <QtCore/QStringList>
#include <QtCore/QDir>
#include <QtCore/QLocale>
#include <QtCore/QTranslator>
#include <QtWebKit/QWebSecurityOrigin>

#include <pluginmanager.h>
#include <pluginview.h>
#include <errorsdialog.h>

using namespace ExtensionSystem;

void preloadLibraries()
{
    QStringList schemes = QWebSecurityOrigin::localSchemes(); // preloading WebKit
}

// returns root path of the application
static QString getRootPath()
{
    // Figure out root:  Up one from 'bin' or 'MacOs'
    QDir rootDir = QApplication::applicationDirPath();
    rootDir.cdUp();
    return rootDir.canonicalPath();
}

static inline QString getPluginPath()
{
    const QString rootDirPath = getRootPath();
    // Build path
    QString pluginPath = rootDirPath;
#if defined Q_OS_MACX
    pluginPath += QLatin1Char('/');
    pluginPath += QLatin1String("PlugIns");
#elif defined Q_OS_WIN
    pluginPath += QLatin1Char('/');
    pluginPath += QLatin1String("plugins");
#elif defined Q_OS_UNIX
    // not Mac UNIXes
    pluginPath += QLatin1Char('/');
    pluginPath += QLatin1String("lib");
    pluginPath += QLatin1String(LIB_SUFFIX);
    pluginPath += QLatin1Char('/');
    pluginPath += QLatin1String("andromeda");
    pluginPath += QLatin1Char('/');
    pluginPath += QLatin1String("plugins");
#endif
    return pluginPath;
}

int main(int argc, char *argv[])
{
    QtSingleApplication app(QLatin1String("Andromeda"), argc, argv);
    app.setOrganizationName(QLatin1String("arch"));
    app.setApplicationName(QLatin1String("andromeda"));

    if (app.isRunning()) {
        app.sendMessage(QLatin1String("activate"));
        return 0;
    }

#ifdef ICON_LOCATION
    app.setWindowIcon(QIcon(ICON_LOCATION + QString("/andromeda.png")));
#endif

    app.setQuitOnLastWindowClosed(false);
    app.addLibraryPath(getPluginPath());

    preloadLibraries();

    PluginManager manager;
    manager.setPluginsFolder(QLatin1String("andromeda"));
    manager.setDefaultPlugins(QStringList() << QLatin1String("Core Plugin"));
    manager.setArguments(app.arguments());
    manager.loadPlugins();

    if (manager.hasErrors()) {
        ErrorsDialog dlg;
        dlg.setMessage(QObject::tr("Errors occured during loading plugins."));
        dlg.setErrors(manager.errors());
        dlg.exec();
    }

    if (manager.plugins().isEmpty())
        return 1;

    PluginView view;
    manager.addObject(&view, QLatin1String("pluginView"));

    return app.exec();
}
