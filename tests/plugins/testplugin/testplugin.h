#ifndef TESTPLUGIN_H
#define TESTPLUGIN_H

#include <QtPlugin>
#include <iplugin.h>

class TestPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT

public:
    TestPlugin();
    bool initialize(const QVariantMap &);
};

#endif // TESTPLUGIN_H
