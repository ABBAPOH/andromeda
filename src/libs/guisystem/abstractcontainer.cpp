#include "abstractcontainer.h"

#include <QtCore/QBuffer>

using namespace GuiSystem;

AbstractContainer::AbstractContainer(QWidget *parent) :
    AbstractEditor(parent)
{
}

/*!
  \reimp
*/
AbstractEditor::Capabilities AbstractContainer::capabilities() const
{
    AbstractEditor *editor = currentEditor();
    if (editor)
        return editor->capabilities();

    return 0;
}

AbstractEditor *AbstractContainer::currentEditor() const
{
    return editor(currentIndex());
}

/*!
  \reimp
*/
bool AbstractContainer::isModified() const
{
    AbstractEditor *editor = currentEditor();
    if (editor)
        return editor->isModified();

    return AbstractEditor::isModified();
}

/*!
  \reimp
*/
void AbstractContainer::setModified(bool modified)
{
    AbstractEditor *editor = currentEditor();
    if (editor)
        return editor->setModified(modified);
}

/*!
  \reimp
*/
bool AbstractContainer::isReadOnly() const
{
    AbstractEditor *editor = currentEditor();

    if (editor)
        return editor->isReadOnly();

    return AbstractEditor::isReadOnly();
}

/*!
  \reimp
*/
void AbstractContainer::setReadOnly(bool readOnly)
{
    AbstractEditor *editor = currentEditor();
    if (editor)
        return editor->setReadOnly(readOnly);
}

/*!
  \reimp
*/
QUrl AbstractContainer::url() const
{
    AbstractEditor *editor = currentEditor();
    if (editor)
        return editor->url();

    return QUrl();
}

/*!
  \brief Returns list of currently opened urls in this container
*/
QList<QUrl> AbstractContainer::urls() const
{
    QList<QUrl> urls;
    for (int i = 0; i < count(); i++) {
        urls.append(editor(i)->url());
    }
    return urls;
}

/*!
  \reimp
*/
void AbstractContainer::open(const QUrl &url)
{
    if (count() == 0) {
        openNewEditor(url);
    } else {
        AbstractEditor *editor = currentEditor();
        if (editor)
            editor->open(url);
    }
}

void AbstractContainer::openNewEditor(const QList<QUrl> &urls)
{
    foreach (const QUrl &url, urls) {
        openNewEditor(url);
    }
}

/*!
  \reimp
*/
void AbstractContainer::cancel()
{
    AbstractEditor *editor = currentEditor();
    if (editor)
        editor->cancel();
}
/*!
  \reimp
*/
void AbstractContainer::close()
{
    AbstractEditor *editor = currentEditor();
    if (editor)
        editor->close();
}

/*!
  \reimp
*/
void AbstractContainer::refresh()
{
    AbstractEditor *editor = currentEditor();
    if (editor)
        editor->refresh();
}

void AbstractContainer::save(const QUrl &url)
{
    AbstractEditor *editor = currentEditor();
    if (editor) {
        if (editor->capabilities() & CanSave)
            editor->save(url);
    }
}

/*!
  \reimp
*/
QIcon AbstractContainer::icon() const
{
    AbstractEditor *editor = currentEditor();
    if (editor)
        return editor->icon();

    return QIcon();
}

/*!
  \reimp
*/
QImage AbstractContainer::preview() const
{
    AbstractEditor *editor = currentEditor();
    if (editor)
        return editor->preview();

    return QImage();
}

/*!
  \reimp
*/
QString AbstractContainer::title() const
{
    AbstractEditor *editor = currentEditor();
    if (editor)
        return editor->title();

    return QString();
}

/*!
  \reimp
*/
QString AbstractContainer::windowTitle() const
{
    AbstractEditor *editor = currentEditor();
    if (editor)
        return editor->windowTitle();

    return QString();
}
