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

void PluginManager::addObject(QObject * object, const QString &type)
{
    QWriteLocker l(&m_lock);

    Q_D(PluginManager);
    if (!object) {
        return;
    }

    if (!d->objects.contains(object)) {
        d->objects.append(object);
        if (object->objectName() != "") {
            d->namedObjects.insertMulti(object->objectName(), object);
        }
        l.unlock();
        emit objectAdded(object, type);
    }
}

void PluginManager::removeObject(QObject * object)
{
    QWriteLocker l(&m_lock);

    Q_D(PluginManager);

    if (!object) {
        return;
    }

    d->objects.removeAll(object);
    l.unlock();
    emit objectRemoved(object);
}

QObjectList PluginManager::objects()
{
    QReadLocker l(&m_lock);
    return d_func()->objects;
}

QObject * PluginManager::object(const QString &name)
{
    QReadLocker l(&m_lock);
    return d_func()->namedObjects.value(name);
}

QObjectList PluginManager::objects(const QString &name)
{
    QReadLocker l(&m_lock);
    return d_func()->namedObjects.values(name);
}

void PluginManager::loadPlugins()
{
    Q_D(PluginManager);

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
    d->foldersToBeLoaded.append(dirPath);

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

    // get all specs from files
    QList<PluginSpec *> newSpecs = loadSpecs(specFiles);

    // resolves dependencies of new specs
    resolveDependencies(newSpecs);

    // enables new plugins
    enableSpecs(newSpecs);
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

void PluginManagerPrivate::fileChanged(const QString &libraryPath)
{
    qDebug() << "PluginManagerPrivate::fileChanged" << libraryPath;

    QFileInfo info(libraryPath);

    if (!info.exists()) {
        PluginSpec *spec = pathToSpec.value(libraryPath);
        if (!spec)
            return;
        spec->setLoaded(false);
        if (!spec->loaded()) {
            pathToSpec.remove(libraryPath);
        }
    }
}

void PluginManagerPrivate::resolveDependencies(QList<PluginSpec *> specsToBeEnabled)
{
    foreach (PluginSpec *spec, specsToBeEnabled) {
        spec->d_ptr->resolveDependencies();
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



