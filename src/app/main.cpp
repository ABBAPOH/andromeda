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
    pluginPath += QLatin1Char('/');
#if defined Q_OS_MACX
#elif defined Q_OS_WIN
#elif defined Q_OS_UNIX
    // not Mac UNIXes
    pluginPath += QLatin1String("lib");
    pluginPath += QLatin1String(LIB_SUFFIX);
    pluginPath += QLatin1String("/andromeda/");
#endif
    pluginPath += QLatin1String("plugins");
    return pluginPath;
}

static inline QString getTranslationsPath()
{
    const QString rootDirPath = getRootPath();
    // Build path
    QString result = rootDirPath;
#if defined Q_OS_MACX
    result += QLatin1Char('/');
    result += QLatin1String("Resources");
    result += QLatin1Char('/');
    result += QLatin1String("translations");
#elif defined Q_OS_WIN
    result += QLatin1Char('/');
    result += QLatin1String("translations");
#elif defined Q_OS_UNIX
    // not Mac UNIXes
    result += QLatin1Char('/');
    result += QLatin1String("share");
    result += QLatin1Char('/');
    result += qApp->applicationName();
    result += QLatin1Char('/');
    result += QLatin1String("translations");
#endif
    return result;
}

static void loadQtTranslation()
{
    QString path = getTranslationsPath();
    QString locale = QLocale::system().name();
    QTranslator *qtTranslator = new QTranslator(qApp);

    qtTranslator->load(QString("qt_") + locale, path);
    qApp->installTranslator(qtTranslator);
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
    loadQtTranslation();

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
