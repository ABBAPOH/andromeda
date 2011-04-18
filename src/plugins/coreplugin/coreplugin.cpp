#include "coreplugin.h"

#include <QtCore/QtPlugin>
#include <EditorManager>

#include "core.h"
#include "perspectivemanager.h"

using namespace CorePlugin;

CorePluginImpl::CorePluginImpl() :
    IPlugin()
{
}

bool CorePluginImpl::initialize()
{
    addObject(new Core);
    addObject(new EditorManager);
    addObject(new PerspectiveManager);

    return true;
}

void CorePluginImpl::shutdown()
{
}

Q_EXPORT_PLUGIN(CorePluginImpl)
