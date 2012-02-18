#ifndef HELLOWORLPLUGIN_H
#define HELLOWORLPLUGIN_H

#include <extensionsystem/iplugin.h>

class HelloWorldPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT

public:
    explicit HelloWorldPlugin();

    bool initialize(const QVariantMap &options);
};

#endif // HELLOWORLPLUGIN_H
