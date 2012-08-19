#ifndef TEXTEDITORFILE_H
#define TEXTEDITORFILE_H

#include "texteditor.h"
#include <guisystem/ifile.h>

namespace TEXTEditor {

class TextEditorFile : public GuiSystem::IFile
{
public:
    explicit TextEditorFile(TextEditor *parent) : IFile(parent), editor(parent) {}

    bool isModified() const { return editor->m_editor->document()->isModified(); }
    void setModified(bool modified) { editor->m_editor->document()->setModified(modified); }

    bool isReadOnly() const { return false; }

    void open(const QUrl &url);
    void save(const QUrl &url);
    
private:
    TextEditor *editor;
    
};

}
#endif // TEXTEDITORFILE_H
