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

/*!
    \fn void PluginManager::addObject(QObject * object, const QString &type)
    \brief Adds \a object to object pool.

    There are 3 kinds of objects - named objects, typed objects and other objects.
    If object name if not an empty string, object counts as named object.
    If \a type if not an empty string, object counts as typed object.

    In fact, there can be many objects with same name.
*/
void PluginManager::addObject(QObject * object, const QString &type)
{
    Q_D(PluginManager);
    if (!object) {
        return;
    }

    if (!d->objects.contains(object)) {
        d->objects.append(object);
        if (object->objectName() != "") {
            d->namedObjects.insertMulti(object->objectName(), object);
        }
        emit objectAdded(object, type);
    }
}

/*!
    \fn void PluginManager::removeObject(QObject * object)
    \brief Removes \a object to object pool.

    \sa PluginManager::addObject
*/
void PluginManager::removeObject(QObject * object)
{
    Q_D(PluginManager);

    if (!object) {
        return;
    }

    d->objects.removeAll(object);
    emit objectRemoved(object);
}

QObjectList PluginManager::objects()
{
    return d_func()->objects;
}

/*!
    \fn QObject * PluginManager::object(const QString &name)
    \brief Returns object from object pool with objectName property equal to \a name.

    \sa PluginManager::addObject
*/
QObject * PluginManager::object(const QString &name)
{
    return d_func()->namedObjects.value(name);
}

/*!
    \fn QObject * PluginManager::object(const QString &name)
    \brief Returns list object from object pool with objectName property equal to \a name.

    \sa PluginManager::addObject
*/
QObjectList PluginManager::objects(const QString &name)
{
    return d_func()->namedObjects.values(name);
}

/*!
    \fn void PluginManager::loadPlugins()
    \brief Loads all plugins from plugins folder.

*/
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

QString PluginManager::pluginsFolder() const
{
    return d_func()->pluginsFolder;
}

void PluginManager::setPluginsFolder(const QString &name)
{
    d_func()->pluginsFolder = name;
}

/*!
    \fn QList<PluginSpec *> PluginManager::plugins() const
    \brief Return list of PluginSpecs loaded at current moment.

*/
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



