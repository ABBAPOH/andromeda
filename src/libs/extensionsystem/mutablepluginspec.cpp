#include "mutablepluginspec.h"
#include "pluginspec_p.h"

using namespace ExtensionSystem;

/*!
    \class ExtensionSystem::MutablePluginSpec

    \brief MutablePluginSpec is a class for editing PluginSpecs.

    This class is used in PluginEditor which allows to modify spec files using
    graphical user interface instead of direct text editing.

    This is a simple wrapper that adds setters for PluginSpec's properties.
*/

/*!
    Constructs MutablePluginSpec with the given \a parent;
*/
MutablePluginSpec::MutablePluginSpec(QObject *parent) :
    PluginSpec(parent)
{
}

/*!
    Setter for PluginSpec::name property.
*/
void MutablePluginSpec::setName(const QString &name)
{
    d_func()->name = name;
}

/*!
    Setter for PluginSpec::version member.
*/
void MutablePluginSpec::setVersion(const ExtensionSystem::Version &version)
{
    d_func()->version = version;
}

/*!
    Setter for PluginSpec::compatibilityVersion member.
*/
void MutablePluginSpec::setCompatibilityVersion(const ExtensionSystem::Version &version)
{
    d_func()->compatibilityVersion =version;
}

/*!
    Setter for PluginSpec::vendor property.
*/
void MutablePluginSpec::setVendor(const QString &vendor)
{
    d_func()->vendor = vendor;
}

/*!
    Setter for PluginSpec::category property.
*/
void MutablePluginSpec::setCategory(const QString &category)
{
    d_func()->category = category;
}

/*!
    Setter for PluginSpec::copyright property.
*/
void MutablePluginSpec::setCopyright(const QString &copyright)
{
    d_func()->copyright = copyright;
}

/*!
    Setter for PluginSpec::license property.
*/
void MutablePluginSpec::setLicense(const QString &license)
{
    d_func()->license = license;
}

/*!
    Setter for PluginSpec::description property.
*/
void MutablePluginSpec::setDescription(const QString &description)
{
    d_func()->description = description;
}

/*!
    Setter for PluginSpec::url property.
*/
void MutablePluginSpec::setUrl(const QString &url)
{
    d_func()->url = url;
}

/*!
    Setter for PluginSpec::dependencies member.
*/
void MutablePluginSpec::setDependencies(const QList<ExtensionSystem::PluginDependency> &dependencies)
{
    d_func()->dependencies = dependencies;
}
