#ifndef TESTPLUGIN2_H
#define TESTPLUGIN2_H

#include <QtPlugin>
#include <ExtensionSystem/IPlugin>

class TestPlugin2 : public ExtensionSystem::IPlugin
{
    Q_OBJECT

public:
    TestPlugin2();
    bool initialize();
};

#endif // TESTPLUGIN2_H
