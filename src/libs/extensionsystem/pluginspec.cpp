#include "pluginspec.h"
#include "pluginspec_p.h"

#include <QtCore/QFileInfo>
#include <QtCore/QPluginLoader>
#include <QtCore/QRegExp>
#include <QtCore/QSettings>
#include <QtCore/QStringList>
#include <QtCore/QXmlStreamWriter>
#include <QtCore/QDebug>

#include "pluginmanager.h"
#include "pluginmanager_p.h"

//#define DEBUG_OUTPUT

using namespace ExtensionSystem;

/*!
    \class ExtensionSystem::Version
    \brief PluginDependency is a small class for representing library version.

    This class consists of 4 subversions - major, minor, build and revision.
*/

/*!
    Constructs null Version object (0.0.0.0 version).

    \sa isNull
*/
Version::Version() :
    build(0),
    revision(0)
{
    this->major = 0;
    this->minor = 0;
}

/*!
    Constructs Version object with the given \a version string.
*/
Version::Version(const QString &version) :
    build(0),
    revision(0)
{
    this->major = 0;
    this->minor = 0;

    // mathes to Major.Minor.Patch.Fix version
    static QRegExp regExp = QRegExp("([0-9]+)(?:\\.([0-9]+))?(?:\\.([0-9]+))?(?:\\.([0-9]+))?");
    if ( regExp.exactMatch(version) ) {
        major = regExp.cap(1).toInt();
        minor = regExp.cap(2).toInt();
        build = regExp.cap(3).toInt();
        revision = regExp.cap(4).toInt();
    }
}

/*!
    Returns true if Version object represents null version, i.e. version 0.0.0.0.
*/
bool Version::isNull() const
{
    return build == 0 && revision == 0 && major == 0 && minor == 0;
}

/*!
    Converts Version object to string.
*/
QString Version::toString() const
{
    return QString("%1.%2.%3.%4").arg(major).arg(minor).arg(build).arg(revision);
}

/*!
    Creates and returns Version object with the given \a version string.
*/
Version Version::fromString(const QString &version)
{
    return Version(version);
}

/*!
    Compares one Version object to an other.
*/
bool Version::operator==(const Version &other) const
{
    return major == other.major && minor == other.minor && build == other.build && revision == other.revision;
}

/*!
    \class ExtensionSystem::PluginDependency
    \brief PluginDependency is a small class for representing library
    dependencies.

    Dependency is identified by 2 fields - name and version.
*/

/*!
    Constructs PluginDependency class with given \a name and \a version.
*/
PluginDependency::PluginDependency(const QString &name, const QString &version)
{
    m_name = name;
    m_version = Version(version);
}

/*!
    Constructs PluginDependency class with given \a name and \a version.
*/
PluginDependency::PluginDependency(const QString &name, const Version &version)
{
    m_name = name;
    m_version = version;
}

/*!
    \fn QString PluginDependency::name() const
    Returns name of dependency library.
*/

/*!
    \fn QString PluginDependency::version() const
    Returns version of dependency library.
*/

/*!
    Compares one PluginDependency to an other.
*/
bool PluginDependency::operator==(const PluginDependency &other)
{
    return m_name == other.m_name && m_version == other.m_version;
}

PluginSpecPrivate::PluginSpecPrivate(PluginSpec *qq) :
    q_ptr(qq),
    plugin(0),
    loader(0),
    loaded(false),
    loadOnStartup(true),
    isDefault(false),
    hasError(false)
{
}

bool PluginSpecPrivate::load()
{
#ifdef DEBUG_OUTPUT
    qDebug() << "loading" << name << version.toString();
    qDebug() << "  resolving dependencies for" << name;
#endif
    if (!resolveDependencies())
        return false;

    bool ok = true;

    QString tmp("Can't load plugin: %1");
    QStringList errorMessages;
    foreach (PluginSpec *spec, dependencySpecs) {
        spec->load();
        if (!spec->loaded()) {
            ok = false;
            errorMessages << tmp.arg(spec->name());
        }
    }

    if (!ok) {
        setError(errorMessages.join("\n"));
        return false;
    }

#ifdef DEBUG_OUTPUT
    qDebug() << "    loading library" << QFileInfo(libraryPath).fileName();
#endif
    if (!loadLibrary())
        return false;

#ifdef DEBUG_OUTPUT
    qDebug() << "      initializing" << name;
#endif
    if (!plugin->initialize()) {
        setError(PluginSpec::tr("Failed to initialize plugin %1").arg(name));
        return false;
    }
#ifdef DEBUG_OUTPUT
    qDebug() << "======";
#endif

    return true;
}

