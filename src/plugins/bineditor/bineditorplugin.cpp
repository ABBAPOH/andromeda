#include "bineditorplugin.h"
#include "bineditor.h"

#include <QtCore/QtPlugin>
#include <GuiSystem/EditorManager>

using namespace GuiSystem;
using namespace BINEditor;

BinEditorPlugin::BinEditorPlugin() :
    ExtensionSystem::IPlugin()
{
}

bool BinEditorPlugin::initialize()
{
    BinEditorFactory *f = new BinEditorFactory(this);
    EditorManager::instance()->addFactory(f);

    return true;
}

Q_EXPORT_PLUGIN(BinEditorPlugin)
