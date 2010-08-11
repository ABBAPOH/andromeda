#include "pluginmanager.h"
#include "pluginmanager_p.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QPluginLoader>
#include <QtCore/QDir>
#include <QtCore/QFileSystemWatcher>
#include <QDebug>

#include "iplugin.h"
#include "pluginspec_p.h"

using namespace ExtensionSystem;

// ============= PluginManager =============

PluginManager *PluginManager::m_instance = 0;

PluginManager *PluginManager::instance()
{
    return m_instance;
}

/*!
    \fn PluginManager::PluginManager(QObject *parent)
    \brief Creates PluginManager with given \a parent.
*/
PluginManager::PluginManager(QObject *parent) :
    QObject(parent),
    d_ptr(new PluginManagerPrivate)
{
    Q_D(PluginManager);
    Q_ASSERT(!m_instance);
    m_instance = this;

    d->watcher = new QFileSystemWatcher(this);
    connect(d->watcher, SIGNAL(directoryChanged(QString)), SLOT(updateDirectory(QString)));
    connect(d->watcher, SIGNAL(fileChanged(QString)), SLOT(updateLibrary(QString)));
    startTimer(5000);
}

/*!
    \fn PluginManager::~PluginManager()
    \brief Destroys PluginManager.
*/
PluginManager::~PluginManager()
{
    Q_D(PluginManager);
    qDeleteAll(d->pluginSpecs);
    delete d_ptr;
}

void PluginManager::loadPlugins()
{
    Q_D(PluginManager);

    // loadSettings()

    // get all specs from files and libraries
    d->loadSpecs();

    // enables all plugins
    d->enableSpecs();
//    foreach (PluginSpec *spec, plugins()) {
//        if (spec->loadOnStartup()) {
//            qDebug() << "enabling" << spec->name();
//            spec->setEnabled(true);
//        }
//    }
    emit pluginsChanged();
}

QString PluginManager::pluginsFolder()
{
    return d_func()->pluginsFolder;
}

void PluginManager::setPluginsFolder(const QString &name)
{
    d_func()->pluginsFolder = name;
}

QList<PluginSpec *> PluginManager::plugins() const
{
    return d_func()->pluginSpecs;
}

void PluginManager::updateDirectory(const QString &dirPath)
{
    Q_D(PluginManager);
    d->pathsToBeUpdated.append(dirPath);
    killTimer(d->updateTimer);
    d->updateTimer = startTimer(5000);
}

// checks if library exists and loads/unloads it if necessary
void PluginManager::updateLibrary(const QString &libraryPath)
{
    d_func()->fileChanged(libraryPath);
}

void PluginManager::timerEvent(QTimerEvent *event)
{
    Q_D(PluginManager);

    if (event->timerId() == d->updateTimer) {
        killTimer(d->updateTimer);
        d->updateTimer = 0;
        foreach (QString dirPath, d->pathsToBeUpdated) {
            d->parseDirectory(dirPath);
        }
        d->pathsToBeUpdated.clear();

        d_func()->enableSpecs();
        emit pluginsChanged();
    }
}

// ============= PluginManagerPrivate =============
PluginManagerPrivate::PluginManagerPrivate()
{
}

static bool lessThanByPluginName(const PluginSpec *first, const PluginSpec *second)
{
    return first->name() < second->name();
}

void PluginManagerPrivate::loadSpecs()
{
    // First load specs from spec cache

    // Second retrieve specs from plugins
    foreach (QObject *object, QPluginLoader::staticInstances()) {
        specFromPlugin(object);
    }

    foreach (QString folder, qApp->libraryPaths()) {
        if (pluginsFolder != "") { // we try to cd into pluginsFolder
            if (!QFileInfo(folder + '/' + pluginsFolder).exists())
                continue;
            else
                folder = folder + '/' + pluginsFolder;
        }
        parseDirectory(folder);
    }

    qSort(pluginSpecs.begin(), pluginSpecs.end(), lessThanByPluginName);
}

void PluginManagerPrivate::specFromPlugin(QObject * object)
{
    IPlugin *plugin = qobject_cast<IPlugin *>(object);
    if (!plugin) {
        qWarning () << "not a plugin";
        return;
    }

    PluginSpec * spec = new PluginSpec(plugin); // static plugin
    pluginSpecs.append(spec);
}

void PluginManagerPrivate::fileChanged(const QString &libraryPath)
{
    qDebug() << "PluginManagerPrivate::fileChanged" << libraryPath;

    QFileInfo info(libraryPath);

    if (!info.exists()) {
        PluginSpec *spec = pathToSpec.value(libraryPath);
        if (!spec)
            return;
        spec->setEnabled(false);
        if (!spec->enabled()) {
            pathToSpec.remove(libraryPath);
        }
    }
}

void PluginManagerPrivate::parseDirectory(const QString &dir)
{
    QDir pluginsDir(dir);
    watcher->addPath(pluginsDir.absolutePath());
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        QString libraryPath = pluginsDir.absoluteFilePath(fileName);
        if (pathToSpec.contains(libraryPath))
            continue;
        PluginSpec * spec = new PluginSpec(libraryPath);
        if (!spec->hasError()) {
            watcher->addPath(libraryPath);
            pathToSpec.insert(libraryPath, spec);
            pluginSpecs.append(spec);
            specsToBeEnabled.append(spec);
        } else
            delete spec;
    }
}

void PluginManagerPrivate::enableSpecs()
{
    foreach (PluginSpec *spec, specsToBeEnabled) {
        if (spec->loadOnStartup()) {
            qDebug() << "enabling" << spec->name();
            spec->setEnabled(true);
        }
    }
    specsToBeEnabled.clear();
}