bool PluginSpecPrivate::loadLibrary()
{
    Q_Q(PluginSpec);

    if (plugin)
        return true;

    if (!loader) {
        loader = new QPluginLoader(q);
        loader->setFileName(libraryPath);
    }

    QObject * object = loader->instance();
    if (!object) {
        setError(PluginSpec::tr("Can't load plugin: ") + loader->errorString());
        return false;
    }

    plugin = qobject_cast<IPlugin *>(object);
    if (!plugin) {
        setError(PluginSpec::tr("Can't load plugin: not a valid plugin"));
        return false;
    }

    return true;
}

bool PluginSpecPrivate::unload()
{
    bool ok = true;
    QString errorMessage;

#ifdef DEBUG_OUTPUT
    qDebug() << "unloading" << name << version.toString();
    qDebug() << "  unloading dependencies for" << name;
#endif
    foreach (PluginSpec *spec, dependentSpecs) {
        spec->unload();
        if (spec->loaded()) {
            ok = false;
            errorMessage += PluginSpec::tr("Can't unload plugin: %1 is not loaded").arg(spec->name());
        }
    }

    if (!ok) {
        setError(errorMessage);
        return false;
    }

#ifdef DEBUG_OUTPUT
    qDebug() << "    shutting down" << name;
#endif
    plugin->shutdown();

#ifdef DEBUG_OUTPUT
    qDebug() << "      unloading library" << QFileInfo(libraryPath).fileName();
#endif
    if (!unloadLibrary())
        return false;
#ifdef DEBUG_OUTPUT
    qDebug() << "======";
#endif

    return true;
}

bool PluginSpecPrivate::unloadLibrary()
{
    if (!loader->unload()) {
        setError(PluginSpec::tr("Can't unload plugin library: %1").arg(loader->errorString()));
        return false;
    }
    delete loader;
    loader = 0;
    plugin = 0;
    return true;
}

//#warning TODO: check circular dependencies
bool PluginSpecPrivate::resolveDependencies()
{
    Q_Q(PluginSpec);

    QList<PluginSpec*> specs = PluginManager::instance()->plugins();
    QList<PluginSpec*> resolvedSpecs;

    PluginSpec * dependencySpec = 0;
    bool ok = true;
    QString errorMessage;

    foreach (PluginDependency dep, dependencies) {
        dependencySpec = 0;
        foreach (PluginSpec *spec, specs) {
            if (spec->provides(dep)) {
                dependencySpec = spec;
                break;
            }
        }
        if (dependencySpec == 0) {
            ok = false;
            errorMessage.append(PluginSpec::tr("Can't resolve dependency '%1(%2)'")
                                .arg(dep.name()).arg(dep.version().toString()));
        } else {
            resolvedSpecs.append(dependencySpec);
        }
    }

    if (!ok) {
        setError(errorMessage);
        return false;
    }

    foreach (PluginSpec *spec, resolvedSpecs) {
        if (!spec->d_ptr->dependentSpecs.contains(q))
            spec->d_ptr->dependentSpecs.append(q);
    }

    dependencySpecs = resolvedSpecs;
    return true;
}

int PluginSpecPrivate::compareVersion(const Version &version1, const Version &version2)
{
    if (version1.major < version2.major)
        return -1;
    if (version1.major > version2.major)
        return 1;

    if (version1.minor < version2.minor)
        return -1;
    if (version1.minor > version2.minor)
        return 1;

    if (version1.build < version2.build)
        return -1;
    if (version1.build > version2.build)
        return 1;

    if (version1.revision < version2.revision)
        return -1;
    if (version1.revision > version2.revision)
        return 1;

    return 0;
}

QString PluginSpecPrivate::getLibraryPath(const QString &path)
{
    QFileInfo info(path);
    QString baseName = info.baseName();
    QString absolutePath = info.absolutePath();
#ifdef Q_OS_WIN
    return absolutePath + "/" + "lib" + baseName + ".dll";
#endif
#ifdef Q_OS_MAC
    return absolutePath + "/" + "lib" + baseName + ".dylib";
#else
#ifdef Q_OS_UNIX
    return absolutePath + "/" + "lib" + baseName + ".so";
#endif
#endif
}

