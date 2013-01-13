#ifndef TESTPLUGIN3_H
#define TESTPLUGIN3_H

#include <QtPlugin>
#include <ExtensionSystem/IPlugin>

class TestPlugin3 : public ExtensionSystem::IPlugin
{
    Q_OBJECT

public:
    TestPlugin3();
    bool initialize();
};

#endif // TESTPLUGIN3_H
