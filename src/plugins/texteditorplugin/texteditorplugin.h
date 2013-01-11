#ifndef TEXTEDITORPLUGIN_H
#define TEXTEDITORPLUGIN_H

#include <ExtensionSystem/IPlugin>

namespace TextEditor {

class TextEditorPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT

public:
    explicit TextEditorPlugin();

    bool initialize();
};

} // namespace TextEditor

#endif // TEXTEDITORPLUGIN_H
