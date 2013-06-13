#ifndef TEXTEDITORPLUGIN_H
#define TEXTEDITORPLUGIN_H

#include <ExtensionSystem/IPlugin>

namespace TextEditor {

class TextEditorPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT

#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "com.arch.Andromeda.TextEditorPlugin")
#endif
public:
    explicit TextEditorPlugin();

    bool initialize();
};

} // namespace TextEditor

#endif // TEXTEDITORPLUGIN_H