void PluginSpecPrivate::setError(const QString &message)
{
    hasError = true;
    errorString = message;
    emit q_func()->error(message);
}

/*!
    \class ExtensionSystem::PluginSpec

    \brief PluginSpec class is a descriptor object for a plugin and it's controller.
*/

/*!
    \internal
*/
PluginSpec::PluginSpec(QObject *parent) :
    QObject(parent),
    d_ptr(new PluginSpecPrivate(this))
{
}

/*!
    \internal
*/
PluginSpec::~PluginSpec()
{
    QSettings s;
    s.beginGroup(name());
    s.setValue("loadOnStartup", loadOnStartup());
    s.endGroup();

    delete d_ptr;
}

/*!
    \property PluginSpec::name
    Holds the name of the plugin. This name is used to resolve dependencies
    among with version.
*/
QString PluginSpec::name() const
{
    return d_func()->name;
}

/*!
    \brief Returns version of the plugin.
    Version should be in format "Major.Minor.Build.Revision".
*/
Version PluginSpec::version() const
{
    return d_func()->version;
}

/*!
    \brief Returns compatibility version of the plugin.
*/
Version PluginSpec::compatibilityVersion() const
{
    return d_func()->compatibilityVersion;
}

/*!
    \property PluginSpec::vendor
    Holds the vendor of the plugin.
*/
QString PluginSpec::vendor() const
{
    return d_func()->vendor;
}

/*!
    \property PluginSpec::category
    Holds the category (i.e. group) that plugin belongs to.

    Categories can be used to group plugins together to display list of
    plugins in more user-friendly way.
*/
QString PluginSpec::category() const
{
    return d_func()->category;
}

/*!
    \property PluginSpec::copyright
    Holds the copyright.
*/
QString PluginSpec::copyright() const
{
    return d_func()->copyright;
}

/*!
    \property PluginSpec::license
    Holds the license name.

    Only license name should be here, not it's full text.
*/
QString PluginSpec::license() const
{
    return d_func()->license;
}

/*!
    \property PluginSpec::description
    Holds human-readable description of the plugin.
*/
QString PluginSpec::description() const
{
    return d_func()->description;
}

/*!
    \property PluginSpec::url
    Holds vendor's site url.
*/
QString PluginSpec::url() const
{
    return d_func()->url;
}

/*!
    \brief Returns list of dependencies that are needed for this plugin.
*/
QList<PluginDependency> PluginSpec::dependencies() const
{
    return d_func()->dependencies;
}

/*!
    \brief Returns list of PluginSpecs that need for this plugin.
*/
QList<PluginSpec*> PluginSpec::dependencySpecs() const
{
    return d_func()->dependencySpecs;
}

/*!
    \brief Returns list of PluginSpecs that require this plugin.
*/
QList<PluginSpec*> PluginSpec::dependentSpecs() const
{
    return d_func()->dependentSpecs;
}

/*!
    \brief Returns path to dynamic library.
    In case of static plugin, returns empty string.

    \sa PluginSpec::isStatic
*/
QString PluginSpec::libraryPath() const
{
    return d_func()->libraryPath;
}

/*!
    \property PluginSpec::loaded
    Holds if plugin is loaded or not.
*/
bool PluginSpec::loaded() const
{
    return d_func()->loaded;
}

/*!
    \brief Loads plugin into memory and initialises it.
*/
void PluginSpec::load()
{
    Q_D(PluginSpec);

    if (d->loaded)
        return;

    d->clearError();

    if (d->load()) {
        d->loaded = true;
        emit loadedChanged(true);
    }
}

/*!
    \brief Shutdowns and unloads plugin from memory.
*/
void PluginSpec::unload()
{
    Q_D(PluginSpec);

    if (!d->loaded)
        return;

    d->clearError();

    if (d->unload()) {
        d->loaded = false;
        emit loadedChanged(false);
    }
}

/*!
    \brief Loads or unloads plugin.
*/
void PluginSpec::setLoaded(bool yes)
{
    if (yes)
        load();
    else
        unload();
}

/*!
    \property PluginSpec::loadOnStartup

    Holds if plugin should be loaded during application startup
    (i.e. when PluginManager::load() function is called).
*/
bool PluginSpec::loadOnStartup() const
{
    return d_func()->loadOnStartup;
}

