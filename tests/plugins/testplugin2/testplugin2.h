#ifndef TESTPLUGIN2_H
#define TESTPLUGIN2_H

#include <QtPlugin>
#include <iplugin.h>

class TestPlugin2 : public ExtensionSystem::IPlugin
{
    Q_OBJECT

public:
    TestPlugin2();
    bool initialize(const QVariantMap &);
};

#endif // TESTPLUGIN2_H
