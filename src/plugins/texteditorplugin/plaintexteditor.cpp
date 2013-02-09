#include "plaintexteditor.h"

#include <QtGui/QVBoxLayout>
#include <QtGui/QToolBar>
#include <QtGui/QVBoxLayout>

#include "plaintextdocument.h"
#include "plaintextedit.h"

using namespace GuiSystem;
using namespace TextEditor;

/*!
    \class PlainTextEditor
*/

/*!
    Creates PlainTextEditor with the given \a parent.
*/
PlainTextEditor::PlainTextEditor(QWidget *parent) :
    AbstractEditor(*new PlainTextDocument, parent)
{
    document()->setParent(this);
    setupUi();

    PlainTextDocument *doc = static_cast<PlainTextDocument *>(document());
    m_editor->setDocument(doc->textDocument());
}

void PlainTextEditor::setDocument(AbstractDocument *document)
{
    PlainTextDocument *textEditorDocument = qobject_cast<PlainTextDocument*>(document);
    if (!textEditorDocument)
        return;

    m_editor->setDocument(textEditorDocument->textDocument());

    AbstractEditor::setDocument(document);
}

void PlainTextEditor::setupUi()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    m_editor = new PlainTextEdit(this);
    layout->addWidget(m_editor);
}

/*!
    \class PlainTextEditorFactory
*/

/*!
    Creates PlainTextEditorFactory with the given \a parent.
*/
PlainTextEditorFactory::PlainTextEditorFactory(QObject *parent) :
    AbstractEditorFactory("texteditor", parent)
{
}

/*!
    \reimp
*/
AbstractEditor * PlainTextEditorFactory::createEditor(QWidget *parent)
{
    return new PlainTextEditor(parent);
}
