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

Q_EXPORT_PLUGIN(TestPlugin)
