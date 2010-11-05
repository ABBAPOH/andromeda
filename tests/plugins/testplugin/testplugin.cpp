#include "testplugin.h"

TestPlugin::TestPlugin()
    : IPlugin()
{
}

bool TestPlugin::initialize()
{
    qDebug("TestPlugin::initialize");
    return true;
}

QList<ExtensionSystem::PluginDependency> TestPlugin::dependencies()
{
    return QList<ExtensionSystem::PluginDependency>() << ExtensionSystem::PluginDependency("Third Test Plugin", "2.0.1");
}

Q_EXPORT_PLUGIN(TestPlugin)
