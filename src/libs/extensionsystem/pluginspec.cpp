#include "pluginspec.h"
#include "pluginspec_p.h"

#include <QtCore/QPluginLoader>
#include <QtCore/QStringList>
#include <QtCore/QRegExp>
#include <QtCore/QSettings>
#include <QtCore/QFileInfo>
#include <QDebug>

#include "iplugin_p.h"
#include "pluginmanager.h"

using namespace ExtensionSystem;

Version::Version() :
        major(0),
        minor(0),
        build(0),
        revision(0)
{
}

Version::Version(const QString &version) :
        major(0),
        minor(0),
        build(0),
        revision(0)
{
    // mathes to Major.Minor.Patch.Fix version
    static QRegExp regExp = QRegExp("([0-9]+)(?:\\.([0-9]+))?(?:\\.([0-9]+))?(?:\\.([0-9]+))?");
    if ( regExp.exactMatch(version) ) {
        major = regExp.cap(1).toInt();
        minor = regExp.cap(2).toInt();
        build = regExp.cap(3).toInt();
        revision = regExp.cap(4).toInt();
    }
}

QString Version::toString() const
{
    return QString("%1.%2.%3.%4").arg(major).arg(minor).arg(build).arg(revision);
}

Version Version::fromString(const QString &version)
{
    return Version(version);
}

bool Version::operator==(const Version &other)
{
    return major == other.major && minor == other.minor && build == other.build && revision == other.revision;
}

PluginSpecPrivate::PluginSpecPrivate(PluginSpec *qq):
        q_ptr(qq),
        plugin(0),
        loader(new QPluginLoader(q_ptr)),
        hasError(false),
        loaded(false),
        loadOnStartup(true)
{
    errorString = "Unknown Error";
}

void PluginSpecPrivate::init(const QString &path)
{
    QSettings specFile(path, QSettings::IniFormat);

    name = specFile.value("name").toString();
    version = Version::fromString(specFile.value("version").toString());
    compatibilityVersion = Version::fromString(specFile.value("compatibilityVersion").toString());
    vendor = specFile.value("vendor").toString();
    category = specFile.value("category").toString();
    copyright = specFile.value("copyright").toString();
    license = specFile.value("license").toString();
    description = specFile.value("description").toString();
    url = specFile.value("url").toString();

    specFile.beginGroup("Dependencies");
    foreach (QString key, specFile.childKeys()) {
        if (key.length() == 1 && key.at(0).isDigit()) {
            QStringList list = specFile.value(key).toStringList();
            if (list.size() == 2) {
                dependencies.append(PluginDependency(list.at(0), list.at(1)));
            }
        }
    }
    specFile.endGroup();

    libraryPath = getLibraryPath(path);
}

bool PluginSpecPrivate::load()
{
    qDebug() << "loading" << name << version.toString();
    qDebug() << "  resolving dependencies for" << name;
    if (!resolveDependencies())
        return false;

    bool ok = true;

    QString errorMessage;
    foreach (PluginSpec *spec, dependencySpecs) {
        spec->load();
        if (!spec->loaded()) {
            ok = false;
            errorMessage += "Can't load plugin: " + spec->name() + " is not loaded";
        }
    }

    if (!ok) {
        setError(errorMessage);
        return false;
    }

    qDebug() << "    loading library" << QFileInfo(libraryPath).fileName();
    if (!loadLibrary())
        return false;

    qDebug() << "      initializing" << name;
    if (!plugin->initialize()) {
        setError("Failed to initialize plugin");
        return false;
    }
    qDebug() << "======";

    return true;
}

bool PluginSpecPrivate::loadLibrary()
{
    if (plugin)
        return true;

    QObject * object = loader->instance();
    if (!object) {
        setError(QObject::tr("Can't load plugin: ", "PluginSpec") + loader->errorString());
        return false;
    }

    IPlugin *plugin = qobject_cast<IPlugin *>(object);
    if (!plugin) {
        setError(QObject::tr("Can't load plugin: not a valid plugin", "PluginSpec"));
        return false;
    }

    this->plugin = plugin;
    return true;
}

