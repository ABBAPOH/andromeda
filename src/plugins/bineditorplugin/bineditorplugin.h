#ifndef BINEDITORPLUGIN_H
#define BINEDITORPLUGIN_H

#include <ExtensionSystem/IPlugin>

namespace BINEditor {

class BinEditorPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT

public:
    explicit BinEditorPlugin();

    bool initialize();
};

} // namespace BINEditor

#endif // BINEDITORPLUGIN_H
