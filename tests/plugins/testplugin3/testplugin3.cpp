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

Q_EXPORT_PLUGIN(TestPlugin3)
