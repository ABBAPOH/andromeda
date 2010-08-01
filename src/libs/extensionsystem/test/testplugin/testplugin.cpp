#include "testplugin.h"

#include <QDebug>

TestPlugin::TestPlugin()
    : IPlugin()
{
    qDebug() << "TestPlugin::TestPlugin" << this->inherits("IPlugin")
            << qobject_cast<IPlugin*>((QObject*)this);
    IPlugin *p = this;
    qDebug() << p;
    p->shutdown();
#if defined(EXTENSIONSYSTEM_LIBRARY)
    qDebug() << "bad";
#endif
}

QVariant TestPlugin::property(IPlugin::Properties property)
{
    switch(property) {
    case IPlugin::Name: return "Test Plugin";
    case IPlugin::Version: return "0.0.1";
    case IPlugin::Vendor: return "arch";
    default: return "";
    }
}

Q_EXPORT_PLUGIN(TestPlugin)
