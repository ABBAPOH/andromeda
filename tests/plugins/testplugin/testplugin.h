#ifndef TESTPLUGIN_H
#define TESTPLUGIN_H

#include <QtPlugin>
#include <ExtensionSystem/IPlugin>

class TestPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT

public:
    TestPlugin();
    bool initialize();
};

#endif // TESTPLUGIN_H
