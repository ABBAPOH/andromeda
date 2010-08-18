#ifndef TESTPLUGIN3_H
#define TESTPLUGIN3_H

#include <QtPlugin>
#include <iplugin.h>

class TestPlugin3 : public ExtensionSystem::IPlugin
{
    Q_OBJECT
public:
    TestPlugin3();
    virtual bool initialize();
    virtual QString property(Properties property);
};

#endif // TESTPLUGIN3_H
