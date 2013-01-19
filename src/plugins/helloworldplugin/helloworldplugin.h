#ifndef HELLOWORLPLUGIN_H
#define HELLOWORLPLUGIN_H

#include <ExtensionSystem/IPlugin>

namespace HelloWorld {

class HelloWorldPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_DISABLE_COPY(HelloWorldPlugin)
public:
    HelloWorldPlugin();
    ~HelloWorldPlugin();

    bool initialize();
};

} // namespace HelloWorld

#endif // HELLOWORLPLUGIN_H
