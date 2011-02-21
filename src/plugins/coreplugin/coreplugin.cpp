#include "coreplugin.h"

#include <QtCore/QtPlugin>
#include <EditorManager>

using namespace CorePlugin;

CorePluginImpl::CorePluginImpl() :
    IPlugin(),
    core(0)
{
}

bool CorePluginImpl::initialize()
{
    addObject(new Core);
    addObject(new EditorManager);

    return true;
}

void CorePluginImpl::shutdown()
{
}

Q_EXPORT_PLUGIN(CorePluginImpl)
