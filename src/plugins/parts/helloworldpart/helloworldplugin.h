#ifndef HELLOWORLPLUGIN_H
#define HELLOWORLPLUGIN_H

#include <ExtensionSystem/IPlugin>

namespace HelloWorld {

class HelloWorldPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_DISABLE_COPY(HelloWorldPlugin)

#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "com.arch.Andromeda.HelloWorldPlugin")
#endif
public:
    HelloWorldPlugin();
    ~HelloWorldPlugin();

    bool initialize();
};

} // namespace HelloWorld

#endif // HELLOWORLPLUGIN_H
