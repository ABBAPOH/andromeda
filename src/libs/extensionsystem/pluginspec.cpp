#include "pluginspec.h"
#include "pluginspec_p.h"

#include <QtCore/QRegExp>
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

/*!
    \fn PluginSpec::PluginSpec(IPlugin * plugin, const QString &path)
    \internal
    Constructs dynamic PluginSpec using data received from IPlugin::property function.
*/
PluginSpec::PluginSpec(IPlugin * plugin, const QString &path) :
        d_ptr(new PluginSpecPrivate)
{
    Q_D(PluginSpec);

    plugin->d_ptr->spec = this;
    d->init(plugin);
    d->isStatic = false;
    d->libraryPath = path;
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
    vendor = plugin->property(IPlugin::Vendor);
    dependencies = plugin->dependencies();
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


