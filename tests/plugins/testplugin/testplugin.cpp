#include "testplugin.h"

TestPlugin::TestPlugin()
    : IPlugin()
{
}

bool TestPlugin::initialize(const QVariantMap &)
{
    qDebug("TestPlugin::initialize");
    return true;
}

Q_EXPORT_PLUGIN(TestPlugin)