bool PluginSpecPrivate::unload()
{
    bool ok = true;
    QString errorMessage;

    qDebug() << "unloading" << name << version.toString();
    qDebug() << "  unloading dependencies for" << name;
    foreach (PluginSpec *spec, dependentSpecs) {
        spec->unload();;
        if (spec->loaded()) {
            ok = false;
            errorMessage += "Can't unload plugin: " + spec->name() + " is not unloaded";
        }
    }

    if (!ok) {
        setError(errorMessage);
        return false;
    }

    qDebug() << "    shutting down" << name;
    plugin->shutdown();

    qDebug() << "      unloading library" << QFileInfo(libraryPath).fileName();
    if (!unloadLibrary())
        return false;
    qDebug() << "======";

    return true;
}

bool PluginSpecPrivate::unloadLibrary()
{
    if (!loader->unload()) {
        setError("Can't unload plugin library: " + loader->errorString());
        return false;
    }
    plugin = 0;
    return true;
}

#warning TODO: check circular dependencies
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
            errorMessage.append(QObject::tr("PluginSpec", "Can't resolve dependency '%1(%2)'")
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
    qWarning() << "PluginSpec error:" << message;
    hasError = true;
    errorString = message;
    emit q_ptr->error(message);
}

/*!
    \fn PluginDependency::PluginDependency(const QString &name, const QString &version)
    \brief Constructs PluginDependency class with given \a name and \a version.
*/
PluginDependency::PluginDependency(const QString &name, const QString &version)
{
    m_name = name;
//    m_version = version;
    m_version = Version(version);
}

/*!
    \fn bool PluginDependency::operator==(const PluginDependency &other)
    \brief Comparator for PluginDependency.
*/
bool PluginDependency::operator==(const PluginDependency &other)
{
    return m_name == other.m_name && m_version == other.m_version;
}

/*!
    \fn PluginSpec::PluginSpec()
    \internal
*/
PluginSpec::PluginSpec() :
        d_ptr(new PluginSpecPrivate(this))
{
}

/*!
    \fn PluginSpec::~PluginSpec()
    \internal
*/
PluginSpec::~PluginSpec()
{
    delete d_ptr;
}

/*!
    \fn PluginSpec::PluginSpec(const QString & path)
    \internal
    Constructs PluginSpec using data received from file located at \a path.
*/
PluginSpec::PluginSpec(const QString & path) :
        d_ptr(new PluginSpecPrivate(this))
{
    Q_D(PluginSpec);
    d->init(path);
    d->loader->setFileName(d->libraryPath);
}

/*!
    \fn QString PluginSpec::name() const
    \brief Returns name of the plugin.
    Name and version are used to resolve dependencies.
*/
QString PluginSpec::name() const
{
    return d_func()->name;
}

/*!
    \fn Version PluginSpec::version() const
    \brief Returns version of the plugin.
    Version should be in format "Major.Minor.Build.Revision".
*/
Version PluginSpec::version() const
{
    return d_func()->version;
}

/*!
    \fn Version PluginSpec::compatibilityVersion() const
    \brief Returns compatibility version of the plugin.
*/
Version PluginSpec::compatibilityVersion() const
{
    return d_func()->compatibilityVersion;
}

/*!
    \fn QString PluginSpec::vendor() const
    \brief Returns vendor of the plugin.
*/
QString PluginSpec::vendor() const
{
    return d_func()->vendor;
}

/*!
    \fn QString PluginSpec::category() const
    \brief Returns category (i.e. group) that plugin belongs to.
*/
QString PluginSpec::category() const
{
    return d_func()->category;
}

/*!
    \fn QString PluginSpec::copyright() const
    \brief Returns copyright.
*/
QString PluginSpec::copyright() const
{
    return d_func()->copyright;
}

/*!
    \fn QString PluginSpec::license() const
    \brief Returns full licence of the plugin.
*/
QString PluginSpec::license() const
{
    return d_func()->license;
}

/*!
    \fn QString PluginSpec::description() const
    \brief Returns description of the plugin.
*/
QString PluginSpec::description() const
{
    return d_func()->description;
}

/*!
    \fn QString PluginSpec::url() const
    \brief Returns vendor's site url.
*/
QString PluginSpec::url() const
{
    return d_func()->url;
}

/*!
    \fn QList<PluginDependency> PluginSpec::dependencies() const
    \brief Returns list of dependencies that are needed for this plugin.
*/
QList<PluginDependency> PluginSpec::dependencies() const
{
    return d_func()->dependencies;
}

/*!
    \fn QList<PluginSpec*> PluginSpec::dependencySpecs() const
    \brief Returns list of PluginSpecs that need for this plugin.
*/
QList<PluginSpec*> PluginSpec::dependencySpecs() const
{
    return d_func()->dependencySpecs;
}

