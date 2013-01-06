#ifndef HELLOWORLPLUGIN_H
#define HELLOWORLPLUGIN_H

#include <extensionsystem/iplugin.h>

namespace HelloWorld {

class HelloWorldPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_DISABLE_COPY(HelloWorldPlugin)
public:
    HelloWorldPlugin();

    bool initialize();
};

} // namespace HelloWorld

#endif // HELLOWORLPLUGIN_H
