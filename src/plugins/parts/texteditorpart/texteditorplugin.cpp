#include "texteditorplugin.h"

#include <QtCore/QtPlugin>
#include <Parts/DocumentManager>
#include <Parts/EditorManager>

#include "plaintextdocument.h"
#include "plaintexteditor.h"

using namespace Parts;
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

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN(TextEditorPlugin)
#endif
