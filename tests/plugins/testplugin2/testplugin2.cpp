#include "testplugin2.h"

#include <QDebug>

TestPlugin2::TestPlugin2()
    : IPlugin()
{
}

bool TestPlugin2::initialize()
{
    qDebug("TestPlugin2::initialize");
    return true;
}

Q_EXPORT_PLUGIN(TestPlugin2)
