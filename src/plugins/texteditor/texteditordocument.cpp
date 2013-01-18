#include "texteditordocument.h"

#include <QFileInfo>
#include <QFileIconProvider>

using namespace GuiSystem;
using namespace TextEditor;

TextEditorDocument::TextEditorDocument(QObject *parent) :
    FileDocument(parent)
{
    setIcon(QIcon(":/icons/texteditor.png"));
}

bool TextEditorDocument::read(QIODevice *device, const QString &fileName)
{
    return editor->open(device);
}

bool TextEditorDocument::write(QIODevice *device, const QString &fileName)
{
    editor->save(device, QFileInfo(fileName).suffix().toUtf8());
    return true;
}
