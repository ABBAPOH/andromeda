#include "pluginmanager.h"
#include "pluginmanager_p.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QPluginLoader>
#include <QtCore/QDir>
#include <QtCore/QFileSystemWatcher>
#include <QtCore/QWriteLocker>
#include <QtCore/QReadLocker>
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
    QObjectPool(*new PluginManagerPrivate, parent)
{
    Q_D(PluginManager);
    Q_ASSERT(!m_instance);
    m_instance = this;
    d->loaded = false;

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
    unloadPlugins();
    qDeleteAll(d->pluginSpecs);
}

/*!
    \fn void PluginManager::loadPlugins()
    \brief Loads all plugins from plugins folder.

*/
void PluginManager::loadPlugins()
{
    Q_D(PluginManager);

    if (d->loaded)
        return;

    // loadSettings()

    // findexisting folders with plugins
    foreach (QString folder, qApp->libraryPaths()) {
        if (pluginsFolder() != "") { // we try to cd into pluginsFolder
            if (!QFileInfo(folder + '/' + pluginsFolder()).exists())
                continue;
            else
                folder = folder + '/' + pluginsFolder();
        }
        d->foldersToBeLoaded.append(folder);
        d->watcher->addPath(folder);
    }

    d->load();
    qDebug("PluginManager::pluginsLoaded");
    emit pluginsLoaded();
    emit pluginsChanged();
}

/*!
    \fn void PluginManager::unloadPlugins()
    \brief Unloads all currently loaded plugins.

    This function automatically called when destroying PluginManager class;
*/
void PluginManager::unloadPlugins()
{
    Q_D(PluginManager);

    if (!d->loaded)
        return;

    foreach (PluginSpec *spec, d->pluginSpecs) {
        spec->unload();
    }
    d->loaded = false;
    qDebug("PluginManager::pluginsUnloaded");
    emit pluginsUnloaded();
}

QString PluginManager::pluginsFolder() const
{
    return d_func()->pluginsFolder;
}

void PluginManager::setPluginsFolder(const QString &name)
{
    d_func()->pluginsFolder = name;
}

bool PluginManager::loaded()
{
    return d_func()->loaded;
}

/*!
    \fn QList<PluginSpec *> PluginManager::plugins() const
    \brief Return list of PluginSpecs loaded at current moment.

*/
QList<PluginSpec *> PluginManager::plugins() const
{
    return d_func()->pluginSpecs;
}

PluginSpec *plugins(const QString &name) const
{
#warning "TODO: realize";
}


void PluginManager::updateDirectory(const QString &dirPath)
{
    Q_D(PluginManager);
    d->foldersToBeLoaded.append(dirPath);

    killTimer(d->updateTimer);
    d->updateTimer = startTimer(5000);
}

// checks if spec exists and loads/unloads it if necessary
void PluginManager::updateLibrary(const QString &specPath)
{
    d_func()->fileChanged(specPath);
}

void PluginManager::timerEvent(QTimerEvent *event)
{
    Q_D(PluginManager);

    if (event->timerId() == d->updateTimer) {
        killTimer(d->updateTimer);
        d->updateTimer = 0;

        d->load();
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

void PluginManagerPrivate::load()
{
    QStringList specFiles = getSpecFiles(foldersToBeLoaded);
    foldersToBeLoaded.clear();

    // get all specs from files
    QList<PluginSpec *> newSpecs = loadSpecs(specFiles);

    // enables new plugins
    enableSpecs(newSpecs);

    loaded = true;
}

QStringList PluginManagerPrivate::getSpecFiles(QStringList folders)
{
    QStringList result;
    foreach (QString folder, folders) {
        QDir dir(folder);
        QStringList entries = dir.entryList(QStringList() << "*.spec");
        foreach (QString entry, entries) {
            result.append(dir.absoluteFilePath(entry));
        }
    }
    return result;
}

QList<PluginSpec*> PluginManagerPrivate::loadSpecs(QStringList specFiles)
{
    QList<PluginSpec*> result;
    foreach (QString specFile, specFiles) {
        if (!pathToSpec.contains(specFile)) {
            PluginSpec *spec = new PluginSpec(specFile);
            if (!spec->hasError()) {
                pluginSpecs.append(spec);
                pathToSpec.insert(specFile, spec);
                result.append(spec);
            } else {
                delete spec;
            }
        }
    }
    return result;
}

void PluginManagerPrivate::fileChanged(const QString &specPath)
{
    qDebug() << "PluginManagerPrivate::fileChanged" << specPath;

    QFileInfo info(specPath);

    if (!info.exists()) {
        PluginSpec *spec = pathToSpec.value(specPath);
        if (!spec)
            return;
        spec->unload();
        if (!spec->loaded()) {
            pathToSpec.remove(specPath);
        }
    }
}

void PluginManagerPrivate::enableSpecs(QList<PluginSpec *> specsToBeEnabled)
{
    foreach (PluginSpec *spec, specsToBeEnabled) {
        if (spec->loadOnStartup()) {
            spec->load();
        }
    }
}



