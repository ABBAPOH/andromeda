#include "proxyeditor.h"

using namespace GuiSystem;

ProxyEditor::ProxyEditor(QWidget *parent) :
    AbstractEditor(parent)
{
}

AbstractEditor *ProxyEditor::sourceEditor() const
{
    return m_editor;
}

void ProxyEditor::setSourceEditor(AbstractEditor *editor)
{
    if (m_editor == editor)
        return;

    disconnectEditor(m_editor);

        m_editor = editor;

    connectEditor(m_editor);

    emit urlChanged(m_editor->url());
    emit iconChanged(m_editor->icon());
    emit titleChanged(m_editor->title());
    emit windowTitleChanged(m_editor->windowTitle());
    emit loadFinished(true);
}

/*!
  \reimp
*/
QUrl ProxyEditor::url() const
{
    AbstractEditor *editor = sourceEditor();
    if (editor)
        return editor->url();

    return QUrl();
}

/*!
  \reimp
*/
void ProxyEditor::open(const QUrl &url)
{
    AbstractEditor *editor = sourceEditor();
    if (editor)
        editor->open(url);
}

/*!
  \reimp
*/
void ProxyEditor::cancel()
{
    AbstractEditor *editor = sourceEditor();
    if (editor)
        editor->cancel();
}
/*!
  \reimp
*/
void ProxyEditor::close()
{
    AbstractEditor *editor = sourceEditor();
    if (editor)
        editor->close();
}

/*!
  \reimp
*/
void ProxyEditor::refresh()
{
    AbstractEditor *editor = sourceEditor();
    if (editor)
        editor->refresh();
}

/*!
  \reimp
*/
QIcon ProxyEditor::icon() const
{
    AbstractEditor *editor = sourceEditor();
    if (editor)
        return editor->icon();

    return QIcon();
}

/*!
  \reimp
*/
QImage ProxyEditor::preview() const
{
    AbstractEditor *editor = sourceEditor();
    if (editor)
        return editor->preview();

    return QImage();
}

/*!
  \reimp
*/
QString ProxyEditor::title() const
{
    AbstractEditor *editor = sourceEditor();
    if (editor)
        return editor->title();

    return QString();
}

/*!
  \reimp
*/
QString ProxyEditor::windowTitle() const
{
    AbstractEditor *editor = sourceEditor();
    if (editor)
        return editor->windowTitle();

    return QString();
}

/*!
  \internal
*/
void ProxyEditor::connectEditor(AbstractEditor *editor)
{
    if (!editor)
        return;

    connect(editor, SIGNAL(urlChanged(QUrl)),
            this, SIGNAL(urlChanged(QUrl)));
    connect(editor, SIGNAL(openTriggered(QUrl)), this, SIGNAL(openTriggered(QUrl)));

    connect(editor, SIGNAL(iconChanged(QIcon)), this, SIGNAL(iconChanged(QIcon)));
    connect(editor, SIGNAL(titleChanged(QString)), this, SIGNAL(titleChanged(QString)));
    connect(editor, SIGNAL(windowTitleChanged(QString)), this, SIGNAL(windowTitleChanged(QString)));

    connect(editor, SIGNAL(loadStarted()), this, SIGNAL(loadStarted()));
    connect(editor, SIGNAL(loadProgress(int)), this, SIGNAL(loadProgress(int)));
    connect(editor, SIGNAL(loadFinished(bool)), this, SIGNAL(loadFinished(bool)));
}

/*!
  \internal
*/
void ProxyEditor::disconnectEditor(AbstractEditor *editor)
{
    if (!editor)
        return;

    // We do not want to disconnect all signals, because there can be other
    // connections in classes inherited from this class
    disconnect(editor, SIGNAL(urlChanged(QUrl)),
            this, SIGNAL(urlChanged(QUrl)));
    disconnect(editor, SIGNAL(openTriggered(QUrl)), this, SIGNAL(openTriggered(QUrl)));

    disconnect(editor, SIGNAL(iconChanged(QIcon)), this, SIGNAL(iconChanged(QIcon)));
    disconnect(editor, SIGNAL(titleChanged(QString)), this, SIGNAL(titleChanged(QString)));
    disconnect(editor, SIGNAL(windowTitleChanged(QString)), this, SIGNAL(windowTitleChanged(QString)));

    disconnect(editor, SIGNAL(loadStarted()), this, SIGNAL(loadStarted()));
    disconnect(editor, SIGNAL(loadProgress(int)), this, SIGNAL(loadProgress(int)));
    disconnect(editor, SIGNAL(loadFinished(bool)), this, SIGNAL(loadFinished(bool)));
}
