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

QString CorePlugin::property(Properties property)
{
    switch(property) {
    case IPlugin::Name: return "Core Plugin";
    case IPlugin::Version: return "0.0.1";
    case IPlugin::CompatibilityVersion: return "0.0.0";
    case IPlugin::Vendor: return "arch";
    case IPlugin::Category: return "Core";
    case IPlugin::Url: return "";
    case IPlugin::Description: return "This is main Andromeda plugin";
    case IPlugin::Copyright: return "arch (c) 2010";
    case IPlugin::License: return "GNU Lesser General Public License";
    default: return "";
    }
}

void CorePlugin::shutdown()
{
    delete core;
}


Q_EXPORT_PLUGIN(CorePlugin)
