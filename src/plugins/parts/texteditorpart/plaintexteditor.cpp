#include "plaintexteditor.h"

#if QT_VERSION >= 0x050000
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#else
#include <QtGui/QToolBar>
#include <QtGui/QVBoxLayout>
#endif

#include "plaintextdocument.h"
#include "plaintextedit.h"
#include "textfind.h"

using namespace Parts;
using namespace TextEditor;

/*!
    \class PlainTextEditor
*/

/*!
    Creates PlainTextEditor with the given \a parent.
*/
PlainTextEditor::PlainTextEditor(QWidget *parent) :
    AbstractEditor(*new PlainTextDocument, parent),
    m_find(new TextFind(this))
{
    document()->setParent(this);
    setupUi();

    connect(m_editor, SIGNAL(cursorPositionChanged()), SLOT(onCursorChanged()));
    connect(m_find, SIGNAL(cursorChanged()), SLOT(onFindCursorChanged()));

    PlainTextDocument *doc = static_cast<PlainTextDocument *>(document());
    m_editor->setDocument(doc->textDocument());
    m_find->setDocument(doc->textDocument());
}

void PlainTextEditor::setDocument(AbstractDocument *document)
{
    PlainTextDocument *textEditorDocument = qobject_cast<PlainTextDocument*>(document);
    if (!textEditorDocument)
        return;

    m_editor->setDocument(textEditorDocument->textDocument());
    m_find->setDocument(textEditorDocument->textDocument());

    AbstractEditor::setDocument(document);
}

IFind * PlainTextEditor::find() const
{
    return m_find;
}

void PlainTextEditor::onCursorChanged()
{
    bool bs = m_find->blockSignals(true); // prevent unneseccary recursion (small speedup)
    m_find->setTextCursor(m_editor->textCursor());
    m_find->blockSignals(bs);
}

void PlainTextEditor::onFindCursorChanged()
{
    m_editor->setTextCursor(m_find->textCursor());
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
