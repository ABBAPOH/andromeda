#ifndef HELLOWORLPLUGIN_H
#define HELLOWORLPLUGIN_H

#include <extensionsystem/iplugin.h>

class HelloWorlPluginImpl : public ExtensionSystem::IPlugin
{
    Q_OBJECT

public:
    explicit HelloWorlPluginImpl();

    bool initialize(const QVariantMap &options);
};

#endif // HELLOWORLPLUGIN_H
