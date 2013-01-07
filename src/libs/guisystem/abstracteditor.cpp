#include "abstracteditor.h"

#include <QtCore/QSettings>
#include <QtCore/QUrl>

#include "abstractdocument.h"
#include "actionmanager.h"

using namespace GuiSystem;

/*!
    \class GuiSystem::AbstractEditor

    This is base class for displaying AbstractDocuments.
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
    Returns current document.
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

    Editor does not takes ownership of a \a document.
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
    \fn void AbstractEditor::documentChanged()

    This signal is emitted when current document is changed.
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
