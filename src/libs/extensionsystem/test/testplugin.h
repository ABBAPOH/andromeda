#ifndef TESTPLUGIN_H
#define TESTPLUGIN_H

#include <QtPlugin>
#include <iplugin.h>

class EXTENSIONSYSTEM_EXPORT TestPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
public:
    TestPlugin();
    virtual bool initialize() { return true; }
    virtual void extensionsInitialized() {}
    virtual QString property(Properties property);
signals:
    void test();
};

#endif // TESTPLUGIN_H
