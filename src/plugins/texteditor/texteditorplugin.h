#ifndef TEXTEDITORPLUGIN_H
#define TEXTEDITORPLUGIN_H

#include <extensionsystem/iplugin.h>

class TextEditorPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT

public:
    explicit TextEditorPlugin();

    bool initialize();
};

#endif // TEXTEDITORPLUGIN_H
