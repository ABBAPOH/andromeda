#ifndef TEXTEDITORDOCUMENT_H
#define TEXTEDITORDOCUMENT_H

#include "texteditor_global.h"

#include <guisystem/filedocument.h>
#include "texteditor.h"

namespace TextEditor {

class TEXTEDITOR_EXPORT TextEditorDocument : public GuiSystem::FileDocument
{
    Q_OBJECT
    Q_DISABLE_COPY(TextEditorDocument)
public:
    explicit TextEditorDocument(QObject *parent = 0);
    void setEditor(TextEditorEditor *editor) { this->editor = editor; }

protected:
    bool read(QIODevice *device, const QString &fileName);
    bool write(QIODevice *device, const QString &fileName);


private:
    TextEditorEditor *editor;
};

} // namespace TextEditor

#endif // TEXTEDITORDOCUMENT_H
