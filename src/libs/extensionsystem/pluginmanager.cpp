#include "pluginmanager.h"
#include "pluginmanager_p.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include <QtCore/QFileSystemWatcher>
#include <QDebug>

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
    QObjectPool(*new PluginManagerPrivate(this), parent)
{
    Q_D(PluginManager);
    Q_ASSERT(!m_instance);
    m_instance = this;
    d->loaded = false;

    d->watcher = new QFileSystemWatcher(this);
    connect(d->watcher, SIGNAL(directoryChanged(QString)), SLOT(updateDirectory(QString)));
    connect(d->watcher, SIGNAL(fileChanged(QString)), SLOT(updateLibrary(QString)));
    startTimer(5000);

    d->formatHandlers.resize(2);
    d->formatHandlers[PluginSpec::XmlFormat] = new PluginSpecXmlHandler;
    d->formatHandlers[PluginSpec::BinaryFormat] = new PluginSpecBinaryHandler;
}

/*!
    \fn PluginManager::~PluginManager()
    \brief Destroys PluginManager.
*/
PluginManager::~PluginManager()
{
    unloadPlugins();

    qDeleteAll(d_func()->formatHandlers);
}

QStringList PluginManager::arguments() const
{
    return d_func()->arguments;
}

void PluginManager::setArguments(const QStringList &arguments)
{
     d_func()->arguments = arguments;
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
            if (!QFileInfo(folder + QLatin1Char('/') + pluginsFolder()).exists())
                continue;
            else
                folder = folder + QLatin1Char('/') + pluginsFolder();
        }
        d->foldersToBeLoaded.append(folder);
        d->watcher->addPath(folder);
    }

    if (!d->load()) {
        qWarning() << "PluginManager warning: Couldn't load plugins";
        qWarning() << "Searched paths:" << qApp->libraryPaths();
        return;
    }
    d->loaded = true;

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

    qDeleteAll(d->pluginSpecs);
    d->pluginSpecs.clear();

    d->loaded = false;
    emit pluginsUnloaded();
}

QStringList PluginManager::defaultPlugins() const
{
    return d_func()->defaultPlugins;
}

void PluginManager::setDefaultPlugins(const QStringList &plugins)
{
    d_func()->defaultPlugins = plugins;
}

bool PluginManager::hasErrors() const
{
    return d_func()->hasErrors;
}

QStringList PluginManager::errors() const
{
    return d_func()->errors;
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

/*!
    \fn PluginSpec *PluginManager::plugin(const QString &name) const
    \brief Return PluginSpecs with name \a name.

*/
PluginSpec *PluginManager::plugin(const QString &name) const
{
    Q_D(const PluginManager);

    for (int i = 0; i < d->pluginSpecs.size(); i++) {
        if (d->pluginSpecs[i]->name() == name)
            return d->pluginSpecs[i];
    }
    return 0;
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

        if (d->load())
            emit pluginsChanged();
    }
}

// ============= PluginManagerPrivate =============

    //static bool lessThanByPluginName(const PluginSpec *first, const PluginSpec *second)
//{
//    return first->name() < second->name();
//}

bool PluginManagerPrivate::load()
{
    QStringList folders = foldersToBeLoaded;
    foldersToBeLoaded.clear();

    QStringList specFiles = getSpecFiles(folders);

    // get all specs from files
    QList<PluginSpec *> newSpecs = loadSpecs(specFiles);

    if (pluginSpecs.isEmpty()) {
        addErrorString(QObject::tr("No plugins found in (%1)").
                       arg(folders.join(QLatin1String(", "))));
        return false;
    }

    if (!opts.parse(arguments)) {
        addErrorString(QObject::tr("Error parsing options : '%1'").arg(opts.errorString()));
        return false;
    }

    // TODO: error about not initialized specs
    // enables new plugins
    enableSpecs(newSpecs);

    return true; // return true if we have new plugins
}

QStringList PluginManagerPrivate::getSpecFiles(QStringList folders)
{
    QStringList result;
    foreach (const QString &folder, folders) {
        QDirIterator it(folder, QStringList() << "*.spec");
        while (it.hasNext()) {
            result.append(it.next());
        }
    }
    return result;
}

QList<PluginSpec*> PluginManagerPrivate::loadSpecs(QStringList specFiles)
{
    QList<PluginSpec*> result;
    foreach (const QString &specFile, specFiles) {
        if (!pathToSpec.contains(specFile)) {

            PluginSpec *spec = new PluginSpec();
            if (!spec->read(specFile)) {
                addErrorString(QObject::tr("Failed to read spec file %1 : '%2'").
                               arg(specFile).
                               arg(spec->errorString()));
                delete spec;
                continue;
            }

            pathToSpec.insert(specFile, spec);
            pluginSpecs.append(spec);
            result.append(spec);
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
        if (spec->loadOnStartup() || spec->d_func()->isDefault) {
            spec->load();
        }
    }
}

QVariantMap PluginManagerPrivate::options(const QString &name)
{
    QVariantMap result;

    PluginSpec *plugin = q_func()->plugin(name);
    foreach (const Option &option, plugin->d_func()->options) {
        QString name = option.name();
        result.insert(name, opts.values().value(name));
    }
    return result;
}

void PluginManagerPrivate::clearError()
{
    hasErrors = false;
    errors.clear();
}

void PluginManagerPrivate::addErrorString(const QString &message)
{
    hasErrors = true;
    errors.append(message);
    emit q_func()->error(message);
}
