#include "testplugin2.h"

#include <QDebug>

TestPlugin2::TestPlugin2()
    : IPlugin()
{
}

QString TestPlugin2::property(Properties property)
{
    switch(property) {
    case IPlugin::Name: return "Second Test Plugin";
    case IPlugin::Version: return "1.0.1";
    case IPlugin::Vendor: return "arch";
    case IPlugin::Category: return "Test";
    default: return "";
    }
}

Q_EXPORT_PLUGIN(TestPlugin2)
