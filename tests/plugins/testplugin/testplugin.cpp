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

Q_EXPORT_PLUGIN(TestPlugin)