/*!
    \fn QList<PluginSpec*> PluginSpec::dependentSpecs() const
    \brief Returns list of PluginSpecs that require this plugin.
*/
QList<PluginSpec*> PluginSpec::dependentSpecs() const
{
    return d_func()->dependentSpecs;
}

/*!
    \fn QString PluginSpec::libraryPath() const
    \brief Returns path to dynamic library.
    In case of static plugin, returns empty string.

    \sa PluginSpec::isStatic
*/
QString PluginSpec::libraryPath() const
{
    return d_func()->libraryPath;
}

/*!
    \fn void PluginSpec::load()
    \brief Loads plugin into memory and initialises it.
*/
void PluginSpec::load()
{
    Q_D(PluginSpec);

    if (d->loaded)
        return;

    if (d->load()) {
        d->loaded = true;
        emit loadedChanged(true);
    }
}

/*!
    \fn void PluginSpec::unload()
    \brief UnLoads plugin from memory and shutdowns it.
*/
void PluginSpec::unload()
{
    Q_D(PluginSpec);

    if (!d->loaded)
        return;

    if (d->unload()) {
        d->loaded = false;
        emit loadedChanged(false);
    }
}

/*!
    \fn void PluginSpec::setEnabled(bool enabled)
    \brief Enables or disables plugin.
    Note that library is loaded or unloaded when necessary.
*/
void PluginSpec::setLoaded(bool yes)
{
    if (yes)
        load();
    else
        unload();
}

/*!
    \fn bool PluginSpec::loaded() const
    \brief Returns if plugin is loaded.
*/
bool PluginSpec::loaded() const
{
    return d_func()->loaded;
}

void PluginSpec::setLoadOnStartup(bool yes)
{
    Q_D(PluginSpec);

    if (d->loadOnStartup == yes)
        return;

    d->loadOnStartup = yes;
    emit loadOnStartupChanged(yes);
}

bool PluginSpec::loadOnStartup() const
{
    return d_func()->loadOnStartup;
}

bool PluginSpec::canBeUnloaded() const
{
    foreach (PluginSpec *spec, dependentSpecs()) {
        if (spec->loaded())
            return false;
    }
    return true;
}

/*!
    \fn QString PluginSpec::errorString()
    \brief Returns message of last occured error.
*/
QString PluginSpec::errorString() const
{
    return d_func()->errorString;
}

/*!
    \fn bool PluginSpec::hasError()
    \brief Returns true if error occured.
*/
bool PluginSpec::hasError() const
{
    return d_func()->hasError;
}

/*!
    \fn IPlugin *PluginSpec::plugin() const
    \brief Returnes pointer to IPlugin implementation.
*/
IPlugin *PluginSpec::plugin() const
{
    return d_func()->plugin;
}

/*!
    \fn bool PluginSpec::provides(const PluginDependency &pluginDependency) const
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

namespace ExtensionSystem {
QDataStream & operator>>(QDataStream &s, PluginDependency &dependency)
{
    QString a, b;
    s >> a;
    s >> b;
    dependency = PluginDependency(a, b);
    return s;
}

QDataStream & operator<<(QDataStream &s, const PluginDependency &dependency)
{
    s << dependency.name();
    s << dependency.version().toString();
    return s;
}

QDataStream & operator>>(QDataStream &s, PluginSpec &pluginSpec)
{
    PluginSpecPrivate * d = pluginSpec.d_func();
    quint32 magic;
    s >> magic;
    s >> d->name;
    QString str;
    s >> str;
    d->version = Version::fromString(str);
    s >> str;
    d->compatibilityVersion = Version::fromString(str);
    s >> d->vendor;
    s >> d->category;
    s >> d->copyright;
    s >> d->license;
    s >> d->url;
    s >> d->dependencies;
    s >> d->libraryPath;
    return s;
}

QDataStream & operator<<(QDataStream &s, const PluginSpec &pluginSpec)
{
    const PluginSpecPrivate * d = pluginSpec.d_func();
    quint32 magic = *((quint32*)"SPEC");
    s << magic;
    s << d->name;
    s << d->version.toString();
    s << d->compatibilityVersion.toString();
    s << d->vendor;
    s << d->category;
    s << d->copyright;
    s << d->license;
    s << d->url;
    s << d->dependencies;
    s << d->libraryPath;
    return s;
}
}
