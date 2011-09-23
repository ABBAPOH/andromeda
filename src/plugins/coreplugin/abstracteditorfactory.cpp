#include "abstracteditorfactory.h"
#include "abstracteditor.h"

using namespace CorePlugin;

/*!
    \brief Creates a AbstractEditorFactory with the given \a parent.
*/
AbstractEditorFactory::AbstractEditorFactory(QObject *parent) :
    QObject(parent)
{
}

/*!
    \brief Destroys factory and all editors created with it.
*/
AbstractEditorFactory::~AbstractEditorFactory()
{
    QList<AbstractEditor *> editors = m_editors;
    qDeleteAll(editors);
}

/*!
    \brief Creates new editor and appends it to list of editors.
*/
AbstractEditor * AbstractEditorFactory::editor(QWidget *parent)
{
    AbstractEditor *editor = createEditor(parent);
    editor->setFactory(this);
    return editor;
}

