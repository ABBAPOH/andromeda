#include "abstractcontainer.h"

using namespace GuiSystem;

AbstractContainer::AbstractContainer(QWidget *parent) :
    AbstractEditor(parent)
{
}

AbstractEditor *AbstractContainer::currentEditor() const
{
    return editor(currentIndex());
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
