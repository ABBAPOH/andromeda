#include "abstracteditor.h"

#include "actionmanager.h"

#include <QtCore/QSettings>
#include <QtCore/QUrl>

#include "abstractdocument.h"

using namespace GuiSystem;

/*!
    \class GuiSystem::AbstractEditor
*/

/*!
    \brief Creates a AbstractEditor with the given \a document and \a parent.
*/
AbstractEditor::AbstractEditor(AbstractDocument &document, QWidget *parent) :
    QWidget(parent),
    m_document(&document)
{
}

/*!
    \brief Destroys AbstractEditor.
*/
AbstractEditor::~AbstractEditor()
{
}

/*!
    Returns current document, or 0, if no document is set.
*/
AbstractDocument * AbstractEditor::document() const
{
    return m_document;
}

/*!
    Sets current document and emits documentChanged() signal.

    Document can't be 0.

    Old document is deleted if editor is it's parent. Reparent old document
    before setting the new one if you don't need to delete old document.
*/
void AbstractEditor::setDocument(AbstractDocument *document)
{
    if (!document)
        return;

    if (m_document == document)
        return;

    if (m_document && m_document->parent() == this)
        m_document->deleteLater();

    m_document = document;
    emit documentChanged();
}

/*!
    \fn void AbstractEditor::documentChanged();

    This signal is emited when current document is changed.
*/

/*!
    \brief Reimplement to return find interface that corresponds to this editor.

    Default imlementation returns 0.
*/
IFind * AbstractEditor::find() const
{
    return 0;
}

/*!
    \brief Reimplement to return history that corresponds to this editor.

    Default imlementation returns 0.
*/
IHistory * AbstractEditor::history() const
{
    return 0;
}

/*!
    \brief Reimplement to restore editor's state.

    Default implementation restores url from byte array and calls AbstractDocument::setUrl.
*/
bool AbstractEditor::restoreState(const QByteArray &state)
{
    document()->setUrl(QUrl::fromEncoded(state));
    return true;
}

/*!
    \brief Reimplement to store editor's state.

    Default implementation stores current url.
*/
QByteArray AbstractEditor::saveState() const
{
    return document()->url().toEncoded();
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
