#ifndef HELLOWORLPLUGIN_H
#define HELLOWORLPLUGIN_H

#include <extensionsystem/iplugin.h>

namespace HelloWorld {

class HelloWorldPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT

public:
    explicit HelloWorldPlugin();

    bool initialize();
};

} // namespace HelloWorld

#endif // HELLOWORLPLUGIN_H
