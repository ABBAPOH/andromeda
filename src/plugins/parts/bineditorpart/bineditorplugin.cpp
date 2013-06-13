#include "bineditorplugin.h"
#include "bineditor.h"

#include <QtCore/QtPlugin>
#include <Parts/DocumentManager>
#include <Parts/EditorManager>

#include "bineditordocument.h"

using namespace Parts;
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

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN(BinEditorPlugin)
#endif
