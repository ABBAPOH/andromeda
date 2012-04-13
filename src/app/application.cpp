#include "application.h"

#include <QtCore/QStringList>
#include <QtCore/QDir>
#include <QtCore/QLocale>
#include <QtCore/QTranslator>

#include <QtGui/QIcon>

#include <extensionsystem/errorsdialog.h>

using namespace ExtensionSystem;

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

Application::Application(int &argc, char **argv) :
    QtSingleApplication(QLatin1String("Andromeda"), argc, argv),
    m_pluginManager(0),
    m_pluginView(0)
{
    setOrganizationName(QLatin1String("arch"));
    setApplicationName(QLatin1String("andromeda"));

#ifdef ICON_LOCATION
    setWindowIcon(QIcon(ICON_LOCATION + QString("/andromeda.png")));
#endif

    setQuitOnLastWindowClosed(false);
    addLibraryPath(getPluginPath());

    m_pluginManager = new PluginManager(this);
    m_pluginManager->setPluginsFolder(QLatin1String("andromeda"));
    m_pluginManager->setDefaultPlugins(QStringList() << QLatin1String("Core Plugin"));
    m_pluginManager->setTranslations(QStringList() <<
                                     QLatin1String("extensionsystem") <<
                                     QLatin1String("guisystem") <<
                                     QLatin1String("widgets"));

    connect(this, SIGNAL(messageReceived(QString)), SLOT(handleMessage(QString)));
}

Application::~Application()
{
    delete m_pluginView;
}

QString Application::currentPath() const
{
    return m_currentPath;
}

bool Application::activateApplication()
{
    QStringList arguments = this->arguments();
    arguments.prepend(QDir::currentPath());
    return sendMessage(arguments.join("\n"));
}

bool Application::loadPlugins()
{
    m_pluginManager->loadPlugins();
    m_pluginManager->postInitialize(arguments());

    if (m_pluginManager->hasErrors()) {
        ErrorsDialog dlg;
        dlg.setMessage(QObject::tr("Errors occured during loading plugins."));
        dlg.setErrors(m_pluginManager->errors());
        dlg.exec();
    }

    if (m_pluginManager->plugins().isEmpty())
        return false;

    m_pluginView = new PluginView;
    m_pluginManager->addObject(m_pluginView, QLatin1String("pluginView"));

    return true;
}

void Application::handleMessage(const QString &message)
{
    QStringList arguments = message.split("\n");
    if (arguments.isEmpty())
        return;

    m_currentPath = arguments.first();
    m_pluginManager->postInitialize(arguments.mid(1));
}

