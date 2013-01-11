#include "texteditor.h"

#include <QtCore/QUrl>
#include <QtGui/QVBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QResizeEvent>

#include "texteditordocument.h"

#include <QTextStream>
#include <QTextDocumentWriter>

using namespace GuiSystem;
using namespace TextEditor;

TextEditorEditor::TextEditorEditor(QWidget *parent) :
    AbstractEditor(*new TextEditorDocument, parent)
{
    document()->setParent(this);
    setupUi();

    connect(m_editor->document(), SIGNAL(modificationChanged(bool)), document(), SLOT(setModified(bool)));

    TextEditorDocument *doc = static_cast<TextEditorDocument *>(document());
    doc->setEditor(this);
}

bool TextEditorEditor::open(QIODevice *device)
{
    m_editor->clear();

    m_editor->setPlainText(device->readAll());

    m_editor->document()->setModified(false);

    return true;
}

bool TextEditorEditor::save(QIODevice *device, const QByteArray &format)
{
    QTextDocumentWriter writer(device, format);

    writer.write(m_editor->document());

    m_editor->document()->setModified(false);

    return true;
}

void TextEditorEditor::setDocument(AbstractDocument *document)
{
    TextEditorDocument *textDocument = qobject_cast<TextEditorDocument*>(document);
    if (!textDocument)
        return;

    textDocument->setEditor(this);
    qDebug("ssr");
    AbstractEditor::setDocument(document);
}

void TextEditorEditor::setupUi()
{
    m_toolBar = new QToolBar(this);
    m_editor = new TextEdit(this);
    m_layout = new QVBoxLayout(this);

    m_toolBar->addAction(m_editor->action(TextEdit::Undo));
    m_toolBar->addAction(m_editor->action(TextEdit::Redo));
    m_toolBar->addAction(m_editor->action(TextEdit::Copy));
    m_toolBar->addAction(m_editor->action(TextEdit::Cut));
    m_toolBar->addAction(m_editor->action(TextEdit::Paste));
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_editor->action(TextEdit::ZoomIn));
    m_toolBar->addAction(m_editor->action(TextEdit::ZoomOut));

    m_layout->setSpacing(0);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->addWidget(m_toolBar);
    m_layout->addWidget(m_editor);

    //connect(m_editor->document(), SIGNAL(modificationChanged(bool)), m_document, SIGNAL(modificationChanged(bool)));
}

TextEditorFactory::TextEditorFactory(QObject *parent) :
    AbstractEditorFactory(parent)
{
}

QByteArray TextEditorFactory::id() const
{
    return "texteditor";
}

QString TextEditorFactory::name() const
{
    return tr("Text editor");
}

QStringList TextEditorFactory::mimeTypes() const
{
    return QStringList() << "text/plain";
}

QIcon TextEditorFactory::icon() const
{
    return QIcon(":/icons/texteditor.png");
}

AbstractDocument * TextEditorFactory::createDocument(QObject *parent)
{
    return new TextEditorDocument(parent);
}

AbstractEditor * TextEditorFactory::createEditor(QWidget *parent)
{
    return new TextEditorEditor(parent);
}
