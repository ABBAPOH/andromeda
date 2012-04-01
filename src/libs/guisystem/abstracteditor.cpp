#include "abstracteditor.h"
#include "abstracteditorfactory.h"

#include "actionmanager.h"

#include <QtCore/QSettings>
#include <QtCore/QUrl>

using namespace GuiSystem;

/*!
  \class AbstractEditor

  \brief AbstractEditor is a base class for opening content located at url.
*/

/*!
  \brief Creates a AbstractEditor with the given \a parent.
*/
AbstractEditor::AbstractEditor(QWidget *parent) :
    QWidget(parent),
    m_factory(0)
{
}

/*!
  \brief Destroys AbstractEditor.
*/
AbstractEditor::~AbstractEditor()
{
    if (m_factory)
        m_factory->m_editors.removeAll(this);
}

/*!
  \property AbstractEditor::id

  \brief This property contains id of a factory that created this view, or
  empty byte array, if view was created using public constructor.
*/
QByteArray AbstractEditor::id() const
{
    if (m_factory)
        return m_factory->id();

    return QByteArray();
}

/*!
  \property AbstractEditor::url

  \brief Contains current url opened in this editor.
*/

/*!
  \fn QUrl AbstractEditor::url() const

  \brief Reimplement this function to return currently opened url.
*/

/*!
  \fn void AbstractEditor::open(const QUrl &url)

  \brief Reimplement this function to do initialization when opening new \a url.

  You need to emit urlChanged signal in this function if new url differs from the old one.
*/

/*!
  \brief Convenience method. Same as AbstractEditor::open.
*/
void AbstractEditor::setUrl(const QUrl &url)
{
    open(url);
}

/*!
  \fn void AbstractEditor::urlChanged(const QUrl &url)

  This signal should be emited every time current url becomes changed.
*/

/*!
  \brief Reimplement to cancel loading url.

  Default implementation emits loadingFinished(false) signal.
*/
void AbstractEditor::cancel()
{
    emit loadFinished(false);
}

/*!
  \brief Reimplement to perform cleanup actions when closing current document.

  This function should always be called before destroying editor and also should
  be called when user requests closing of current editor.
*/
void AbstractEditor::close()
{
}

/*!
  \brief Reimplement to reload currently opened url.

  Default implementation does nothing.
*/
void AbstractEditor::refresh()
{
}

/*!
  \brief Reimplement to reset editor to initial state.

  This method should load empty or default page into an editor and completely clear history.

  Default implementation does nothing.
*/
void AbstractEditor::clear()
{
}

/*!
  \fn void openTriggered(const QUrl &url)

  \brief Emit this signal when user triggers opening url in current window and
  tab.
*/

/*!
  \fn void openNewEditorTriggered(const QList<QUrl> &urls)

  \brief Emit this signal when user triggers opening urls in current window and
  in new tab or new page.
*/

/*!
  \fn void openNewWindowTriggered(const QList<QUrl> &urls)

  \brief Emit this signal when user triggers opening urls in new window.
*/

/*!
  \property AbstractEditor::icon

  \brief Icon that can be shown on toolbars, menus and so on.
*/

/*!
  \brief Reimplement to return current view's icon.
*/
QIcon AbstractEditor::icon() const
{
    return QIcon();
}

/*!
  \fn void iconChanged(const QIcon &icon)

  \brief Emit this signal when view's icon is changed.
*/

/*!
  \brief Reimplement to return current preview image for this editor.

  This image can be used in bookmarks or persistent history.

  Default impelentation returns empty image.
*/
QImage AbstractEditor::preview() const
{
    return QImage();
}

/*!
  \property AbstractEditor::title

  \brief Title that can be show to user.
*/

/*!
  \brief Reimplement to return current view's title.

  Value returned by this function usually used as tab title.
*/
QString AbstractEditor::title() const
{
    return QString();
}
/*!
  \property AbstractEditor::windowTitle() const

  \brief String that can be displayed as a window title.
*/

/*!
  \brief Reimplement to return current editors window title.

  Value returned by this function is used as a main window title.

  Default impelentation returns empty string.
*/
QString AbstractEditor::windowTitle() const
{
    return QString();
}

/*!
  \brief Reimplement to return file interface that corresponds to this editor.

  Default imlementation returns 0.
*/
IFile * AbstractEditor::file() const
{
    return 0;
}

/*!
  \brief Reimplement to return find interface that corresponds to this editor.

  Default imlementation returns 0.
*/
IFind *AbstractEditor::find() const
{
    return 0;
}

/*!
  \fn void AbstractEditor::windowTitleChanged(const QString &title)

  This signal should be emited when window title changes.
*/

/*!
  \brief Reimplement to return history that corresponds to this editor.

  Default imlementation returns 0.
*/
IHistory * AbstractEditor::history() const
{
    return 0;
}

/*!
  \reimp

  Restored url from byte array and calls AbstractEditor::open.
*/
bool AbstractEditor::restoreState(const QByteArray &state)
{
    open(QUrl::fromEncoded(state));
    return true;
}

/*!
  \reimp

  Stores url.
*/
QByteArray AbstractEditor::saveState() const
{
    return url().toEncoded();
}

/*!
  \fn void AbstractEditor::loadStarted()

  This signal should be emited when editor starts loading time consuming data.
*/

/*!
  \fn void AbstractEditor::loadProgress(int)

  This signal should be emited to notify about loading progress.
*/

/*!
  \fn void AbstractEditor::loadFinished(bool)

  This signal should be emited when editor stops loading time consuming data.
  \a ok will indicate whether the opening was successful or any error occurred.
*/

/*!
  \brief Returns factory that created this editor.
*/
AbstractEditorFactory * AbstractEditor::factory() const
{
    return m_factory;
}

void AbstractEditor::setFactory(AbstractEditorFactory *factory)
{
    m_factory = factory;
}

/*!
  \brief Returns action manager instance.
*/
ActionManager * AbstractEditor::actionManager() const
{
    return ActionManager::instance();
}

/*!
  \brief Adds \a action to this widget and registers it in ActionManager
  using an \a id.
*/
void AbstractEditor::addAction(QAction *action, const QByteArray &id)
{
    QWidget::addAction(action);
    registerAction(action, id);
}

/*!
  \brief Registers action in ActionManager.
*/
void AbstractEditor::registerAction(QAction *action, const QByteArray &id)
{
    actionManager()->registerAction(action, id);
}