void PluginSpec::setLoadOnStartup(bool yes)
{
    Q_D(PluginSpec);

    if (d->loadOnStartup == yes)
        return;

    d->loadOnStartup = yes;
    emit loadOnStartupChanged(yes);
}

/*!
    \property PluginSpec::canBeUnloaded

    Holds whether plugin can be unloaded or not (i.e. it is not default plugin).
*/
bool PluginSpec::canBeUnloaded() const
{
    if (d_func()->isDefault)
        return false;

    return true;
}

/*!
    \brief Returns true if error occured.
*/
bool PluginSpec::hasError() const
{
    return d_func()->hasError;
}

/*!
    \brief Returns message of last occured error.
*/
QString PluginSpec::errorString() const
{
    Q_D(const PluginSpec);

    return !d->errorString.isEmpty() ? d->errorString : tr("No error", "PluginSpec");
}

/*!
    \brief Returnes pointer to IPlugin implementation.
*/
IPlugin *PluginSpec::plugin() const
{
    return d_func()->plugin;
}

/*!
    Returns if this plugin can be used to fill in a of the given \a dependency.
    \sa PluginSpec::dependencies()
*/
bool PluginSpec::provides(const PluginDependency &dependency) const
{
    if (QString::compare(dependency.name(), name(), Qt::CaseInsensitive) != 0)
         return false;
    return (PluginSpecPrivate::compareVersion(version(), dependency.version()) >= 0) &&
           (PluginSpecPrivate::compareVersion(compatibilityVersion(), dependency.version()) <= 0);
}

/*!
    Reads plugins spec file located at the given \a path.

    Returns true on success.
*/
bool PluginSpec::read(const QString &path)
{
    Q_D(PluginSpec);

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        d->setError(tr("Failed to open file %1 : '%2'").
                    arg(path).
                    arg(file.errorString()));
        return false;
    }

    foreach (PluginSpecFormatHandler *h, PluginManager::instance()->d_func()->handlers()) {
        if (h && h->canRead(&file)) {
            if (!h->read(&file, d)) {
                d->setError(tr("Cannot read file %1 : '%2'").
                            arg(path).
                            arg(h->errorString()));
                return false;
            }
            break;
        }
    }

    // TODO: implement
    // validate();

    Options &opts = PluginManager::instance()->d_func()->options();
    d->isDefault = PluginManager::instance()->defaultPlugins().contains(name());
    foreach (Option opt, d->options) {
        if (!d->isDefault)
            opt.setShortName(QChar());

        if (!opts.addOption(opt)) {
            d->setError(tr("Failed to add option %1 : '%2'").
                        arg(opt.name()).
                        arg(opts.errorString()));
        }
        if (!d->defaultOption.isEmpty() && opts.defaultOption().isEmpty()) {
            if (d->isDefault)
                opts.setDefaultOption(d->defaultOption);
        }

    }

    d->libraryPath = d->getLibraryPath(path);

    QSettings s;
    s.beginGroup(name());
    if (s.contains("loadOnStartup"))
        d->loadOnStartup = s.value("loadOnStartup").toBool();
    s.endGroup();

    return true;
}

/*!
    Writes PluginSpec to the file located at \a path. If file already exists,
    it is overwritten.

    \a format parameter is deprecated and shouldn't be used - the only supported
    format is xml; binary format is broken.
*/
bool PluginSpec::write(const QString &path, Format format)
{
    Q_D(PluginSpec);

    PluginSpecFormatHandler *h = PluginManager::instance()->d_func()->handler(format);
    if (h) {
        QFile file(path);
        if (!file.open(QIODevice::WriteOnly)) {
            d->setError(tr("Failed to open file %1 : '%2'").
                        arg(path).
                        arg(file.errorString()));
            return false;
        }

        if (!h->write(&file, d_func())) {
            d->setError(tr("Failed to write file %1 : '%2'").
                        arg(path).
                        arg(h->errorString()));
            return false;
        }
    }

    return true;
}

/*!
    \fn void PluginSpec::loadedChanged(bool loaded)

    This signal is emitted when PluginSpec::loaded property is changed.
*/

/*!
    \fn void PluginSpec::loadOnStartupChanged(bool loads)

    This signal is emitted when PluginSpec::loadOnStartup property is changed.
*/

/*!
    \fn void PluginSpec::error(const QString &message)

    This signal is emitted when error occurs. \a message is a human-readable
    error string.
*/
