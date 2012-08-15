#include "texteditorplugin.h"
#include "texteditor.h"

#include <QtCore/QtPlugin>
#include <guisystem/editormanager.h>

using namespace GuiSystem;
using namespace TEXTEditor;

TextEditorPlugin::TextEditorPlugin() :
    ExtensionSystem::IPlugin()
{
}

bool TextEditorPlugin::initialize()
{
    TextEditorFactory *f = new TextEditorFactory(this);
    EditorManager::instance()->addFactory(f);

    return true;
}

Q_EXPORT_PLUGIN(TextEditorPlugin)
