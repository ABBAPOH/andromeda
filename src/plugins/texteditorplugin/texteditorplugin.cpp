#include "texteditorplugin.h"

#include <QtCore/QtPlugin>
#include <GuiSystem/DocumentManager>
#include <GuiSystem/EditorManager>

#include "plaintextdocument.h"
#include "plaintexteditor.h"

using namespace GuiSystem;
using namespace TextEditor;

TextEditorPlugin::TextEditorPlugin() :
    ExtensionSystem::IPlugin()
{
}

bool TextEditorPlugin::initialize()
{
    DocumentManager::instance()->addFactory(new PlainTextDocumentFactory(this));
    EditorManager::instance()->addFactory(new PlainTextEditorFactory(this));

    return true;
}

Q_EXPORT_PLUGIN(TextEditorPlugin)
