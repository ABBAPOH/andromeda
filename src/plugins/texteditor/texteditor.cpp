#include "texteditor.h"
#include "texteditorfile.h"
#include <QtCore/QUrl>
#include <QtGui/QLabel>
#include <QtGui/QResizeEvent>
#include <QDebug>

using namespace GuiSystem;
using namespace TEXTEditor;

void TextEditorFile::save(const QUrl &url)
{
#if (QT_VERSION >= QT_VERSION_CHECK(4, 8, 0))
    if (!url.isLocalFile())
#else
    if (url.scheme() != QLatin1String("file"))
#endif
        return;

    QString path = url.toLocalFile();
    QFile file(path);
    if (!file.open(QFile::WriteOnly))
        return;

    editor->m_editor->saveFile(&file);
}

TextEditor::TextEditor(QWidget *parent) :
    AbstractEditor(parent),
    m_file(new TextEditorFile(this))
{
    setupUi();
}

void TextEditor::setupUi()
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

    connect(m_editor->document(), SIGNAL(modificationChanged(bool)), m_file, SIGNAL(modificationChanged(bool)));
}

void TextEditor::open(const QUrl &url)
{
    if (m_url == url)
        return;

    emit loadStarted();

    m_url = url;
    QString path = url.toLocalFile();
    if (path.isEmpty()) {
        emit loadFinished(false);
        return;
    }

    QFile file(path);
    if (!file.open(QFile::ReadOnly)) {
        emit loadFinished(false);
        return;
    }

    m_editor->open(url.toLocalFile());

    emit urlChanged(url);
    emit iconChanged(icon());
    emit titleChanged(title());
    emit windowTitleChanged(windowTitle());
    emit loadFinished(true);
}

QUrl TextEditor::url() const
{
    return m_url;
}

QIcon TextEditor::icon() const
{
    return QIcon::fromTheme("accessories-text-editor", QIcon(":/icons/texteditor.png"));
}

QString TextEditor::title() const
{
    return m_editor->currentFileName();
}

QString TextEditor::windowTitle() const
{
    return m_editor->currentFileName();
}

IFile *TextEditor::file() const
{
    return m_file;
}

TextEditorFactory::TextEditorFactory(QObject *parent) :
    AbstractEditorFactory(parent)
{
}

QByteArray TextEditorFactory::id() const
{
    return "texteditor";
}

QStringList TextEditorFactory::mimeTypes() const
{
    return QStringList() << "text/plain";
}

AbstractEditor * TextEditorFactory::createEditor(QWidget *parent)
{
    return new TextEditor(parent);
}
