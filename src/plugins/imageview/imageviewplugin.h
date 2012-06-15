#ifndef HELLOWORLPLUGIN_H
#define HELLOWORLPLUGIN_H

#include <extensionsystem/iplugin.h>

class ImageViewPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT

public:
    explicit ImageViewPlugin();

    bool initialize();
};

#endif // HELLOWORLPLUGIN_H
