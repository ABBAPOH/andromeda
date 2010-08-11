#ifndef TESTPLUGIN_H
#define TESTPLUGIN_H

#include <QtPlugin>
#include <iplugin.h>

class TestPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
public:
    TestPlugin();
    virtual bool initialize();
    virtual QString property(Properties property);
    virtual QList<ExtensionSystem::PluginDependency> dependencies();

};

#endif // TESTPLUGIN_H
