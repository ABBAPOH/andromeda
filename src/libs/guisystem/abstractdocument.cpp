#include "abstractdocument.h"
#include "abstractdocument_p.h"

#include <QDebug>

using namespace GuiSystem;

/*!
  \class GuiSystem::AbstractDocument

  \brief AbstractDocument is an interface for file saving API.
*/

/*!
  \brief Constructs an AbstractDocument with the given \a parent.
*/
AbstractDocument::AbstractDocument(QObject *parent) :
    QObject(parent),
    d_ptr(new AbstractDocumentPrivate(this))
{
}

/*!
    Constructs an AbstractDocument with the given \a parent and private class \a dd.
*/
AbstractDocument::AbstractDocument(AbstractDocumentPrivate &dd, QObject *parent) :
    QObject(parent),
    d_ptr(&dd)
{
}

/*!
    Destroys an AbstractDocument.
*/
AbstractDocument::~AbstractDocument()
{
    delete d_ptr;
}

/*!
    \property AbstractDocument::icon
    \brief This property holds document's icon.
*/
QIcon AbstractDocument::icon() const
{
    Q_D(const AbstractDocument);
    return d->icon;
}

/*!
    \brief Sets current document's icon.
    This function should be called by AbstractDocument implementations when
    they open an url.
*/
void AbstractDocument::setIcon(const QIcon &icon)
{
    Q_D(AbstractDocument);
    d->icon = icon;
    emit iconChanged(icon);
}

/*!
    \property AbstractDocument::modified

    \brief This property holds whether the document has been modified by the user.

    Default value is false.
*/
bool AbstractDocument::isModified() const
{
    Q_D(const AbstractDocument);
    return d->modified;
}

void AbstractDocument::setModified(bool modified)
{
    Q_D(AbstractDocument);
    if (d->modified == modified)
        return;

    d->modified = modified;
    emit modificationChanged(d->modified);
}

/*!
    \fn void AbstractDocument::modificationChanged(bool modified)

    \brief Emit this signal whenever document changes it's modification state (i.e. modified by user).
*/

/*!
    \property AbstractDocument::readOnly

    \brief This property holds whether the document is read only
    (i.e. can be saved with same url) or not.

    Default value is true.
*/

bool AbstractDocument::isReadOnly() const
{
    Q_D(const AbstractDocument);
    return d->readOnly;
}

void AbstractDocument::setReadOnly(bool readOnly)
{
    Q_D(AbstractDocument);
    if (d->readOnly == readOnly)
        return;

    d->readOnly = readOnly;
    emit readOnlyChanged(d->readOnly);
}

/*!
  \fn void AbstractDocument::readOnlyChanged(bool readOnly)

  \brief Emit this signal whenever document changes it's modification state (i.e. modified by user).
*/

/*!
    \property AbstractDocument::title

    \brief Title that is shown to user.

    For exapmple, title can be displayed in a tab or in a window title.
*/
QString AbstractDocument::title() const
{
    Q_D(const AbstractDocument);
    return d->title;
}

/*!
    \brief Sets current document's title.
    This function should be called by AbstractDocument implementations when
    they open an url.
*/
void AbstractDocument::setTitle(const QString &title)
{
    Q_D(AbstractDocument);
    if (d->title == title)
        return;
    d->title = title;
    emit titleChanged(title);
}

/*!
    Returns current url.
*/
QUrl AbstractDocument::url() const
{
    Q_D(const AbstractDocument);
    return d->url;
}

/*!
    Sets current url and emits urlChanged() signal if nececcary.
*/
void AbstractDocument::setUrl(const QUrl &url)
{
    Q_D(AbstractDocument);
    if (d->url == url)
        return;

    bool ok = openUrl(url);
    if (!ok)
        qWarning() << "AbstractDocument::setUrl: Failed to open url" << url;

    d->url = url;
    emit urlChanged(url);
}

/*!
    \brief Reimplement to reset document to initial state.

    Default implementation does nothing.
*/
void AbstractDocument::clear()
{
}

/*!
    \brief Reimplement to reload currently opened url.

    Default implementation does nothing.
*/
void AbstractDocument::reload()
{
}

/*!
  \brief Reimplement to save currently opened document to \a url.

  Empty url is passed when user requests to save document to current url.
*/
void AbstractDocument::save(const QUrl &url)
{
}

/*!
    \brief Reimplement to cancel loading url.

    Default implementation does nothing.
*/
void AbstractDocument::stop()
{
}

/*!
    \fn void AbstractDocument::loadStarted()

    This signal should be emitted when document starts loading time consuming data.
*/

/*!
    \fn void AbstractDocument::loadProgress(int)

    This signal should be emitted to notify about loading progress.
*/

/*!
    \fn void AbstractDocument::loadFinished(bool)

    This signal should be emitted when editor stops loading time consuming data.
    \a ok will indicate whether the opening was successful or any error occurred.
*/

/*!
    \fn bool AbstractDocument::openUrl(const QUrl &url)
    \brief Reipmlement this function to open data located at \a url in a document.

    \note This function is called before changing AbstractDocument::url property, so you can
    retreive an old url via url() method.
*/
