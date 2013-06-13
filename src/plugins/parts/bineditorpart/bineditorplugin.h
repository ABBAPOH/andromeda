#ifndef BINEDITORPLUGIN_H
#define BINEDITORPLUGIN_H

#include <ExtensionSystem/IPlugin>

namespace BINEditor {

class BinEditorPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT

#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "com.arch.Andromeda.BinEditorPlugin")
#endif
public:
    explicit BinEditorPlugin();

    bool initialize();
};

} // namespace BINEditor

#endif // BINEDITORPLUGIN_H
