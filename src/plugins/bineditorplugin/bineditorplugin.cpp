#include "bineditorplugin.h"
#include "bineditor.h"

#include <QtCore/QtPlugin>
#include <GuiSystem/DocumentManager>
#include <GuiSystem/EditorManager>

#include "bineditordocument.h"

using namespace GuiSystem;
using namespace BINEditor;

BinEditorPlugin::BinEditorPlugin() :
    ExtensionSystem::IPlugin()
{
}

bool BinEditorPlugin::initialize()
{
    DocumentManager::instance()->addFactory(new BinEditorDocumentFactory(this));
    EditorManager::instance()->addFactory(new BinEditorFactory(this));

    return true;
}

Q_EXPORT_PLUGIN(BinEditorPlugin)
