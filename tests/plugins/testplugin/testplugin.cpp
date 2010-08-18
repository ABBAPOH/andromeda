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

QString TestPlugin::property(Properties property)
{
    switch(property) {
    case IPlugin::Name: return "Test Plugin";
    case IPlugin::Version: return "0.0.1";
    case IPlugin::CompatibilityVersion: return "0.0.0";
    case IPlugin::Vendor: return "arch";
    case IPlugin::Category: return "Test";
    case IPlugin::Url: return "http://www.ya.ru";
    case IPlugin::Description: return "This plugin is just a test plugin. It is used only for testing.\
                Trying to use not for testing is very bad. Nobody knows what can heppen to you:)";
    case IPlugin::Copyright: return "arch inc. (c) 2010";
    case IPlugin::License: return "GNU Lesser General Public License";
    default: return "";
    }
}

QList<ExtensionSystem::PluginDependency> TestPlugin::dependencies()
{
    return QList<ExtensionSystem::PluginDependency>() << ExtensionSystem::PluginDependency("Third Test Plugin", "2.0.1");
}

Q_EXPORT_PLUGIN(TestPlugin)
