#include "coreplugin.h"

#include <QtCore/QtPlugin>

CorePluginImpl::CorePluginImpl() :
    IPlugin(),
    core(0)
{
}

bool CorePluginImpl::initialize()
{
    core = new CorePlugin::Core();

    return true;
}

void CorePluginImpl::shutdown()
{
    delete core;
}

Q_EXPORT_PLUGIN(CorePluginImpl)
