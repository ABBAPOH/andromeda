#ifndef BINEDITORPLUGIN_H
#define BINEDITORPLUGIN_H

#include <extensionsystem/iplugin.h>

class BinEditorPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT

public:
    explicit BinEditorPlugin();

    bool initialize();
};

#endif // BINEDITORPLUGIN_H
