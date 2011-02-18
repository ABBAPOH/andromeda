#include "coreplugin.h"

#include <QtCore/QtPlugin>

CorePluginImpl::CorePluginImpl() :
    IPlugin(),
    core(0)
{
}
#include <QDebug>

bool CorePluginImpl::initialize()
{
    core = new CorePlugin::Core();
    qDebug() << "lol" << this->metaObject()->d.stringdata;

    return true;
}

void CorePluginImpl::shutdown()
{
    delete core;
}

Q_EXPORT_PLUGIN(CorePluginImpl)
