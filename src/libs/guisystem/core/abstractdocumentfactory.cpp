#include "abstractdocumentfactory.h"

#include <QtCore/QVariant>

#include "abstractdocument.h"

using namespace GuiSystem;

/*!
    \class GuiSystem::AbstractDocumentFactory

    \brief AbstractDocumentFactory is a base class for factories for creating AbstractEditors.
*/

/*!
    \brief Creates a AbstractDocumentFactory with the given \a parent.
*/
AbstractDocumentFactory::AbstractDocumentFactory(const QByteArray &id, QObject *parent) :
    QObject(parent),
    m_id(id)
{
    Q_ASSERT_X(!id.isEmpty(),
               "AbstractDocumentFactory::AbstractDocumentFactory",
               "Id can't be empty");
}

/*!
    \brief Destroys factory and all documents created with it.
*/
AbstractDocumentFactory::~AbstractDocumentFactory()
{
    QList<AbstractDocument *> documents = m_documents;
    qDeleteAll(documents);
}

/*!
    \brief Creates and returns new document.
*/
AbstractDocument * AbstractDocumentFactory::document(QObject *parent)
{
    AbstractDocument *editor = createDocument(parent);
    editor->setProperty("id", id());
    connect(editor, SIGNAL(destroyed(QObject*)), SLOT(onDocumentDestroyed(QObject*)));
    m_documents.append(editor);
    return editor;
}

/*!
    Returns factory's id.
*/
QByteArray AbstractDocumentFactory::id() const
{
    return m_id;
}

/*!
    \internal
*/
void AbstractDocumentFactory::onDocumentDestroyed(QObject *object)
{
    AbstractDocument *document = static_cast<AbstractDocument *>(object);
    m_documents.removeOne(document);
}

/*!
    \fn AbstractDocument * AbstractDocumentFactory::createDocument(QObject *parent)

    \brief Reipmlement this function to create new document.
*/
