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

    return true;
}

void CorePlugin::shutdown()
{
    delete core;
}

Q_EXPORT_PLUGIN(CorePlugin)
