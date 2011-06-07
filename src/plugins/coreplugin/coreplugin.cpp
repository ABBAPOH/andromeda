#include "coreplugin.h"

#include <QtCore/QtPlugin>

#include "core.h"
#include "perspectivemanager.h"
#include "settingsdialog_p.h"

using namespace CorePlugin;

CorePluginImpl::CorePluginImpl() :
    IPlugin()
{
}

bool CorePluginImpl::initialize()
{
    addObject(new Core);
    addObject(new PerspectiveManager);
    addObject(new CategoryModel);

    return true;
}

void CorePluginImpl::shutdown()
{
}

Q_EXPORT_PLUGIN(CorePluginImpl)
