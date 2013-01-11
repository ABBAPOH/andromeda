#ifndef PLAINTEXTEDITOR_H
#define PLAINTEXTEDITOR_H

#include "texteditor_global.h"

#include <GuiSystem/AbstractEditor>
#include <GuiSystem/AbstractEditorFactory>

class QVBoxLayout;
class QToolBar;

namespace TextEditor {

class PlainTextEdit;

class PlainTextDocument;
class TEXTEDITOR_EXPORT PlainTextEditor : public GuiSystem::AbstractEditor
{
    Q_OBJECT
    Q_DISABLE_COPY(PlainTextEditor)

public:
    explicit PlainTextEditor(QWidget *parent = 0);

    void setDocument(GuiSystem::AbstractDocument *document);

private:
    void setupUi();

    PlainTextEdit *m_editor;
    QToolBar *m_toolBar;
    QString m_currentFile;
};

class PlainTextEditorFactory : public GuiSystem::AbstractEditorFactory
{
    Q_OBJECT
    Q_DISABLE_COPY(PlainTextEditorFactory)

public:
    explicit PlainTextEditorFactory(QObject *parent = 0);

protected:
    GuiSystem::AbstractEditor *createEditor(QWidget *parent);
};

} // namespace TextEditor

#endif // PLAINTEXTEDITOR_H
