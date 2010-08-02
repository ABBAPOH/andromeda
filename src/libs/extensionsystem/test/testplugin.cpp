#include "testplugin.h"

TestPlugin::TestPlugin()
    : IPlugin()
{
}

QString TestPlugin::property(Properties property)
{
    switch(property) {
    case IPlugin::Name: return "Test Plugin";
    case IPlugin::Version: return "0.0.1";
    case IPlugin::Vendor: return "arch";
    default: return "";
    }
}

QList<ExtensionSystem::PluginDependency> TestPlugin::dependencies()
{
    return QList<ExtensionSystem::PluginDependency>() << ExtensionSystem::PluginDependency("Third Test Plugin", "2.0.2");
}

Q_EXPORT_PLUGIN(TestPlugin)
