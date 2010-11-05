#include "coreplugin.h"

#include <QtCore/QtPlugin>

CorePlugin::CorePlugin() :
    IPlugin(),
    core(0)
{
}

bool CorePlugin::initialize()
{
    core = new Core::Core();

    qDebug("Core is alive");
    return true;
}

void CorePlugin::shutdown()
{
    delete core;

    qDebug("Core is dead");
}

Q_EXPORT_PLUGIN(CorePlugin)
