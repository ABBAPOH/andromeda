#ifndef TESTPLUGIN3_H
#define TESTPLUGIN3_H

#include <QtPlugin>
#include <iplugin.h>

class TestPlugin3 : public ExtensionSystem::IPlugin
{
    Q_OBJECT

public:
    TestPlugin3();
    bool initialize(const QVariantMap &);
};

#endif // TESTPLUGIN3_H
