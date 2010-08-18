#include "testplugin3.h"

#include <QDebug>

TestPlugin3::TestPlugin3()
    : IPlugin()
{
}

bool TestPlugin3::initialize()
{
    qDebug("TestPlugin3::initialize");
    return true;
}

QString TestPlugin3::property(Properties property)
{
    switch(property) {
    case IPlugin::Name: return "Third Test Plugin";
    case IPlugin::Version: return "2.0.1";
    case IPlugin::Vendor: return "arch";
    case IPlugin::Category: return "Test";
    default: return "";
    }
}

Q_EXPORT_PLUGIN(TestPlugin3)
