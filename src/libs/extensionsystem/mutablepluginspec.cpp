#include "mutablepluginspec.h"
#include "pluginspec_p.h"

using namespace ExtensionSystem;

MutablePluginSpec::MutablePluginSpec(QObject *parent) :
    PluginSpec(parent)
{
}

void MutablePluginSpec::setName(const QString &name)
{
    d_func()->name = name;
}

void MutablePluginSpec::setVersion(const ExtensionSystem::Version &version)
{
    d_func()->version = version;
}

void MutablePluginSpec::setCompatibilityVersion(const ExtensionSystem::Version &version)
{
    d_func()->compatibilityVersion =version;
}

void MutablePluginSpec::setVendor(const QString &vendor)
{
    d_func()->vendor = vendor;
}

void MutablePluginSpec::setCategory(const QString &category)
{
    d_func()->category = category;
}

void MutablePluginSpec::setCopyright(const QString &copyright)
{
    d_func()->copyright = copyright;
}

void MutablePluginSpec::setLicense(const QString &license)
{
    d_func()->license = license;
}

void MutablePluginSpec::setDescription(const QString &description)
{
    d_func()->description = description;
}

void MutablePluginSpec::setUrl(const QString &url)
{
    d_func()->url = url;
}

void MutablePluginSpec::setDependencies(const QList<ExtensionSystem::PluginDependency> &dependencies)
{
    d_func()->dependencies = dependencies;
}
