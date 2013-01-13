#include "pluginmanager.h"
#include "pluginmanager_p.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include <QtCore/QFileSystemWatcher>
#include <QtCore/QLocale>
#include <QtCore/QTranslator>
#include <QDebug>

using namespace ExtensionSystem;

// ============= PluginManager =============

PluginManager *PluginManager::m_instance = 0;

PluginManager *PluginManager::instance()
{
    return m_instance;
}

// returns root path of the application
static QString getRootPath()
{
    // Figure out root:  Up one from 'bin' or 'MacOs'
    QDir rootDir = QCoreApplication::applicationDirPath();
    rootDir.cdUp();
    return rootDir.canonicalPath();
}

static inline QString getDefaultTranslationsPath()
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

/*!
    \class ExtensionSystem::PluginManager
    \brief PluginManager is a class for searching, (un)loading and monitoring for plugins.
*/

/*!
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

    d->clearError();
    setTranslationsDir(getDefaultTranslationsPath());
}

/*!
    \brief Destroys PluginManager.
*/
PluginManager::~PluginManager()
{
    unloadPlugins();

    qDeleteAll(d_func()->formatHandlers);
}

/*!
    \brief Loads all plugins from plugins folder.

    Plugins loaded in order specified byt their dependencies, i.e. dependency
    plugins are loaded before and dependent plugins are loaded after plugin.

    Plugins are searched in subfolders specified by pluginsFolder property in
    QCoreApplication::libraryPaths().

    PluginManager also automatically loads translations for plugins, Qt and
    additional list of translations, specified by PluginManager::translations.
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
    \brief Performs post initialization for all pugins with given \a arguments.

    This function should be called after all arguments are parsed or new
    arguments arive (for example, when new instance is started with different
    arguments).
*/
void PluginManager::postInitialize(const QStringList &arguments)
{
    Q_D(PluginManager);

    if (!d->loaded)
        return;

    if (!d->opts.parse(arguments)) {
        d->addErrorString(PluginManager::tr("Error parsing options : '%1'").arg(d->opts.errorString()));
        return;
    }

    foreach (PluginSpec *spec, plugins()) {
        if (!spec->loaded())
            continue;

        QString name = spec->name();
        QVariantMap options = d->options(name);
        spec->plugin()->postInitialize(options);
    }
}

/*!
    \brief Unloads all currently loaded plugins.

    Plugins are unloaded in reverse order to the oreder they were loaded.
    This function is automatically called when destroying PluginManager class.
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

    d->unloadTranslations();

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

/*!
    \property PluginManager::hasErrors

    Holds whether errors occured during lad operation, or not.
*/
bool PluginManager::hasErrors() const
{
    return d_func()->hasErrors;
}

/*!
    \property PluginManager::errors

    This property contains list of all errors occured during last operation.
*/
QStringList PluginManager::errors() const
{
    return d_func()->errors;
}

/*!
    \property PluginManager::pluginsFolder

    This propety holds subfolder in a QCoreApplication::libraryPaths() where
    PluginManager searches for plugins.
*/
QString PluginManager::pluginsFolder() const
{
    return d_func()->pluginsFolder;
}

void PluginManager::setPluginsFolder(const QString &name)
{
    d_func()->pluginsFolder = name;
}

/*!
    \property PluginManager::translationsDir

    This propety holds folder where PluginManager should look for
    translations for plugins and/or specified libraries.

    \sa PluginManager::translations
*/
QString PluginManager::translationsDir() const
{
    return d_func()->translationsDir;
}

void PluginManager::setTranslationsDir(const QString &dir)
{
    d_func()->translationsDir = dir;
}

/*!
    \property PluginManager::translations

    This property hold additional translations that should be loaded by
    PluginManager. Typically, this is list of translations for librries used
    by application and plugins.
*/
QStringList PluginManager::translations() const
{
    return d_func()->translations;
}

void PluginManager::setTranslations(const QStringList &translations)
{
    d_func()->translations = translations;
}

/*!
    \property PluginManager::loaded

    Holds whether plugins are loaded (i.e. load() function was called), or not.
*/
bool PluginManager::loaded()
{
    return d_func()->loaded;
}

/*!
    \brief Return list of PluginSpecs loaded at current moment.
*/
QList<PluginSpec *> PluginManager::plugins() const
{
    return d_func()->pluginSpecs;
}

/*!
    \brief Return PluginSpec with the given \a name.
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

/*!
    \fn void PluginManager::pluginsLoaded()
    \brief This signal is emitted after plugins loading is finished.
*/

/*!
    \fn void PluginManager::pluginsUnloaded()
    \brief This signal is emitted after plugins unloading is finished.
*/

/*!
    \fn void PluginManager::error(const QString &error)
    \brief This signal is emitted every time an error occured.
*/

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

/*!
    \reimp
*/
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
        addErrorString(PluginManager::tr("No plugins found in (%1)").
                       arg(folders.join(QLatin1String(", "))));
        return false;
    }

    loadLibsTranslations();
    loadPluginsTranslations(specFiles);

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
                addErrorString(PluginManager::tr("Failed to read spec file %1 : '%2'").
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

void PluginManagerPrivate::loadLibsTranslations()
{
    QStringList translations = this->translations;
    translations.prepend(QLatin1String("qt"));

    loadTranslations(translations);
}

void PluginManagerPrivate::loadPluginsTranslations(const QStringList &specFiles)
{
    QStringList translations;
    foreach (const QString &specFile, specFiles)
        translations.append(QFileInfo(specFile).baseName());

    loadTranslations(translations);
}

void PluginManagerPrivate::loadTranslations(const QStringList &translations)
{
    QString locale = QLocale::system().name();
    foreach (const QString &translation, translations) {
        QTranslator *translator = new QTranslator();
        translators.append(translator);

        QString path = QString("%1_%2").arg(translation).arg(locale);
        bool ok = translator->load(path, translationsDir);
        if (!ok) {
            qWarning() << "PluginManagerPrivate::loadTranslations"
                       << "Failed to load translation file"
                       << path;
        } else {
            qApp->installTranslator(translator);
        }
    }
}

void PluginManagerPrivate::unloadTranslations()
{
    qDeleteAll(translators);
    translators.clear();
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
