#include "abstracteditorfactory.h"
#include "abstracteditor.h"

#include <QtCore/QVariant>

#include "abstractdocument.h"

using namespace GuiSystem;

/*!
    \class GuiSystem::AbstractEditorFactory

    \brief AbstractEditorFactory is a base class for factories for creating AbstractEditors.
*/

/*!
    \brief Creates a AbstractEditorFactory with the given \a parent.
*/
AbstractEditorFactory::AbstractEditorFactory(QObject *parent) :
    QObject(parent)
{
}

/*!
    \brief Destroys factory and all editors and documents created with it.
*/
AbstractEditorFactory::~AbstractEditorFactory()
{
    QList<AbstractEditor *> editors = m_editors;
    qDeleteAll(editors);
}

/*!
    \brief Creates and returns new editor.
*/
AbstractEditor * AbstractEditorFactory::editor(QWidget *parent)
{
    AbstractEditor *editor = createEditor(parent);
    editor->setProperty("id", id());
    connect(editor, SIGNAL(destroyed(QObject*)), SLOT(onEditorDestroyed(QObject*)));
    m_editors.append(editor);
    return editor;
}

/*!
    \internal
*/
void AbstractEditorFactory::onEditorDestroyed(QObject *object)
{
    AbstractEditor *editor = static_cast<AbstractEditor *>(object);
    m_editors.removeOne(editor);
}

/*!
    \fn AbstractDocument * AbstractEditorFactory::createEditor(QWidget *parent)

    \brief Reipmlement this function to create new editor.
*/
