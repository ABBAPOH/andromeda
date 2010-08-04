#include "pluginspec.h"
#include "pluginspec_p.h"

#include <QtCore/QRegExp>
#include <QtCore/QPluginLoader>
#include <QDebug>

#include "iplugin_p.h"

using namespace ExtensionSystem;


// ============= PluginDependency =============
/*!
    \fn PluginDependency::PluginDependency(const QString &name, const QString &version)
    \brief Constructs PluginDependency class with given \a name and \a version.
*/
PluginDependency::PluginDependency(const QString &name, const QString &version)
{
    m_name = name;
    m_version = version;
}

/*!
    \fn bool PluginDependency::operator==(const PluginDependency &other)
    \brief Comparator for PluginDependency.
*/
bool PluginDependency::operator==(const PluginDependency &other)
{
    return m_name == other.m_name && m_version == other.m_version;
}

// ============= PluginSpec =============
/*!
    \fn PluginSpec::PluginSpec()
    \internal
*/
PluginSpec::PluginSpec() :
        d_ptr(new PluginSpecPrivate)
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
    \fn PluginSpec::PluginSpec(IPlugin * plugin)
    \internal
    Constructs static PluginSpec using data received from IPlugin::property function.
*/
PluginSpec::PluginSpec(IPlugin * plugin) :
        d_ptr(new PluginSpecPrivate)
{
    Q_D(PluginSpec);

    plugin->d_ptr->spec = this;
    d->init(plugin);
    d->isStatic = true;
}

PluginSpec::PluginSpec(const QString & path) :
        d_ptr(new PluginSpecPrivate)
{
    Q_D(PluginSpec);
    d->libraryPath = path;
    if (!d->loadLibrary()) {
        return;
    }
    d->isStatic = false;
    d->init(d->plugin);
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
    \fn QString PluginSpec::version() const
    \brief Returns version of the plugin.
    Version should be in format "Major.Minor.Path".
*/
QString PluginSpec::version() const
{
    return d_func()->version;
}

/*!
    \fn QString PluginSpec::compatibilityVersion() const
    \brief Returns compatibility version of the plugin.
*/
QString PluginSpec::compatibilityVersion() const
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
    \brief Returns list of dependent PluginSpecs.
*/
QList<PluginSpec*> PluginSpec::dependencySpecs() const
{
    return d_func()->dependencySpecs;
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
    \fn bool PluginSpec::isStatic() const
    \brief Returns true if plugin is linked statically.
*/
bool PluginSpec::isStatic() const
{
    return d_func()->isStatic;
}

void PluginSpec::setEnabled(bool enabled)
{
    Q_D(PluginSpec);
    if (enabled == d->enabled)
        return;

    if (enabled) {
        if (d->loadLibrary()) {
            d->enabled = true;
            emit enabledChanged(enabled);
        }
    } else {
        if (d->unloadLibrary()) {
            d->enabled = false;
            emit enabledChanged(enabled);
        }
    }
}

bool PluginSpec::enabled() const
{
    return d_func()->enabled;
}

QString PluginSpec::errorString()
{
    return d_func()->errorString;
}

bool PluginSpec::hasError()
{
    return d_func()->hasError;
}

IPlugin *PluginSpec::plugin() const
{
    return d_func()->plugin;
}

/*!
    \fn bool PluginSpec::provides(const QString &pluginName, const QString &pluginVersion) const
    Returns if this plugin can be used to fill in a dependency of the given \a pluginName and \a version.
    \sa PluginSpec::dependencies()
*/
bool PluginSpec::provides(const QString &pluginName, const QString &pluginVersion) const
{
    if (QString::compare(pluginName, name(), Qt::CaseInsensitive) != 0)
         return false;
    return (PluginSpecPrivate::compareVersion(version(), pluginVersion) >= 0) &&
           (PluginSpecPrivate::compareVersion(compatibilityVersion(), pluginVersion) <= 0);
}

// ============= PluginSpecPrivate =============
PluginSpecPrivate::PluginSpecPrivate():
        isStatic(false),
        hasError(false)
{
}

void PluginSpecPrivate::init(IPlugin * plugin)
{
    this->plugin = plugin;
    name = plugin->property(IPlugin::Name);
    version = plugin->property(IPlugin::Version);
    compatibilityVersion = plugin->property(IPlugin::CompatibilityVersion);
    vendor = plugin->property(IPlugin::Vendor);
    category = plugin->property(IPlugin::Category);
    copyright = plugin->property(IPlugin::Copyright);
    license = plugin->property(IPlugin::License);
    description = plugin->property(IPlugin::Description);
    url = plugin->property(IPlugin::Url);
    dependencies = plugin->dependencies();

    enabled = true;
}

bool PluginSpecPrivate::loadLibrary()
{
    QPluginLoader loader(libraryPath);
    QObject * object = loader.instance();
    if (!object) {
        hasError = true;
        errorString = QObject::tr("PluginSpec", "Can't load plugin: ") + loader.errorString();
        qWarning () << "Can't load plugin: " + loader.errorString();
        return false;
    }

    IPlugin *plugin = qobject_cast<IPlugin *>(object);
    if (!plugin) {
        hasError = true;
        errorString = QObject::tr("PluginSpec", "Can't load plugin: not a valid plugin" );
        qWarning () << "not a plugin";
        return false;
    }

    this->plugin = plugin;
    return true;
}

bool PluginSpecPrivate::unloadLibrary()
{
    return true;
}

bool PluginSpecPrivate::resolveDependencies(const QList<PluginSpec*> &specs)
{
    PluginSpec * dependencySpec = 0;
    foreach (PluginDependency dep, dependencies) {
        dependencySpec = 0;
        foreach (PluginSpec *spec, specs) {
            if (spec->provides(dep.name(), dep.version())) {
                dependencySpec = spec;
                break;
            }
        }
        if (dependencySpec == 0) {
            hasError = true;
            errorString.append(QObject::tr("PluginSpec", "Can't resolve dependency '%1(%2)'")
                                .arg(dep.name()).arg(dep.version()));
//            break; // don't break to find ALL unresolved dependencies
        } else {
            dependencySpecs.append(dependencySpec);
        }
    }

    if (hasError)
        return false;
    return true;
}

// from Qt Creator
int PluginSpecPrivate::compareVersion(const QString &version1, const QString &version2)
{
    static QRegExp regExp = QRegExp("([0-9]+)(?:\\.([0-9]+))?(?:\\.([0-9]+))?"); // mathes to Major.Minor.Patch version
    QRegExp gerExp1 = regExp;
    QRegExp gerExp2 = regExp;
    if ( !gerExp1.exactMatch(version1) || !gerExp2.exactMatch(version2) )
        return 0;
    int number1;
    int number2;
    for (int i = 0; i < 3; ++i) {
        number1 = gerExp1.cap(i+1).toInt();
        number2 = gerExp2.cap(i+1).toInt();
        if (number1 < number2)
            return -1;
        if (number1 > number2)
            return 1;
    }
    return 0;
}


