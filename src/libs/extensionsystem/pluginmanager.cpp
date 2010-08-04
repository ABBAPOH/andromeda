#include "pluginmanager.h"
#include "pluginmanager_p.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QPluginLoader>
#include <QtCore/QDir>
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
    Q_ASSERT(!m_instance);
    m_instance = this;
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
    // fill specs dependency lists
    d->resolveDependencies();

    // get loadQueue in correct order
    QList<PluginSpec *> loadQueue = d->loadQueue();

    // initialize plugins
    foreach (PluginSpec *spec, loadQueue) {
        spec->plugin()->initialize();
    }

    // initialize using extensions
    foreach (PluginSpec *spec, loadQueue) {
        spec->plugin()->extensionsInitialized();
    }
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

// ============= PluginManagerPrivate =============
PluginManagerPrivate::PluginManagerPrivate()
{
}

QList<PluginSpec *> PluginManagerPrivate::loadQueue()
{
    QList<PluginSpec *> result;
    foreach (PluginSpec* spec, pluginSpecs) {
        if (result.contains(spec))
            continue;
        QList<PluginSpec *> circularCheck;
        QList<PluginSpec *> specsForCheck;
        specsForCheck.append(spec);
        PluginSpec* currentSpec = 0;
        while (!specsForCheck.isEmpty()) {
            currentSpec = specsForCheck.takeFirst();
            if (circularCheck.contains(currentSpec)) {
                spec->d_ptr->errorString = "circular inclusion detected";
                spec->d_ptr->hasError = true;
                qWarning() << "circular inclusion detected";
                circularCheck.clear();
                break;
            }
            circularCheck.append(currentSpec);
            specsForCheck.append(currentSpec->dependencySpecs());
        }

        for (int i = circularCheck.size() - 1; i >= 0; i--) {
            result.append(circularCheck.at(i));
        }
    }
    return result;
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
        QDir pluginsDir = QDir(folder);
        if (pluginsFolder != "") // we try to cd into pluginsFolder
            if (!pluginsDir.cd(pluginsFolder))
                continue;

        foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
            QString libraryPath = pluginsDir.absoluteFilePath(fileName);
            PluginSpec * spec = new PluginSpec(libraryPath);
            if (!spec->hasError()) {
                pluginSpecs.append(spec);
            } else
                delete spec;
        }
    }

    qSort(pluginSpecs.begin(), pluginSpecs.end(), lessThanByPluginName);
}

void PluginManagerPrivate::resolveDependencies()
{
    foreach (PluginSpec * spec, pluginSpecs) {
        spec->d_ptr->resolveDependencies(pluginSpecs);
    }
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


